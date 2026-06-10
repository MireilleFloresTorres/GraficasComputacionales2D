#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"

/// @brief Puntero global a la ventana principal de la aplicación
Window* g_window = nullptr;
CShape Circle(ShapeType::CIRCLE); 
CShape line(ShapeType::LINE); 

sf::CircleShape shape(50.f); 

/** @brief Libera los recursos globales de la aplicación*/
void destroy() {
    SAFE_PTR_RELEASE(g_window);
}

/**
 * @brief Punto de entrada. Inicializa la ventana y ejecuta el game loop principal
 *
 * Ciclo:
 * 1. Procesa eventos (cierre de ventana)
 * 2. Limpia el buffer con negro
 * 3. Dibuja Circle y line
 * 4. Presenta el frame
 *
 * @return 0 al cerrar correctamente
 */
int main()
{
    /// @brief create the window
    g_window = new Window(800, 600, "My window");
  
    Circle.getShape()->setFillColor(sf::Color(100, 250, 50)); 

    /// @brief run the program as long as the window is open
    while (g_window->isOpen())
    {
        /// @brief check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = g_window->m_window->pollEvent())
        {
            /// @brief "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
            {
                g_window->close();
            }
        }

        /// @brief clear the window with black color
        g_window->clear(sf::Color::Black);

        /// @brief draw everything here...
        Circle.draw(*g_window); 
        line.draw(*g_window); 

        /// @brief end the current frame
        g_window->display();
    }

    destroy();
    return 0; 

}