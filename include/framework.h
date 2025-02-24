/*

    MIT License

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
#if !defined(FRAMEWORK_H)
#define FRAMEWORK_H

#include <memory>
#include <string>
#include <stdint.h>
#include <vector>

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <glad/glad.h>
#endif

/* Forward decls */
class IFrameworkApp;

/* Typedefs */
typedef std::unique_ptr<IFrameworkApp>         FrameworkAppUniquePtr;
typedef std::unique_ptr<std::vector<uint8_t> > Uint8VectorUniquePtr;

enum class MouseButton : uint8_t
{
    LEFT,
    RIGHT,
    MIDDLE,
    UNKNOWN
};

/* Global functions */
namespace Framework
{
    void report_error(const std::string& in_error);
}

/* App interface */
class IFrameworkApp
{
public:
    virtual ~IFrameworkApp()
    {
        /* Stub */
    }

    virtual void configure_imgui(const int& in_width,
                                 const int& in_height) = 0;
    virtual void render_frame   (const int& in_width,
                                 const int& in_height) = 0;

    virtual void on_file_dropped_callback(const std::string&   in_filename,
                                          Uint8VectorUniquePtr in_data_u8_vec_ptr)
    {
        /* Stub */
    }

    virtual void on_mouse_button_callback(const double&      in_x,
                                          const double&      in_y,
                                          const MouseButton& in_mouse_button,
                                          const bool&        in_is_pressed)
    {
        /* Stub */
    }

    virtual void on_scroll_callback(const double& in_xoffset,
                                    const double& in_yoffset)
    {
        /* Stub */
    }
};

extern FrameworkAppUniquePtr create_app();

#endif /* FRAMEWORK_H */