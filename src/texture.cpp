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
#include "texture.h"
#include <algorithm>

Framework::Texture::Texture(const TextureType&             in_type,
                            const std::array<uint32_t, 3>& in_extents,
                            const TextureFormat&           in_format)
    :m_extents(in_extents),
     m_format (in_format),
     m_id     (0),
     m_type   (in_type)
{
    /* Stub */
}

Framework::Texture::~Texture()
{
    if (m_id != 0)
    {
        glDeleteTextures(1,
                        &m_id);

        m_id = 0;
    }
}

Framework::TextureUniquePtr Framework::Texture::create_immutable_2d(const bool&                    in_single_mip,
                                                                    const TextureFormat&           in_format,
                                                                    const std::array<uint32_t, 2>& in_extents,
                                                                    const uint32_t&                in_n_layers)
{
    TextureUniquePtr result_ptr(
        new Texture(TextureType::_2D,
                    {in_extents.at(0), in_extents.at(1), 1},
                    in_format)
    );

    if (!result_ptr->init(!in_single_mip) )
    {
        Framework::report_error("Texture initialization failed.");

        result_ptr.reset();
    }

    return result_ptr;
}

Framework::TextureUniquePtr Framework::Texture::create_immutable_3d(const bool&                    in_single_mip,
                                                                    const TextureFormat&           in_format,
                                                                    const std::array<uint32_t, 3>& in_extents)
{
    TextureUniquePtr result_ptr(
        new Texture(TextureType::_3D,
                    in_extents,
                    in_format)
    );

    if (!result_ptr->init(!in_single_mip) )
    {
        Framework::report_error("Texture initialization failed.");

        result_ptr.reset();
    }

    return result_ptr;
}

Framework::TextureUniquePtr Framework::Texture::create_immutable_cube(const bool&          in_single_mip,
                                                                      const TextureFormat& in_format,
                                                                      const uint32_t&      in_extents)
{
    TextureUniquePtr result_ptr(
        new Texture(TextureType::CUBE,
                    {in_extents, in_extents, 6},
                    in_format)
    );

    if (!result_ptr->init(!in_single_mip) )
    {
        Framework::report_error("Texture initialization failed.");

        result_ptr.reset();
    }

    return result_ptr;
}

GLuint Framework::Texture::get_id() const
{
    if (m_id == 0)
    {
        Framework::report_error("Texture ID is 0.");
    }

    return m_id;
}

std::array<uint32_t, 3> Framework::Texture::get_mip_size(const uint32_t& in_n_mip) const
{
    std::array<uint32_t, 3> result{};

    if (m_mip_size_vec.size() > in_n_mip)
    {
        result = m_mip_size_vec.at(in_n_mip);
    }
    else
    {
        Framework::report_error("Invalid mip index specified for Framework::Texture::get_mip_size()");
    }

    return result;
}

bool Framework::Texture::init(const bool& in_mipped)
{
    uint32_t n_mips            = 0;
    bool     result            = false;
    GLenum   texture_target_gl = GL_NONE;

    /* Allocate an ID */
    glGenTextures(1,
                 &m_id);

    if (m_id == 0)
    {
        Framework::report_error("glGenTextures() returned a zeroed out ID.");

        goto end;
    }

    /* Determine how many mips we're going to need. */
    m_mip_size_vec.push_back(m_extents);

    while (m_mip_size_vec.back() != std::array<uint32_t, 3>{1, 1, 1})
    {
        std::array<uint32_t, 3> new_mip_size{};

        new_mip_size =
        {
            std::max(1u, m_mip_size_vec.back().at(0) / 2),
            std::max(1u, m_mip_size_vec.back().at(1) / 2),
            std::max(1u, m_mip_size_vec.back().at(2) / 2),
        };

        m_mip_size_vec.emplace_back(new_mip_size);
    }

    n_mips = static_cast<uint32_t>(m_mip_size_vec.size() );

    /* Set up immutable storage */
    switch (m_type)
    {
        case TextureType::_2D:
        {
            if (m_extents.at(2) == 1)
            {
                glBindTexture(GL_TEXTURE_2D,
                              m_id);

                glTexStorage2D(GL_TEXTURE_2D,
                               n_mips,
                               static_cast<GLenum>(m_format),
                               m_extents.at(0),
                               m_extents.at(1) );

                texture_target_gl = GL_TEXTURE_2D;
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D_ARRAY,
                              m_id);

                glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                               n_mips,
                               static_cast<GLenum>(m_format),
                               m_extents.at(0),
                               m_extents.at(1),
                               m_extents.at(2) );

                texture_target_gl = GL_TEXTURE_2D_ARRAY;
            }

            break;
        }

        case TextureType::_3D:
        {
            glBindTexture(GL_TEXTURE_3D,
                          m_id);

            glTexStorage3D(GL_TEXTURE_3D,
                           n_mips,
                           static_cast<GLenum>(m_format),
                           m_extents.at(0),
                           m_extents.at(1),
                           m_extents.at(2) );

            texture_target_gl = GL_TEXTURE_3D;
            break;
        }

        case TextureType::CUBE:
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP,
                          m_id);

            glTexStorage2D(GL_TEXTURE_CUBE_MAP,
                           n_mips,
                           static_cast<GLenum>(m_format),
                           m_extents.at(0),
                           m_extents.at(1) );

            texture_target_gl = GL_TEXTURE_CUBE_MAP;
            break;
        }

        default:
        {
            Framework::report_error("Invalid texture type requested.");

            goto end;
        }
    }

    glTexParameteri(texture_target_gl,
                    GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST);
    glTexParameteri(texture_target_gl,
                    GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST);

    result = true;
end:
    return result;
}

Framework::TextureType Framework::Texture::get_type() const
{
    return m_type;
}