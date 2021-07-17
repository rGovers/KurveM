#include "Editors/WeightPaintingEditor.h"

#include "Object.h"

WeightPaintingEditor::WeightPaintingEditor(Editor* a_editor, Workspace* a_workspace)
{
    m_editor = a_editor;

    m_workspace = a_workspace;
}
WeightPaintingEditor::~WeightPaintingEditor()
{

}

e_EditorMode WeightPaintingEditor::GetEditorMode()
{
    return EditorMode_WeightPainting;
}

void WeightPaintingEditor::DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize)
{
    a_object->Draw(a_camera, a_winSize);
}

void WeightPaintingEditor::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize)
{

}
void WeightPaintingEditor::LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize)
{

}

void WeightPaintingEditor::Update(Camera* a_camera, const glm::vec2& a_screenSize, double a_delta)
{
    
}