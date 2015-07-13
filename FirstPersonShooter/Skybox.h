#ifndef _SKYBOX_
#define _SKYBOX_
#include "Vector3D.h"
#include <Windows.h>
#include "HelperFunctions.h"

// We need to define this for glTexParameteri()
#define GL_CLAMP_TO_EDGE	0x812F						// This is for our skybox textures

class Skybox
{
public: 
	Skybox(); 
	~Skybox();

	//to easier choose which texture we want to use 
	enum {FRONT = 0, BACK = 1, LEFT = 2, RIGHT = 3, BOTTOM = 4, TOP = 5};
	//Create skybox at a position 
	void CreateSkyBox(float x, float y, float z, float width, float height, float length);
	void LoadTexture(LPWSTR front, LPWSTR back, LPWSTR left, LPWSTR right, LPWSTR bottom, LPWSTR top);

private: 
	UINT m_Textures[5]; //Holds texture of the skybox

};


#endif 
