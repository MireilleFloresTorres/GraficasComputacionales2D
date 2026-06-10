#pragma once
#include "Prerequisites.h"

class Window; 

/**
 * @class CShape
 * @brief Encapsula una figura de SFML y permite su creación y renderizado
 */
class
CShape {
public: 

	CShape() = default; 

	//el explicit evita que se hagan conversiones no deseadas
	explicit CShape(ShapeType shapeType); 

	~CShape() = default; 

	void 
	draw(Window& window); 

	//esta parte deja que el código acceda a la figura 
	//para moficarla, ya sea tamaño, color etc. 
	sf::Shape*
	getShape(); 
private: 

	/**
	* @brief Fábrica que crea una forma sengún lo que se especifique en el ShapeType
	* @param shapeType Tipo de figura a crear ya sea circulo, rectangulo, triangulo, etc.
	* @return Puntero único a la figura creada.
	*/

	static std::unique_ptr<sf::Shape>
		//se guarda el tipo de figura
		//el metodo create shape es una fabrica
		createShape(ShapeType shapeType); 
private: 
	//figura almacenada
	std::unique_ptr<sf::Shape> m_shape = nullptr; 
	//Tipo de figura representada
	ShapeType m_shapeType; 
};