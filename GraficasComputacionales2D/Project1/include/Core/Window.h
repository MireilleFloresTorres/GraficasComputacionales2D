#pragma once
#include "Prerequisites.h"

/**
 * @class Window
 * @brief Clase envoltoria para la ventana de renderizado de SFML
 */
class
Window {
public: 
	Window() = default; 
	Window(int width, int height, const std::string& tittle);
	~Window() = default; 

	//void
	//handleEvents(EngineGUI& engineGUI);
	/// @brief Consulta si la ventana sigue abierta, retorna t o f
	bool
	isOpen() const; 
	
	/// @brief limpia la pantalla antes de dibujar el siguiente frame
	void
	clear(const sf::Color& color = sf::Color(0, 0, 0, 255));

	/**
	 * @brief Dibuja un objeto renderizable
	 * @param drawable es el objeto a dibujar
	 * @param states Estados de renderizado en el proceso 
	 */
	void
	draw(const sf::Drawable& drawable,
		const sf::RenderStates& states = sf::RenderStates::Default);

	/**
	* @brief Muestra en la pantalla lo que se rendeirza 
	*/
	//intercambia buferes
	void
	display();

	void
	close(); 

	/// @brief actualiza con frames
	void
	update(); 

	void
	render(); 

	void
	destroy(); 

	void handleResize(const sf::Vector2u& size);

	void
	applyCameraView(const sf::Vector2f& position,
			float zoom,
			float rotationDeg = 0.f);

	/// @brief puntero inteligente de la ventana real
	std::unique_ptr<sf::RenderWindow> m_window = nullptr; 


private: 
	sf::View m_view;
	sf::Time deltaTime;
	sf::Clock clock;
	sf::Vector2f m_baseViewSize;

private:
	int m_width{ 0 };
	int m_height{ 0 };
	std::string m_title;

public:
	void setMSAALevel(unsigned int level);
};	