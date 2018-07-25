// Carcassonne.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vld.h"
#include <iostream>
#include <fstream>
#include <vector>
//#include <queue>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "GameObject.hpp"
#include "EmptyField.hpp"
#include "Tile.hpp"

#include "Creator.hpp"
#include "Engine.hpp"
#include "MouseHandler.hpp"
//#include "Game.hpp"
//#include "SideMenu.hpp"

using namespace std;

const float WINDOW_WIDTH = 1060.f;
const float WINDOW_HEIGHT = 740.f;
const float BOARD_WIDTH = 800.f;
const float BOARD_HEIGHT = 620.f;


/*
template<class T>
void show_shp(std::shared_ptr<T> &shp)
{
	std::cout << "Shared pointer: ";
	std::cout << shp.get() << std::endl;
	std::cout << "Use count: " << shp.use_count() << std::endl;
}
*/


int main()
{
	// Tworzymy okno i planszę
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Carcassonne prototype", sf::Style::Default);
    sf::Event evnt;

	sf::RenderTexture board;
	board.create(BOARD_WIDTH, BOARD_HEIGHT);
	ViewEngine::resizeView(1, board);
	ViewEngine::resizeView(1, board);
	

	//EmptyField sqfTab[Board::EF_X][Board::EF_Y];
	EmptyField::LoadTexture();
	EmptyField **EFTab = Creator::SetEmptyFields();


	std::vector<Tile> tileVector = Creator::LoadTileData();	
	int tileIterator = 0;

	// Stawiamy początkową płytkę
	int mid_val = Board::EF_X / 2;
	tileVector[0].Place(&EFTab[mid_val][mid_val]);
	Engine::AddAvailableEFs(EFTab[mid_val][mid_val]);
	tileIterator++;

	//sf::View view;
	//view.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//ViewEngine::pView = &view;
	//ViewEngine::resizeView((int)1, window);

	
	// Testowa miniaturka płytki
	GameObject tileThumbnail;
	tileThumbnail.setPosition(910.f, 270.f);
	tileThumbnail.setTexture(tileVector[tileIterator].getTexture());
	tileThumbnail.setSize(sf::Vector2f(140.f, 140.f));
	setOriginToCenter(tileThumbnail);

	// Cień płytki którą chcemy postawić
	GameObject tileShadow;
	tileShadow.setTexture(tileVector[tileIterator].getTexture());
	tileShadow.setSize(Board::Square);
	setOriginToCenter(tileShadow);
	sf::Sprite *tileShadow_s = tileShadow.getSprite();
	tileShadow_s->setColor(sf::Color(255, 255, 255, 120));

	
	// Sprite przechowujący planszę
	sf::Sprite board_s;
	board_s.setPosition(Board::boardOffset);

    while (window.isOpen())
    {
		while (window.pollEvent(evnt))
		{
			switch (evnt.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonReleased:
				MouseHandler::Release();
				break;
			case sf::Event::KeyReleased:
				KeyHandler::Release();
				break;
			case sf::Event::MouseWheelScrolled:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
				{
					int delta = evnt.mouseWheelScroll.delta;
					ViewEngine::resizeView(delta, board);
				}
				else
				{
					// Przed położeniem płytki (test)
					if (tileIterator < tileVector.size())
					{
						int delta = -evnt.mouseWheelScroll.delta;
						tileVector[tileIterator].Rotate(delta);
						Rotate(tileThumbnail, delta);
						Rotate(tileShadow, delta);
					}
				}
				break;

			}
		}

		board.clear(sf::Color(170, 170, 120));

		EmptyField::DrawAll(board);
		Tile::DrawAll(board);
		
		
		//for each (EmptyField *p_sqf in Engine::AvailableEFs)
		// - nie działa, gdyż w czasie wykonywania pętli
		// modyfikujemy wektor AvailableEFs!
		
		for each (EmptyField *EF in EmptyField::EF_vector)
			if (EF->clicked(board, window))
			{
				// Obsługa kliknięcia w dowolne pole
				if (EF->_plus != nullptr)

					if (tileIterator < tileVector.size())
					{
						if (Engine::BorderMatch(EF, tileVector[tileIterator]))
						{
							Engine::PlaceTheTile(EF, tileVector, tileIterator,
								tileThumbnail, tileShadow);
						}
						else
						{
							std::cout << "The tile cannot be placed." << std::endl;
						}
					}


				//-------------------------
			}
		
		// Rysujemy "plusy" na dostępnych polach
		Engine::DrawAvailableEFs(board);

		for each (EmptyField *EF in EmptyField::EF_vector)
			if (EF->mouseEntered(board, window))
			{
				// Obsługa najechania na pole myszą

				if (EF->_plus != nullptr)
				{
					tileShadow.setPosition(EF->getPosition());
					tileShadow.draw(board);
				}
			}




		board.display();

        window.clear(sf::Color(140, 70, 20));
		
		board_s.setTexture(board.getTexture());

		window.draw(board_s);

		tileThumbnail.draw(window); // Miniaturka płytki


		if (MouseHandler::buttonPressed(sf::Mouse::Left))
		{
			sf::Vector2f coord = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			std::cout << "(" << coord.x << ", " << coord.y << ")" << std::endl;
		}
				
		
		ViewEngine::moveView(board, window);
        window.display();
    }

	Creator::ClearEmptyFields(EFTab);
    return 0;
}

