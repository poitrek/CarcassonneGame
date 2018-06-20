// Carcassonne.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vld.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <random>
#include <chrono>
#include <regex>
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


///sf::Vector2f CoordOrigin{ 0.f, 0.f }; Niepotrzebne
// Początek układu współrzędnych - już nieaktualny
/*
template<class T>
void show_shp(std::shared_ptr<T> &shp)
{
	std::cout << "Shared pointer: ";
	std::cout << shp.get() << std::endl;
	std::cout << "Use count: " << shp.use_count() << std::endl;
}
*/

const std::string Creator::data_filename{ "../Carcassonne/Dane_v2.txt" };

void Creator::SetTileAttributes(Tile &tile, std::vector<int> &indx_vector, std::vector<int> &othr_vector)
{
	// Dla każdego indeksu dodajemy do listy plist
	// obiekt odpowiedniej klasy
	for (int indx : indx_vector)
	{		
		// Sposób 2
		switch (indx)
		{
		case 2:
		tile.plist.emplace_back(shared_ptr<Town>(new Town));
		break;
		case 1:
		tile.plist.emplace_back(shared_ptr<Trail>(new Trail));
		break;
		case 0:
		tile.plist.emplace_back(shared_ptr<Field>(new Field));
		break;
		default:
		break;
		}

	}	

	int i = othr_vector[0] - 1;

	tile.U = &tile.plist[i];  // Ustawiamy odpowiedni wskaźnik
	tile.U.type = Tile::E_Border(indx_vector[i]); // Odpowiedni typ granicy
	
	i = othr_vector[1] - 1;
	tile.R = &tile.plist[i];
	tile.R.type = Tile::E_Border(indx_vector[i]);

	i = othr_vector[2] - 1;
	tile.D = &tile.plist[i];
	tile.D.type = Tile::E_Border(indx_vector[i]);

	i = othr_vector[3] - 1;
	tile.L = &tile.plist[i];
	tile.L.type = Tile::E_Border(indx_vector[i]);
	
	/*
	cout << &tile.plist[0] << endl;
	cout << tile.U.p << endl;

	cout << "* "<< tile.plist[0] << endl;
	cout << "* " << *(tile.U) << endl;
	*/

	for (int j = 0; j < 8; j++)
	{
		
		i = othr_vector[j + 4] - 1;
		if (i == -1)
		{
			tile.f[j] = nullptr;
		}
		else
		{
			tile.f[j] = &tile.plist[i];
			// Nie wiem, czy aby na pewno nie spowoduje to niechcianych problemów
			// Jednak wątpię, aby to działało
			//tile.f[j] = &std::shared_ptr<Field>(dynamic_cast<Field*>(p));
		}
	
	}
	
	/* Kwestię konwersji możemy też ominąć, zmieniając typ
	shared_ptr<Field> *f[8]  na
	shared_ptr<Prop> *f[8]
	Niemniej możliwe, że i tak zajdzie w programie potrzeba
	użycia RTTI z inteligentnymi wskaźnikami
	*/
}


// Nowa wersja (nieużyta)
std::vector<Tile> loadTileDatav2()
{
	std::ifstream file;
	file.open(Creator::data_filename);
	if (!file.is_open())
	{
		std::cout << "Error: nie udalo sie otworzyc pliku z danymi" << std::endl;
		std::cin.get();
		exit(1);
	}

	std::vector<Tile> _tileVector;

	for (int _i = 0; _i < 10; _i++)
	{
		// Wersja z wyrażeniem regularnym
		string s = "x(\\d{1,2})\\s*(\\w+.(png|jpg|bmp))\\s*\\n";
		s += "(\\d\\s)+\\n";
		s += "(\\d\\s){4}\\n";
		s += "(\\d\\s){8}\\n";
		regex r(s);
		smatch sm;

		string _string;
			
		if (regex_match(_string, sm, r))
		{

		}

		//----------


		int num;
		file >> num;		
		
		std::string t_filename;
		file >> t_filename;
				
		std::vector<int> indx_vector;
		string buffer = "";
		while (buffer != "#")
		{
			file >> buffer;
			indx_vector.push_back(stoi(buffer));
		}
		std::vector<int> othr_vector;
		for (int i = 0; i < 12; i++)
		{
			int x; file >> x;
			othr_vector.push_back(x);
		}

		Tile tile(t_filename);
		Creator::SetTileAttributes(tile, indx_vector, othr_vector);

		for (int i = 0; i < num; i++)
			_tileVector.push_back(tile);	


	}

	return _tileVector;
}


