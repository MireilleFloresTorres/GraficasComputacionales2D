#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"

Window* g_window = nullptr;
CShape Circle(ShapeType::CIRCLE); 
CShape line(ShapeType::LINE); 

sf::CircleShape shape(50.f); 

void destroy() {
    SAFE_PTR_RELEASE(g_window);
}

int main()
{
    // create the window
    g_window = new Window(800, 600, "My window");
  
    Circle.getShape()->setFillColor(sf::Color(100, 250, 50)); 

    // run the program as long as the window is open
    while (g_window->isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = g_window->m_window->pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
            {
                g_window->close();
            }
        }

        //sf::CircleShape shape(50.f);
        // set the shape color to green
        //shape.setFillColor(sf::Color(100, 250, 50));

        // clear the window with black color
        g_window->clear(sf::Color::Black);

        // draw everything here...
        Circle.draw(*g_window); 
        line.draw(*g_window); 

        // end the current frame
        g_window->display();
    }

    destroy();
    return 0; 

}