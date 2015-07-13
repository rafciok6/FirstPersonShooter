#include "Scene.h"
#include <gl\GL.h> 
#include <gl\GLU.h> 
#include "HelperFunctions.h"

Scene::Scene()
{
	m_pTerrain = new Terrain();
}
Scene::~Scene()
{
	glDeleteTextures(1, g_Texture); 
	delete m_pTerrain; 
}

void Scene::Init()
{
	glEnable(GL_DEPTH_TEST);							// Enables depth testing
	glEnable(GL_TEXTURE_2D);							// Enable texture mapping
	glEnable(GL_CULL_FACE);

	// This is where we load all of our textures.  We are loading
	// just one in this case, but you can add many more if you want.
	// We pass in our global texture array, our file we want to load, and the texture ID we want associated with it
	// as an array index 0.  When it comes out, the g_Texture will now reference our texture map.

	Helper::CreateTexture(g_Texture[0], L"brickwall_1.bmp");			// Load "bitmap.bmp" into openGL as a texture

	//Load Terrain
	m_pTerrain->LoadRawFile("Terrain.raw"); 
	m_pTerrain->LoadTexture(L"Terrain.bmp");
	m_pTerrain->LoadWaterTexture(L"water.bmp"); 

	m_Skybox.LoadTexture(L"Skybox/Front.bmp", L"Skybox/Back.bmp", L"Skybox/Left.bmp", L"Skybox/Right.bmp", L"Skybox/Bottom.bmp", L"Skybox/Top.bmp");

	m_camera.PositionCamera(Vector3D(200, 0.5f, 200), Vector3D(0, 0.5, 0), Vector3D(0, 1, 0));
}

void Scene::Render()
{
	m_camera.Look();

	// 	  Position      View	   Up Vector
	//gluLookAt(0, 0, 6, 0, 0, 0, 0, 1, 0);		// This determines where the camera's position and view is
	glColor3ub(255, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex3f(0, 1, 0);
	glVertex3f(-1, 0, 0);
	glVertex3f(1, 0, 0);
	glEnd();

	// Below we draw a texture mapped square. Remember, GL_QUADS draws a 4 point polygon.
	// In order to assign a texture map to a polygon, we need to call glBindTexture().
	// This passes in the type of texture map (always use GL_TEXTURE_2D) and the index
	// into our generated texture array - g_Texture[].  If we want to use another
	// texture map, and we have multiple loaded, we just change the index into the array.
	//DrawGroundLines();

	glColor3f(1.0, 1.0, 1.0);
	// Bind the texture stored at the zero index of g_Texture[]
	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

	// Display a quad texture to the screen
	glBegin(GL_QUADS);

	// glTexCoord2f() takes the X and Y offset (or U and V) into the bitmap.
	// Then, the next point sent to be rendered attaches that part of the bitmap
	// to itself.  The (U, V) coordinates range from (0, 0) being the top left corner
	// of the bitmap, to (1, 1) being the bottom left corner of the bitmap.
	// You can go above 1 but it just is wrapped around back to zero and repeats the texture.
	// Try setting the 1's to 2's and see what it does, then try setting them to 0.5's.
	// The higher the number, the more instances of the texture will appear on the square,
	// Where the lower the number, it stretches the incomplete texture over the surface of the square.
	// For every vertice we need a U V coordinate, as shown below.  You might have to play
	// around with the values to make it texture correctly, otherwise it will be flipped, upside down,
	// or skewed.  It also depends on where you are looking at it.  We are looking down the -Z axis.

	// Display the top left vertex
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, 1, 2);

	// Display the bottom left vertex
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, 0, 2);

	// Display the bottom right vertex
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1, 0, 2);

	// Display the top right vertex
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1, 1, 2);

	glEnd();

	//glPushMatrix(); 
	//glTranslatef(1.0, -100.0, 1.0); 
	m_pTerrain->Render();
	//glPopMatrix(); 

	m_pTerrain->CheckForCollision(&m_camera, 10); 
	m_Skybox.CreateSkyBox(500, 0, 500, 3000, 3000, 3000); 

	m_camera.Update();
}

//drawing the green lines as a ground 
void Scene::DrawGroundLines()
{

	// This function was added to give a better feeling of moving around.
	// A black background just doesn't give it to ya :)  We just draw 100
	// green lines vertical and horizontal along the X and Z axis.

	// Turn the lines GREEN
	glColor3ub(0, 255, 0);

	// Draw a 1x1 grid along the X and Z axis'
	for (float i = -50; i <= 50; i += 1)
	{
		// Start drawing some lines
		glBegin(GL_LINES);

		// Do the horizontal lines (along the X)
		glVertex3f(-50, 0, i);
		glVertex3f(50, 0, i);

		// Do the vertical lines (along the Z)
		glVertex3f(i, 0, -50);
		glVertex3f(i, 0, 50);

		// Stop drawing lines
		glEnd();
	}

}
