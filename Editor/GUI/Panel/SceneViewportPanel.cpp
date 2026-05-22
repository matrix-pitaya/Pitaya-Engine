#include<Editor/GUI/Panel/SceneViewportPanel.h>
#include<Editor/GUI/IconFontCppHeaders/IconsFontAwesome6.h>
#include<Editor/Editor.h>
#include<Game/Common/FuncTable.h>
#include<Game/Component/Disabled.h>
#include<Game/Component/Light.h>
#include<Core/Utils/Math.h>
#include<gtc/epsilon.hpp>

void Pitaya::Editor::SceneViewportPanel::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
		// Gizmos 菜单
        if (ImGui::BeginMenu(ICON_FA_SHAPES " Gizmos"))
        {
            auto& gizmoState = Pitaya::Editor::Editor::Instance().GetGUI().GetContext().GizmoState;
            ImGui::MenuItem(ICON_FA_VIDEO "  Cameras", nullptr, &gizmoState.ShowCameraGizmo);
            ImGui::MenuItem(ICON_FA_LIGHTBULB "  Lights", nullptr, &gizmoState.ShowLightGizmo);
            ImGui::MenuItem(ICON_FA_BORDER_ALL "  Grid", nullptr, &gizmoState.ShowGrid);
            ImGui::MenuItem(ICON_FA_CUBES "  Colliders", nullptr, &gizmoState.ShowColliders);
            ImGui::EndMenu();
        }

        // 视口菜单
        if (ImGui::BeginMenu(ICON_FA_CAMERA " View"))
        {
            if (ImGui::MenuItem("Perspective")) 
            { 
                //TODO  切换透视/正交的代码 
            }
            if (ImGui::MenuItem("Top", "Num 7")) 
            { 
                
            }
            if (ImGui::MenuItem("Front", "Num 1")) 
            { 
                
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}
void Pitaya::Editor::SceneViewportPanel::DrawGizmos()
{
	const auto& context = Pitaya::Editor::Editor::Instance().GetGUI().GetContext();
    const auto& toolState = context.ToolState;
	const auto& gizmoState = context.GizmoState;
	const auto& selection = context.Selection;

    auto& editorCamera = Pitaya::Editor::Editor::Instance().GetCamera();
    const auto& editorCameraView = editorCamera.GetCameraSnapshot().View;
    const auto& editorCameraProjection = editorCamera.GetCameraSnapshot().Projection;
    const auto& editorCameraViewProj = editorCamera.GetCameraSnapshot().ViewProjection;     // 拿到编辑器相机的投影视图矩阵 (ViewProjection)

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
    const constexpr float ViewManipulateSize = 80.0f; // 大小
    ImVec2 viewManipulatePos = ImVec2(
        viewportBoundsMax.x - ViewManipulateSize,
        viewportBoundsMin.y);
    glm::mat4 dummyViewMatrix = editorCameraView;
    ImGuizmo::ViewManipulate(
        glm::value_ptr(dummyViewMatrix),
        10.0f,
        viewManipulatePos,
        ImVec2(ViewManipulateSize, ViewManipulateSize),
        0x00000000U);
    if (glm::any(glm::epsilonNotEqual(dummyViewMatrix[0], editorCameraView[0], 0.001f)) ||
        glm::any(glm::epsilonNotEqual(dummyViewMatrix[1], editorCameraView[1], 0.001f)) ||
        glm::any(glm::epsilonNotEqual(dummyViewMatrix[2], editorCameraView[2], 0.001f)) ||
        glm::any(glm::epsilonNotEqual(dummyViewMatrix[3], editorCameraView[3], 0.001f)))
    { editorCamera.ApplyViewMatrix(dummyViewMatrix); }  

    if (auto* scene = Pitaya::Game::GetActiveScene())
    {
        // 绘制Billboard公告板
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImDrawList* drawList = window->DrawList;
        if (gizmoState.ShowCameraGizmo)
        {
            for (auto [entity, camera, transform] : scene->ECS.GetView<Pitaya::Game::Camera, Pitaya::Game::Transform>(entt::exclude<Pitaya::Game::Disabled>).each()) // 遍历当前场景中所有的「场景相机」(CameraComponent)
            {
                glm::vec3 worldPos = transform.GetWorldPosition();  //获取这个场景相机的 3D 世界坐标
                glm::vec4 clipSpacePos = editorCameraViewProj * glm::vec4(worldPos, 1.0f);    //将 3D 坐标转换到剪裁空间 (Clip Space)
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

                // 当前选中Entity为该camera (TODO 通过引擎渲染管线渲染)
                if (selection.SelectedEntity == entity)
                {
                    const auto frustum = camera.GetCameraState().BuildSnapshot(transform.GetWorldPosition(), transform.GetWorldForward(), transform.GetWorldUp()).CreateFrustum();
                    const auto& pLeft = frustum.Planes[0];
                    const auto& pRight = frustum.Planes[1];
                    const auto& pBottom = frustum.Planes[2];
                    const auto& pTop = frustum.Planes[3];
                    const auto& pNear = frustum.Planes[4];
                    const auto& pFar = frustum.Planes[5];

                    glm::vec3 worldCorners[8] = {
                        //计算近截面 4 个角 (顺序：左下 0, 右下 1, 右上 2, 左上 3)
                        Pitaya::Core::CalculatePlaneIntersection(pNear, pBottom, pLeft),
                        Pitaya::Core::CalculatePlaneIntersection(pNear, pBottom, pRight),
                        Pitaya::Core::CalculatePlaneIntersection(pNear, pTop, pRight),
                        Pitaya::Core::CalculatePlaneIntersection(pNear, pTop, pLeft),

                        //计算远截面 4 个角 (顺序：左下 4, 右下 5, 右上 6, 左上 7)
                        Pitaya::Core::CalculatePlaneIntersection(pFar, pBottom, pLeft),
                        Pitaya::Core::CalculatePlaneIntersection(pFar, pBottom, pRight),
                        Pitaya::Core::CalculatePlaneIntersection(pFar, pTop, pRight),
                        Pitaya::Core::CalculatePlaneIntersection(pFar, pTop, pLeft) };
                   
                    // 将 8 个世界顶点投影到编辑器的 2D 屏幕或视口空间
                    glm::mat4 editorVP = editorCameraViewProj;
                    ImVec2 screenPoints[8];
                    bool isPointValid[8]; // 用于标记顶点是否在编辑器相机的前面，避免 Z-Clip 反转导致线条乱飞
                    for (uint32_t i = 0; i < 8; i++)
                    {
                        glm::vec4 clipPos = editorVP * glm::vec4(worldCorners[i], 1.0f);
                        if (clipPos.w <= 0.1f) { isPointValid[i] = false;  continue; }  // Z截断剔除 如果 w <= 0.1f，说明这个点跑到摄像机背后去了

                        isPointValid[i] = true;
                        glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w; // 透视除法
                        float screenX = (ndc.x + 1.0f) * 0.5f;  // 将 NDC (-1 到 1) 映射到 0 到 1 (Y轴反转)
                        float screenY = (1.0f - ndc.y) * 0.5f;
                        screenPoints[i] = ImVec2(   // 映射到最终 ImGui 相关的像素坐标
                            viewportPos.x + screenX * viewportSize.x,
                            viewportPos.y + screenY * viewportSize.y);
                    }

                    // 使用 ImGui 绘制细线视锥体
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    constexpr const ImU32 ColorDef = IM_COL32(255, 255, 255, 180);
                    constexpr const float Thickness = 1.0f; // 线条极细，凸显专业感
                    auto ImGuiDrawLine = [&](const glm::vec3& worldA, const glm::vec3& worldB)
                        {
                            glm::vec4 clipA = editorCameraViewProj * glm::vec4(worldA, 1.0f);
                            glm::vec4 clipB = editorCameraViewProj * glm::vec4(worldB, 1.0f);
                            if ([](glm::vec4& clipA, glm::vec4& clipB) -> bool
                                {
                                    constexpr const float W_MIN = 0.1f; // 锁定近截面安全距离 防止除以 0 导致溢出
                                    // 如果两个点都在摄像机背后 这条线段连画都不用画 直接跳过
                                    if (clipA.w < W_MIN && clipB.w < W_MIN) { return false; }
                                    // 如果有一个点在正常视野 另一个点在摄像机后方
                                    if (clipA.w < W_MIN || clipB.w < W_MIN)
                                    {
                                        // 算出线段刚好穿过 W_MIN (截面) 时的插值比例 t
                                        float t = (W_MIN - clipA.w) / (clipB.w - clipA.w);
                                        glm::vec4 intersect = clipA + t * (clipB - clipA); // 计算交点
                                        // 将由于处于摄像机后方而导致畸变的点，替换为安全交点
                                        if (clipA.w < W_MIN) { clipA = intersect; }
                                        else { clipB = intersect; }
                                    }
                                    return true;
                                }(clipA, clipB))
                            {
                                glm::vec3 ndcA = glm::vec3(clipA) / clipA.w;
                                glm::vec3 ndcB = glm::vec3(clipB) / clipB.w;

                                // 映射到 ImGui 的二维空间
                                ImVec2 screenA(viewportPos.x + (ndcA.x + 1.0f) * 0.5f * viewportSize.x,
                                    viewportPos.y + (1.0f - ndcA.y) * 0.5f * viewportSize.y);
                                ImVec2 screenB(viewportPos.x + (ndcB.x + 1.0f) * 0.5f * viewportSize.x,
                                    viewportPos.y + (1.0f - ndcB.y) * 0.5f * viewportSize.y);

                                drawList->AddLine(screenA, screenB, ColorDef, Thickness);
                            }
                        };

                    // 画近截面 (0-1-2-3)
                    ImGuiDrawLine(worldCorners[0], worldCorners[1]); ImGuiDrawLine(worldCorners[1], worldCorners[2]);
                    ImGuiDrawLine(worldCorners[2], worldCorners[3]); ImGuiDrawLine(worldCorners[3], worldCorners[0]);

                    // 画远截面 (4-5-6-7)
                    ImGuiDrawLine(worldCorners[4], worldCorners[5]); ImGuiDrawLine(worldCorners[5], worldCorners[6]);
                    ImGuiDrawLine(worldCorners[6], worldCorners[7]); ImGuiDrawLine(worldCorners[7], worldCorners[4]);

                    // 画四大透视连接棱
                    ImGuiDrawLine(worldCorners[0], worldCorners[4]); ImGuiDrawLine(worldCorners[1], worldCorners[5]);
                    ImGuiDrawLine(worldCorners[2], worldCorners[6]); ImGuiDrawLine(worldCorners[3], worldCorners[7]);
                }
            }
        }

        if (gizmoState.ShowLightGizmo)
        {
            auto DrawGizmoLine = [&](const glm::vec3& worldA, const glm::vec3& worldB, ImU32 color, float thickness = 1.0f)
                {
                    glm::vec4 clipA = editorCameraViewProj * glm::vec4(worldA, 1.0f);
                    glm::vec4 clipB = editorCameraViewProj * glm::vec4(worldB, 1.0f);
                    constexpr const float W_MIN = 0.1f;
                    if (clipA.w < W_MIN && clipB.w < W_MIN) { return; }
                    if (clipA.w < W_MIN || clipB.w < W_MIN)
                    {
                        float t = (W_MIN - clipA.w) / (clipB.w - clipA.w);
                        glm::vec4 intersect = clipA + t * (clipB - clipA);
                        if (clipA.w < W_MIN) { clipA = intersect; }
                        else { clipB = intersect; }
                    }
                    glm::vec3 ndcA = glm::vec3(clipA) / clipA.w;
                    glm::vec3 ndcB = glm::vec3(clipB) / clipB.w;
                    ImVec2 screenA(viewportBoundsMin.x + (ndcA.x + 1.0f) * 0.5f * viewportWidth,
                        viewportBoundsMin.y + (1.0f - ndcA.y) * 0.5f * viewportHeight);
                    ImVec2 screenB(viewportBoundsMin.x + (ndcB.x + 1.0f) * 0.5f * viewportWidth,
                        viewportBoundsMin.y + (1.0f - ndcB.y) * 0.5f * viewportHeight);
                    drawList->AddLine(screenA, screenB, color, thickness);
                };
            auto DrawGizmoCircle = [&](const glm::vec3& center, const glm::vec3& right, const glm::vec3& up, float radius, ImU32 color, int segments = 32)
                {
                    float angleStep = glm::two_pi<float>() / segments;
                    glm::vec3 prevPoint = center + right * radius;
                    for (int i = 1; i <= segments; ++i)
                    {
                        float angle = i * angleStep;
                        glm::vec3 nextPoint = center + right * (radius * cos(angle)) + up * (radius * sin(angle));
                        DrawGizmoLine(prevPoint, nextPoint, color);
                        prevPoint = nextPoint;
                    }
                };

            for (auto [entity, light, transform] : scene->ECS.GetView<Pitaya::Game::Light, Pitaya::Game::Transform>(entt::exclude<Pitaya::Game::Disabled>).each())
            {
                glm::vec3 worldPos = transform.GetWorldPosition();

                glm::vec4 clipSpacePos = editorCameraViewProj * glm::vec4(worldPos, 1.0f);
                if (clipSpacePos.w > 0.2f)
                {
                    glm::vec3 ndcSpacePos = glm::vec3(clipSpacePos) / clipSpacePos.w;
                    ImVec2 screenPos = ImVec2(
                        viewportBoundsMin.x + (ndcSpacePos.x + 1.0f) * 0.5f * viewportWidth,
                        viewportBoundsMin.y + (1.0f - ndcSpacePos.y) * 0.5f * viewportHeight);

                    ImU32 iconColor = IM_COL32(255, 235, 120, 210); // 暖偏黄色
                    const char* icon = ICON_FA_LIGHTBULB;
                    ImFont* font = ImGui::GetFont();
                    float iconSize = 32.0f;
                    ImVec2 textSize = font->CalcTextSizeA(iconSize, FLT_MAX, 0.0f, icon);
                    screenPos.x -= textSize.x * 0.5f;
                    screenPos.y -= textSize.y * 0.5f;
                    drawList->AddText(font, iconSize, screenPos, iconColor, icon);
                }

                // 如果且被选中 画出三维线框阵列
                if (selection.SelectedEntity == entity)
                {
                    // 获取空间朝向
                    glm::vec3 fwd = transform.GetWorldForward();
                    glm::vec3 up = transform.GetWorldUp();
                    glm::vec3 right = transform.GetWorldRight();

                    // 读取你在Inspector面板调好的真实颜色作为线条颜色
                    glm::vec3 lColor = light.GetColor();
                    ImU32 lineColor = IM_COL32(
                        glm::clamp(static_cast<int>(lColor.x * 255), 0, 255),
                        glm::clamp(static_cast<int>(lColor.y * 255), 0, 255),
                        glm::clamp(static_cast<int>(lColor.z * 255), 0, 255),
                        200); // 200 用于保留一定透明度以免喧宾夺主

                    auto type = light.GetType();
                    if (type == Pitaya::Game::LightType::Directional) // 方向光 Directional
                    {
                        float r = 1.0f;
                        DrawGizmoCircle(worldPos, right, up, r, lineColor);
                        DrawGizmoLine(worldPos, worldPos + fwd * 4.0f, lineColor, 2.0f); // 突出的主干粗线
                        DrawGizmoLine(worldPos + right * r, worldPos + right * r + fwd * 4.0f, lineColor);  // 边缘平行的那四根阳光射线
                        DrawGizmoLine(worldPos - right * r, worldPos - right * r + fwd * 4.0f, lineColor);
                        DrawGizmoLine(worldPos + up * r, worldPos + up * r + fwd * 4.0f, lineColor);
                        DrawGizmoLine(worldPos - up * r, worldPos - up * r + fwd * 4.0f, lineColor);
                    }
                    else if (type == Pitaya::Game::LightType::Point) // 点光源 Point
                    {
                        float r = light.GetRadius();
                        DrawGizmoCircle(worldPos, right, up, r, lineColor);  // XY 切面
                        DrawGizmoCircle(worldPos, fwd, up, r, lineColor);    // ZY 切面
                        DrawGizmoCircle(worldPos, right, fwd, r, lineColor); // XZ 切面
                    }
                    else if (type == Pitaya::Game::LightType::Spot) // 聚光灯 Spot
                    {
                        float r = light.GetRadius();
                        float outerAngle = glm::radians(light.GetOuterAngle());
                        float coneRadius = r * glm::tan(outerAngle * 0.5f); // 计算远端光斑圆锥底面的切面与圆心
                        glm::vec3 centerFar = worldPos + fwd * r;
                        DrawGizmoCircle(centerFar, right, up, coneRadius, lineColor);   // 画最远端大圆
                        DrawGizmoLine(worldPos, centerFar + right * coneRadius, lineColor); // 塔尖圆心到远端四个角画边线
                        DrawGizmoLine(worldPos, centerFar - right * coneRadius, lineColor);
                        DrawGizmoLine(worldPos, centerFar + up * coneRadius, lineColor);
                        DrawGizmoLine(worldPos, centerFar - up * coneRadius, lineColor);
                        DrawGizmoLine(worldPos, centerFar, lineColor, 1.5f);    // 顺带一根实心轴
                    }
                }
            }
        }

        // 绘制EntityGizmos
        TransformTool currentTool = toolState.ActiveTool;
        if (selection.SelectedEntity != entt::null && currentTool != TransformTool::Select)
        {
            if (auto* selectedTransform = scene->ECS.GetComponent<Pitaya::Game::Transform>(selection.SelectedEntity))
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
                    glm::value_ptr(editorCameraView),
                    glm::value_ptr(editorCameraProjection),
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
                    toolState.IsLocal ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD,
                    glm::value_ptr(transformMatrix), // 交互结果写入回这个 transformMatrix
                    nullptr,
                    snap ? snapValues : nullptr);

                // 将处理完的新矩阵写回到物体的 Transform 中
                if (ImGuizmo::IsUsing())
                {
                    glm::mat4 newWorldMatrix = transformMatrix;
                    glm::mat4 newLocalMatrix = newWorldMatrix; // 默认等于世界矩阵 (假设它是根节点)

                    // 如果实体有父节点，我们需要将这个新的世界矩阵转换回相对父节点的局部矩阵
                    if (scene->ECS.HasComponent<Pitaya::Game::Parent>(selection.SelectedEntity))
                    {
                        if (auto* parent = scene->ECS.GetComponent<Pitaya::Game::Parent>(selection.SelectedEntity))
                        {
                            if (parent->GetId() != entt::null)
                            {
                                if (auto* parentTransform = scene->ECS.GetComponent<Pitaya::Game::Transform>(parent->GetId()))
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