#pragma once

class LocalModel;

class TransformVisualizer
{
private:
    static TransformVisualizer* Instance;

    LocalModel* m_arm;

    LocalModel* m_scaleHandle;
    LocalModel* m_translationHandle;

    LocalModel* m_rotationHandle;

    TransformVisualizer();
protected:

public:
    ~TransformVisualizer();
    
    static void Init();
    static void Destroy();

    inline static LocalModel* GetArm() 
    {
        return Instance->m_arm;
    }

    inline static LocalModel* GetTranslationHandle() 
    {
        return Instance->m_translationHandle;
    }
    inline static LocalModel* GetScaleHandle() 
    {
        return Instance->m_scaleHandle;
    }
    inline static LocalModel* GetRotationHandle()
    {
        return Instance->m_rotationHandle;
    }
};