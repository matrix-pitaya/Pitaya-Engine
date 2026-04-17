#include<Editor/GUI/Panel/SceneViewportPanel.h>
#include<Editor/Editor.h>
#include<Game/Common/FuncTable.h>

#include<Core/Utils/Console.h>

void Pitaya::Editor::SceneViewportPanel::DrawGizmos()
{
    auto ToImGuizmoOPERATION = [](TransformTool tool) -> ImGuizmo::OPERATION
        {
            switch (tool)
            {
                case TransformTool::Select:     return static_cast<ImGuizmo::OPERATION>(-1);
                case TransformTool::Translate:  return ImGuizmo::OPERATION::TRANSLATE;
                case TransformTool::Rotate:     return ImGuizmo::OPERATION::ROTATE;   
                case TransformTool::Scale:      return ImGuizmo::OPERATION::SCALE;    
            }
        };

	const auto& context = Pitaya::Editor::Editor::Instance().GetGUI().GetContext();
    const auto& state = context.State;
	const auto& selection = context.Selection;
    if (selection.SelectedEntity == entt::null) { return; }
	TransformTool currentTool = state.ActiveTool;
    if (currentTool == TransformTool::Select) { return; }

    if (auto* scene = Pitaya::Game::GetActiveScene())
    {
        if (auto* selectedTransform = scene->GetComponent<Pitaya::Game::Transform>(selection.SelectedEntity))
        {
            const Pitaya::Editor::Camera& editorCamera = Pitaya::Editor::Editor::Instance().GetCamera();

            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(viewportBoundsMin.x, viewportBoundsMin.y, viewportSize.x, viewportSize.y);

            // 获取 Editor 相机的 View 和 Projection 矩阵
            const glm::mat4& cameraView = editorCamera.GetCameraSnapshot().View;
            const glm::mat4& cameraProjection = editorCamera.GetCameraSnapshot().Projection;

            // 获取当前选中物体的世界矩阵
            glm::mat4 transformMatrix = selectedTransform->GetWorldMatrix();

            // 按住 Ctrl 进行网格吸附 (Snapping)
            bool snap = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
            float snapValue = 0.5f; // 移动吸附0.5m
            if (currentTool == TransformTool::Rotate) { snapValue = 45.0f; } // 旋转吸附45度
            float snapValues[3] = { snapValue, snapValue, snapValue };

            // 调用 ImGuizmo 的核心函数开始绘制和交互
            ImGuizmo::Manipulate(
                glm::value_ptr(cameraView),
                glm::value_ptr(cameraProjection),
                ToImGuizmoOPERATION(currentTool),
                state.IsLocal ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD,
                glm::value_ptr(transformMatrix), // 交互结果写入回这个 transformMatrix
                nullptr,
                snap ? snapValues : nullptr);
            
            // 将处理完的新矩阵写回到物体的 Transform 中
            if (ImGuizmo::IsUsing())
            {
                glm::mat4 newWorldMatrix = transformMatrix;
                glm::mat4 newLocalMatrix = newWorldMatrix; // 默认等于世界矩阵 (假设它是根节点)

                // 如果实体有父节点，我们需要将这个新的世界矩阵转换回相对父节点的局部矩阵
                if (scene->HasComponent<Pitaya::Game::Parent>(selection.SelectedEntity))
                {
                    if (auto* parent = scene->GetComponent<Pitaya::Game::Parent>(selection.SelectedEntity))
                    {
                        if (parent->GetId() != entt::null)
                        {
                            if (auto* parentTransform = scene->GetComponent<Pitaya::Game::Transform>(parent->GetId()))
                            {
                                glm::mat4 parentWorldMatrix = parentTransform->GetWorldMatrix();
                                newLocalMatrix = glm::inverse(parentWorldMatrix) * newWorldMatrix;  // 核心数学公式：局部矩阵 = 逆(父节点世界矩阵) * 新的世界矩阵
                            }
                        }
                    }
                }

                // 将计算好的局部矩阵分解为 Local Position, Rotation, Scale
                selectedTransform->SetLocalFromMatrix(newLocalMatrix);
                selectedTransform->MarkDirty(); // 标记脏数据
            }
        }        
    }

    // 获取鼠标在屏幕上的绝对坐标
    ImVec2 mousePos = ImGui::GetMousePos();

    // 计算出你的 Viewport 画面在屏幕上的实际包围盒
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 viewportMin = ImVec2(windowPos.x + ImGui::GetWindowContentRegionMin().x,
        windowPos.y + ImGui::GetWindowContentRegionMin().y);
    ImVec2 viewportMax = ImVec2(windowPos.x + ImGui::GetWindowContentRegionMax().x,
        windowPos.y + ImGui::GetWindowContentRegionMax().y);

    //将全局鼠标坐标转换为相对于 Viewport 画面的相对坐标 [0, width], [0, height]
    ImVec2 relativeMousePos(mousePos.x - viewportMin.x, mousePos.y - viewportMin.y);

    // 判定是否满足点选条件
    bool isLeftClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    bool isHoveringViewport = isHovered; // 从你父类继承的变量，代表鼠标在窗口内
    bool isGizmoHovered = ImGuizmo::IsOver();
    bool isCameraMoving = ImGui::IsKeyDown(ImGuiKey_LeftAlt); // 假设你的编辑器按住Alt转动相机

    if (isLeftClicked && isHoveringViewport && !isGizmoHovered && !isCameraMoving)
    {
        // 如果相对坐标超出了画面范围则不处理
        if (relativeMousePos.x >= 0 && relativeMousePos.y >= 0 &&
            relativeMousePos.x < (viewportMax.x - viewportMin.x) &&
            relativeMousePos.y < (viewportMax.y - viewportMin.y))
        {
            // ----------------------------------------------------
            // TODO 在这里执行：【获取点击到的 Entity】
            // entt::entity clickedEntity = GetEntityAtPixels(relativeMousePos.x, relativeMousePos.y);
            // this->SetSelectedEntity(clickedEntity);
            // ----------------------------------------------------
        }
    }
}
