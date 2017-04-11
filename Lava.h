#ifndef LAVA_H
#define LAVA_H
#include <vector>
#include <string>
#include "Timer.h"

using namespace std; 

class Lava
{
public:
	struct Map
	{
		wstring	filename; 

	};
	Lava();
	~Lava();

	//hämta raw filerna
	void LoadRawFile(); 

	void ReleaseAll(); 
private:
	Timer lavaTime; 

};


#endif
