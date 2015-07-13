#ifndef _TERRAIN_
#define _TERRAIN_ 
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h> 
#include "Camera.h"
#include "HelperFunctions.h"

#define MAP_SIZE 1024 //size of the raw file 
#define STEP_SIZE 16 //Width and height of each quad 
#define HEIGHT_RATIO 1.5f //this is the ratio that Y is scaled accordingly 


class Terrain
{
public: 
	Terrain(); 
	~Terrain(); 

	//Loads raw file of the specific size from the file
	void LoadRawFile(LPSTR fileName); 
	//turns the heighmap into the primitives and draws them to the screen
	void Render(); 

	//load the terrain texture 
	void LoadTexture(LPWSTR fileName);

	//check for camera collition wih the terrain at point  (if collide update camera position)
	void CheckForCollision(Camera *cam, int offset); 

	void SetWaterHeight(float height) { m_waterHeight = height; }
	void LoadWaterTexture(LPWSTR fileName); 

private: 
	//returns the height (0 to 255) from a heightmap given an X and Y 
	int Height(int x, int y); 
	//sets the color value for a particular index, depending on the height index
	void SetVertexColor(int x, int y); 
	//This sets the current texture coordinate of the terrain, based on the X and Z
	void SetTextureCoord(float x, float z); 

	void RenderWater(); 

	bool m_bAddWater; //true if water texture loaded 
	float m_waterHeight; // the height of the water 
	BYTE m_HeighMap[MAP_SIZE * MAP_SIZE]; // Holds the height map data 
	UINT m_TerrainTexture; //holds the texture of the terrain 
	UINT m_WaterTexture; //holds water texture 
};


#endif 