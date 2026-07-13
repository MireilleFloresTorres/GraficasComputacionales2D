#pragma once
#include "Prerequisites.h"

#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/System.H"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/SteeringTarget.h"

/**
     * @class UISystem
     * @brief Sistema de interfaz de usuario basado en ImGui para el motor.
     *
     * UISystem se encarga de dibujar y gestionar los paneles de edición del
     * editor, incluyendo la lista de entidades y el Inspector. También configura
     * el docking de ImGui y aplica un tema visual personalizado inspirado
     * en Unreal Engine.
     *
     * Este sistema permite al usuario:
     * - Seleccionar entidades desde un listado (Outliner).
     * - Visualizar y editar componentes como Transform, Render, Camera
     *   y SteeringTarget desde el panel Inspector
     * - Configurar comportamientos de steering (Seek, Flee, Arrive, Wander,
     *   Pursuit, Obstacle Avoidance) y su entidad/posición objetivo.
     *
     * Hereda de ECS::System y se ejecuta una vez por frame mediante OnUpdate
     */

namespace ECS {
	class UISystem final : public System {
	public: 
		UISystem() = default; 

        /**
        * @brief Actualiza y dibuja la interfaz cada frame.
        * @param registry Registro ECS con las entidades y componentes
        */
		void OnUpdate(Registry& registry, float deltatime) override {
            ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspaceFlags);
            DrawOutliner(registry); 
            DrawDetails(registry);

		}
      
         ///@brief Inicializa la configuración de ImGui (docking y estilo visual).
        void OnStart(Registry& /*registry*/) override {
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            setupUnrealStyle();
        }

        /**
        * @brief Dibuja el panel "Entities" con la lista de entidades vivas
        * @param registry Registro ECS con las entidades.
        */
		void DrawOutliner(Registry& registry) {
            ImGui::Begin("Entities");
            {
                for (const ECS::EntityID& id : registry.GetAllEntities()) {
                    if (!registry.IsAlive(id)) continue;

                    ECS::EntityIndex idx = ECS::GetEntityIndex(id);
                    std::string label = "Entity " + std::to_string(idx);

                    bool isSelected = (selectedEntity == id);
                    if (ImGui::Selectable(label.c_str(), isSelected)) {
                        selectedEntity = id;
                    }
                }
            }
            ImGui::End();
		}

