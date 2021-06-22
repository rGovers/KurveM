#pragma once

class Texture;

class RenderTexture
{
private: 
    unsigned int m_handle;

    Texture*     m_texture;
    Texture*     m_depthTexture;

protected:

public:
    RenderTexture(int a_width, int a_height);
    ~RenderTexture();

    int GetWidth() const;
    int GetHeight() const;

    void Resize(int a_width, int a_height);

    inline int GetHandle() const
    {
        return m_handle;
    }

    inline Texture* GetTexture() const
    {
        return m_texture;
    }
    inline Texture* GetDepthTexture() const
    {
        return m_depthTexture;
    }
};