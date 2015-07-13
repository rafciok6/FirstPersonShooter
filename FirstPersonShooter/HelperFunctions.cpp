#include "HelperFunctions.h"
#include <stdio.h> 

namespace Helper
{
	void CalculateFrameRate(HWND hwnd)
	{
		// Below we create a bunch of static variables because we want to keep the information
		// in these variables after the function quits.  We could make these global but that would
		// be somewhat messy and superfluous.  Note, that normally you don't want to display this to
		// the window title bar.  This is because it's slow and doesn't work in full screen.
		// Try using the 3D/2D font's.  Check out other tutorials which do this later.

		static float framesPerSecond = 0.0f;							// This will store our fps
		static float lastTime = 0.0f;							// This will hold the time from the last frame
		static char strFrameRate[50] = { 0 };								// We will store the string here for the window title
	

		// Here we get the current tick count and multiply it by 0.001 to convert it from milliseconds to seconds.
		// GetTickCount() returns milliseconds (1000 ms = 1 second) so we want something more intuitive to work with.
		float currentTime = GetTickCount() * 0.001f;

		// Increase the frame counter
		++framesPerSecond;

		// Now we want to subtract the current time by the last time that was stored.  If it is greater than 1
		// that means a second has passed and we need to display the new frame rate.  Of course, the first time
		// will always be greater than 1 because lastTime = 0.  The first second will NOT be true, but the remaining
		// ones will.  The 1.0 represents 1 second.  Let's say we got 12031 (12.031) from GetTickCount for the currentTime,
		// and the lastTime had 11230 (11.230).  Well, 12.031 - 11.230 = 0.801, which is NOT a full second.  So we try again
		// the next frame.  Once the currentTime - lastTime comes out to be greater than a second (> 1), we calculate the
		// frames for this last second.
		if (currentTime - lastTime > 1.0f)
		{
			// Here we set the lastTime to the currentTime.  This will be used as the starting point for the next second.
			// This is because GetTickCount() counts up, so we need to create a delta that subtract the current time from.
			lastTime = currentTime;

			// Copy the frames per second into a string to display in the window title bar
			sprintf_s(strFrameRate, "Current Frames Per Second: %d", int(framesPerSecond));

			wchar_t n[51];

			//to convert from char to wchar_t (unicode) 
			MultiByteToWideChar(0, 0, strFrameRate, 50, n, 51);
			// Set the window title bar to our string
			SetWindowText(hwnd, n);

			// Reset the frames per second
			framesPerSecond = 0;
		}
	}

	//This method creates a texture in OpenGL that we can texture map 
	//Create texture from the BitMap File 
	bool CreateTexture(GLuint &textureID, LPCWSTR szFileName)
	{
		HBITMAP	hBMP;  //handle to the bit map 
		BITMAP bitmap; //Bitmap structure 

		glGenTextures(1, &textureID);  //create the texture 
		hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

		if (!hBMP) //does the bitmap exist? 
		{
			return FALSE; //If not return false
		}

		GetObject(hBMP, sizeof(bitmap), &bitmap);  // Get The Object
												   // hBMP:        Handle To Graphics Object
												   // sizeof(bitmap): Size Of Buffer For Object Information
												   // &bitmap:        Buffer For Object Information

		//Pixel Storage Mode (World Alignment / 4 Bytes) 
		glPixelStorei(GL_TEXTURE_2D, textureID);

		//Typical Texture Generation Using Data From The Bitmap 
		glBindTexture(GL_TEXTURE_2D, textureID); //Bind To Texture ID
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  //Linear Min Filter 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //Linear Mag Filter
		glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.bmWidth, bitmap.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap.bmBits); 
																					  // MUST NOT BE INDEX BMP, BUT RGB
		DeleteObject(hBMP);                                                           // Delete The Object

		return TRUE;                                                                  // Loading Was Successful
	}
}