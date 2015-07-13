#include "GLWindow.h"
#include "HelperFunctions.h"

#include <gl\GLU.h> 
#include <gl\GL.h> 
#include <cassert> 
#include <Windows.h> 
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

GLWindow::GLWindow()
{

}

GLWindow::GLWindow(LPWSTR windowName, int width, int height, DWORD dwStyle, HINSTANCE hInstance)
{
	CreateGLWindow(windowName, width, height, dwStyle, hInstance); 
}

GLWindow::~GLWindow()
{
	//DestroyGLWindow(); 
}

void GLWindow::DestroyGLWindow()
{
	if (m_hRc)
	{
		//Free rendering memory 
		wglMakeCurrent(NULL, NULL);
		//Delete OpenGL rendering context 
		wglDeleteContext(m_hRc); 
	}
	if (m_hdc)
	{
		//release HDC
		ReleaseDC(m_hWnd,m_hdc);
	}
	if (m_FullScreen)
	{
		//Switch back to desktop 
		ChangeDisplaySettings(NULL, 0); 
		//Show Mouse Pointer 
		ShowCursor(TRUE);
	}

	UnregisterClass(L"GLWindow", m_hInstance); // Free the window class 
	PostQuitMessage(0); //Post a QUIT Message to the window 

}

HWND GLWindow::CreateGLWindow(LPWSTR windowName, int width, int height, DWORD dwStyle, HINSTANCE hInstance)
{
	WNDCLASS wndClass;

	memset(&wndClass, 0, sizeof(WNDCLASS));   //init the size of the class 
	wndClass.style = CS_HREDRAW | CS_VREDRAW; //regular drawing capabilities
	wndClass.lpfnWndProc = GLWindow::s_WndProc; 
	wndClass.hInstance = hInstance; //assign the instance 
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //whiteBackground
	wndClass.lpszClassName = L"GLWindow";  //className

	RegisterClass(&wndClass); 

	//if full screen set the style 
	if (m_FullScreen && !dwStyle)
	{
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen(); 
		ShowCursor(FALSE);
	}
	else if (!dwStyle)
	{
		dwStyle = WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_CLIPCHILDREN; 
	}

	m_hInstance = hInstance; 


	//Adjust window rectangle to requested size 
	RECT windowRect; 
	windowRect.left = 0; 
	windowRect.right = width; 
	windowRect.top = 0; 
	windowRect.bottom = height; 

	//Adjust window to the requested size
	AdjustWindowRect(&windowRect, dwStyle, false); 

	//Create Window
	m_hWnd = CreateWindow(L"GLWindow", windowName, dwStyle, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL, NULL, hInstance, this); 

	if (!m_hWnd) //if we cannot get a handle  return null 
		return NULL; 

	ShowWindow(m_hWnd, SW_SHOWNORMAL); //show window 
	UpdateWindow(m_hWnd); 

	SetFocus(m_hWnd); // Sets keyboard focus to the window

	return m_hWnd; 
}

WPARAM GLWindow::MainLoop()
{
	MSG msg; 

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break; 
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
		else
		{
			//m_camera.SetViewByMouse(); //move camera by mouse 
			//CheckForInput(); 
			//m_camera.Update(); 
			//Computationally expensive things should go here 
			RenderScene(); 
		}
	}
	//crean everything from opengl 
	DestroyGLWindow();
	return (msg.wParam); 
}

bool GLWindow::SetupPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd = { 0 }; 
	int pixelFormat; 

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); //set size
	pfd.nVersion = 1; //always 1 
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; // pas opengl flags
	pfd.dwLayerMask = PFD_MAIN_PLANE; 
	pfd.iPixelType = PFD_TYPE_RGBA; 
	pfd.cColorBits = SCREEN_DEPTH; //set color bits
	pfd.cDepthBits = SCREEN_DEPTH; //is ignored for RGBA
	pfd.cAccumBits = 0; 
	pfd.cStencilBits = 0; 

	//Get pixel format that best mactches the one passed in form the device
	if ((pixelFormat = ChoosePixelFormat(m_hdc, &pfd)) == FALSE)
	{
		MessageBox(NULL, TEXT("Choose Pixel Format Failed"), TEXT("Error"), MB_OK); 
		return FALSE; 
	}

	//Set the pixel format choosed from above 
	if (SetPixelFormat(m_hdc, pixelFormat, &pfd) == FALSE)
	{
		MessageBox(NULL, TEXT("Choose Pixel Format Failed"), TEXT("Error"), MB_OK);
		return FALSE; 
	}

	return TRUE;
}