std::vector<Tile> Creator::LoadTileData()
{
	std::ifstream file;
	file.open(Creator::data_filename);
	if (!file.is_open())
	{
		std::cout << "Error: nie udalo sie otworzyc pliku z danymi" << std::endl;
		std::cin.get();
		exit(1);
	}

	int numberOfTiles;
	file >> numberOfTiles;

	std::vector<Tile> _tileVector;
	for (int _it = 0; _it < numberOfTiles; _it++)
	{
		// 0. LICZBA KOPII PŁYTKI
		int copy_number{ 1 };
		
		{
			string s_cn;
			file >> s_cn;

			regex r("x(\\d{1,2})");
			smatch sm;
			if (regex_match(s_cn, sm, r))
			{
				copy_number = stoi(sm[1]);
			}
		}
		// 1. NAZWA PLIKU
		std::string t_filename;
		file >> t_filename;

		t_filename = "../Carcassonne/images/" + t_filename;

		Tile tile(t_filename);

		// 2. LISTA OBIEKTÓW
		std::string prop_list_s;
		std::getline(file, prop_list_s);
		std::getline(file, prop_list_s);
		std::istringstream iss(prop_list_s);

		std::vector<int> indx_temp; // Tymczasowy wektor pomocniczy,
						// przechowujący indeksy obiektów znajdujących
						//  się na odczytywanej płytce

		// Split'ujemy pobraną z pliku linię do wektora
		for (std::string s; iss >> s; indx_temp.push_back(stoi(s)));

		// Wypełniamy wstępnie wektor obiektów na płytce
		for (int indx : indx_temp)
		{
			switch (indx)
			{
			case 2:
				tile.plist.emplace_back(shared_ptr<Town>(new Town));
				break;
			case 1:
				tile.plist.emplace_back(shared_ptr<Trail>(new Trail));
				break;
			case 0:
				tile.plist.emplace_back(shared_ptr<Field>(new Field));
				break;
			default:
				break;
			}
		}
		
		// 3. U, R, D, L, u1 u2 r1 r2 d1 d2 l1 l2
		// Wczytujemy do kolejnego wektora 12 pozostałych parametrów
		// opisujących granice płytki
		
		/*
		std::vector<int> other_temp;
		for (int i = 0; i < 12; i++)
		{
			int x; file >> x;
			other_temp.push_back(x);
		}*/

		// Indeksy U, R, D, L
		file >> tile.iU >> tile.iR >> tile.iD >> tile.iL;
		tile.iU--; tile.iR--; tile.iD--; tile.iL--;
		// Typy granic U, R, D, L
		tile.U.type = Tile::E_Border(indx_temp[tile.iU]);
		tile.R.type = Tile::E_Border(indx_temp[tile.iR]);
		tile.D.type = Tile::E_Border(indx_temp[tile.iD]);
		tile.L.type = Tile::E_Border(indx_temp[tile.iL]);
		// Indeksy u1,u2,r1,...
		for (int i = 0; i < 8; i++)
		{
			file >> tile.iF[i];
			tile.iF[i]--;
		}

		//Creator::SetTileAttributes(tile, indx_temp, other_temp);		
		//tile.Show();

		for (int i = 0; i < copy_number; i++)
			_tileVector.push_back(tile);
	}

	file.close();


	// Mieszamy wektor z płytkami
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		
	std::shuffle(_tileVector.begin() + 1, _tileVector.end(),
		std::default_random_engine(seed));

	return _tileVector;
}

