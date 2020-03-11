#include "CPU.h"

#define debugging (0)

unsigned char systemFont[80] = 
{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

CPU::CPU(void)
{
	this->init();
	if (!buff.loadFromFile("sfx.wav"))
	{
		cout << "ERROR: could not locate file 'sfx.wav'" << endl;
		cout << "Please ensure that this file exists" << endl;
	}
	else
	{
		beep.setBuffer(buff);
		beep.setVolume(1);
	}
}

void CPU::init()
{
	renderEngine.clear();
	pc = 0x200;    //Reset program counter
	opcode = 0;    //clear the current opcode;
	I = 0;    //Clear the index register
	sp = 0;    //Clear the stack pointer
	appSize = 0;
	for (int i = 0; i < 4096; i++)    //Clear memory and load system font
	{
		if ( i < 80)
		{
			memory[i] = systemFont[i];
		}
		else
		{
			memory[i] = 0;
		}
	}
	for (int i = 0; i < 16; i++)    //Clear registers, stack, and input
	{
		V[i] = 0;
		stack[i] = 0;
		key[i] = 0;
	}
}

void CPU::render(sf::RenderWindow &window)
{
	renderEngine.draw(window);
}

bool CPU::loadApp(const char *path)
{
	this->init();    //Reset the emulator
	printf("Loading: %s\n", path);    //Print the filepath
	FILE * pFile = fopen(path, "rb");    //Open the file
	if (pFile == NULL)    //File not found
	{
		fputs ("File error", stderr); 
		return false;
	}
	fseek(pFile , 0 , SEEK_END);    //Find the end of the file
	appSize = ftell(pFile);    //Current position, in this case the end of the file
	rewind(pFile);    //Go back to the beginning
	printf("Filesize: %d bytes\n", (int)appSize);    //Print the file size
	char * buffer = (char*)malloc(sizeof(char) * appSize);    //malloc a buffer to load the data into
	if (buffer == NULL)    //Ya done messed up
	{
		fputs ("Memory error", stderr); 
		return false;
	}
	size_t result = fread (buffer, 1, appSize, pFile);    //Read in the file
	if (result != appSize)	//Expected size did not match actual size
	{
		fputs("Reading error", stderr); 
		return false;
	}
	if((4096-512) > appSize)    //Check the program size
	{
		for(int i = 0; i < appSize; i++)    //Load into memory
			memory[i + 512] = buffer[i];
	}
	else
	{
		printf("Error: ROM too big for memory");
	}
	fclose(pFile);   //Close the file
	free(buffer);    //Give the memory for the buffer back

	return true;
}

void CPU::emulateCycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];    //Fetch opcode
	if debugging printf("0x%x\n", opcode);
	unsigned short x = 0, y = 0;    //For the draw functions
	bool keypress = false;    //For input functions
	//Add decoding stuff here
	switch(opcode & 0xF000)
	{
	case 0x0000:
		switch(opcode & 0x00FF)
		{
		case 0x00E0:
			renderEngine.clear();
			break;
		case 0x00EE:
			--sp;
			pc = stack[sp];
			break;
		default:
			//cout << "Error, unrecognized opcode" << endl;
			break;
		}
		pc += 2;
		break;
	case 0x1000:    //1NNN: Jump to address NNN
		pc = opcode & 0x0FFF;
		break;
	case 0x2000:    //2NNN: Call subroutine at NNN
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		break;
	case 0x3000:    //3XNN: Skip the nex instruction if VX = NN
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		{
			pc += 2;
		}
		pc += 2;
		break;
	case 0x4000:    //4XNN: Skips the next instruction if VX != NN
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		{
			pc += 2;
		}
		pc += 2;
		break;
	case 0x5000:    //5XY0: Skips the nex instrucion if VX = VY
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
		{
			pc += 2;
		}
		pc += 2;
		break;
	case 0x6000:    //6XNN: Sets VX = NN
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		pc += 2;
		break;
	case 0x7000:    //7XNN: Adds NN to VX
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		pc += 2;
		break;
	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0001:
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0002:
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0003:
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0004:
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
			{
				V[0xF] = 1;
			}
			else
			{
				V[0xF] = 0;
			}
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0005:
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
			{
				V[0xF] = 0;
			}
			else
			{
				V[0xF] = 1;
			}
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0006:
			V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x1);
			V[(opcode & 0x0F00) >> 8] >>= 1;
			pc += 2;
			break;
		case 0x0007:
			if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// VY-VX
			{
				V[0xF] = 0; // there is a borrow
			}
			else
			{
				V[0xF] = 1;
			}
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];				
			pc += 2;
			break;
		case 0x000E:
			V[0xF] = (V[(opcode & 0x0F00) >> 8] >> 7);
			V[(opcode & 0x0F00) >> 8] <<= 1;
			pc += 2;
			break;
		default:
			cout << "Unrecognized opcode " << opcode << endl;
		}
		break;
	case 0x9000:    //9XY0: Skips the next instruction if VX != VY
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode &0x00F0) >> 4])
		{
			pc += 2;
		}
		pc += 2;
		break;
	case 0xA000:    //ANNN: Sets the index register to NNN
		I = (opcode & 0x0FFF);
		pc += 2;
		break;
	case 0xB000:
		pc = (opcode & 0x0FFF) + V[0];
		break;
	case 0xC000:
		V[(opcode & 0x0F00) >> 8] = ((rand() % 0xFF) & (opcode & 0x00FF));
		pc += 2;
		break;
	case 0xD000:    //DXYN: Draw a sprite at coordinate (VX, VY), width 8 pixels, height N pixels
		x = V[(opcode & 0x0F00) >> 8];
		y = V[(opcode & 0x00F0) >> 4];
		V[0xf] = 0;
		for (int i = 0; i < (opcode & 0x000F); i++)
		{
			int row = memory[I + i];
			for (int j = 0; j < 8; j++)
			{
				if (((row & (0x80 >> j))) != 0)
				{
					if (renderEngine.isActive(x + j, y + i))
					{
						V[15] = 1;
					}
					renderEngine.flip(x + j, y + i);
				}
			}
		}
		pc += 2;
		break;
	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E:
			//printf("Press key 0x%x\n", V[(opcode & 0x0F00) >> 8]);
			if (input.isPressed(V[(opcode & 0x0F00) >> 8]))
			{
				pc += 2;
			}
			break;
		case 0x00A1:
			if (!input.isPressed(V[(opcode & 0x0F00) >> 8]))
			{
				pc += 2;
			}
			break;
		default:
		    printf("Unrecognized opcode 0x%x\n", opcode);
		    break;
		}
		pc += 2;
		break;
	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007:
			V[(opcode & 0x0F00) >> 8] = delayTimer;
			break;
		case 0x000A:
			keypress = false;
				input.update();
				for (int i = 0; i < 16; i++)
				{
					if (input.isPressed(i))
					{
						V[(opcode & 0x0F00) >> 8] = i;
						keypress = true;
						break;
					}
				}
				if (!keypress)
					return;
			break;
		case 0x0015:
			delayTimer = V[(opcode & 0x0F00) >> 8];
			break;
		case 0x0018:
			soundTimer = V[(opcode & 0x0F00) >> 8];
			break;
		case 0x001E:
			I += V[(opcode & 0x0F00) >> 8];
			break;
		case 0x0029:
			I = V[(opcode & 0x0F00) >> 8] * 0x5;
			break;
		case 0x0033:
			memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;	
			break;
		case 0x0055:
			for (int i = 0; i < (opcode & 0x0F00) >> 8; i++)
			{
				memory[i + I] = V[i];
			}
			I += ((opcode & 0x0F00) >> 8) + 1;
			break;
		case 0x0065:
			for (int i = 0; i < (opcode & 0x0F00) >> 8; i++)
			{
				V[i] = memory[i + I];
			}
			I += ((opcode & 0x0F00) >> 8) + 1;
			break;
		}
		pc += 2;
		break;
	default:
		printf("Unrecognized opcode 0x%x\n", opcode);
		break;
	}

	if (delayTimer > 0)
	{
		--delayTimer;
	}
	if (soundTimer > 0)
	{
		if (soundTimer == 1)
		{
			if debugging cout << "BEEP!" << endl;
			beep.play();
		}
		--soundTimer;
	}
}

void CPU::setKeys()
{
	input.update();
}

void CPU::printApp()
{
	int temp = 0;
	for(int i = 0x200; i < 0x200 + appSize; i += 2)
	{
		temp = memory[i] << 8 | memory[i+1];
		printf("0x%x\n", temp);
	}
}

CPU::~CPU()
{
}
