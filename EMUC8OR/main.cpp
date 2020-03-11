#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "CPU.h"

#include <ctime>

int main(int argc, char* argv[])
{
	string path;
	if (argc == 1)
	{
		cout << "Filepath of Chip 8 application: ";
		cin >> path;
	}
	else
	{
		path = argv[1];
	}
	//path = "breakout.ch8";
	Renderer renderEngine;
	CPU chip8;
	chip8.loadApp(path.c_str());
    sf::RenderWindow window(sf::VideoMode(renderEngine.getRez().x, renderEngine.getRez().y), "EMUC8OR");
	chip8.printApp();
	cout << "Beginning emulation" << endl;
    while (window.isOpen())
    {
		clock_t start = clock();	//issues with SFML vsync
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
		chip8.emulateCycle();
		chip8.setKeys();
        window.clear();
		chip8.render(window);
		//while (clock() - start < 1/60.f);	//hacked framelimiter
        window.display();
    }

    return 0;
}