void GLWindow::ResizeOpenGLScreen(int width, int height)
{
	//Prevent divide by 0 error 
	if (height == 0)
	{
		height = 1; 
	}
	glViewport(0, 0, width, height); 
	// Make our viewport the whole window.
	// We could make the view smaller inside
	// Our window if we wanted too.
	// The glViewport takes (x, y, width, height).
	// This basically means, what are our drawing boundaries
	// on the screen in client coordinates (you can have like 4
	// viewports rendering different things in one window, like 3DS Max.

	//Select Projection Matrix 
	glMatrixMode(GL_PROJECTION); 
	//reset the matrix 
	glLoadIdentity(); 

	// Calculate The Aspect Ratio Of The Window
	// The parameters are:
	// (view angle, aspect ratio of the width to the height, 
	//  the closest distance to the camera before it clips, 
	// FOV		// Ratio				//  the farthest distance before it stops drawing).

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 4, 4000.0f);
	// * Note * - The farthest distance should be at least 1 if you don't want some
	// funny artifacts when dealing with lighting and distance polygons.  This is a special
	// thing that not many people know about.  If it's less than 1 it creates little flashes
	// on far away polygons when lighting is enabled.

	glMatrixMode(GL_MODELVIEW); //select the modelview matrix 
	glLoadIdentity(); //reset the matrix 


}

void GLWindow::InitOpenGL(int width, int height)
{
	//set HDC (don't free it until end of the program) 
	m_hdc = GetDC(m_hWnd);

	//set pixel format 
	if (!SetupPixelFormat())
	{
		PostQuitMessage(0); //If error quit 
	}

	// We need now to create a rendering context AFTER we setup the pixel format.
	// A rendering context is different than a device context (hdc), but that is
	// what OpenGL uses to draw/render to.  Because OpenGL can be used on
	// Macs/Linux/Windows/etc.. It has it's on type of rendering context that is
	// The same for EACH operating system, but it piggy backs our HDC information.
	m_hRc = wglCreateContext(m_hdc);					// This creates a rendering context from our hdc
	wglMakeCurrent(m_hdc, m_hRc);						// This makes the rendering context we just created the one we want to use.

	//remove the error when primitives from far away will be drawn over the closest ones
	glEnable(GL_DEPTH_TEST); 
	//This allow us to use texture mapping otherwise is just a color 
	glEnable(GL_TEXTURE_2D); //Enable Texture Mapping 

	ResizeOpenGLScreen(width, height);					// Setup the screen translations and viewport
}

void GLWindow::Init()
{
	GetClientRect(m_hWnd, &m_windowRect);					// Assign the windows rectangle to a global RECT
	InitOpenGL(m_windowRect.right, m_windowRect.bottom); // Init OpenGL with the global rect


	//m_camera.PositionCamera(Vector3D(0, 0.5f, 6), Vector3D(0, 0.5, 0), Vector3D(0, 1, 0)); 


	m_Scene.Init(); 
	// This is where we load all of our textures.  We are loading
	// just one in this case, but you can add many more if you want.
	// We pass in our global texture array, our file we want to load, and the texture ID we want associated with it
	// as an array index 0.  When it comes out, the g_Texture will now reference our texture map.

	//Helper::CreateTexture(g_Texture[0], L"brickwall_1.bmp");			// Load "bitmap.bmp" into openGL as a texture


	// *Hint* We will put all our game/program init stuff here.
	// Some things include loading models, textures and network initialization.
}

void GLWindow::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The View

//	m_camera.Look(); 
	/*gluLookAt(m_camera.GetPositionVector().m_x, m_camera.GetPositionVector().m_y, m_camera.GetPositionVector().m_z,
		m_camera.GetViewVector().m_x, m_camera.GetViewVector().m_y, m_camera.GetViewVector().m_z,
		m_camera.GetUpVector().m_x, m_camera.GetUpVector().m_y, m_camera.GetUpVector().m_z); */
	m_Scene.Render(); 
										// Stop drawing QUADS

	SwapBuffers(m_hdc);									// Swap the backbuffers to the foreground

	Helper::CalculateFrameRate(m_hWnd); 
}

