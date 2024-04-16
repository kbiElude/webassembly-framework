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
#include "sampler.h"

Framework::Sampler::Sampler(const WrapMode&           in_wrap_s,
                            const WrapMode&           in_wrap_t,
                            const WrapMode&           in_wrap_r,
                            const MinFilter&          in_min_filter,
                            const MagFilter&          in_mag_filter,
                            const float&              in_min_lod,
                            const float&              in_max_lod,
                            const TextureCompareFunc& in_compare_func)
    :m_compare_func(in_compare_func),
     m_id          (0),
     m_mag_filter  (in_mag_filter),
     m_max_lod     (in_max_lod),
     m_min_lod     (in_min_lod),
     m_min_filter  (in_min_filter),
     m_wrap_r      (in_wrap_r),
     m_wrap_s      (in_wrap_s),
     m_wrap_t      (in_wrap_t)
{
    /* Stub */
}

Framework::Sampler::~Sampler()
{
    if (m_id != 0)
    {
        glDeleteSamplers(1,
                        &m_id);

        m_id = 0;
    }
}

Framework::SamplerUniquePtr Framework::Sampler::create(const WrapMode&           in_wrap_s,
                                                       const WrapMode&           in_wrap_t,
                                                       const WrapMode&           in_wrap_r,
                                                       const MinFilter&          in_min_filter,
                                                       const MagFilter&          in_mag_filter,
                                                       const float&              in_min_lod,
                                                       const float&              in_max_lod,
                                                       const TextureCompareFunc& in_compare_func)
{
    Framework::SamplerUniquePtr result_ptr;

    result_ptr.reset(
        new Sampler(in_wrap_s,
                    in_wrap_t,
                    in_wrap_r,
                    in_min_filter,
                    in_mag_filter,
                    in_min_lod,
                    in_max_lod,
                    in_compare_func)
    );

    if (result_ptr != nullptr)
    {
        if (!result_ptr->init() )
        {
            result_ptr.reset();
        }
    }

    return result_ptr;
}

GLint Framework::Sampler::get_gl_int_for_mag_filter(const MagFilter& in_mag_filter) const
{
    GLint result = 0;

    switch (in_mag_filter)
    {
        case MagFilter::LINEAR:  result = GL_LINEAR;  break;
        case MagFilter::NEAREST: result = GL_NEAREST; break;

        default:
        {
            Framework::report_error("Unsupported MagFilter value specified for get_gl_int_for_mag_filter().");
        }
    }

    return result;
}

GLint Framework::Sampler::get_gl_int_for_min_filter(const MinFilter& in_min_filter) const
{
    GLint result = 0;

    switch (in_min_filter)
    {
        case MinFilter::LINEAR:                 result = GL_LINEAR;                 break;
        case MinFilter::LINEAR_MIPMAP_LINEAR:   result = GL_LINEAR_MIPMAP_LINEAR;   break;
        case MinFilter::LINEAR_MIPMAP_NEAREST:  result = GL_LINEAR_MIPMAP_NEAREST;  break;
        case MinFilter::NEAREST:                result = GL_NEAREST;                break;
        case MinFilter::NEAREST_MIPMAP_LINEAR:  result = GL_NEAREST_MIPMAP_LINEAR;  break;
        case MinFilter::NEAREST_MIPMAP_NEAREST: result = GL_NEAREST_MIPMAP_NEAREST; break;

        default:
        {
            Framework::report_error("Unsupported MinFilter value specified for get_gl_int_for_min_filter().");
        }
    }

    return result;
}

GLint Framework::Sampler::get_gl_int_for_texture_compare_func(const TextureCompareFunc& in_func) const
{
    GLint result = 0;

    switch (in_func)
    {
        case TextureCompareFunc::ALWAYS:   result = GL_ALWAYS;   break;
        case TextureCompareFunc::EQUAL:    result = GL_EQUAL;    break;
        case TextureCompareFunc::GEQUAL:   result = GL_GEQUAL;   break;
        case TextureCompareFunc::GREATER:  result = GL_GREATER;  break;
        case TextureCompareFunc::LEQUAL:   result = GL_LEQUAL;   break;
        case TextureCompareFunc::LESS:     result = GL_LESS;     break;
        case TextureCompareFunc::NEVER:    result = GL_NEVER;    break;
        case TextureCompareFunc::NOTEQUAL: result = GL_NOTEQUAL; break;

        default:
        {
            Framework::report_error("Unsupported TextureCompareFunc value specified for get_gl_int_for_texture_compare_func().");
        }
    }

    return result;
}

GLint Framework::Sampler::get_gl_int_for_wrap_mode(const WrapMode& in_wrap_mode) const
{
    GLint result = 0;

    switch (in_wrap_mode)
    {
        case WrapMode::CLAMP_TO_EDGE:   result = GL_CLAMP_TO_EDGE;   break;
        case WrapMode::MIRRORED_REPEAT: result = GL_MIRRORED_REPEAT; break;
        case WrapMode::REPEAT:          result = GL_REPEAT;          break;

        default:
        {
            Framework::report_error("Unsupported MagFilter value specified for get_gl_int_for_mag_filter().");
        }
    }

    return result;
}

GLuint Framework::Sampler::get_id()
{
    return m_id;
}

bool Framework::Sampler::init()
{
    bool result = false;

    glGenSamplers(1,
                 &m_id);

    if (m_id == 0)
    {
        Framework::report_error("Could not generate a sampler ID.");

        goto end;
    }

    {
        const auto mag_filter_gl = get_gl_int_for_mag_filter(m_mag_filter);
        const auto min_filter_gl = get_gl_int_for_min_filter(m_min_filter);
        const auto wrap_r_gl     = get_gl_int_for_wrap_mode(m_wrap_r);
        const auto wrap_s_gl     = get_gl_int_for_wrap_mode(m_wrap_s);
        const auto wrap_t_gl     = get_gl_int_for_wrap_mode(m_wrap_t);

        glSamplerParameteri(m_id, GL_TEXTURE_MIN_FILTER, min_filter_gl);
        glSamplerParameteri(m_id, GL_TEXTURE_MAG_FILTER, mag_filter_gl);
        glSamplerParameterf(m_id, GL_TEXTURE_MIN_LOD,    m_min_lod);
        glSamplerParameterf(m_id, GL_TEXTURE_MAX_LOD,    m_max_lod);
        glSamplerParameteri(m_id, GL_TEXTURE_WRAP_R,     wrap_r_gl);
        glSamplerParameteri(m_id, GL_TEXTURE_WRAP_S,     wrap_t_gl);
        glSamplerParameteri(m_id, GL_TEXTURE_WRAP_T,     wrap_t_gl);
    }

    if (m_compare_func != TextureCompareFunc::DISABLED)
    {
        const auto texture_compare_func_gl = get_gl_int_for_texture_compare_func(m_compare_func);

        glSamplerParameteri(m_id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(m_id, GL_TEXTURE_COMPARE_FUNC, texture_compare_func_gl);
    }
    else
    {
        glSamplerParameteri(m_id, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    }

    result = true;
end:
    return result;
}