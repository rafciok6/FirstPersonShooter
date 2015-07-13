#ifndef _GLWINDOW_
#define _GLWINDOW_
#include <Windows.h> 
#include "Camera.h"
#include "Config.h"
#include "Scene.h"



class GLWindow
{
public: 
	GLWindow(); 
	GLWindow(LPWSTR windowName, int width, int height, DWORD dwStyle, HINSTANCE hInstance);
	~GLWindow(); 
	HWND CreateGLWindow(LPWSTR windowName, int width, int height, DWORD dwStyle, HINSTANCE hInstance);
	WPARAM MainLoop(); 
	void RenderScene();
	void DestroyGLWindow();
	void SetFullScreen(bool fullScreen);
	//Init the main game window 
	void Init(); 

	

	//Handle all of the messages to this window 
	LRESULT  WinProc( UINT message, WPARAM wParam, LPARAM lParam);
	//_________________________________________________


private: 	
	//was private 
	void ChangeToFullScreen(); 
	//Set Pixel Format
	bool SetupPixelFormat(); 
	//Resize viewport for OpenGL 
	void ResizeOpenGLScreen(int width, int height); 
	//All Initialization for OpenGL
	void InitOpenGL(int width, int height);  
	////Hnadle User Input 
	//void CheckForInput();
	//Here create the long Ptr to handle WinProc for this window 
	static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 

//	Camera m_camera; //Main Camera
	HINSTANCE m_hInstance; //hold the hInstance of the window 
	HDC m_hdc; //device handle 
	HGLRC m_hRc; //rendering context for OpenGL 
	RECT m_windowRect;  // This holds the window dimensions
	bool m_FullScreen; //Is full screen 

	Scene m_Scene; 
	//UINT g_Texture[1];					// This is our texture data array


protected: 
	HWND m_hWnd; // handle to the main window 

};




#endif _GLWINDOW_