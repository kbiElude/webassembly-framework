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
#if !defined(SHADER_H)
#define SHADER_H

#include "framework.h"

namespace Framework
{
    /* Forward decls */
    class                           Shader;
    typedef std::unique_ptr<Shader> ShaderUniquePtr;

    enum class ShaderStage : uint8_t
    {
        FRAGMENT,
        VERTEX,

        UNKNOWN
    };

    class Shader
    {
    public:
        /* Public functions */
        static ShaderUniquePtr create(const ShaderStage& in_shader_stage,
                                      const std::string& in_glsl);

        GLuint get_id() const
        {
            return m_id;
        }

        ~Shader();

    private:
        /* Private functions */
        Shader(const ShaderStage& in_shader_stage,
               const std::string& in_glsl);

        bool init();

        /* Private Variables */
        GLuint m_id;

        std::string       m_glsl;
        const ShaderStage m_shader_stage;
    };
}

#endif /* SHADER_H */