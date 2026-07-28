#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Camera.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/SteeringTarget.h"
#include "ECS/Systems/IASystem.h"
#include "ECS/Components/Obstacle.h"

/**
 * @brief Inicializa ImGui, registra sistemas ECS, crea entidades de prueba y ejecuta el game loop principal.
 *
 * Primero se inicializa ImGui sobre la ventana SFML y se registran los sistemas del ECS (IA, cámara,
 * render y UI) en el orden en que deben ejecutarse cada frame, luego se crean las entidades de prueba
 * (círculo, triángulo con steering Seek, obstáculo y cámara) con sus respectivos componentes., para finalmente
 * se entra al game loop, que procesa eventos de SFML/ImGui, actualiza los sistemas del registry con el
 * delta time y presenta cada frame renderizado.
 *
 * @return 0 si termina correctamente, -1 si falla la inicialización de ImGui.
 */
Window g_window(Window(800, 600, "Labrid Engine")); ///< Ventana principal de la aplicación.
ECS::Registry registry; // Registro ECS global


/** @brief Libera recursos de ImGui / SFML antes de cerrar la aplicación.*/
void destroy()
{
    ImGui::SFML::Shutdown();
}

#include <array>
#include <algorithm>

void DrawMSAASettings() {
    ImGui::Begin("MSAA Settings");

    const unsigned int currentLevel =
        g_window.m_window->getSettings().antiAliasingLevel;

    ImGui::Text("Current MSAA Level: %ux", currentLevel);
    ImGui::Separator();

    static constexpr std::array<unsigned int, 4> msaaLevels{ 0, 2, 4, 8 };
    static constexpr const char* msaaLabels[]{
        "Disabled", "2x MSAA", "4x MSAA", "8x MSAA"
    };

    static int selectedIndex = [&]()
        {
            const auto iterator = std::find(
                msaaLevels.begin(), msaaLevels.end(), currentLevel);
            if (iterator == msaaLevels.end())
                return 0;
            return static_cast<int>(std::distance(msaaLevels.begin(), iterator));
        }();

    ImGui::SetNextItemWidth(150.0f);
    ImGui::Combo("MSAA Level", &selectedIndex, msaaLabels,
        static_cast<int>(std::size(msaaLabels)));

    const unsigned int selectedLevel =
        msaaLevels[static_cast<std::size_t>(selectedIndex)];
    const bool hasChanges = selectedLevel != currentLevel;

    if (!hasChanges) ImGui::BeginDisabled();
    if (ImGui::Button("Apply")) {
        g_window.setMSAALevel(selectedLevel);
    }
    if (!hasChanges) ImGui::EndDisabled();

    ImGui::SameLine();
    if (hasChanges)
        ImGui::Text("Pending: %ux", selectedLevel);
    else
        ImGui::TextDisabled("No pending changes");

    ImGui::Separator();
    ImGui::TextWrapped("Changing MSAA recreates the rendering window.");
    ImGui::End();
}

/**brief Inicializa ImGui, registra sistemas ECS, crea entidades de prueba y ejecuta el game loop principal.
 *@return 0 si termina correctamente, -1 si falla la inicialización de ImGui*/
int main()
{
    // m_window es un puntero a sf::RenderWindow.
    if (!ImGui::SFML::Init(*g_window.m_window)) {
        return -1;
    }

    // Registrar sistemas en el ECS.
    registry.AddSystem<ECS::IASystem>();
    registry.AddSystem<ECS::CameraSystem>(g_window);
    registry.AddSystem<ECS::RenderSystem>(g_window);
    registry.AddSystem<ECS::UISystem>();
    sf::Clock deltaClock;
    
    // El steering se asigna desde el Inspector
    ECS::EntityID circle = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
    registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color::White, "Textures/ColorChecker.png"));
    registry.AddComponent<ECS::Physics>(circle);
    registry.AddComponent<ECS::SteeringTarget>(circle); // vacío, se configura en UI

    // Triángulo — igual
    ECS::EntityID tri = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 200.f, 200.f });
    registry.AddComponent<ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Yellow));
    registry.AddComponent<ECS::Physics>(tri);
    auto& triSteering = registry.AddComponent<ECS::SteeringTarget>(tri);
    triSteering.behavior = ECS::SteeringBehavior::Seek;
    triSteering.followEntity = circle;
    triSteering.followAnEntity = true;
    triSteering.enabled = true;

    // Obstáculo — solo Transform y Render, con tag Obstacle
    ECS::EntityID obstacle = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(obstacle, sf::Vector2f{ 300.f, 250.f });
    registry.AddComponent<ECS::Render>(obstacle, ECS::Render::Make(CIRCLE, sf::Color(200, 50, 50)));
    registry.AddComponent<ECS::Obstacle>(obstacle);

    ECS::EntityID cam = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(cam, sf::Vector2f{ 400.f, 300.f });
    auto& camComp = registry.AddComponent<ECS::Camera>(cam);
    camComp.followTarget = circle;     // la cámara sigue al player
    camComp.followSpeed = 5.f;        // sube para que se pegue más rápido
    camComp.zoom = 1;


   

    while (g_window.isOpen()) {
        while (const std::optional event =
            g_window.m_window->pollEvent()) {
            // ImGui debe recibir todos los eventos de SFML.
            ImGui::SFML::ProcessEvent(*g_window.m_window, *event);

            if (event->is<sf::Event::Closed>()) {
                g_window.close();
            }
            // Resize event: update the view to the new size of the window.
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                g_window.handleResize(resized->size);
            }
        }

        const sf::Time elapsedTime = deltaClock.restart();
        const float dt = elapsedTime.asSeconds();

        // Iniciar el frame de ImGui.
        ImGui::SFML::Update(*g_window.m_window, elapsedTime);

        // Limpiar la ventana.
        g_window.clear(sf::Color::Black);

        DrawMSAASettings();
        // Renderizar los elementos de tu ECS.
        registry.UpdateSystems(dt);

        // Renderizar ImGui después de la escena.
        ImGui::SFML::Render(*g_window.m_window);

        // Presentar el frame.
        g_window.display();
    }

    destroy();

    return 0;
}