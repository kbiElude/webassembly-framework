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
#if !defined(TEXTURE_H)
#define TEXTURE_H

#include "framework.h"
#include <array>

namespace Framework
{
    /* Forward decls */
    class Texture;

    /* Type defs */
    typedef std::unique_ptr<Texture> TextureUniquePtr;

    /* Enums */
    enum class TextureFormat
    {
        /* Core WebGL 2.0 formats */
        R10G1B10_A2          = GL_RGB10_A2,
        R11G11B10_SFLOAT     = GL_R11F_G11F_B10F,
        R16_SFLOAT           = GL_R16F,
        R16G16_SFLOAT        = GL_RG16F,
        R16G16B16_SFLOAT     = GL_RGB16F,
        R16G16B16A16_SFLOAT  = GL_RGBA16F,
        R32_SFLOAT           = GL_R32F,
        R32G32_SFLOAT        = GL_RG32F,
        R32G32B32_SFLOAT     = GL_RGB32F,
        R32G32B32A32_SFLOAT  = GL_RGBA32F,
        R4G4B4A4_UNORM       = GL_RGBA4,
        R5G5B5_A1            = GL_RGB5_A1,
        R5G6B5_UNORM         = GL_RGB565,
        R8_UINT              = GL_R8UI,
        R8_UNORM             = GL_R8,
        R8G8_UINT            = GL_RG8UI,
        R8G8_UNORM           = GL_RG8,
        R8G8B8_UINT          = GL_RGB8UI,
        R8G8B8_UNORM         = GL_RGB8,
        R8G8B8A8_UINT        = GL_RGBA8UI,
        R8G8B8A8_UNORM       = GL_RGBA8,
        R9G9B9E5_SFLOAT      = GL_RGB9_E5,
        SR8G8B8_UNORM        = GL_SRGB8,
        SR8G8B8_ALPHA8_UNORM = GL_SRGB8_ALPHA8,

        UNKNOWN
    };

    class Texture
    {
    public:
        /* Public functions */
        static TextureUniquePtr create_immutable_2d  (const bool&                    in_single_mip,
                                                      const TextureFormat&           in_format,
                                                      const std::array<uint32_t, 2>& in_extents,
                                                      const uint32_t&                in_n_layers);
        static TextureUniquePtr create_immutable_3d  (const bool&                    in_single_mip,
                                                      const TextureFormat&           in_format,
                                                      const std::array<uint32_t, 3>& in_extents);
        static TextureUniquePtr create_immutable_cube(const bool&                    in_single_mip,
                                                      const TextureFormat&           in_format,
                                                      const uint32_t&                in_extents);

        ~Texture();

        GLuint                  get_id      ();
        std::array<uint32_t, 3> get_mip_size(const uint32_t& in_n_mip) const;

    private:
        /* Private type defs */
        enum class TextureType : uint8_t
        {
            _2D,
            _3D,
            CUBE,

            UNKNOWN
        };

        /* Private functions */
        Texture(const TextureType&             in_type,
                const std::array<uint32_t, 3>& in_extents,
                const TextureFormat&           in_format);

        bool init(const bool& in_mipped);

        /* Private variables */
        const std::array<uint32_t, 3> m_extents;
        const TextureFormat           m_format;
        const TextureType             m_type;

        GLuint                                m_id;
        std::vector<std::array<uint32_t, 3> > m_mip_size_vec;
    };
}
#endif /* TEXTURE_H */