        /**
         * @brief Dibuja el panel "Inspector" con los componentes de la entidad seleccionada.
         * @param registry Registro ECS con las entidades y componentes.
         */
        void DrawDetails(Registry& registry) {
            ImGui::Begin("Inspector");
            {
                if (selectedEntity != ECS::NULL_ENTITY && registry.IsAlive(selectedEntity)) {

                    ECS::EntityIndex idx = ECS::GetEntityIndex(selectedEntity);
                    std::string title = "Entity " + std::to_string(idx);
                    ImGui::Text("%s", title.c_str());

                    // --- Transform ---
                    if (registry.HasComponent<ECS::Transform>(selectedEntity)) {
                        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ECS::Transform& t = registry.GetComponent<ECS::Transform>(selectedEntity);
                            vec2Control("Position", &t.position.x, 0.0f, 120.0f);
                            vec2Control("Rotation", &t.rotation, 0.0f, 120.0f);
                            vec2Control("Scale", &t.scale.x, 1.0f, 120.0f);
                        }
                    }

                    // --- Render ---
                    if (registry.HasComponent<ECS::Render>(selectedEntity)) {
                        if (ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ECS::Render& r = registry.GetComponent<ECS::Render>(selectedEntity);
                            ImGui::Checkbox("Visible", &r.visible);
                            float color[4] = {
                                r.fillColor.r / 255.f,
                                r.fillColor.g / 255.f,
                                r.fillColor.b / 255.f,
                                r.fillColor.a / 255.f
                            };
                            if (ImGui::ColorEdit4("Fill Color", color)) {
                                r.fillColor.r = static_cast<uint8_t>(color[0] * 255.f);
                                r.fillColor.g = static_cast<uint8_t>(color[1] * 255.f);
                                r.fillColor.b = static_cast<uint8_t>(color[2] * 255.f);
                                r.fillColor.a = static_cast<uint8_t>(color[3] * 255.f);
                            }
                        }
                    }

                    // --- Camera ---
                    if (auto* cam = registry.TryGetComponent<ECS::Camera>(selectedEntity)) {
                        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {


                            ImGui::Checkbox("Active", &cam->active); 
                            ImGui::DragFloat("Zoom", &cam->zoom, 0.01f, 0.05f, 10.f); 

                            if (cam->followTarget == ECS::NULL_ENTITY)
                                ImGui::Text("Follow target: (ninguno)");
                            else
                                ImGui::Text("Follow Target: %allu",
                                    static_cast<unsigned long long>(cam->followTarget)); 
                        }
                    }
                }
                else {
                    ImGui::Text("No entity selected");
                }

                // --- Steering ---
                if (auto* st = registry.TryGetComponent<ECS::SteeringTarget>(selectedEntity)) {
                    if (ImGui::CollapsingHeader("Steering", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::Checkbox("Enabled", &st->enabled);
                        // Dropdown de comportamiento
                        const char* behaviors[] = { "Seek", "Flee", "Arrive", "Wander", "Pursuit", "Obstacle Avoidance" };
                        int current = static_cast<int>(st->behavior);
                        if (ImGui::Combo("Behavior", &current, behaviors, 6)) {
                            st->behavior = static_cast<ECS::SteeringBehavior>(current);
                            if (auto* ph = registry.TryGetComponent<ECS::Physics>(selectedEntity)) {
                                ph->velocity = { 0.f, 0.f };
                                ph->acceleration = { 0.f, 0.f };
                            }
                        }

                        //Dropdown de entidad objetivo
                        ImGui::Separator();
                        ImGui::Text("Target Entity");

                        //Construye lista de entidades disponibles
                        std::vector<ECS::EntityID> aliveEntities;
                        std::vector<std::string>   entityLabels;
                        for (const ECS::EntityID& id : registry.GetAllEntities()) {
                            if (!registry.IsAlive(id)) continue;
                            if (id == selectedEntity)  continue; // no se sigue a sí mismo
                            aliveEntities.push_back(id);
                            entityLabels.push_back("Entity " + std::to_string(ECS::GetEntityIndex(id)));
                        }

                        // Opción "ninguno" al inicio
                        std::vector<const char*> labelPtrs;
                        labelPtrs.push_back("(Position fija)");
                        for (auto& lbl : entityLabels)
                            labelPtrs.push_back(lbl.c_str());

                        // Encuentra índice actual en la lista
                        int targetIdx = 0; // default = posición fija
                        if (st->followAnEntity) {
                            for (int i = 0; i < (int)aliveEntities.size(); ++i) {
                                if (aliveEntities[i] == st->followEntity) {
                                    targetIdx = i + 1; // +1 por el "(Position fija)"
                                    break;
                                }
                            }
                        }

                        if (ImGui::Combo("Target", &targetIdx, labelPtrs.data(), (int)labelPtrs.size())) {
                            if (targetIdx == 0) {
                                st->followAnEntity = false;
                            }
                            else {
                                st->followAnEntity = true;
                                st->followEntity = aliveEntities[targetIdx - 1];
                            }
                        }

                        // Si es posición fija, permite editarla
                        if (!st->followAnEntity)
                            vec2Control("Target Pos", &st->targetPosition.x, 0.f, 120.f);

                        // Slow radius solo para Arrive
                        if (st->behavior == ECS::SteeringBehavior::Arrive)
                            ImGui::DragFloat("Slowing R.", &st->slowRadius, 1.f, 10.f, 500.f);

                        // Physics
                        if (auto* ph = registry.TryGetComponent<ECS::Physics>(selectedEntity)) {
                            ImGui::Separator();
                            ImGui::DragFloat("Max Speed", &ph->maxSpeed, 1.f, 0.f, 1000.f);
                            ImGui::DragFloat("Max Force", &ph->maxForce, 0.1f, 0.f, 100.f);
                        }
                    }
                }
            }
            ImGui::End();
       }

