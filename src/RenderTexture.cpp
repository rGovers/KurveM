#include "RenderTexture.h"

#include <glad/glad.h>

#include "Texture.h"

RenderTexture::RenderTexture(int a_width, int a_height)
{
    m_depthTexture = new Texture(a_width, a_height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
    m_texture = new Texture(a_width, a_height, GL_RGBA, GL_RGBA);

    glGenFramebuffers(1, &m_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetHandle(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->GetHandle(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
RenderTexture::~RenderTexture()
{
    delete m_depthTexture;
    delete m_texture;

    glDeleteFramebuffers(1, &m_handle);
}

int RenderTexture::GetWidth() const
{
    return m_depthTexture->GetWidth();
}
int RenderTexture::GetHeight() const
{
    return m_depthTexture->GetHeight();
}

void RenderTexture::Resize(int a_width, int a_height)
{
    m_texture->Resize(a_width, a_height);
    m_depthTexture->Resize(a_width, a_height);
}