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
#if !defined(SAMPLER_H)
#define SAMPLER_H

#include "framework.h"
#include <array>

namespace Framework
{
    /* Forward decls */
    class Sampler;

    /* Type defs */
    typedef std::unique_ptr<Sampler> SamplerUniquePtr;

    /* Enums */
    enum class MagFilter
    {
        LINEAR  = GL_LINEAR,
        NEAREST = GL_NEAREST,

        UNKNOWN
    };

    enum class MinFilter
    {
        LINEAR  = GL_LINEAR,
        NEAREST = GL_NEAREST,

        // Linear texel filtering, linear mip filtering
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,

        // Linear texel filtering, nearest mip filtering
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,

        // Nearest texel filtering, linear mip filtering
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,

        // Nearest texel filtering, nearest mip filtering
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,

        UNKNOWN
    };

    enum class TextureCompareFunc
    {
        ALWAYS   = GL_ALWAYS,
        EQUAL    = GL_EQUAL,
        GEQUAL   = GL_GEQUAL,
        GREATER  = GL_GREATER,
        LEQUAL   = GL_LEQUAL,
        LESS     = GL_LESS,
        NEVER    = GL_NEVER,
        NOTEQUAL = GL_NOTEQUAL,

        DISABLED = UINT16_MAX
    };

    enum class WrapMode
    {
        CLAMP_TO_EDGE   = GL_CLAMP_TO_EDGE,
        MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
        REPEAT          = GL_REPEAT,

        UNKNOWN
    };

    class Sampler
    {
    public:
        /* Public functions */
        static SamplerUniquePtr create(const WrapMode&           in_wrap_s,
                                       const WrapMode&           in_wrap_t,
                                       const WrapMode&           in_wrap_r,
                                       const MinFilter&          in_min_filter,
                                       const MagFilter&          in_mag_filter,
                                       const float&              in_min_lod,
                                       const float&              in_max_lod,
                                       const TextureCompareFunc& in_compare_func = TextureCompareFunc::DISABLED);

        ~Sampler();

        GLuint get_id();

    private:
        /* Private type defs */

        /* Private functions */
        Sampler(const WrapMode&           in_wrap_s,
                const WrapMode&           in_wrap_t,
                const WrapMode&           in_wrap_r,
                const MinFilter&          in_min_filter,
                const MagFilter&          in_mag_filter,
                const float&              in_min_lod,
                const float&              in_max_lod,
                const TextureCompareFunc& in_compare_func);

        bool init();

        GLint get_gl_int_for_mag_filter          (const MagFilter&          in_mag_filter) const;
        GLint get_gl_int_for_min_filter          (const MinFilter&          in_min_filter) const;
        GLint get_gl_int_for_texture_compare_func(const TextureCompareFunc& in_func)       const;
        GLint get_gl_int_for_wrap_mode           (const WrapMode&           in_wrap_mode)  const;

        /* Private variables */
        GLuint m_id;

        const TextureCompareFunc m_compare_func;
        const MagFilter          m_mag_filter;
        const float              m_max_lod;
        const MinFilter          m_min_filter;
        const float              m_min_lod;
        const WrapMode           m_wrap_r;
        const WrapMode           m_wrap_s;
        const WrapMode           m_wrap_t;
    };
}
#endif /* SAMPLER_H */