void GLWindow::ChangeToFullScreen()
{

	DEVMODE dmSettings; //Device Mode variable 
	memset(&dmSettings, 0, sizeof(dmSettings)); //make sure memory is clear 


	// Get current settings -- This function fills in our settings.
	// This makes sure NT and Win98 machines change correctly.
	if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmSettings))
	{
		// Display error message if we couldn't get display settings
		MessageBox(NULL, TEXT("Could Not Enum Display Settings"), TEXT("Error"), MB_OK);
		return;
	}

	dmSettings.dmPelsWidth = SCREEN_WIDTH; //select window width 
	dmSettings.dmPelsHeight = SCREEN_HEIGHT; //select window height

	// This function actually changes the screen to full screen.
	// CDS_FULLSCREEN gets rid of the start Bar.
	// We always want to get a result from this function to check if we failed.
	int result = ChangeDisplaySettings(&dmSettings, CDS_FULLSCREEN);

	// Check if we didn't receive a good return message from the function
	if (result != DISP_CHANGE_SUCCESSFUL)
	{
		// Display the error message and quit the program
		MessageBox(NULL, TEXT("Display Mode Not Compatible"), TEXT("Error"), MB_OK);
		PostQuitMessage(0);
	}
}

void GLWindow::SetFullScreen(bool fullScreen)
{
	m_FullScreen = fullScreen;
}

//void GLWindow::CheckForInput()
//{
//	if ((GetKeyState(VK_UP) & 0x80) || (GetKeyState('W') & 0x80)) //0x80 means that button is down 
//	{
//		//move camera forward 
//		m_camera.MoveCamera(CAMERA_SPEED); 
//	}
//	if ((GetKeyState(VK_DOWN) & 0x80) || (GetKeyState('S') & 0x80))
//	{
//		//move camera down 
//		m_camera.MoveCamera(-CAMERA_SPEED);
//	}
//	//if (GetKeyState(VK_LEFT) & 0x80)
//	//{
//	//	//rotate camera left by positive speed 
//	//	m_camera.RotateView(CAMERA_SPEED, 0, 1, 0);
//	//}
//	//if (GetKeyState(VK_RIGHT) & 0x80)
//	//{
//	//	//rotate camera right by negative speed
//	//	m_camera.RotateView(-CAMERA_SPEED, 0, 1, 0);
//	//}
//}


//Set the pointer to the winproc. Remember YOU NEED TO PASS "this" As A LATS PARAMETER IN WindowCreate function 
LRESULT CALLBACK GLWindow::s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GLWindow * self;
	if (uMsg == WM_NCCREATE)
	{
		//self = (GLWindow *)lParam; 
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam); 
		self = static_cast<GLWindow *>(lpcs->lpCreateParams);
	    self->m_hWnd = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self)); 
		self = reinterpret_cast<GLWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	
	else
	{
		self = reinterpret_cast<GLWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA)); 
	}
	if (self)
	{
		return self->WinProc( uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd,  uMsg, wParam, lParam); 
	}


}



//Window Procedure 
LRESULT  GLWindow::WinProc( UINT message, WPARAM wParam, LPARAM lParam)
{

	LONG    lRet = 0;
	PAINTSTRUCT    ps;
	LRESULT lres;
	// GLWindow glWin; 
	 //static GLWindow *glWin;
	//glWin = (GLWindow*) GetWindowLongPtr(hWnd, GWL_USERDATA); 

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:										// If the window is resized
		if (!m_FullScreen)								// Don't worry about this if we are in full screen (otherwise may cause problems)
		{												// LoWord=Width, HiWord=Height
			ResizeOpenGLScreen(LOWORD(lParam), HIWORD(lParam));
			GetClientRect(m_hWnd, &m_windowRect);				// Get the window rectangle
		}
		break;

	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(m_hWnd, &ps);							// Init the paint struct		
		EndPaint(m_hWnd, &ps);							// EndPaint, Clean up
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		
		//case VK_UP:
		//	m_camera.MoveCamera(CAMERA_SPEED); 
		//	//RenderScene();

		//	break; 
		//case VK_DOWN: 
		//	m_camera.MoveCamera(-CAMERA_SPEED); 
		//	//RenderScene();
		//	break;


		case VK_ESCAPE:
			/*DestroyGLWindow();*/
			PostQuitMessage(0);			// Quit if we pressed ESCAPE
			break; 
		}
	
	//case WM_DESTROY:									// If the window is destroyed
	//	//DestroyGLWindow(); 									// Release memory and restore settings
	//	break;

	default:											// Return by default
		lRet = DefWindowProc(m_hWnd, message, wParam, lParam);
		break;
	}

	return lRet;										// Return by default
}