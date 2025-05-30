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
        D32_SFLOAT           = GL_DEPTH_COMPONENT32F,
        R10G10B10_A2         = GL_RGB10_A2,
        R11G11B10_SFLOAT     = GL_R11F_G11F_B10F,
        R16_SFLOAT           = GL_R16F,
        R16_SINT             = GL_R16I,
        R16_UINT             = GL_R16UI,
        R16G16_SFLOAT        = GL_RG16F,
        R16G16_SINT          = GL_RG16I,
        R16G16_UINT          = GL_RG16UI,
        R16G16B16_SFLOAT     = GL_RGB16F,
        R16G16B16_SINT       = GL_RGB16I,
        R16G16B16_UINT       = GL_RGB16UI,
        R16G16B16A16_SFLOAT  = GL_RGBA16F,
        R16G16B16A16_SINT    = GL_RGBA16I,
        R16G16B16A16_UINT    = GL_RGBA16UI,
        R32_SFLOAT           = GL_R32F,
        R32_SINT             = GL_R32I,
        R32_UINT             = GL_R32UI,
        R32G32_SFLOAT        = GL_RG32F,
        R32G32_SINT          = GL_RG32I,
        R32G32_UINT          = GL_RG32UI,
        R32G32B32_SFLOAT     = GL_RGB32F,
        R32G32B32_SINT       = GL_RGB32I,
        R32G32B32_UINT       = GL_RGB32UI,
        R32G32B32A32_SFLOAT  = GL_RGBA32F,
        R32G32B32A32_SINT    = GL_RGBA32I,
        R32G32B32A32_UINT    = GL_RGBA32UI,
        R4G4B4A4_UNORM       = GL_RGBA4,
        R5G5B5_A1            = GL_RGB5_A1,
        R5G6B5_UNORM         = GL_RGB565,
        R8_SINT              = GL_R8I,
        R8_UINT              = GL_R8UI,
        R8_SNORM             = GL_R8_SNORM,
        R8_UNORM             = GL_R8,
        R8G8_SINT            = GL_RG8I,
        R8G8_SNORM           = GL_RG8_SNORM,
        R8G8_UINT            = GL_RG8UI,
        R8G8_UNORM           = GL_RG8,
        R8G8B8_SINT          = GL_RGB8I,
        R8G8B8_SNORM         = GL_RGB8_SNORM,
        R8G8B8_UINT          = GL_RGB8UI,
        R8G8B8_UNORM         = GL_RGB8,
        R8G8B8A8_SINT        = GL_RGBA8I,
        R8G8B8A8_SNORM       = GL_RGBA8_SNORM,
        R8G8B8A8_UINT        = GL_RGBA8UI,
        R8G8B8A8_UNORM       = GL_RGBA8,
        R9G9B9E5_SFLOAT      = GL_RGB9_E5,
        SR8G8B8_UNORM        = GL_SRGB8,
        SR8G8B8_ALPHA8_UNORM = GL_SRGB8_ALPHA8,

        // GL_EXT_texture_norm16
        //R16_SNORM            = GL_R16_SNORM_EXT,
        R16_SNORM            = 0x8F98,
        //R16_UNORM            = GL_R16_EXT,
        R16_UNORM            = 0x822A,
        //R16G16_SNORM         = GL_RG16_SNORM_EXT,
        R16G16_SNORM         = 0x8F99,
        //R16G16_UNORM         = GL_RG16_EXT,
        R16G16_UNORM         = 0x822C,
        //R16G16B16_SNORM      = GL_RGB16_SNORM_EXT,
        R16G16B16_SNORM      = 0x8F9A,
        //R16G16B16_UNORM      = GL_RGB16_EXT,
        R16G16B16_UNORM      = 0x8054,
        //R16G16B16A16_SNORM   = GL_RGBA16_SNORM_EXT,
        R16G16B16A16_SNORM   = 0x8F9B,
        //R16G16B16A16_UNORM   = GL_RGBA16_EXT,
        R16G16B16A16_UNORM   = 0x805B,

        // WEBGL_compressed_texture_3tc_srgb
        //BC1_RGB_SRGB   = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
        BC1_RGB_SRGB   = 0x8C4C,
        //BC1_RGBA_SRGB  = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
        BC1_RGBA_SRGB  = 0x8C4D,
        //BC1_RGBA_UNORM = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
        BC1_RGBA_UNORM = 0x83F1,

        UNKNOWN
    };

    enum class TextureType : uint8_t
    {
        _2D,
        _3D,
        CUBE,

        UNKNOWN
    };

    class Texture
    {
    public:
        /* Public functions */
        static TextureUniquePtr create_immutable_2d  (const bool&                    in_single_mip,
                                                      const TextureFormat&           in_format,
                                                      const std::array<uint32_t, 2>& in_extents,
                                                      const uint32_t&                in_n_layers,
                                                      const uint32_t*                in_opt_n_mips_ptr = nullptr);
        static TextureUniquePtr create_immutable_3d  (const bool&                    in_single_mip,
                                                      const TextureFormat&           in_format,
                                                      const std::array<uint32_t, 3>& in_extents,
                                                      const uint32_t*                in_opt_n_mips_ptr = nullptr);
        static TextureUniquePtr create_immutable_cube(const bool&                    in_single_mip,
                                                      const TextureFormat&           in_format,
                                                      const uint32_t&                in_extents,
                                                      const uint32_t*                in_opt_n_mips_ptr = nullptr);

        ~Texture();

        GLuint                  get_id      ()                         const;
        TextureType             get_type    ()                         const;
        std::array<uint32_t, 3> get_mip_size(const uint32_t& in_n_mip) const;

    private:

        /* Private functions */
        Texture(const TextureType&             in_type,
                const std::array<uint32_t, 3>& in_extents,
                const TextureFormat&           in_format,
                const uint32_t*                in_opt_n_mips_ptr);

        bool init(const bool& in_mipped);

        /* Private variables */
        const std::array<uint32_t, 3> m_extents;
        const TextureFormat           m_format;
        const TextureType             m_type;

        GLuint                                m_id;
        std::vector<std::array<uint32_t, 3> > m_mip_size_vec;
        uint32_t                              m_n_mips;
    };
}
#endif /* TEXTURE_H */