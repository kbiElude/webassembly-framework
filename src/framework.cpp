/*  MIT License

    Copyright (c) 2024 Dominik Witczak

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/* NOTE: Code below is a mash-up of code grabbed from emscripten and imgui examples.
 *       Nothing exciting in here. Just the most basic stuff needed to run actual app
 *       under both Windows and in web browsers befriended to WebAssembly and ES2.0 support.
 */
#include "framework.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <vector>

#ifdef __EMSCRIPTEN__
    #include "emscripten_mainloop_stub.h"
    #include <unistd.h>
#endif

#include <GLFW/glfw3.h>

static auto        g_app_ptr                     = create_app();
static bool        g_font_atlas_rebuild_requested = false;
static bool        g_lazy_file_drop_requested     = false;
static std::string g_reported_error_string;


void Framework::report_error(const std::string& in_error)
{
    if (g_reported_error_string.size() == 0)
    {
        g_reported_error_string = in_error;
    }
}

void Framework::request_font_atlas_rebuild()
{
    g_font_atlas_rebuild_requested = true;
}

void Framework::request_lazy_file_drop()
{
    g_lazy_file_drop_requested = true;
}

#if !defined(__EMSCRIPTEN__)
static void glfw_windowcontentscale_callback(GLFWwindow* window,
                                             float       xscale,
                                             float       /* yscale */)
{
    g_app_ptr->on_dpi_scale_changed(xscale);
}
#endif

static void glfw_cursorpos_callback(GLFWwindow* window,
                                    double      x,
                                    double      y)
{
    g_app_ptr->on_mouse_pos_callback(x,
                                     y);
}

/* Returns false if a fatal error was reported and the drop loop should abort. */
static bool process_dropped_file(const char* in_path_ptr)
{
    bool        result      = false;
    FILE*       file_handle = ::fopen    (in_path_ptr,
                                          "rb");
    std::string file_name   = std::string(in_path_ptr);
    size_t      file_size   = 0;

    if (file_handle == nullptr)
    {
        Framework::report_error("Failed to open [" + file_name + "].");

        goto end;
    }

    ::fseek(file_handle,
            0L,
            SEEK_END);

#if defined(_WIN32)
    file_size = ::_ftelli64(file_handle);
#else
    file_size = ::ftello64(file_handle);
#endif

    ::fseek(file_handle,
            0L,
            SEEK_SET);

    if (file_size == 0)
    {
        ::fclose(file_handle);

        result = true;

        goto end;
    }

#if !defined(__EMSCRIPTEN__)
    if (g_lazy_file_drop_requested)
    {
        ::fclose(file_handle);

        g_app_ptr->on_file_dropped_callback(file_name,
                                            nullptr);

        result = true;

        goto end;
    }
#endif

    {
        Uint8VectorUniquePtr file_data_u8_vec_ptr(new std::vector<uint8_t>(file_size));

        if (::fread(file_data_u8_vec_ptr->data(),
                    static_cast<size_t>(file_size),
                    1u,
                    file_handle) != 1)
        {
            ::fclose(file_handle);

            Framework::report_error("Failed to read drag & dropped file [" + file_name + "].");

            goto end;
        }

        ::fclose(file_handle);

        #if defined(__EMSCRIPTEN__)
        {
            ::unlink(in_path_ptr);
        }
        #endif

        g_app_ptr->on_file_dropped_callback(file_name,
                                            std::move(file_data_u8_vec_ptr));
    }

    result = true;
end:
    return result;
}

static void glfw_drop_callback(GLFWwindow* window,
                               int         n_paths,
                               const char* paths[])
{
    assert(g_app_ptr != nullptr);

    for (int32_t n_path = 0;
                 n_path < n_paths;
               ++n_path)
    {
        if (!::process_dropped_file(paths[n_path]))
        {
            goto end;
        }
    }

end:
    ;
}

static void glfw_error_callback(int         error,
                                const char* description)
{
    Framework::report_error("GLFW reported an error: " + std::string(description) );
}

static void glfw_mousebutton_callback(GLFWwindow* window,
                                      int         button,
                                      int         action,
                                      int         mods)
{
    const bool is_pressed   = (action == GLFW_PRESS);
    const auto mouse_button = (button == GLFW_MOUSE_BUTTON_LEFT)   ? MouseButton::LEFT
                            : (button == GLFW_MOUSE_BUTTON_RIGHT)  ? MouseButton::RIGHT
                            : (button == GLFW_MOUSE_BUTTON_MIDDLE) ? MouseButton::MIDDLE
                                                                   : MouseButton::UNKNOWN;
    double     x            = 0;
    double     y            = 0;

    glfwGetCursorPos(window,
                    &x,
                    &y);

    if (mouse_button != MouseButton::UNKNOWN)
    {
        g_app_ptr->on_mouse_button_callback(x,
                                            y,
                                            mouse_button,
                                            is_pressed);
    }
}

