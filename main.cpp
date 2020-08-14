#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <iostream>
#include <vector>
using namespace std;

#include "includes/Tily.h"

tgui::ChildWindow::Ptr initTileMapPalette(sf::Texture& tileMap)
{
	auto window = tgui::ChildWindow::create();
	auto tilePalette = tgui::Grid::create();

	return window;
}

sf::Vector2f getRelativeCoords(sf::Vector2f position, sf::Vector2i mouse)
{
    return sf::Vector2f(mouse.x - position.x, mouse.y - position.y);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Tile Map Editor");
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

    map.setPosition(100, 100);

    sf::RectangleShape background(sf::Vector2f(500, 500));


    ////////////////// TGUI ////////////////////
    tgui::Gui gui{window};
    
    auto canvas_window = tgui::ChildWindow::create();
    canvas_window->setSize(800, 600);
    canvas_window->setPosition(50, 50);
    canvas_window->setResizable(true);
    gui.add(canvas_window);

    auto canvas_drawing = tgui::Canvas::create();
    canvas_drawing->setSize(canvas_window->getSize());
    canvas_drawing->setPosition(0, 0);
    canvas_window->add(canvas_drawing);

    //////// some variables //////
    sf::Vector2i mouse = sf::Mouse::getPosition(window);

    map.setPosition(50, 50);

    while(window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed) window.close();

            if(event.type == sf::Event::MouseMoved) mouse = sf::Mouse::getPosition(window);

            gui.handleEvent(event);
        } 
        // check for mouse presses
        
        if(canvas_drawing->mouseOnWidget(sf::Vector2f(mouse.x, mouse.y)))
        {
            sf::Vector2f canvas_drawing_position = canvas_window->getPosition() + canvas_window->getChildWidgetsOffset();
            sf::Vector2f mouse_relative_to_canvas_window = getRelativeCoords(canvas_drawing_position, mouse);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                sf::Vector2f mouse_relative_to_tile_map = getRelativeCoords(map.getPosition(), sf::Vector2i(mouse_relative_to_canvas_window.x, mouse_relative_to_canvas_window.y));
                map.Set(mouse_relative_to_canvas_window.x / ty::Settings::TileSize, mouse_relative_to_canvas_window.y / ty::Settings::TileSize, 0, 2);
            }
            else if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                map.Set(mouse_relative_to_canvas_window.x / ty::Settings::TileSize, mouse_relative_to_canvas_window.y / ty::Settings::TileSize, 0, 1);
            }
            else if(sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
                map.setPosition(mouse_relative_to_canvas_window);
            }
        }
        

    
    

        window.clear(sf::Color(60, 60, 60)); // clear the screen

        canvas_drawing->draw(background);

        map.DrawLayer(0, canvas_drawing, sf::RenderStates(&tileMap_t)); // draw the tile map

        gui.draw(); // draw gui like buttons

        window.display(); // update the screen
    }

    return EXIT_SUCCESS;
}
