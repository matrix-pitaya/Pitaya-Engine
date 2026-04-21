#include<Editor/GUI/Panel/InspectorPanel.h>
#include<Editor/Editor.h>
#include<Game/Component/Tag.h>
#include<Game/Component/Transform.h>
#include<Game/Component/MeshRenderer.h>
#include<Game/Component/MaterialOverride.h>
#include<Game/Component/Camera.h>
#include<Game/Component/RigidBody.h>
#include<Game/Component/Script.h>

#include<entt/entt.hpp>
#include<entt/meta/factory.hpp> 
#include<entt/meta/meta.hpp>
#include<glm.hpp>
#include<type_traits> 

namespace
{
    // 组件信息
    struct ComponentItem
    {
        const char* Name = nullptr;
        void(*AddFunc)(entt::entity) = nullptr;
        bool(*HasFunc)(entt::entity) = nullptr;
    };
    template<typename T>
    void AddComponentAction(entt::entity e)
    {
        if (auto* scene = Pitaya::Game::GetActiveScene())
        {
            if (!scene->HasComponent<T>(e))
            {
                scene->AddComponent<T>(e);
            }
        }
    }
	template<typename T>
    bool HasComponentAction(entt::entity e)
    {
		auto* scene = Pitaya::Game::GetActiveScene();
		return scene && scene->HasComponent<T>(e);
    }
    inline constexpr const auto AvailableComponents = std::to_array<ComponentItem>({    // 可添加的组件列表
            { "Mesh Renderer",      AddComponentAction<Pitaya::Game::MeshRenderer>,         HasComponentAction<Pitaya::Game::MeshRenderer> },
            { "Material Override",  AddComponentAction<Pitaya::Game::MaterialOverride>,     HasComponentAction<Pitaya::Game::MaterialOverride> },
            { "Camera",             AddComponentAction<Pitaya::Game::Camera>,               HasComponentAction<Pitaya::Game::Camera> },
            { "Rigidbody",          AddComponentAction<Pitaya::Game::RigidBody>,            HasComponentAction<Pitaya::Game::RigidBody> },
            { "Script",             AddComponentAction<Pitaya::Game::Script>,               HasComponentAction<Pitaya::Game::Script> }});


