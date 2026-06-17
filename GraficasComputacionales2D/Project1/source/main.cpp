#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Systems/RenderSystem.h"

Window g_window(Window(800, 600, "Labrid Engine"));
ECS::Registry registry; 

void destroy() {
	//SAFE_PTR_RELEASE(g_window);
}

int
main() {
	registry.AddSystem<ECS::RenderSystem>(g_window); 

	ECS::EntityID circle = registry.CreateEntity(); 
	registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
	registry.AddComponent < ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50))); 

	ECS::EntityID tri = registry.CreateEntity();
	registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 200.f, 200.f }, 45.f);
	registry.AddComponent < ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));



	///@brief run the program as long as the window is open
	while(g_window.isOpen()) {
		//check all the window events thet were triggered since the last iteration of the loop
		while (const std::optional event = g_window.m_window->pollEvent()) {

			if (event->is<sf::Event::Closed>()) {
				g_window.close(); 
			}
		}

		float dt = 1.f / 60.f; 

		g_window.clear(sf::Color::Black); 

		registry.UpdateSystems(dt);

		g_window.display(); 
	}
	destroy();
	return 0;
}