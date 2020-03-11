#ifndef RENDERER_H_
#define RENDERER_H_

using namespace std;

#include <vector>
#include "SFML\Graphics.hpp"

class Renderer
{
public:
	Renderer();
	Renderer(sf::Vector2i resolution);
	~Renderer();

	void setPixel(int x, int y, bool setting);    //Returns true if the pixel was changed, false otherwise
	void flip(int x, int y);
	void clear();    //Reset all pixels to black
	bool isActive(int x, int y);
	void draw(sf::RenderWindow &window);	//Draws the current frame
	sf::Vector2i getRez();

private:
	bool display [2048];    //Emulated pixels
	sf::Vector2i resolution;    //Resolution, in real pixels
	sf::RectangleShape pixel;
};

#endif