        /// @brief Aplica un tema visual estilo Unreal Engine (naranja + grises oscuros) a ImGui.
        void
            setupUnrealStyle() {
            ImGuiStyle& style = ImGui::GetStyle();

            // Formas
            style.WindowRounding = 0.0f;
            style.ChildRounding = 0.0f;
            style.FrameRounding = 2.0f;
            style.GrabRounding = 2.0f;
            style.PopupRounding = 0.0f;
            style.ScrollbarRounding = 2.0f;
            style.TabRounding = 2.0f;

            // Tamaños
            style.WindowPadding = ImVec2(8.0f, 8.0f);
            style.FramePadding = ImVec2(5.0f, 3.0f);
            style.ItemSpacing = ImVec2(8.0f, 4.0f);
            style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
            style.ScrollbarSize = 13.0f;
            style.GrabMinSize = 10.0f;
            style.WindowBorderSize = 1.0f;
            style.FrameBorderSize = 0.0f;
            style.WindowTitleAlign = ImVec2(0.0f, 0.5f);

            // Paleta Unreal Engine — naranja + grises muy oscuros
            ImVec4* c = style.Colors;

            // Fondos
            c[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
            c[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
            c[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);

            // Bordes
            c[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
            c[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

            // Frames (inputs, sliders)
            c[ImGuiCol_FrameBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
            c[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
            c[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);

            // Título de ventana
            c[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
            c[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
            c[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);

            // Scrollbar
            c[ImGuiCol_ScrollbarBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
            c[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
            c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
            c[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.85f, 0.45f, 0.00f, 1.00f); // naranja

            // Checkmark y Slider — acento naranja
            c[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.50f, 0.00f, 1.00f);
            c[ImGuiCol_SliderGrab] = ImVec4(0.85f, 0.45f, 0.00f, 1.00f);
            c[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

            // Botones
            c[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            c[ImGuiCol_ButtonHovered] = ImVec4(0.85f, 0.45f, 0.00f, 1.00f); // naranja
            c[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.55f, 0.00f, 1.00f);

            // Headers (CollapsingHeader)
            c[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            c[ImGuiCol_HeaderHovered] = ImVec4(0.85f, 0.45f, 0.00f, 0.80f);
            c[ImGuiCol_HeaderActive] = ImVec4(0.95f, 0.50f, 0.00f, 1.00f);

            // Separadores
            c[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
            c[ImGuiCol_SeparatorHovered] = ImVec4(0.85f, 0.45f, 0.00f, 1.00f);
            c[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.55f, 0.00f, 1.00f);

            // Resize grip
            c[ImGuiCol_ResizeGrip] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
            c[ImGuiCol_ResizeGripHovered] = ImVec4(0.85f, 0.45f, 0.00f, 1.00f);
            c[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.55f, 0.00f, 1.00f);

            // Tabs
            c[ImGuiCol_Tab] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
            c[ImGuiCol_TabHovered] = ImVec4(0.85f, 0.45f, 0.00f, 0.80f);
            c[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            c[ImGuiCol_TabUnfocused] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
            c[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

            // Texto
            c[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
            c[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
            c[ImGuiCol_TextSelectedBg] = ImVec4(0.85f, 0.45f, 0.00f, 0.35f);

            // Selectable activo
            c[ImGuiCol_NavHighlight] = ImVec4(0.95f, 0.50f, 0.00f, 1.00f);
            c[ImGuiCol_DragDropTarget] = ImVec4(0.95f, 0.50f, 0.00f, 1.00f);
        }

        /**
         * @brief Dibuja un control de dos componentes (X, Y) con botones de reset, estilo Unreal.
         * @param label Etiqueta del campo.
         * @param values Puntero a un arreglo de 2 floats (X, Y) a editar.
         * @param resetValue Valor asignado al presionar los botones de reset.
         * @param columnWidth Ancho de la columna de la etiqueta.
         */
        void
            vec2Control(const std::string& label,
                float* values,
                float resetValue,
                float columnWidth) {

            ImGui::PushID(label.c_str());

            // Columna izquierda: nombre del campo
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label.c_str());
            ImGui::NextColumn();

            // Calcula tamaños
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f });
            float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
            float  itemWidth = (ImGui::CalcItemWidth() - buttonSize.x * 2.0f) / 2.0f;

            // --- Botón X (naranja Unreal) ---
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.85f, 0.35f, 0.00f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.00f, 0.50f, 0.00f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.70f, 0.25f, 0.00f, 1.0f });
            if (ImGui::Button("X", buttonSize)) values[0] = resetValue;
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::PushItemWidth(itemWidth);
            ImGui::DragFloat("##X", &values[0], 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // --- Botón Y (gris claro para contraste) ---
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.30f, 0.30f, 0.30f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.50f, 0.50f, 0.50f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.20f, 0.20f, 0.20f, 1.0f });
            if (ImGui::Button("Y", buttonSize)) values[1] = resetValue;
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::PushItemWidth(itemWidth);
            ImGui::DragFloat("##Y", &values[1], 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();
            ImGui::Columns(1);
            ImGui::PopID();
        }

    private: 
    //Enttidad seleccionada
        ECS::EntityID selectedEntity = ECS::NULL_ENTITY; 
	};
}