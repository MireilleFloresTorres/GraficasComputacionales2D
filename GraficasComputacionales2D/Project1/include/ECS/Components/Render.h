#pragma once
#include "Prerequisites.h"

namespace ECS {
    struct Render {
        /**
    * @brief Componente que contiene la representación visual (shape) de una entidad y su textura opcional.
    */
        std::shared_ptr<sf::Shape> shape;
        std::shared_ptr<sf::Texture> texture;
        sf::Color fillColor{ sf::Color::White };
        bool visible{ true };

        /**
        * @brief Crea y configura un componente Render con la forma, color y textura indicados.
        * @param type Tipo de figura a crear
        * @param color Color de relleno a aplicar a la figura
        * @param texturePath Ruta del archivo de textura a cargar si está vacío, no se asigna textura.
        */
        static Render Make(ShapeType type, const sf::Color& color, const std::string& texturePath = "");
    };
}