#include "Core/CShape.h"
#include "Core/Window.h"

/**
 * @brief Construye la figura inicializando el tipo y creando el Shape correspondiente
 * shapeType es el Tipo de figura a crear
 */
CShape::CShape(ShapeType shapeType) : m_shapeType(shapeType), 
m_shape(createShape(shapeType)) {
// . es una lita de inicialización, aquí van las variables miembro
}

/**
 * @brief Dibuja la figura en la ventana si el puntero interno es válido
 * @param en window está lo que se va a renderizar
 */
void
CShape::draw(Window& window) {
	//verifica que unique_ptr no se nullptr antes de usarlo
	if (m_shape) {
		// se pone desreferencia 
		window.draw(*m_shape); 
	}
}

/** @brief Retorna el puntero raw al sf::Shape interno. */
sf::Shape* CShape::getShape()
{
	return m_shape.get(); 
}

std::unique_ptr<sf::Shape>
CShape::createShape(ShapeType shapeType) {

	switch (shapeType)
	{
		
	case EMPTY: 
		return nullptr; 

	/// @brief Implementación del circulo con posición, tamaño y color
	case CIRCLE: 
	{
		auto circle = std::make_unique<sf::CircleShape>(50.f); 
		circle->setFillColor(sf::Color::White); 
		circle->setPosition({ 100.0f, 100.0f }); 
		return circle; 
	}

	/// @brief  del recatangle con posición, tamaño y color
	case RECTANGLE: 
	{
		auto rectangle = std::make_unique<sf::RectangleShape>
			(sf::Vector2f(100.f, 50.f)); 
		rectangle->setFillColor(sf::Color::White); 
		rectangle->setPosition({ 200.0f, 200.0f }); 
		return rectangle;
	}

	/// @brief Implementación del triangle con posición, tamaño y color
	case TRIANGLE:
	{
		auto triangle = std::make_unique<sf::ConvexShape>(3); 
		triangle->setPoint(0, sf::Vector2f(0.f, 0.f));
		triangle->setPoint(1, sf::Vector2f(100.f, 0.f));
		triangle->setPoint(2, sf::Vector2f(50.f, 100.f)); 
		triangle->setFillColor(sf::Color::White); 
		triangle->setPosition({ 300.0f, 300.0f }); 
		return triangle; 

	}

	/// @brief Implementación del poligono con posición, tamaño y color
	case POLYGON:
	{
		auto polygon = std::make_unique<sf::ConvexShape>(5); 
		polygon->setPoint(0, sf::Vector2f(0.f, 0.f));
		polygon->setPoint(1, sf::Vector2f(100.f, 0.f));
		polygon->setPoint(2, sf::Vector2f(120.f, 50.f));
		polygon->setPoint(3, sf::Vector2f(50.f, 100.f));
		polygon->setPoint(4, sf::Vector2f(-20.f, 50.f)); 
		polygon->setFillColor(sf::Color::White); 
		polygon->setPosition({ 400.0f, 400.0f }); 
		return polygon;
	}

	/// @brief Implementación de un rectangulo con posición, tamaño y color, pero con un grosor de 1 para simular una línea
	case LINE:
	{
		auto line = std::make_unique<sf::RectangleShape>(sf::Vector2f(100.f, 1.f)); 
		line->setFillColor(sf::Color::White); 
		line->setPosition({ 500.0f, 500.0f }); 
		return line;
	}
	default: 
		return nullptr; 
	}
}