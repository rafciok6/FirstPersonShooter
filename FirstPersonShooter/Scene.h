#ifndef _SCENE_
#define _SCENE_
#include <Windows.h> 
#include "Terrain.h"
#include "Skybox.h"

class Scene
{
public:
	Scene();
	~Scene();

	void Init();
	void Render();

private:
	//drawing the green lines as a ground 
	void DrawGroundLines(); 

	UINT g_Texture[1];					// This is our texture data array
	Terrain* m_pTerrain; 
	Camera m_camera; //Main Camera
	Skybox m_Skybox; 
};


#endif