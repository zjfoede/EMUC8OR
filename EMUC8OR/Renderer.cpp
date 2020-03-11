#include "Renderer.h"

#include <iostream>

#define defaultPixelSize 12

Renderer::Renderer(sf::Vector2i resolution)    //Under development
{
	this->resolution = resolution;    //Initialize resolution
	int pixelX = resolution.x / 64;    //Set "pixel" sizes
	int pixelY = resolution.y / 32;
	pixel.setFillColor(sf::Color::White);
	pixel.setSize(sf::Vector2f(pixelX, pixelY));
	pixel.setPosition(0, 0);
	for (int i = 0; i < 2048; i++)    //Initialize screen to black
	{
		display[i] = false;
	}
}

Renderer::Renderer()
{
	this->resolution = sf::Vector2i(64*defaultPixelSize, 32*defaultPixelSize);    //Initialize resolution
	pixel.setSize(sf::Vector2f(defaultPixelSize, defaultPixelSize));
	pixel.setFillColor(sf::Color::White);
	for (int i = 0; i < 2048; i++)    //Initialize screen to black
	{
		display[i] = false;
	}
}

Renderer::~Renderer()
{
	//free(display);
}

/**
* Sets the pixel at the virtual location (x, y) to white if setting is true, black if setting is false
*/
void Renderer::setPixel(int x, int y, bool setting)
{
	x %= 64;
	y %= 32;
	int index = 64 * y + x;
	display[index] = setting;
}

void Renderer::flip(int x, int y)
{
	x %= 64;
	y %= 32;
	int index = 64 * y + x;
	display[index] ^= 1;
}

void Renderer::clear()
{
	for (int i = 0; i < 2048; i++)
	{
		display[i] = false;
	}
}

bool Renderer::isActive(int x, int y)
{
	x %= 64;
	y %= 32;
	int index = 64 * y + x;
	return (display[index]);
}

void Renderer::draw(sf::RenderWindow &window)
{
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (display[(64 * y) + x])
			{
				sf::Vector2f realPos(x * pixel.getSize().x, y * pixel.getSize().y);
				pixel.setPosition(realPos);
				window.draw(pixel);
			}
		}
	}
	pixel.setPosition(0, 0);
}

sf::Vector2i Renderer::getRez()
{
	return resolution;
}