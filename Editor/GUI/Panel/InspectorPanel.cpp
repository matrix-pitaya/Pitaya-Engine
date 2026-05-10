#include<Editor/GUI/Panel/InspectorPanel.h>
#include<Editor/Editor.h>
#include<Game/Component/Metadata.h>
#include<Game/Component/Transform.h>
#include<Game/Component/MeshRenderer.h>
#include<Game/Component/MaterialOverride.h>
#include<Game/Component/Camera.h>
#include<Game/Component/RigidBody.h>
#include<Game/Component/Script.h>
#include<Game/Component/Light.h>

#include<entt/entt.hpp>
#include<glm.hpp>
#include<cmath>
#include<type_traits>
#include<gtc/type_ptr.hpp>

namespace
{
    struct ComponentItem
    {
        const char* Icon = nullptr;
        const char* Name = nullptr;
        void(*AddFunc)(entt::entity) = nullptr;
        bool(*HasFunc)(entt::entity) = nullptr;
    };
    template<typename T>
    void AddComponentAction(entt::entity e)
    {
        if (auto* scene = Pitaya::Game::GetActiveScene())
        {
            if (!scene->HasComponent<T>(e)) { scene->AddComponent<T>(e); }
        }
    }
    template<typename T>
    bool HasComponentAction(entt::entity e)
    {
        auto* scene = Pitaya::Game::GetActiveScene();
        return scene && scene->HasComponent<T>(e);
    }
    inline constexpr const auto AvailableComponents = std::to_array<ComponentItem>({
            { ICON_FA_LIGHTBULB,      "Light",                      AddComponentAction<Pitaya::Game::Light>,                    HasComponentAction<Pitaya::Game::Light> },
            { ICON_FA_CAMERA,         "Camera",                     AddComponentAction<Pitaya::Game::Camera>,                   HasComponentAction<Pitaya::Game::Camera> },
            { ICON_FA_CUBES,          "Mesh Renderer",              AddComponentAction<Pitaya::Game::MeshRenderer>,             HasComponentAction<Pitaya::Game::MeshRenderer> },
            { ICON_FA_PAINT_ROLLER,   "Material Override",          AddComponentAction<Pitaya::Game::MaterialOverride>,         HasComponentAction<Pitaya::Game::MaterialOverride> },
            { ICON_FA_WEIGHT_HANGING, "Rigidbody",                  AddComponentAction<Pitaya::Game::RigidBody>,                HasComponentAction<Pitaya::Game::RigidBody> },
            { ICON_FA_FILE_CODE,      "Script",                     AddComponentAction<Pitaya::Game::Script>,                   HasComponentAction<Pitaya::Game::Script> } });


