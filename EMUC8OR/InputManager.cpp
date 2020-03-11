#include "InputManager.h"


InputManager::InputManager()
{
	controls[0] = sf::Keyboard::Num1;
	controls[1] = sf::Keyboard::Num2;
	controls[2] = sf::Keyboard::Num3;
	controls[3] = sf::Keyboard::Num4;
	controls[4] = sf::Keyboard::Q;
	controls[5] = sf::Keyboard::W;
	controls[6] = sf::Keyboard::E;
	controls[7] = sf::Keyboard::R;
	controls[8] = sf::Keyboard::A;
	controls[9] = sf::Keyboard::S;
	controls[10] = sf::Keyboard::D;
	controls[11] = sf::Keyboard::F;
	controls[12] = sf::Keyboard::Z;
	controls[13] = sf::Keyboard::X;
	controls[14] = sf::Keyboard::C;
	controls[15] = sf::Keyboard::V;
}

bool InputManager::isPressed(unsigned int key)
{
	return pressed[key];
}

void InputManager::update()		//polling solution, not great
{
	for (int i = 0; i < 16; i++)
	{
		pressed[i] = sf::Keyboard::isKeyPressed(controls[i]);
	}
}