static void glfw_scroll_callback(GLFWwindow* window,
                                 double      xoffset,
                                 double      yoffset)
{
    g_app_ptr->on_scroll_callback(xoffset,
                                  yoffset);
}

int main(int, char**)
{
    int         result     = 1;
    GLFWwindow* window_ptr = nullptr;

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit() )
    {
        assert(false);

        goto end;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API,            GLFW_OPENGL_ES_API);

    if (g_app_ptr != nullptr && g_app_ptr->should_open_maximized() )
    {
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    }
    
    // Create window with graphics context
    window_ptr = glfwCreateWindow(1280,
                                  720,
                                  g_app_ptr->get_window_title(),
                                  nullptr,  /* monitor */
                                  nullptr); /* share   */

    if (window_ptr == nullptr)
    {
        assert(false);

        goto end;
    }

    if (g_app_ptr == nullptr)
    {
        assert(false);

        goto end;
    }

    glfwSetDropCallback       (window_ptr, glfw_drop_callback);
    glfwSetMouseButtonCallback(window_ptr, glfw_mousebutton_callback);
    glfwSetCursorPosCallback  (window_ptr, glfw_cursorpos_callback);
    glfwSetScrollCallback     (window_ptr, glfw_scroll_callback);
    #if !defined(__EMSCRIPTEN__)
    glfwSetWindowContentScaleCallback(window_ptr, glfw_windowcontentscale_callback);
    #endif

    glfwMakeContextCurrent(window_ptr);
    glfwSwapInterval      (1); // Enable vsync

    #if !defined(__EMSCRIPTEN__)
    {
        int version = gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress) );

        if (version == 0)
        {
            assert(false);

            goto end;
        }
    }
    #endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    {
        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename  = nullptr;
    }

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window_ptr,
                                 true); /* install_callbacks */

    #ifdef __EMSCRIPTEN__
    {
        ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
    }
    #endif

    ImGui_ImplOpenGL3_Init("#version 100");

    // Detect OS DPI scale and let the app configure fonts/style accordingly.
    {
        float xscale = 1.0f;

        #if defined(__EMSCRIPTEN__)
        {
            xscale = static_cast<float>(EM_ASM_DOUBLE({ return window.devicePixelRatio || 1.0; }));
        }
        #else
        {
            float yscale = 1.0f;

            glfwGetWindowContentScale(window_ptr, &xscale, &yscale);
        }
        #endif

        g_app_ptr->on_imgui_init(xscale);
    }

    // Main loop
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window_ptr))
#endif
    {
        glfwPollEvents();

        g_app_ptr->pre_frame();

        if (g_font_atlas_rebuild_requested)
        {
            g_font_atlas_rebuild_requested = false;

            ImGui_ImplOpenGL3_DestroyFontsTexture();
            ImGui::GetIO().Fonts->Build();
            ImGui_ImplOpenGL3_CreateFontsTexture();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame   ();

        ImGui::NewFrame();
        {
            int display_h = 0;
            int display_w = 0;

            glfwGetFramebufferSize(window_ptr,
                                  &display_w,
                                  &display_h);

            if (g_reported_error_string.size() == 0)
            {
                // Let the app record imgui commands as needed..
                g_app_ptr->configure_imgui(display_w,
                                           display_h);

                ImGui::Render();

                // Follow up with a rendering callback.
                g_app_ptr->render_frame(display_w,
                                        display_h);
            }
            else
            {
                // Show the panic window
                ImVec2 window_size;

                ImGui::Begin("I give up.", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
                {
                    ImGui::Text("An error was reported by the app: %s",
                                g_reported_error_string.c_str() );

                    window_size = ImGui::GetWindowSize();

                    ImGui::SetWindowPos(ImVec2( (display_w - window_size.x) / 2, (display_h - window_size.y) / 2) );
                }
                ImGui::End();

                // Center it.

                ImGui::Render();

                glClear(GL_COLOR_BUFFER_BIT);
            }
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData() );

        glfwSwapBuffers(window_ptr);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    g_app_ptr.reset();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown   ();
    ImGui::DestroyContext     ();

    glfwDestroyWindow(window_ptr);
    glfwTerminate    ();

    result = 0;
end:
    return result;
}