EmptyField** Creator::SetEmptyFields()
{
	EmptyField **EFtab = new EmptyField* [Board::EF_X];
	for (int i = 0; i < Board::EF_X; i++)
	{
		EFtab[i] = new EmptyField[Board::EF_Y];
	}
	// Pozycje
	sf::Vector2f sqfSize = EmptyField::getSize();
	sf::Vector2f buffer{ sqfSize*0.5f };
	for (int i = 0; i < Board::EF_X; i++)
		for (int j = 0; j < Board::EF_Y; j++)
		{
			// w celach testowych
			EFtab[i][j].indX = i;
			EFtab[i][j].indY = j;
			
			EFtab[i][j].tile = nullptr;
			// Poprawione indeksowanie
			EFtab[i][j].setPosition(buffer.x + j * sqfSize.x, buffer.y + i * sqfSize.y);	
		}

	// Sąsiednie pola
	for (int i = 1; i < Board::EF_X - 1; i++)
	{
		for (int j = 1; j < Board::EF_Y - 1; j++)
		{
			EFtab[i][j].Up = &EFtab[i - 1][j];
			EFtab[i][j].Down = &EFtab[i + 1][j];
			EFtab[i][j].Left = &EFtab[i][j - 1];
			EFtab[i][j].Right = &EFtab[i][j + 1];

		}
	}
	for (int i = 1; i < Board::EF_X - 1; i++)
	{
		EFtab[i][0].Up = &EFtab[i - 1][0];
		EFtab[i][0].Down = &EFtab[i + 1][0];
		//EFtab[i][0].Left = &EFtab[i][- 1];
		EFtab[i][0].Right = &EFtab[i][1];

		EFtab[i][Board::EF_Y - 1].Up = &EFtab[i - 1][Board::EF_Y - 1];
		EFtab[i][Board::EF_Y - 1].Down = &EFtab[i + 1][Board::EF_Y - 1];
		EFtab[i][Board::EF_Y - 1].Left = &EFtab[i][Board::EF_Y - 2];
		//EFtab[i][Board::EF_Y - 1].Right = &EFtab[i][Board::EF_Y];
	}
	for (int j = 1; j < Board::EF_Y - 1; j++)
	{
		//EFtab[0][j].Up = &EFtab[- 1][j];
		EFtab[0][j].Down = &EFtab[1][j];
		EFtab[0][j].Left = &EFtab[0][j - 1];
		EFtab[0][j].Right = &EFtab[0][j + 1];

		EFtab[Board::EF_X - 1][j].Up = &EFtab[Board::EF_X - 2][j];
		//EFtab[Board::EF_X - 1][j].Down = &EFtab[Board::EF_X][j];
		EFtab[Board::EF_X - 1][j].Left = &EFtab[Board::EF_X - 1][j - 1];
		EFtab[Board::EF_X - 1][j].Right = &EFtab[Board::EF_X - 1][j + 1];
	}
	{
		EFtab[0][0].Right = &EFtab[0][1];
		EFtab[0][0].Down = &EFtab[1][0];
		EFtab[0][Board::EF_Y - 1].Left = &EFtab[0][Board::EF_Y - 2];
		EFtab[0][Board::EF_Y - 1].Down = &EFtab[1][Board::EF_Y - 1];
		EFtab[Board::EF_X - 1][0].Up = &EFtab[Board::EF_X - 2][0];
		EFtab[Board::EF_X - 1][0].Right = &EFtab[Board::EF_X - 1][1];
		EFtab[Board::EF_X - 1][Board::EF_Y - 1].Up = &EFtab[Board::EF_X - 2][Board::EF_Y - 1];
		EFtab[Board::EF_X - 1][Board::EF_Y - 1].Left = &EFtab[Board::EF_X - 1][Board::EF_Y - 2];
	}

	return EFtab;
}

void Creator::ClearEmptyFields(EmptyField** &EFtab)
{
	for (int i = 0; i < Board::EF_X; i++)
	{
		delete[] EFtab[i];
	}
	delete[] EFtab;
}

int main2()
{
	std::cout << "--------------------------------" << std::endl;
	std::cout << "Hello world!" << std::endl;
	

	std::vector<Tile> tileVector = Creator::LoadTileData();



	tileVector.clear();
	std::cin.get();
	return 0;
}



int main()
{
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
		
		
		switch (Engine::GameState)
		{
		case Engine::placingTile:


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


			break;
		case Engine::ADDITIONAL_1:

			Engine::PlaceThePawn2(board, window);

			break;
		case Engine::placingPawn:
			Engine::PlaceThePawn3(board, window);
			break;
		default:
			break;
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
		
		// Wypisz pozycje pól planszy
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			/*
			for (int i = 0; i < Board::EF_X - 5; i++)
			{
				for (int j = 0; j < Board::EF_Y - 3; j++)
				{
					sf::Vector2f pos = sqfTab[i][j].getPosition();
					std::cout << "(" << pos.x << ", " << pos.y << ") ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl << std::endl;
			*/
			for (int i = 0; i < Board::EF_X; i++)
			{
				for (int j = 0; j < Board::EF_Y; j++)
				{
					if (EFTab[i][j].tile != nullptr)
						cout << " t";
					else
						cout << "  ";
				}
				cout << endl;
			}
		}
		
		ViewEngine::moveView(board, window);
        window.display();
    }

	Creator::ClearEmptyFields(EFTab);
    return 0;
}

