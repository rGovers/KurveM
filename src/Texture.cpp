#include "Texture.h"

#include <glad/glad.h>

Texture::Texture(int a_width, int a_height, int a_pixelFormat, int a_internalPixelFormat) :
    Texture(a_width, a_height, a_pixelFormat, a_internalPixelFormat, nullptr)
{
    
}
Texture::Texture(int a_width, int a_height, int a_pixelFormat, int a_internalPixelFormat, unsigned char* a_data)
{
    m_width = a_width;
    m_height = a_height;

    m_pixelFormat = a_pixelFormat;
    m_internalPixelFormat = a_internalPixelFormat;

    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalPixelFormat, m_width, m_height, 0, m_pixelFormat, GL_UNSIGNED_BYTE, a_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
Texture::~Texture()
{
    glDeleteTextures(1, &m_handle);
}

void Texture::Resize(int a_width, int a_height)
{
    m_width = a_width;
    m_height = a_height;

    glBindTexture(GL_TEXTURE_2D, m_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalPixelFormat, m_width, m_height, 0, m_pixelFormat, GL_UNSIGNED_BYTE, 0);
}