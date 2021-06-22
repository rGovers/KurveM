#pragma once

class Texture
{
private:
    unsigned int m_handle;

    int          m_width;
    int          m_height;
         
    int          m_pixelFormat;
    int          m_internalPixelFormat;
protected:

public:
    Texture(int a_width, int a_height, int a_pixelFormat, int a_internalPixelFormat);
    ~Texture();

    inline int GetHandle() const
    {
        return m_handle;
    }

    inline int GetWidth() const
    {
        return m_width;
    }
    inline int GetHeight() const
    {
        return m_height;
    }

    void Resize(int a_width, int a_height);
};