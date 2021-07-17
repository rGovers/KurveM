#pragma once

class Camera;

class CameraController
{
private:
    Camera* m_camera;

    bool    m_orthoDown;

protected:

public:
    CameraController(Camera* a_camera);
    ~CameraController();

    void Update(double a_delta);
    void FocusUpdate();
};