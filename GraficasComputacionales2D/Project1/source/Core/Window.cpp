#include "Core/Window.h"

Window::Window(int width, int height, const std::string& tittle) {
	m_window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode({ static_cast<unsigned int>(width),
						static_cast<unsigned int>(height) }),
		tittle,
		sf::Style::Default);

	//make_unique nunca retorna nullptr, solo verificar que existe
	if (m_window) {
		m_window->setFramerateLimit(60);
		MESSAGE("Window", "Window", "Window created successfully");
	}
}

bool Window::isOpen() const {    
		return m_window->isOpen();
}

void
Window::close() {
	if (m_window) {
		m_window->close();
	}
	else {
		ERROR("Window", "close", "Window is null");
	}
}

void
Window::clear(const sf::Color& color) {
	if (m_window) {
		m_window->clear(color); 
	}
	else {
		ERROR("Window", "draw", "Window is null"); 
	}
}

void
Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
	if (m_window) {
		m_window->draw(drawable, states); 
	}
	else {
		ERROR("Window", "draw", "Window is null"); 
	}
}

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
Window::update() {
	//almacena el delta time el tiempo de la ulrima vez llmadp
	//reinicia el reloj 
	deltaTime = clock.restart(); 
}

void
Window::render() {

}

void
Window::destroy() {
	m_window.reset(); 
}