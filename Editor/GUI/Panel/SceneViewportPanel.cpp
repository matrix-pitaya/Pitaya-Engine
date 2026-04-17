#include<Editor/GUI/Panel/SceneViewportPanel.h>
#include<Editor/GUI/IconFontCppHeaders/IconsFontAwesome6.h>
#include<Editor/Editor.h>
#include<Game/Common/FuncTable.h>
#include<gtc/epsilon.hpp>

void Pitaya::Editor::SceneViewportPanel::DrawGizmos()
{
	const auto& context = Pitaya::Editor::Editor::Instance().GetGUI().GetContext();
    const auto& state = context.State;
	const auto& selection = context.Selection;

    auto& editorCamera = Pitaya::Editor::Editor::Instance().GetCamera();
    const auto& cameraView = editorCamera.GetCameraSnapshot().View;
    const auto& cameraProjection = editorCamera.GetCameraSnapshot().Projection;

    ImVec2 viewportBoundsMin = ImGui::GetWindowContentRegionMin();
    viewportBoundsMin.x += ImGui::GetWindowPos().x;
    viewportBoundsMin.y += ImGui::GetWindowPos().y;
    ImVec2 viewportBoundsMax = ImGui::GetWindowContentRegionMax();
    viewportBoundsMax.x += ImGui::GetWindowPos().x;
    viewportBoundsMax.y += ImGui::GetWindowPos().y;
    float viewportWidth = viewportBoundsMax.x - viewportBoundsMin.x;
    float viewportHeight = viewportBoundsMax.y - viewportBoundsMin.y;
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(viewportBoundsMin.x, viewportBoundsMin.y, viewportWidth, viewportHeight);
    ImGuizmo::SetOrthographic(editorCamera.GetCameraState().Type == Pitaya::Core::CameraType::Orthographic);

    // 绘制右上角挂件的尺寸
    const constexpr float viewManipulateSize = 80.0f; // 大小
    ImVec2 viewManipulatePos = ImVec2(
        viewportBoundsMax.x - viewManipulateSize,
        viewportBoundsMin.y);
    glm::mat4 dummyViewMatrix = cameraView;
    ImGuizmo::ViewManipulate(
        glm::value_ptr(dummyViewMatrix),
        10.0f,
        viewManipulatePos,
        ImVec2(viewManipulateSize, viewManipulateSize),
        0x00000000U);
    if (glm::any(glm::epsilonNotEqual(dummyViewMatrix[0], cameraView[0], 0.001f)) ||
        glm::any(glm::epsilonNotEqual(dummyViewMatrix[1], cameraView[1], 0.001f)) ||
        glm::any(glm::epsilonNotEqual(dummyViewMatrix[2], cameraView[2], 0.001f)) ||
        glm::any(glm::epsilonNotEqual(dummyViewMatrix[3], cameraView[3], 0.001f)))
    { editorCamera.ApplyViewMatrix(dummyViewMatrix); }  

    if (auto* scene = Pitaya::Game::GetActiveScene())
    {
        // 绘制Billboard公告板
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImDrawList* drawList = window->DrawList;
        glm::mat4 editorViewProj = editorCamera.GetCameraSnapshot().ViewProjection;     // 拿到编辑器相机的投影视图矩阵 (ViewProjection)
        for (auto [entity, transform, camera] : scene->GetView<Pitaya::Game::Transform, Pitaya::Game::Camera>().each()) // 遍历当前场景中所有的「场景相机」(CameraComponent)
        {
            glm::vec3 worldPos = transform.GetWorldPosition();  //获取这个场景相机的 3D 世界坐标
            glm::vec4 clipSpacePos = editorViewProj * glm::vec4(worldPos, 1.0f);    //将 3D 坐标转换到剪裁空间 (Clip Space)
            if (clipSpacePos.w <= 0.2f) { continue; }  // 检查物体是否在摄像机后面 如果在后面就不画
            glm::vec3 ndcSpacePos = glm::vec3(clipSpacePos) / clipSpacePos.w;   //透视除法 转换到标准化设备坐标 (NDC) [-1, 1]
            float viewportWidth = viewportBoundsMax.x - viewportBoundsMin.x;    // 将 NDC 映射到 Viewport 窗口的二维像素坐标
            float viewportHeight = viewportBoundsMax.y - viewportBoundsMin.y;
            ImVec2 screenPos = ImVec2(
                viewportBoundsMin.x + (ndcSpacePos.x + 1.0f) * 0.5f * viewportWidth,
                viewportBoundsMin.y + (1.0f - ndcSpacePos.y) * 0.5f * viewportHeight); // UI的Y轴是朝下的，所以 1.0 - y
            ImU32 iconColor = IM_COL32(255, 255, 255, 200);
            const char* icon = ICON_FA_VIDEO;
            ImFont* font = ImGui::GetFont(); // 获取当前字体
            float iconSize = 32.0f;          // 图标大小
            ImVec2 textSize = font->CalcTextSizeA(iconSize, FLT_MAX, 0.0f, icon);
            screenPos.x -= textSize.x * 0.5f;
            screenPos.y -= textSize.y * 0.5f;
            drawList->AddText(font, iconSize, screenPos, iconColor, icon);
        }

        // 绘制EntityGizmos
        TransformTool currentTool = state.ActiveTool;
        if (selection.SelectedEntity != entt::null && currentTool != TransformTool::Select)
        {
            if (auto* selectedTransform = scene->GetComponent<Pitaya::Game::Transform>(selection.SelectedEntity))
            {
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
                    [](TransformTool tool) -> ImGuizmo::OPERATION
                        {
                            switch (tool)
                            {
                                case TransformTool::Select:     return static_cast<ImGuizmo::OPERATION>(-1);
                                case TransformTool::Translate:  return ImGuizmo::OPERATION::TRANSLATE;
                                case TransformTool::Rotate:     return ImGuizmo::OPERATION::ROTATE;
                                case TransformTool::Scale:      return ImGuizmo::OPERATION::SCALE;
                            }
                        }(currentTool),
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
    }
}

/*
    // 点击选中物体
    ImVec2 mousePos = ImGui::GetMousePos();     // 获取鼠标在屏幕上的绝对坐标
    ImVec2 windowPos = ImGui::GetWindowPos();   // 计算出你的 Viewport 画面在屏幕上的实际包围盒
    ImVec2 viewportMin = ImVec2(windowPos.x + ImGui::GetWindowContentRegionMin().x,
        windowPos.y + ImGui::GetWindowContentRegionMin().y);
    ImVec2 viewportMax = ImVec2(windowPos.x + ImGui::GetWindowContentRegionMax().x,
        windowPos.y + ImGui::GetWindowContentRegionMax().y);
    ImVec2 relativeMousePos(mousePos.x - viewportMin.x, mousePos.y - viewportMin.y);    //将全局鼠标坐标转换为相对于 Viewport 画面的相对坐标 [0, width], [0, height]
    bool isLeftClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);  // 判定是否满足点选条件
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
    */