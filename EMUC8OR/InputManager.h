#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <SFML\System.hpp>
#include <SFML/Window.hpp>

class InputManager
{
public:
	InputManager();
	bool isPressed(unsigned int key);
	void update();

private:
	bool pressed[16];
	sf::Keyboard::Key controls[0xF];
};

#endif