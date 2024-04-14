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
#include "program.h"
#include <assert.h>

Framework::Program::Program(const Shader* in_vs_ptr,
                            const Shader* in_fs_ptr)
    :m_fs_ptr(in_fs_ptr),
     m_id    (0),
     m_vs_ptr(in_vs_ptr)
{
    assert(in_fs_ptr != nullptr);
    assert(in_vs_ptr != nullptr);
}

Framework::Program::~Program()
{
    if (m_id != 0)
    {
        glDeleteProgram(m_id);
    }
}

Framework::ProgramUniquePtr Framework::Program::create(const Shader* in_vs_ptr,
                                                       const Shader* in_fs_ptr)
{
    ProgramUniquePtr result_ptr(new Program(in_vs_ptr,
                                            in_fs_ptr) );

    if (!result_ptr->init() )
    {
        result_ptr.reset();
    }

    return result_ptr;
}

GLint Framework::Program::get_uniform_location(const char* in_uniform_name_ptr) const
{
    GLint result = -1;

    {
        auto map_iterator = m_uniform_name_to_id_map.find(in_uniform_name_ptr);

        if (map_iterator != m_uniform_name_to_id_map.end() )
        {
            result = map_iterator->second;
        }
    }

    return result;
}

bool Framework::Program::init()
{
    bool result = false;

    /* Bake the program first. */
    m_id = glCreateProgram();

    if (m_id == 0)
    {
        Framework::report_error("glCreateProgram() returned an ID of 0.");

        goto end;
    }

    glAttachShader(m_id, m_vs_ptr->get_id() );
    glAttachShader(m_id, m_fs_ptr->get_id() );

    glLinkProgram(m_id);

    {
        GLint link_status = 0;

        glGetProgramiv(m_id,
                      GL_LINK_STATUS,
                     &link_status);

        if (link_status != GL_TRUE)
        {
            Framework::report_error("Program failed to link.");

            goto end;
        }
    }

    /* Enumerate active uniforms so that it is not necessary to call glGetUniformLocation() when rendering frames. */
    {
        GLint                n_active_uniforms                       = 0;
        GLint                uniform_name_max_length_incl_terminator = 0;
        std::vector<uint8_t> uniform_name_u8_vec;

        glGetProgramiv(m_id,
                       GL_ACTIVE_UNIFORMS,
                      &n_active_uniforms);

        {

            glGetProgramiv(m_id,
                           GL_ACTIVE_UNIFORM_MAX_LENGTH,
                          &uniform_name_max_length_incl_terminator);

            uniform_name_u8_vec.resize(uniform_name_max_length_incl_terminator);
        }

        for (uint32_t n_active_uniform = 0;
                      n_active_uniform < n_active_uniforms;
                    ++n_active_uniform)
        {
            memset(uniform_name_u8_vec.data(),
                   0,
                   uniform_name_max_length_incl_terminator);

            {
                GLint  uniform_size = 0;
                GLenum uniform_type = GL_NONE;

                glGetActiveUniform(m_id,
                                   n_active_uniform,
                                   uniform_name_max_length_incl_terminator,
                                   nullptr, /* length */
                                  &uniform_size,
                                  &uniform_type,
                                   reinterpret_cast<GLchar*>(uniform_name_u8_vec.data() ));
            }

            if (uniform_name_u8_vec.size() > 0)
            {
                const char* uniform_name_ptr = reinterpret_cast<GLchar*>(uniform_name_u8_vec.data() );
                const auto  uniform_location = glGetUniformLocation     (m_id,
                                                                         uniform_name_ptr);

                if (uniform_location == -1)
                {
                    Framework::report_error(std::string("Invalid uniform location reported for an active uniform [") + std::string(uniform_name_ptr) + "].");

                    goto end;
                }

                if (m_uniform_name_to_id_map.find(uniform_name_ptr) != m_uniform_name_to_id_map.end())
                {
                    Framework::report_error("Uniform [" + std::string(uniform_name_ptr) + "] reported more than once.");

                    goto end;
                }

                m_uniform_name_to_id_map[uniform_name_ptr] = uniform_location;
            }
            else
            {
                Framework::report_error(std::string("Zero-sized uniform name was reported for index [") + std::to_string(n_active_uniform) + "].");

                goto end;
            }
        }
    }

    result = true;
end:
    return result;
}