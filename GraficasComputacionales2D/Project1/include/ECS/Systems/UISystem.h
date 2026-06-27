#pragma once
#include "Prerequisites.h"

#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/System.H"

namespace ECS {
	class UISystem final : public System {
	public: 
		UISystem() = default; 

		void OnUpdate(Registry& registry, float deltatime) override {
            DrawOutliner(registry); 
            DrawDetails(registry);

		}

        void OnStart(Registry& /*registry*/) override {
            // Habilitar docking.
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        }

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
                            ImGui::DragFloat2("Position", &t.position.x, 1.f);
                            ImGui::DragFloat("Rotation", &t.rotation, 0.5f);
                            ImGui::DragFloat2("Scale", &t.scale.x, 0.01f);
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
                }
                else {
                    ImGui::Text("No entity selected");
                }
            }
            ImGui::End();
       }

    private: 
    //Enttidad seleccionada
        ECS::EntityID selectedEntity = ECS::NULL_ENTITY; 
	};
}