#pragma once
#include "Prerequisites.h"

/**
* @brief ECS::Componentes/Render.h
* Render -Componente de DATOS PUROS que guarda la forma
* dibujable (sf::Shaape ) para ser compatible con el pool
* a diferencia de CShape, que tiene el move
*
* La posicion/rotaciín/escala no viquen aquí: vienen del
* componente Transform y las aplica del RenderSystem cada frame
*/

namespace ECS {

	struct Render {
		std::shared_ptr<sf::Shape> shape;
		sf::Color fillColor{ sf::Color::White };
		bool visible{ true };

		Render() = default;

		explicit Render(std::shared_ptr<sf::Shape> s,
			sf::Color color = sf::Color::White) noexcept
			: shape(std::move(s)), fillColor(color) {

		}

		/**
		* @brief Factory de conveniencia (reusa tu enum ShapeType) --
		* Crea la forma centrada en su origen,  para que el Transform
		* la possicione por su CENTRO (no por la esquina sup izq)
		*/

		[[nodiscard]] static Render
			Make(ShapeType type, sf::Color color = sf::Color::White) {
			std::shared_ptr<sf::Shape> s;
			switch (type) {

			case CIRCLE: {
				auto c = std::make_shared<sf::CircleShape>(50.f);
				c->setOrigin({ 50.f, 50.f });
				s = c;
				break;
			}
			case RECTANGLE: {
				sf::Vector2f size{ 100.f, 50.f };
				auto r = std::make_shared<sf::RectangleShape>(size);
				r->setOrigin(size / 2.f);
				s = r;
				break;
			}

			case TRIANGLE: {
				auto t = std::make_shared<sf::ConvexShape>(3);
				t->setPoint(0, { 0.f, 0.f });
				t->setPoint(1, { 100.f, 0.f });
				t->setPoint(2, { 50.f, 100.f });
				t->setOrigin({ 50.f, 50.f });

				s = t;
				break;
			}

			case POLYGON: {
				auto p = std::make_shared<sf::ConvexShape>(5);

				p->setPoint(0, sf::Vector2f(0.f, 0.f));
				p->setPoint(1, sf::Vector2f(100.f, 0.f));
				p->setPoint(2, sf::Vector2f(120.f, 50.f));
				p->setPoint(3, sf::Vector2f(50.f, 100.f));
				p->setPoint(4, sf::Vector2f(-20.f, 50.f));

				s = p;
				break;
			}

			case LINE: {
				auto l =
					std::make_shared<sf::RectangleShape>(
						sf::Vector2f(100.f, 1.f));

				s = l;
				break;
			}

			default:
				break;
			}

			if (s) s->setFillColor(color);
			return Render{ s, color };

		}

	};

}