#include "Core/Window.h"

/**
 * @brief Crea la ventana SFML y establece el límite de fotogramas a 60 FPS.
 * @param width  Ancho en píxeles.
 * @param height Alto en píxeles.
 * @param tittle Título de la ventana.
 */
Window::Window(int width, int height, const std::string& tittle) {
	m_window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode({ static_cast<unsigned int>(width),
						static_cast<unsigned int>(height) }),
		tittle,
		sf::Style::Default);

	//make_unique nunca retorna nullptr, solo verificar que existe
	if (m_window) {
		m_window->setFramerateLimit(60);
		m_baseViewSize = sf::Vector2f(static_cast<float>(width),
			static_cast<float>(height));
		m_view = m_window->getDefaultView();

		MESSAGE("Window", "Window", "Window created successfully");
	}
}

/** @brief Retorna true si la ventana sigue abierta. */
bool Window::isOpen() const {    
		return m_window->isOpen();
}

/** @brief Cierra la ventana, entonces egistra error si el puntero es nulo. */
void
Window::close() {
	if (m_window) {
		m_window->close();
	}
	else {
		ERROR("Window", "close", "Window is null");
	}
}

/**
 * @brief Limpia el buffer con el color indicado
 */
void
Window::clear(const sf::Color& color) {
	if (m_window) {
		m_window->clear(color); 
	}
	else {
		ERROR("Window", "draw", "Window is null"); 
	}
}

/**
 * @brief Dibuja un objeto en la ventana
 * @param states  Estados de render (transform, shader, etc.).
 */
void
Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
	if (m_window) {
		m_window->draw(drawable, states); 
	}
	else {
		ERROR("Window", "draw", "Window is null"); 
	}
}

/** @brief Presenta el buffer renderizado en pantalla */
void 
Window::display() {
	if (m_window) {
		m_window->display(); 
	}
	else {
		ERROR("Window", "display", "Window is null"); 
	}
}

void
Window::handleResize(const sf::Vector2u& size) {
	if (!m_window) {
		ERROR("Window", "handleResize", "Window is null"); 
		return; 
	}

	const sf::Vector2f fsize(static_cast<float>(size.x),
		static_cast<float>(size.y)); 

	m_baseViewSize = fsize; 
	m_view.setSize(fsize);
	m_view.setCenter({ 0.f, 0.f }); 
	m_window->setView(m_view); 
}

void Window::applyCameraView(const sf::Vector2f& center, float zoom, float rotationDeg)
{
	if (!m_window)
	{
		ERROR("Window", "applyCameraView", "Window is null");
		return;
	}
	if (zoom <= 0.f) zoom = 1.f; 
	m_view.setSize(m_baseViewSize / zoom); 
	m_view.setCenter(center);
	m_view.setRotation(sf::degrees(rotationDeg)); 
	m_window->setView(m_view);
}

void 
Window::update() {
	//almacena el delta time el tiempo de la ulrima vez llmadp
	//reinicia el reloj 
	deltaTime = clock.restart(); 
}

/** @brief Reservado para lógica de render por frame. Actualmente sin implementación */
void
Window::render() {

}

/** @brief Libera el puntero */
void
Window::destroy() {
	m_window.reset(); 
}