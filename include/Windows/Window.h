#pragma once

class Window
{
private:

protected:

public:
    Window();
    virtual ~Window();

    virtual void Update(double a_delta) = 0;
};