    template<typename Component, typename DrawFunc>
    inline void DrawComponentUI(entt::entity e, const char* name, DrawFunc drawfunc)
    {
        if (auto* scene = Pitaya::Game::GetActiveScene())
        {
            if (auto* component = scene->GetComponent<Component>(e))
            {
				// Tag 和 Transform 作为基础组件不可移除
                constexpr bool isRemovable = !std::is_same_v<Component, Pitaya::Game::Tag> &&
                    !std::is_same_v<Component, Pitaya::Game::Transform>;

                bool removeComponent = false;
                bool isOpen = ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen);

                if constexpr (isRemovable)
                {
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Remove Component")) { removeComponent = true; }
                        ImGui::EndPopup();
                    }
                }

                if (isOpen && !removeComponent)
                {
                    ImGui::Indent(16.0f);
                    drawfunc(component);
                    ImGui::Unindent(16.0f);
                }

                if constexpr (isRemovable)
                {
                    if (removeComponent) { scene->RemoveComponent<Component>(e); }
                }
            }
        }
    }
    inline void DrawTagUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::Tag>(e, "Tag", 
            [](Pitaya::Game::Tag* tag)
            {
                constexpr const float LABEL_WIDTH = 70.0f;
                constexpr const float RIGHT_PADDING = 8.0f;

                {   // Name
                    char buffer[Pitaya::Game::Tag::MAX_STR_SIZE] = {};
                    Pitaya::Core::CopyStringToCharArray(tag->GetName(), buffer);

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Name");
                    ImGui::SameLine(LABEL_WIDTH);

                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::InputText("##Name_TagComponent", buffer, Pitaya::Game::Tag::MAX_STR_SIZE))
                    {
                        tag->SetName(buffer);
                    }
                    ImGui::PopItemWidth();
                }

                {   // Tag
                    char buffer[Pitaya::Game::Tag::MAX_STR_SIZE] = {};
                    Pitaya::Core::CopyStringToCharArray(tag->GetTag(), buffer);

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Tag");
                    ImGui::SameLine(LABEL_WIDTH);

                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::InputText("##Tag_TagComponent", buffer, Pitaya::Game::Tag::MAX_STR_SIZE))
                    {
                        tag->SetTag(buffer);
                    }
                    ImGui::PopItemWidth();
                }
            });
    }
    inline void DrawTransformUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::Transform>(e, "Transform", 
            [](Pitaya::Game::Transform* transform)
            {
                constexpr const float LABEL_WIDTH = 70.0f;
                float availWidth = ImGui::GetContentRegionAvail().x;
                float fieldWidth = (availWidth - LABEL_WIDTH - 8.0f) / 3.0f;

                {   // Position 
                    glm::vec3 pos = transform->GetLocalPosition();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Position");
                    ImGui::SameLine(LABEL_WIDTH);

                    ImGui::PushItemWidth(fieldWidth);
                    if (ImGui::DragFloat("##PosX", &pos.x, 0.1f)) { transform->SetLocalPosition(pos); }
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##PosY", &pos.y, 0.1f)) { transform->SetLocalPosition(pos); }
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##PosZ", &pos.z, 0.1f)) { transform->SetLocalPosition(pos); }
                    ImGui::PopItemWidth();
                }

                {   // Rotation
                    glm::vec3 rotDeg = glm::degrees(transform->GetLocalEulerAngles());

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Rotation");
                    ImGui::SameLine(LABEL_WIDTH);

                    ImGui::PushItemWidth(fieldWidth);

                    bool isChanged = false;
                    if (ImGui::DragFloat("##RotX", &rotDeg.x, 0.1f)) { isChanged = true; }
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##RotY", &rotDeg.y, 0.1f)) { isChanged = true; }
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##RotZ", &rotDeg.z, 0.1f)) { isChanged = true; }
                    if (isChanged) { transform->SetLocalEulerAngles(glm::radians(rotDeg)); }

                    ImGui::PopItemWidth();
                }

                {   // Scale
                    glm::vec3 scale = transform->GetLocalScale();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Scale");
                    ImGui::SameLine(LABEL_WIDTH);

                    ImGui::PushItemWidth(fieldWidth);
                    if (ImGui::DragFloat("##ScaleX", &scale.x, 0.1f, 0.001f, 1000.0f)) { transform->SetLocalScale(scale); }
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##ScaleY", &scale.y, 0.1f, 0.001f, 1000.0f)) { transform->SetLocalScale(scale); }
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##ScaleZ", &scale.z, 0.1f, 0.001f, 1000.0f)) { transform->SetLocalScale(scale); }
                    ImGui::PopItemWidth();
                }
            });
    }
    inline void DrawMeshRendererUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::MeshRenderer>(e, "MeshRenderer",
            [](Pitaya::Game::MeshRenderer* meshRenderer) 
            {
                constexpr const float LABEL_WIDTH = 90.0f;
                constexpr const float RIGHT_PADDING = 8.0f;

                {   // Mesh切换
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Mesh");
                    ImGui::SameLine(LABEL_WIDTH);

                    // 无内存分配获取显示的 Mesh 名字
                    char meshLabel[256] = {};
                    const auto& currentMesh = meshRenderer->GetMesh();
                    if (currentMesh)
                    {
                        // TODO: 替换为实际 Mesh 获取名字的接口，如 currentMesh->GetName().c_str()
                        std::snprintf(meshLabel, sizeof(meshLabel), "%s (Mesh)", "TODO 获取MeshName");
                    }
                    else
                    {
                        std::snprintf(meshLabel, sizeof(meshLabel), "None (Mesh)");
                    }

                    // 画出暗色槽位按钮
                    float frameWidth = ImGui::GetContentRegionAvail().x - RIGHT_PADDING;
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                    ImGui::Button(meshLabel, ImVec2(frameWidth, 0));

                    ImGui::PopStyleColor(3);

                    // 设置拖拽接收目标 (Drop Target)
                    if (ImGui::BeginDragDropTarget())
                    {
                        // 注：这里的 CONTENT_BROWSER_ITEM 必须和 ProjectPanel 发送时使用的标号完全一致
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                        {
                            // TODO 
                            // 数据到达！处理数据转化
                            // 假设你在 ProjectPanel 发送的是 GUID
                            // Pitaya::Core::GUID droppedGuid = *(const Pitaya::Core::GUID*)payload->Data;

                            // 你在 MeshRenderer.h 里定义了 LoadMesh(GUID)，可以直接调用
                            // meshRenderer->LoadMesh(droppedGuid); 
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::Spacing();
                }

                {   // LayerMask
                    uint32_t rawMask = static_cast<uint32_t>(meshRenderer->GetLayerMask());
                    const char* previewText = (rawMask == 0) ? "Custom" :
                        ((rawMask != 0) && ((rawMask & (rawMask - 1)) == 0) ?
                            Pitaya::Render::ToString(static_cast<Pitaya::Render::RenderLayer>(rawMask)).data() :
                            "Mixed...");

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Layer Mask");
                    ImGui::SameLine(LABEL_WIDTH);

                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::BeginCombo("##LayerMaskCombo", previewText))
                    {
                        uint32_t oldMask = rawMask;

                        // 选项 A：快捷设置 - 全不选
                        bool isEmpty = (rawMask == 0);
                        if (ImGui::Checkbox("Empty", &isEmpty) && isEmpty) { rawMask = 0; }

                        ImGui::Separator();

                        // 选项 B：使用 CheckboxFlags
                        ImGui::CheckboxFlags("Default", &rawMask, (uint32_t)Pitaya::Render::RenderLayer::Default);
                        ImGui::CheckboxFlags("Transparent", &rawMask, (uint32_t)Pitaya::Render::RenderLayer::Transparent);
                        ImGui::CheckboxFlags("UI", &rawMask, (uint32_t)Pitaya::Render::RenderLayer::UI);
                        ImGui::CheckboxFlags("Editor Only", &rawMask, (uint32_t)Pitaya::Render::RenderLayer::EditorOnly);

                        if (oldMask != rawMask) { meshRenderer->SetLayerMask(static_cast<Pitaya::Render::RenderLayer>(rawMask)); }

                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();
                }
            });
	}
    inline void DrawMaterialOverrideUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::MaterialOverride>(e, "Material Override",
            [](Pitaya::Game::MaterialOverride* materialOverride)
            {
                constexpr float LABEL_WIDTH = 90.0f;

                auto& materials = materialOverride->GetOverrideMaterials();
                int indexToRemove = -1;
                for (size_t i = 0; i < materials.size(); ++i)
                {
                    ImGui::PushID(static_cast<int>(i));

                    // 排序逻辑 将整行作为一个拖拽源和目标
                    ImGui::BeginGroup();

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(ICON_FA_GRIP_LINES);
                    ImGui::SameLine();

                    // 排序 DND 逻辑
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) 
                    {
                        ImGui::SetDragDropPayload("MATERIAL_MOVE_PROJ", &i, sizeof(size_t));
                        ImGui::Text("Moving Element %zu", i);
                        ImGui::EndDragDropSource();
                    }
                    if (ImGui::BeginDragDropTarget()) 
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL_MOVE_PROJ")) 
                        {
                            size_t sourceIdx = *static_cast<const size_t*>(payload->Data);
                            if (sourceIdx != i) { std::swap(materials[sourceIdx], materials[i]); }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    // 槽位渲染
                    ImGui::Text("Element %zu", i);
                    ImGui::SameLine(LABEL_WIDTH);

                    float deleteBtnWidth = 30.0f;
                    float slotWidth = ImGui::GetContentRegionAvail().x - deleteBtnWidth - 5.0f;

                    const char* matName = materials[i] ? "Material Asset" : "None (Material)";

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                    ImGui::Button(matName, ImVec2(slotWidth, 0));
                    ImGui::PopStyleColor();

                    // 接收外部资源拖拽
                    if (ImGui::BeginDragDropTarget()) 
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) 
                        {
							// TODO
                            // const char* path = (const char*)payload->Data;
                            // materials[i] = AssetManager::Load<Material>(path);
                        }
                        ImGui::EndDragDropTarget();
                    }

                    // 删除按钮
                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_TRASH_CAN, ImVec2(deleteBtnWidth, 0))) { indexToRemove = (int)i; }

                    ImGui::EndGroup();
                    ImGui::PopID();
                }

                if (indexToRemove != -1) { materials.erase(materials.begin() + indexToRemove); }

                // 增加按钮
                if (ImGui::Button(ICON_FA_PLUS " Add Slot", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { materials.emplace_back(); }
            });
    }
    inline void DrawCameraUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::Camera>(e, "Camera",
            [](Pitaya::Game::Camera* camera)
            {
                constexpr float LABEL_WIDTH = 90.0f;
                constexpr float RIGHT_PADDING = 8.0f;

                {   //Camera States
                    auto state = camera->GetCameraState();
                    bool changed = false;

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Projection");
                    ImGui::SameLine(LABEL_WIDTH);
                    const char* projTypes[] = { "Perspective", "Orthographic" };
                    int currentType = static_cast<int>(state.Type);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::Combo("##Proj", &currentType, projTypes, 2)) 
                    {
                        state.Type = static_cast<Pitaya::Core::CameraType>(currentType);
                        changed = true;
                    }

                    if (state.Type == Pitaya::Core::CameraType::Perspective) 
                    {
                        ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("FOV"); ImGui::SameLine(LABEL_WIDTH);
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                        if (ImGui::DragFloat("##FOV", &state.FovDegrees, 0.5f, 1.0f, 179.0f)) { changed = true; }
                    }
                    else 
                    {
                        ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Size"); ImGui::SameLine(LABEL_WIDTH);
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                        if (ImGui::DragFloat("##Size", &state.OrthoSize, 0.1f, 0.01f, 1000.0f)) { changed = true; }
                    }

                    if (changed) { camera->SetCameraState(state); }
                    ImGui::Spacing();
                }

                {   // Render Target
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Target");
                    ImGui::SameLine(LABEL_WIDTH);

                    const char* rtName = camera->GetRenderTargetIsReady() ? "Custom RenderTarget" : "Screen Backbuffer";
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
                    ImGui::Button(rtName, ImVec2(ImGui::GetContentRegionAvail().x - RIGHT_PADDING, 0));
                    ImGui::PopStyleColor();

                    if (ImGui::BeginDragDropTarget()) 
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) 
                        {
                            // Pitaya::Core::GUID guid = ParseGUID((const char*)payload->Data);
                            // camera->SetRenderTarget(guid);
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::Spacing();
                }

                {   // Post Process Setting
                    ImGui::Separator();
                    ImGui::TextUnformatted("Post Process");

                    // 获取后处理设置引用
                    auto& setting = camera->GetPostProcessSetting();
                    int stepToRemove = -1;
                    for (uint8_t i = 0; i < setting.StepCount; ++i)
                    {
                        auto& step = setting.Steps[i];
                        ImGui::PushID(i);

                        // 排序交互层
                        ImGuiTreeNodeFlags flags = 
                            ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowOverlap | 
                            ImGuiTreeNodeFlags_DefaultOpen;
                        bool open = ImGui::TreeNodeEx("##PPNode", flags);

                        // 排序 DND 逻辑
                        if (ImGui::BeginDragDropSource()) 
                        {
                            ImGui::SetDragDropPayload("PP_STEP_MOVE", &i, sizeof(uint8_t));
                            ImGui::Text("Moving %s", Pitaya::Render::ToString(step.Type).data());
                            ImGui::EndDragDropSource();
                        }
                        if (ImGui::BeginDragDropTarget()) 
                        {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PP_STEP_MOVE")) 
                            {
                                uint8_t sourceIdx = *static_cast<const uint8_t*>(payload->Data);
                                if (sourceIdx != i) { setting.SwapSteps(sourceIdx, i); }
                            }
                            ImGui::EndDragDropTarget();
                        }

                        // 标题显示与删除按钮
                        ImGui::SameLine();
                        ImGui::Text("%s  %s", ICON_FA_BARS, Pitaya::Render::ToString(step.Type).data());

                        float trashBtnPos = ImGui::GetContentRegionMax().x - 30.0f;
                        ImGui::SameLine(trashBtnPos);
                        if (ImGui::Button(ICON_FA_TRASH_CAN)) { stepToRemove = i; }

                        if (open)
                        {
                            // 具体参数调节面板
                            if (step.Type == Pitaya::Render::PostProcessType::Bloom) 
                            {
                                Pitaya::Render::BloomParams p; 
                                std::memcpy(&p, step.ShaderParams, sizeof(p));
                                bool c = false;
                                c |= ImGui::DragFloat("Threshold", &p.Threshold, 0.01f, 0.0f, 5.0f);
                                c |= ImGui::DragFloat("Intensity", &p.Intensity, 0.01f, 0.0f, 10.0f);
                                if (c) { step.SetParams(p); }
                            }
                            else if (step.Type == Pitaya::Render::PostProcessType::ToneMapping) 
                            {
                                Pitaya::Render::ToneMappingParams p; 
                                std::memcpy(&p, step.ShaderParams, sizeof(p));
                                if (ImGui::DragFloat("Exposure", &p.Exposure, 0.01f, 0.0f, 10.0f)) { step.SetParams(p); }
                            }
                            else if (step.Type == Pitaya::Render::PostProcessType::GammaCorrection) 
                            {
                                Pitaya::Render::GammaCorrectionParams p; 
                                std::memcpy(&p, step.ShaderParams, sizeof(p));
                                if (ImGui::DragFloat("Gamma", &p.Gamma, 0.01f, 0.1f, 4.0f)) { step.SetParams(p); }
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                        ImGui::Spacing();
                    }

                    if (stepToRemove != -1) { setting.RemoveStep((uint8_t)stepToRemove); }

                    // 添加按钮
                    if (setting.StepCount < Pitaya::Render::PostProcessSetting::MAX_STEPS)
                    {
                        if (ImGui::Button(ICON_FA_PLUS " Add Post Effect", ImVec2(ImGui::GetContentRegionAvail().x - RIGHT_PADDING, 0))) { ImGui::OpenPopup("AddPPPopup"); }

                        if (ImGui::BeginPopup("AddPPPopup")) 
                        {
                            if (ImGui::MenuItem("Bloom")) { Pitaya::Render::PostProcessStep s; s.SetParams(Pitaya::Render::BloomParams{}); setting.AddStep(s); }
                            if (ImGui::MenuItem("ToneMapping")) { Pitaya::Render::PostProcessStep s; s.SetParams(Pitaya::Render::ToneMappingParams{}); setting.AddStep(s); }
                            if (ImGui::MenuItem("GammaCorrection")) { Pitaya::Render::PostProcessStep s; s.SetParams(Pitaya::Render::GammaCorrectionParams{}); setting.AddStep(s); }
                            ImGui::EndPopup();
                        }
                    }
                }
            });
    }
    inline void DrawRigidbodyUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::RigidBody>(e, "RigidBody",
            [](Pitaya::Game::RigidBody* rigidBody)
            {
                constexpr const float LABEL_WIDTH = 90.0f;
                constexpr const float RIGHT_PADDING = 8.0f;

                {   // RigidBody Type
                    // 获取当前刚体类型
                    Pitaya::Physics::RigidBodyType currentType = rigidBody->GetType();

                    // 将当前枚举转换为界面显示的名称
                    const char* previewText = Pitaya::Physics::ToString(currentType).data();

                    // 渲染左侧文字标签
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Body Type");
                    ImGui::SameLine(LABEL_WIDTH);

                    // 渲染对应的下拉菜单框
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::BeginCombo("##BodyTypeCombo", previewText))
                    {
                        // 选项 A：Static
                        bool isStatic = (currentType == Pitaya::Physics::RigidBodyType::Static);
                        if (ImGui::Selectable("Static", isStatic)) { rigidBody->SetType(Pitaya::Physics::RigidBodyType::Static); }
                        if (isStatic) { ImGui::SetItemDefaultFocus(); } // 打开面板时，视角默认聚焦在当前选项上

                        // 选项 B：Kinematic
                        bool isKinematic = (currentType == Pitaya::Physics::RigidBodyType::Kinematic);
                        if (ImGui::Selectable("Kinematic", isKinematic)) { rigidBody->SetType(Pitaya::Physics::RigidBodyType::Kinematic); }
                        if (isKinematic) { ImGui::SetItemDefaultFocus(); }

                        // 选项 C：Dynamic
                        bool isDynamic = (currentType == Pitaya::Physics::RigidBodyType::Dynamic);
                        if (ImGui::Selectable("Dynamic", isDynamic)) { rigidBody->SetType(Pitaya::Physics::RigidBodyType::Dynamic); }
                        if (isDynamic) { ImGui::SetItemDefaultFocus(); }

                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();
                }
            });
    }
    inline void DrawScriptUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::Script>(e, "Script",
            [](Pitaya::Game::Script* script)
            {
                int scriptIndex = 0; 
                for (const auto& scriptInstance : script->GetScriptInstance())
                {
                    ImGui::PushID(scriptIndex);
                    
                    ImGuiTreeNodeFlags flags =
                        ImGuiTreeNodeFlags_Framed |
                        ImGuiTreeNodeFlags_DefaultOpen;

                    if (ImGui::TreeNodeEx("##ScriptInstanceNode", flags, "%s", scriptInstance.ClassName.data()))
                    {
                        // TODO: 未来在这里实现类似 Unity 的反射与序列化变量渲染
                        ImGui::TextDisabled("Serialization not implemented yet...");    // 暂时放置一行提示文字
                        ImGui::TreePop();
                    }

                    ImGui::Spacing();

                    ImGui::PopID();
                    scriptIndex++;
                }
            });
    }
    inline constexpr const auto ComponentDrawFuncs = std::to_array<void(*)(entt::entity)>(   // 绘制组件函数表
        { DrawTagUI, DrawTransformUI, DrawMeshRendererUI, DrawMaterialOverrideUI, DrawCameraUI, DrawRigidbodyUI, DrawScriptUI });
}

