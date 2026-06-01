#pragma once
#include "Prerequisites.h"

class Window; 

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
	static std::unique_ptr<sf::Shape>
		//se guarda el tipo de figura
		//el metodo create shape es una fabrica
		createShape(ShapeType shapeType); 
private: 
	std::unique_ptr<sf::Shape> m_shape = nullptr; 
	ShapeType m_shapeType; 
};