    template<typename Component, typename DrawFunc>
    inline void DrawComponentUI(entt::entity e, const char* name, DrawFunc drawfunc)
    {
        if (auto* scene = Pitaya::Game::GetActiveScene())
        {
            if (auto* component = scene->GetComponent<Component>(e))
            {
                constexpr const bool isRemovable = !std::is_same_v<Component, Pitaya::Game::Metadata> &&
                    !std::is_same_v<Component, Pitaya::Game::Transform>;

                ImGui::PushID(name);
                ImGui::SetNextItemAllowOverlap();
                bool isOpen = ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
                bool removeComponent = false;
                if constexpr (isRemovable)
                {
                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) { ImGui::OpenPopup("ComponentSettings"); }

                    float frameHeight = ImGui::GetFrameHeight();
                    float buttonSize = frameHeight - 4.0f;
                    float offsetX = ImGui::GetContentRegionMax().x - buttonSize - 6.0f;

                    ImGui::SameLine(offsetX);
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

                    if (ImGui::Button(ICON_FA_GEAR, ImVec2(buttonSize, buttonSize))) { ImGui::OpenPopup("ComponentSettings"); }

                    ImGui::PopStyleVar(2);
                    ImGui::PopStyleColor(3);

                    if (ImGui::BeginPopup("ComponentSettings"))
                    {
                        if (ImGui::MenuItem(ICON_FA_TRASH_CAN " Remove Component")) { removeComponent = true; }
                        ImGui::EndPopup();
                    }
                }

                if (isOpen && !removeComponent)
                {
                    ImGui::Indent(16.0f);
                    drawfunc(scene, component);
                    ImGui::Unindent(16.0f);
                }

                if constexpr (isRemovable)
                {
                    if (removeComponent) { scene->RemoveComponent<Component>(e); }
                }

                ImGui::PopID();
            }
        }
    }
    inline void DrawMetadataUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::Metadata>(e, ICON_FA_TAG " Metadata",
            [](Pitaya::Game::Scene*, Pitaya::Game::Metadata* metadata)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

                // [Checkbox] [Name Input]
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
                bool active = metadata->IsActive();
                if (ImGui::Checkbox("##Active", &active)) { metadata->SetActive(active); }

                ImGui::PopStyleColor(); 
                ImGui::SameLine(0, 8.0f); 

                char nameBuffer[Pitaya::Game::Metadata::MAX_STR_SIZE] = {};
                Pitaya::Core::CopyStringToCharArray(metadata->GetName(), nameBuffer);

                ImGui::PushItemWidth(-1.0f);
                if (ImGui::InputText("##Name", nameBuffer, Pitaya::Game::Metadata::MAX_STR_SIZE)) { metadata->SetName(nameBuffer); }
                ImGui::PopItemWidth();

                ImGui::PopStyleVar(2);
            });
    }
    inline void DrawTransformUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::Transform>(e, ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT " Transform",
            [](Pitaya::Game::Scene*, Pitaya::Game::Transform* transform)
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
                    // Normalize -0.0f to 0.0f to avoid displaying "-0.000"
                    if (std::abs(rotDeg.x) < 0.001f) { rotDeg.x = 0.0f; }
                    if (std::abs(rotDeg.y) < 0.001f) { rotDeg.y = 0.0f; }
                    if (std::abs(rotDeg.z) < 0.001f) { rotDeg.z = 0.0f; }
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
    inline void DrawCameraUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::Camera>(e, ICON_FA_CAMERA " Camera",
            [](Pitaya::Game::Scene* scene, Pitaya::Game::Camera* camera)
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

                    const char* rtName = camera->GetIsRenderToMainDisplayRT() ? ICON_FA_DISPLAY " Screen Backbuffer" : ICON_FA_IMAGE " Custom RenderTarget";
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
                    ImGui::Button(rtName, ImVec2(ImGui::GetContentRegionAvail().x - RIGHT_PADDING, 0));
                    ImGui::PopStyleColor();

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                        {
                            // Pitaya::Core::GUID guid = ParseGUID((const char*)payload->Data);   
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::Spacing();
                }

                {   // Culling Mask
                    uint32_t rawMask = static_cast<uint32_t>(camera->GetCullingMask());
                    const char* previewText = (rawMask == 0) ? "Custom" :
                        ((rawMask != 0) && ((rawMask & (rawMask - 1)) == 0) ?
                            Pitaya::Render::ToString(static_cast<Pitaya::Render::RenderLayer>(rawMask)).data() : "Mixed");

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Culling Mask");
                    ImGui::SameLine(LABEL_WIDTH);

                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::BeginCombo("##CullingMaskCombo", previewText))
                    {
                        uint32_t oldMask = rawMask;
                        bool isEmpty = (rawMask == 0);
                        if (ImGui::Checkbox("Empty", &isEmpty) && isEmpty) { rawMask = 0; }

                        ImGui::Separator();
                        ImGui::CheckboxFlags("Default", &rawMask, (uint32_t)Pitaya::Render::RenderLayer::Default);
                        ImGui::CheckboxFlags("Transparent", &rawMask, (uint32_t)Pitaya::Render::RenderLayer::Transparent);
                        ImGui::CheckboxFlags("UI", &rawMask, (uint32_t)Pitaya::Render::RenderLayer::UI);
                        ImGui::CheckboxFlags("Editor Only", &rawMask, (uint32_t)Pitaya::Render::RenderLayer::EditorOnly);

                        if (oldMask != rawMask) { camera->SetCullingMask(static_cast<Pitaya::Render::RenderLayer>(rawMask)); }
                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();
                }

                {   // Post Process Setting
                    ImGui::Separator();
                    ImGui::TextUnformatted("Post Process");

                    auto& setting = camera->GetPostProcessSetting();
                    int stepToRemove = -1;
                    for (uint8_t i = 0; i < setting.StepCount; ++i)
                    {
                        auto& step = setting.Steps[i];
                        ImGui::PushID(i);

                        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DefaultOpen;
                        bool open = ImGui::TreeNodeEx("##PPNode", flags);

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

                        ImGui::SameLine();
                        ImGui::Text("%s  %s", ICON_FA_BARS, Pitaya::Render::ToString(step.Type).data());

                        float trashBtnPos = ImGui::GetContentRegionMax().x - 30.0f;
                        ImGui::SameLine(trashBtnPos);
                        if (ImGui::Button(ICON_FA_TRASH_CAN)) { stepToRemove = i; }

                        if (open)
                        {
                            if (step.Type == Pitaya::Render::PostProcessType::Bloom)
                            {
                                Pitaya::Render::BloomParams p;
                                std::memcpy(&p, step.ShaderParams, sizeof(p));
                                if (ImGui::DragFloat("Threshold", &p.Threshold, 0.01f, 0.0f, 5.0f) ||
                                    ImGui::DragFloat("Intensity", &p.Intensity, 0.01f, 0.0f, 10.0f))
                                {
                                    step.SetParams(p);
                                }
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

                    if (setting.StepCount < Pitaya::Render::PostProcessSetting::MAX_STEPS)
                    {
                        if (ImGui::Button(ICON_FA_PLUS " Add Post Effect", ImVec2(ImGui::GetContentRegionAvail().x - RIGHT_PADDING, 0))) { ImGui::OpenPopup("AddPPPopup"); }

                        if (ImGui::BeginPopup("AddPPPopup"))
                        {
                            if (ImGui::MenuItem(ICON_FA_WAND_MAGIC_SPARKLES " Bloom")) { Pitaya::Render::PostProcessStep s; s.SetParams(Pitaya::Render::BloomParams()); setting.AddStep(s); }
                            if (ImGui::MenuItem(ICON_FA_IMAGE " ToneMapping")) { Pitaya::Render::PostProcessStep s; s.SetParams(Pitaya::Render::ToneMappingParams()); setting.AddStep(s); }
                            if (ImGui::MenuItem(ICON_FA_CIRCLE_HALF_STROKE " GammaCorrection")) { Pitaya::Render::PostProcessStep s; s.SetParams(Pitaya::Render::GammaCorrectionParams()); setting.AddStep(s); }
                            ImGui::EndPopup();
                        }
                    }
                }
            });
    }
    inline void DrawLightUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::Light>(e, ICON_FA_LIGHTBULB " Light",
            [](Pitaya::Game::Scene*, Pitaya::Game::Light* light)
            {
                constexpr float LABEL_WIDTH = 90.0f;
                constexpr float RIGHT_PADDING = 8.0f;

                auto DrawLabel = [](const char* text)
                    {
                        ImGui::AlignTextToFramePadding();
                        ImGui::TextUnformatted(text);
                        ImGui::SameLine(LABEL_WIDTH);
                    };

                {   // Type
                    DrawLabel("Type");
                    int currentType = static_cast<int>(light->GetType());
                    const char* typeNames[] = { "Directional", "Point", "Spot" };
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::Combo("##LightType", &currentType, typeNames, 3)) { light->SetType(static_cast<decltype(light->GetType())>(currentType)); }
                }
                
                {   // Color
                    DrawLabel("Color");
                    glm::vec3 color = light->GetColor();
                    float fieldWidth = ImGui::GetContentRegionAvail().x - RIGHT_PADDING;
                    ImVec4 imColor = ImVec4(color.x, color.y, color.z, 1.0f);
                    if (ImGui::ColorButton("##LightColorBtn", imColor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_HDR, ImVec2(fieldWidth, 0.0f))) { ImGui::OpenPopup("LightColorPickerPopup"); }
                    if (ImGui::BeginPopup("LightColorPickerPopup"))
                    {
                        if (ImGui::ColorPicker3("##picker", glm::value_ptr(color), ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float)) { light->SetColor(color); }
                        ImGui::EndPopup();
                    }
                }

                {   // Intensity
                    DrawLabel("Intensity");
                    float intensity = light->GetIntensity();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::DragFloat("##LightIntensity", &intensity, 0.05f, 0.0f, 1000.0f)) { light->SetIntensity(intensity); }
                }

                {   // Cast Shadow
                    DrawLabel("Cast Shadow");
                    bool castShadow = light->GetEnableCastShadow();
                    if (ImGui::Checkbox("##LightCastShadow", &castShadow)) { light->SetEnableCastShadow(castShadow); }
                }

                auto typeCheck = light->GetType();

                {   // Radius
                    if (typeCheck == Pitaya::Game::LightType::Point || typeCheck == Pitaya::Game::LightType::Spot)
                    {
                        DrawLabel("Radius");
                        float radius = light->GetRadius();
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                        if (ImGui::DragFloat("##LightRadius", &radius, 0.1f, 0.01f, 1000.0f)) { light->SetRadius(radius); }
                    }
                }
                
                {   // Angles
                    if (typeCheck == Pitaya::Game::LightType::Spot)
                    {
                        DrawLabel("Inner Angle");
                        float inner = light->GetInnerAngle();
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                        if (ImGui::DragFloat("##LightInner", &inner, 0.5f, 0.0f, light->GetOuterAngle())) { light->SetInnerAngle(inner); }

                        DrawLabel("Outer Angle");
                        float outer = light->GetOuterAngle();
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                        if (ImGui::DragFloat("##LightOuter", &outer, 0.5f, light->GetInnerAngle(), 180.0f)) { light->SetOuterAngle(outer); }
                    }
                }

                ImGui::Dummy(ImVec2(0.0f, 2.0f));
            });
    }
    inline void DrawMeshRendererUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::MeshRenderer>(e, ICON_FA_CUBES " Mesh Renderer",
            [](Pitaya::Game::Scene*, Pitaya::Game::MeshRenderer* meshRenderer)
            {
                constexpr const float LABEL_WIDTH = 90.0f;
                constexpr const float RIGHT_PADDING = 8.0f;

                {   // Mesh切换
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Mesh");
                    ImGui::SameLine(LABEL_WIDTH);

                    char meshLabel[256] = {};
                    const auto& currentMesh = meshRenderer->GetMesh();
                    if (currentMesh)
                    {
                        std::snprintf(meshLabel, sizeof(meshLabel), "%s %s (Mesh)", ICON_FA_CUBES, "TODO 获取MeshName");
                    }
                    else
                    {
                        std::snprintf(meshLabel, sizeof(meshLabel), ICON_FA_CUBE " None (Mesh)");
                    }

                    float frameWidth = ImGui::GetContentRegionAvail().x - RIGHT_PADDING;
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                    ImGui::Button(meshLabel, ImVec2(frameWidth, 0));

                    ImGui::PopStyleColor(3);

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                        {
                            // TODO 
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::Spacing();
                }

                {   // LayerMask
                    Pitaya::Render::RenderLayer currentLayer = meshRenderer->GetLayerMask();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Layer");
                    ImGui::SameLine(LABEL_WIDTH);
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::BeginCombo("##LayerCombo", Pitaya::Render::ToString(currentLayer).data()))
                    {
                        auto DrawLayerSelectable = [&](const char* label, Pitaya::Render::RenderLayer layerVal) 
                            {
                                bool isSelected = (currentLayer == layerVal);
                                if (ImGui::Selectable(label, isSelected)) { meshRenderer->SetLayerMask(layerVal); }
                                if (isSelected) { ImGui::SetItemDefaultFocus(); }
                            };
                        DrawLayerSelectable("Default", Pitaya::Render::RenderLayer::Default);
                        DrawLayerSelectable("Transparent", Pitaya::Render::RenderLayer::Transparent);
                        DrawLayerSelectable("UI", Pitaya::Render::RenderLayer::UI);
                        DrawLayerSelectable("Editor Only", Pitaya::Render::RenderLayer::EditorOnly);
                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();
                }

                {
                    // Shadow
                    ImGui::Spacing();

                    // Cast Shadows 勾选框
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Cast Shadows");
                    ImGui::SameLine(LABEL_WIDTH);

                    bool castShadow = meshRenderer->GetEnableShadowCast();
                    if (ImGui::Checkbox("##CastShadow", &castShadow))
                    {
                        meshRenderer->SetEnableShadowCast(castShadow);
                    }

                    // Receive Shadows 勾选框
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Receive Shadows");
                    ImGui::SameLine(LABEL_WIDTH);

                    bool receiveShadow = meshRenderer->GetReceiveShadow();
                    if (ImGui::Checkbox("##ReceiveShadow", &receiveShadow))
                    {
                        meshRenderer->SetReceiveShadow(receiveShadow);
                    }

                    ImGui::Spacing();
                }
            });
    }
    inline void DrawMaterialOverrideUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::MaterialOverride>(e, ICON_FA_PAINT_ROLLER " Material Override",
            [](Pitaya::Game::Scene*, Pitaya::Game::MaterialOverride* materialOverride)
            {
                constexpr float LABEL_WIDTH = 90.0f;
                auto& materials = materialOverride->GetOverrideMaterials();
                int indexToRemove = -1;
                for (size_t i = 0; i < materials.size(); ++i)
                {
                    ImGui::PushID(static_cast<int>(i));
                    ImGui::BeginGroup();

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(ICON_FA_GRIP_LINES);
                    ImGui::SameLine();

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

                    ImGui::Text("Element %zu", i);
                    ImGui::SameLine(LABEL_WIDTH);

                    float deleteBtnWidth = 30.0f;
                    float slotWidth = ImGui::GetContentRegionAvail().x - deleteBtnWidth - 5.0f;

                    const char* matName = materials[i] ? ICON_FA_FILE_IMAGE " Material Asset" : ICON_FA_FILE " None (Material)";

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                    ImGui::Button(matName, ImVec2(slotWidth, 0));
                    ImGui::PopStyleColor();

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                        {
                            // TODO
                        }
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_TRASH_CAN, ImVec2(deleteBtnWidth, 0))) { indexToRemove = (int)i; }

                    ImGui::EndGroup();
                    ImGui::PopID();
                }

                if (indexToRemove != -1) { materials.erase(materials.begin() + indexToRemove); }

                if (ImGui::Button(ICON_FA_PLUS " Add Slot", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { materials.emplace_back(); }
            });
    }
    inline void DrawRigidbodyUI(entt::entity e)
    {
        DrawComponentUI<Pitaya::Game::RigidBody>(e, ICON_FA_WEIGHT_HANGING " RigidBody",
            [](Pitaya::Game::Scene*, Pitaya::Game::RigidBody* rigidBody)
            {
                constexpr const float LABEL_WIDTH = 90.0f;
                constexpr const float RIGHT_PADDING = 8.0f;

                {
                    Pitaya::Physics::RigidBodyType currentType = rigidBody->GetType();
                    const char* previewText = Pitaya::Physics::ToString(currentType).data();

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Body Type");
                    ImGui::SameLine(LABEL_WIDTH);

                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - RIGHT_PADDING);
                    if (ImGui::BeginCombo("##BodyTypeCombo", previewText))
                    {
                        bool isStatic = (currentType == Pitaya::Physics::RigidBodyType::Static);
                        if (ImGui::Selectable("Static", isStatic)) { rigidBody->SetType(Pitaya::Physics::RigidBodyType::Static); }
                        if (isStatic) { ImGui::SetItemDefaultFocus(); }

                        bool isKinematic = (currentType == Pitaya::Physics::RigidBodyType::Kinematic);
                        if (ImGui::Selectable("Kinematic", isKinematic)) { rigidBody->SetType(Pitaya::Physics::RigidBodyType::Kinematic); }
                        if (isKinematic) { ImGui::SetItemDefaultFocus(); }

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
        DrawComponentUI<Pitaya::Game::Script>(e, ICON_FA_FILE_CODE " Script",
            [](Pitaya::Game::Scene*, Pitaya::Game::Script* script)
            {
                int scriptIndex = 0;
                for (const auto& scriptInstance : script->GetScriptInstance())
                {
                    ImGui::PushID(scriptIndex);
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;

                    if (ImGui::TreeNodeEx("##ScriptInstanceNode", flags, "%s %s", ICON_FA_CODE, scriptInstance.ClassName.data()))
                    {
                        ImGui::TextDisabled("Serialization not implemented yet...");
                        ImGui::TreePop();
                    }
                    ImGui::Spacing();
                    ImGui::PopID();
                    scriptIndex++;
                }
            });
    }
    inline constexpr const auto ComponentDrawFuncs = std::to_array<void(*)(entt::entity)>(
        { DrawMetadataUI, DrawTransformUI, DrawLightUI, DrawCameraUI, DrawMeshRendererUI, 
        DrawMaterialOverrideUI, DrawRigidbodyUI, DrawScriptUI });
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

    for (auto ComponentDrawFunc : ComponentDrawFuncs) { ComponentDrawFunc(entity); }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(ICON_FA_PLUS " Add Component", ImVec2(-1, 0)))
    {
        ImVec2 buttonMin = ImGui::GetItemRectMin();
        ImVec2 buttonSize = ImGui::GetItemRectSize();
        ImGui::SetNextWindowPos(ImVec2(buttonMin.x, buttonMin.y + buttonSize.y));
        ImGui::SetNextWindowSize(ImVec2(buttonSize.x, 0));

        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        const constexpr uint32_t MAX_SEARCH_SIZE = 256;

        static char InputBuffer[MAX_SEARCH_SIZE] = {};
        ImGui::PushItemWidth(-1);
        ImGui::InputTextWithHint("##Search", ICON_FA_MAGNIFYING_GLASS " Search...", InputBuffer, sizeof(InputBuffer));
        ImGui::PopItemWidth();
        ImGui::Spacing();

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
                if (hasComp) { ImGui::BeginDisabled(); }
                char labelBuf[512] = {};
                std::snprintf(labelBuf, sizeof(labelBuf), "%s  %s", item.Icon, item.Name);
                if (ImGui::Selectable(labelBuf))
                {
                    item.AddFunc(entity);
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