void Pitaya::Editor::InspectorPanel::OnImGuiRender()
{
    switch (Pitaya::Editor::Editor::Instance().GetGUI().GetContext().Selection.Type)
    {
        case Pitaya::Editor::GUI::Context::Selection::Type::Entity:  DrawEntity(); break;
        case Pitaya::Editor::GUI::Context::Selection::Type::File:    DrawFile();   break;
    }
}
void Pitaya::Editor::InspectorPanel::DrawEntity()
{
    entt::entity entity = Pitaya::Editor::Editor::Instance().GetGUI().GetContext().Selection.SelectedEntity;
    if (entity == entt::null) { return; }

    // 绘制组件
    for (auto ComponentDrawFunc : ComponentDrawFuncs) { ComponentDrawFunc(entity); }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Add Component", ImVec2(-1, 0)))
    {
        // 在打开弹窗前 计算并设置弹窗位置 按钮正下方 左对齐
        ImVec2 buttonMin = ImGui::GetItemRectMin();
        ImVec2 buttonSize = ImGui::GetItemRectSize();
        ImGui::SetNextWindowPos(ImVec2(buttonMin.x, buttonMin.y + buttonSize.y));
        ImGui::SetNextWindowSize(ImVec2(buttonSize.x, 0)); // 宽度和按钮完全一致 高度自动

        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        const constexpr uint32_t MAX_SEARCH_SIZE = 256;

        // 顶部搜索框
        static char InputBuffer[MAX_SEARCH_SIZE] = {};
        ImGui::PushItemWidth(-1);
        ImGui::InputTextWithHint("##Search", "Search...", InputBuffer, sizeof(InputBuffer));
        ImGui::PopItemWidth();
        ImGui::Spacing();

        // 遍历并过滤
        char inputLower[MAX_SEARCH_SIZE] = {};
        Pitaya::Core::CopyStringToCharArray(InputBuffer, inputLower);
        Pitaya::Core::ToLower(inputLower);

        ImGui::BeginChild("ComponentList", ImVec2(0, 150), true);
        for (auto& item : AvailableComponents)
        {
            char nameLower[MAX_SEARCH_SIZE] = {};
            Pitaya::Core::CopyStringToCharArray(item.Name, nameLower);
            Pitaya::Core::ToLower(nameLower);

            if (inputLower[0] == '\0' || std::strstr(nameLower, inputLower) != nullptr)
            {
                bool hasComp = item.HasFunc(entity);
				if (hasComp) { ImGui::BeginDisabled(); }    // 避免重复添加
                if (ImGui::Selectable(item.Name))
                {
                    // 添加组件
                    item.AddFunc(entity);

                    // 清空输入框
                    InputBuffer[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                if (hasComp) { ImGui::EndDisabled(); }
            }
        }
        ImGui::EndChild();

        ImGui::EndPopup();
    }
}
void Pitaya::Editor::InspectorPanel::DrawFile()
{
    //TODO 显示文件信息
}
