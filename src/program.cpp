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

bool Framework::Program::init()
{
    bool result = false;

    m_id = glCreateProgram();

    if (m_id == 0)
    {
        report_error("glCreateProgram() returned an ID of 0.");

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
            report_error("Program failed to link.");

            goto end;
        }
    }

    result = true;
end:
    return result;
}