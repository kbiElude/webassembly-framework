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
 *       under both Windows and in web browsers befriended to WebAssembly and ES3.1 support.
 */
#include "framework.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#ifdef __EMSCRIPTEN__
    #include "emscripten_mainloop_stub.h"
#endif

#include <GLFW/glfw3.h>


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr,
            "GLFW Error %d: %s\n",
            error,
            description);

    assert(false);
}

int main(int, char**)
{
    int         result           = 1;
    GLFWwindow* window_ptr       = nullptr;

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit() )
    {
        assert(false);

        goto end;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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
        ImGui::NewFrame           ();
        {
            imgui_callback();
        }

        // Rendering
        ImGui::Render();
        {
            int display_h = 0;
            int display_w = 0;

            glfwGetFramebufferSize(window_ptr,
                                  &display_w,
                                  &display_h);

            render_callback(display_w,
                            display_h);
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData() );

        glfwSwapBuffers(window_ptr);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

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
