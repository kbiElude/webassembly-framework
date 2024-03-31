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
#include "shader.h"
#include <assert.h>
#include <vector>

Shader::Shader(const ShaderStage& in_shader_stage,
               const std::string& in_glsl)
    :m_glsl        (in_glsl),
     m_id          (0),
     m_shader_stage(in_shader_stage)
{
    /* Stub */
}

Shader::~Shader()
{
    if (m_id != 0)
    {
        glDeleteShader(m_id);
    }
}

ShaderUniquePtr Shader::create(const ShaderStage& in_shader_stage,
                               const std::string& in_glsl)
{
    ShaderUniquePtr result_ptr(new Shader(in_shader_stage,
                                          in_glsl) );

    if (!result_ptr->init() )
    {
        result_ptr.reset();
    }

    return result_ptr;
}

bool Shader::init()
{
    bool       result          = false;
    const auto shader_stage_gl = (m_shader_stage == ShaderStage::FRAGMENT) ? GL_FRAGMENT_SHADER
                               : (m_shader_stage == ShaderStage::VERTEX)   ? GL_VERTEX_SHADER
                                                                           : UINT32_MAX;

    m_id = glCreateShader(shader_stage_gl);

    if (m_id == 0)
    {
        report_error("glCreateShader() return a shader with zero id.");

        goto end;
    }

    {
        const char* glsl_raw_ptr = m_glsl.c_str();

        glShaderSource(m_id,
                       1,               /* count */
                      &glsl_raw_ptr,
                       nullptr);        /* length */
    }

    glCompileShader(m_id);

    {
        GLint compile_status = 0;

        glGetShaderiv(m_id,
                      GL_COMPILE_STATUS,
                     &compile_status);

        if (compile_status != GL_TRUE)
        {
            const char*          info_log_data_ptr             = nullptr;
            GLint                info_log_excl_terminator      = 0;
            GLint                info_log_incl_terminator_size = 0;
            std::vector<uint8_t> info_log_u8_vec;

            glGetShaderiv(m_id,
                          GL_INFO_LOG_LENGTH,
                         &info_log_incl_terminator_size);

            info_log_u8_vec.resize(info_log_incl_terminator_size);

            info_log_data_ptr = reinterpret_cast<const char*>(info_log_u8_vec.data() );

            glGetShaderInfoLog(m_id,
                               info_log_incl_terminator_size,
                              &info_log_excl_terminator,
                               reinterpret_cast<GLchar*>(info_log_u8_vec.data() ));

            report_error("Shader failed to compile due to the following error:\n\n" +
                         std::string(info_log_data_ptr) );

            goto end;
        }
    }

    result = true;
end:
    return result;
}