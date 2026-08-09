#include "ECS/Components/Render.h"

namespace ECS {

    Render Render::Make(ShapeType type, const sf::Color& color, const std::string& texturePath) {
        std::cerr << "DEBUG texturePath recibido: [" << texturePath << "]\n";
        Render render;
        render.fillColor = color;

        switch (type) {
        case CIRCLE: {
            auto circle = std::make_shared<sf::CircleShape>(30.f);
            circle->setOrigin({ 30.f, 30.f });
            render.shape = circle;
            break;
        }
        case RECTANGLE: {
            auto rect = std::make_shared<sf::RectangleShape>(sf::Vector2f{ 60.f, 40.f });
            rect->setOrigin({ 30.f, 20.f });
            render.shape = rect;
            break;
        }
        case TRIANGLE: {
            auto tri = std::make_shared<sf::ConvexShape>(3);
            tri->setPoint(0, { 0.f, -30.f });
            tri->setPoint(1, { -25.f, 25.f });
            tri->setPoint(2, { 25.f, 25.f });
            tri->setOrigin({ 0.f, 5.f });
            render.shape = tri;
            break;
        }
        default:
            break;
        }
        if (!texturePath.empty()) {
            render.texture = std::make_shared<sf::Texture>();
            if (render.texture->loadFromFile(texturePath)) {
                render.shape->setTexture(render.texture.get());
            }
            else {
                std::cerr << "Ruta que fallo: " << texturePath << "\n";
                ERROR("Render", "Make", "No se pudo cargar la textura");
                render.texture.reset();
            }
        }

        return render;
    }

}