#include <Windows.h> 
#include <gl\GL.h> 
#include <gl\GLU.h> 

namespace Helper
{
	//Calculate frame per seconds and disply it in the title bar in the windowed mode 
	void CalculateFrameRate(HWND hwnd); 
	
	//This method creates a texture in OpenGL that we can texture map 
	bool CreateTexture(GLuint &textureID, LPCWSTR szFileName);
}