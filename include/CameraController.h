#pragma once

class Camera;

class CameraController
{
private:
    const float Distance = 2.5f;

    Camera* m_camera;

    bool    m_orthoDown;

protected:

public:
    CameraController(Camera* a_camera);
    ~CameraController();

    void Update(double a_delta);
    void FocusUpdate();
};