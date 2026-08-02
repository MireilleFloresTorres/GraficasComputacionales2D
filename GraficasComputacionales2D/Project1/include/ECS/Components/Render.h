#pragma once
#include "Prerequisites.h"

namespace ECS {
    struct Render {
        std::shared_ptr<sf::Shape> shape;
        std::shared_ptr<sf::Texture> texture;
        sf::Color fillColor{ sf::Color::White };
        bool visible{ true };

        static Render Make(ShapeType type, const sf::Color& color, const std::string& texturePath = "");
    };
}