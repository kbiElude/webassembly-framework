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

static auto        g_app_ptr               = create_app();
static std::string g_reported_error_string;


void Framework::report_error(const std::string& in_error)
{
    if (g_reported_error_string.size() == 0)
    {
        g_reported_error_string = in_error;
    }
}

static void glfw_drop_callback(GLFWwindow* window,
                               int         n_paths,
                               const char* paths[])
{
    /* Cache each file and report to the app. */
    assert(g_app_ptr != nullptr);

    for (int32_t n_path = 0;
                 n_path < n_paths;
               ++n_path)
    {
        FILE*       file_handle = ::fopen(paths[n_path],
                                          "rb");
        std::string file_name   = std::string(paths[n_path]);
        long        file_size   = 0;

        if (file_handle == nullptr)
        {
            Framework::report_error("Failed to open [" + file_name + "].");

            goto end;
        }

        ::fseek(file_handle,
                0L,
                SEEK_END);

        file_size = ::ftell(file_handle);

        ::fseek(file_handle,
                0L,
                SEEK_SET);

        if (file_size > 0)
        {
            Uint8VectorUniquePtr file_data_u8_vec_ptr(new std::vector<uint8_t>(file_size) );

            if (::fread(file_data_u8_vec_ptr->data(),
                        static_cast<size_t>       (file_size),
                        1u,
                        file_handle) != 1)
            {
                Framework::report_error("Failed to read drag & dropped file [" + file_name + "].");

                goto end;
            }

            ::fclose(file_handle);

            #if defined(__EMSCRIPTEN__)
            {
                ::unlink(paths[n_path]);
            }
            #endif

            g_app_ptr->on_file_dropped_callback(file_name,
                                                std::move(file_data_u8_vec_ptr) );
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

    // Create window with graphics context
    window_ptr = glfwCreateWindow(1280,
                                  720,
                                  "Window",
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

    glfwSetDropCallback   (window_ptr, glfw_drop_callback);
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

    // Main loop
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window_ptr))
#endif
    {
        glfwPollEvents();

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
                g_app_ptr->configure_imgui();

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