#ifndef CPU_H_
#define CPU_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <bitset>
#include <cstdio>
#include <cmath>

#include "SFML/Audio.hpp"

#include "Renderer.h"
#include "InputManager.h"

using namespace std;

class CPU
{
public:
	CPU(void);
	~CPU(void);
	void render(sf::RenderWindow &window);
	void init();
	bool loadApp(const char *path);
	void emulateCycle();
	void setKeys();
	void printApp();

private:
	Renderer renderEngine;
	InputManager input;
	unsigned short opcode;
	unsigned char memory[4096];	//System memory
	unsigned char V[16];	//CPU registers
	unsigned short I;	//Index register
	unsigned short pc;	//Program counter
	unsigned char delayTimer;
	unsigned char soundTimer;
	unsigned short stack[16];	//Stack for program counter
	unsigned short sp;	//Stack pointer
	unsigned char key[16];
	unsigned long appSize;
	sf::SoundBuffer buff;
	sf::Sound beep;
};

#endif