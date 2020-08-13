
#include <TGUI/TGUI.hpp>
#include <iostream>
#include <vector>
using namespace std;

#include "Tily.h"


int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Tile Map Editor");
    window.setFramerateLimit(60);

    sf::Texture tileMap_t;
    if(!tileMap_t.loadFromFile("res/img/World Tile Map.png")) return(-1);

    ty::Settings::TileSize = 32;
    ty::Settings::TextureSize = 736;
    ty::Settings::ChunkWidth = 16;
    ty::Settings::ChunkHeight = 16;

    ty::Map map;
    map.Create(100, 100, 1);

    map.Set(0, 0, 0, 66);

    int ID = 1;

    ////////////////// TGUI ////////////////////
    tgui::Gui gui{window};
    
    auto canvas_window = tgui::ChildWindow::create();
    canvas_window->setSize(400, 400);
    canvas_window->setPosition(50, 50);
    canvas_window->setResizable(true);
    gui.add(canvas_window);

    auto canvas_drawing = tgui::Canvas::create();
    canvas_window->setSize(300, 300);
    canvas_window->setPosition(0, 0);
    canvas_window->add(canvas_drawing);

    while(window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed) window.close();

            if(event.type == sf::Event::KeyPressed)
            {
                // do something..
            }
            gui.handleEvent(event);
        } 
        cout << canvas_window->getChildWidgetsOffset().y << endl;

        // check for mouse presses
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i mp = sf::Mouse::getPosition(window);
			sf::Vector2f ap = window.mapPixelToCoords(mp);
			map.Set(ap.x / ty::Settings::TileSize, ap.y / ty::Settings::TileSize, 0, ID);
		}
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            sf::Vector2f mouse = sf::Mouse::getPosition();

            sf::Vector2i ap = mouse - canvas_window->getPosition() - canvas_window->getWidgetOffset();

			map.Set(ap.x / ty::Settings::TileSize, ap.y / ty::Settings::TileSize, 0, 0);
		}


        // draw graphics
        window.clear(sf::Color(60, 60, 60));

        map.DrawLayer(0, canvas_drawing, sf::RenderStates(&tileMap_t));

        gui.draw();
        window.display();
    }

    return EXIT_SUCCESS;
}
