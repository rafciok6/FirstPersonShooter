#include "GLWindow.h"
#include <Windows.h> 
#include <stdio.h> 
#include <stdlib.h> 







int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int iShow)
{
	GLWindow mainWindow; 

	//Ask If Full Screen 
	if (MessageBox(NULL, TEXT("Yes to FullScreen Mode"), TEXT("Options"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		mainWindow.SetFullScreen(true); 
	}

	mainWindow.CreateGLWindow(L"Pierwsze Okno", SCREEN_WIDTH, SCREEN_HEIGHT, 0, hInstance); 

	mainWindow.Init(); 
	return mainWindow.MainLoop(); 

}