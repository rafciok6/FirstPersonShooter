#include "Terrain.h"
#include <gl\GL.h> 
#include <gl\GLU.h> 
#include <stdio.h>
#include <stdlib.h>
#include "Vector3D.h"


Terrain::Terrain()
{
	m_bAddWater = false;
	m_waterHeight = 120.0f; 
}

Terrain::~Terrain()
{

}

//	This loads a .raw file into an array of bytes.  Each value is a height value.
void Terrain::LoadRawFile(LPSTR fileName)
{
	FILE *pFile = NULL; 

	//open the file in read/byte mode 
	pFile = fopen(fileName, "rb"); 

	//check if file exists and we can open it 
	if (pFile == NULL)
	{
		//disply error message 
		MessageBox(NULL, TEXT("Can't find the Height map file!"), TEXT("Error"), MB_OK); 
		return; 
	}

	// Here we load the .raw file into our pHeightMap data array.
	// We are only reading in '1', and the size is the (width * height)
	fread(m_HeighMap, 1, MAP_SIZE * MAP_SIZE, pFile);

	// After we read the data, it's a good idea to check if everything read fine.
	int result = ferror(pFile);

	// Check if we received an error.
	if (result)
	{
		MessageBox(NULL, TEXT("Can't get data!"), TEXT("Error"), MB_OK);
	}

	// Close the file.
	fclose(pFile);

}

//	This renders the height map as QUADS
void Terrain::Render()
{
	int X = 0, Y = 0;						// Create some variables to walk the array with.
	int x, y, z;							// Create some variables for readability
	//float fColor = 0.0f;					// Create a variable to hold our color of the polygon
	float bSwitchSides = false;

	if (!m_HeighMap) return;					// Make sure our height data is valid

	//glBegin(GL_QUADS);					// Render Quads


	// The difference from the way we render the terrain from
	// the original way we started with, is that we don't use GL_QUADS
	// anymore, we now use a GL_TRIANGLE_STIP.  This means that we don't
	// need to pass in the same vertex more than once.  Each 2 vertices
	// are connected to the next 2.  Since we want to do this in one strip,
	// we are going to need to reverse the order every other column.  It's
	// like moving the lawn.  Go to the end and turn around and come back
	// the way you came.  If you don't do it this way, you will get polygons
	// stretching across the whole terrain.  We could just do a new glBegin()
	// and glEnd() for every column, but I believe this way is faster.  
	// Not that that really matters though, because rendering a terrain
	// with glVertex*() calls in incredibly slow.  We will most likely want
	// to eventually switch this to vertex arrays.

	// Bind the terrain texture to our terrain
	glBindTexture(GL_TEXTURE_2D, m_TerrainTexture);

	//we want to render treinagle strip 
	glBegin(GL_TRIANGLE_STRIP); 

	//go throuht all of the rows of the heightmap 
	for (X = 0; X < MAP_SIZE; X += STEP_SIZE)
	{
		//check if we need to render the opposite way for this column
		if (bSwitchSides)
		{
			//render a column of the terrain, for this current x
			//we start at MAP_SIZE and render down to 0
			for (Y = MAP_SIZE; Y >= 0; Y -= STEP_SIZE)
			{
				//Get the (X, Y, Z) value for the bottom left vertex 
				x = X + STEP_SIZE; 
				y = Height(X + STEP_SIZE, Y); 
				z = Y; 

				//Set the current texture coordinate and render the vertex
				SetTextureCoord((float)x, (float(z))); 
				glVertex3i(x, y, z); 
			}
		}
		else
		{
			// Render a column of the terrain, for this current X.
			// We start at 0 and render down up to MAP_SIZE.
			for (Y = 0; Y <= MAP_SIZE; Y += STEP_SIZE)
			{
				// Get the (X, Y, Z) value for the bottom right vertex		
				x = X + STEP_SIZE;
				y = Height(X + STEP_SIZE, Y);
				z = Y;

				// Set the current texture coordinate and render the vertex
				SetTextureCoord((float)x, (float)z);
				glVertex3i(x, y, z);

				// Get the (X, Y, Z) value for the bottom left vertex		
				x = X;
				y = Height(X, Y);
				z = Y;

				// Set the current texture coordinate and render the vertex
				SetTextureCoord((float)x, (float)z);
				glVertex3i(x, y, z);
			}
		}
		// Switch the direction the column renders to allow the fluid tri strips
		//bSwitchSides = !bSwitchSides;
	}

	glEnd(); 

	if (m_bAddWater)
	{
		// Now we actually draw the water blended over the reflection, which
		// creates the effect that the reflection is in the water.  We blend
		// with a simple alpha to one minus alpha blending function.  The color
		// is set to a blue, with the alpha value at 50% (0.5).  We don't want
		// the alpha too high or else we wouldn't see the reflection.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.5f, 0.5f, 0.9f, 0.5f);
		RenderWater();
		glDisable(GL_BLEND);
		// This block of code right here is for an added effect of
		// what is draw when you are under the water.  This isn't necessary,
		// but I thought I would add it to improve the tutorial's realism.
		// When we go underwater we want to be able to look up out of the
		// water.  To do that we just render the water again, but change
		// the culling to front-face, then change our water color.  Nothing special.
		// We could just do a check to see if we are under or over the water,
		// then only render the water once, but I think you can do that yourself.
		glCullFace(GL_FRONT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.2f, 0.2f, 0.9f, 0.5f);
		RenderWater();
		glDisable(GL_BLEND);
		glCullFace(GL_BACK);
	}
}

//returns the height (0 to 255) from a heightmap given an X and Y 
int Terrain::Height(int x, int y)

{
	// This is used to index into our height map array.
	// When ever we are dealing with arrays, we want to make sure
	// that we don't go outside of them, so we make sure that doesn't
	// happen with a %.  This way x and y will cap out at (MAX_SIZE - 1)

	int _x = x % MAP_SIZE;					// Error check our x value
	int _y = y % MAP_SIZE;					// Error check our y value

	if (!m_HeighMap) return 0;				// Make sure our data is valid

	// Below, we need to treat the single array like a 2D array.
	// We can use the equation: index = (x + (y * arrayWidth) ).
	// This is assuming we are using this assumption array[x][y]
	// otherwise it's the opposite.  Now that we have the correct index,
	// we will return the height in that index.

	return m_HeighMap[_x + (_y * MAP_SIZE)];	// Index into our height array and return the height
}

//sets the color value for a particular index, depending on the height index
void Terrain::SetVertexColor(int x, int y)
{
	if (!m_HeighMap) return;					// Make sure our height data is valid

	// Here we set the color for a vertex based on the height index.
	// To make it darker, I start with -0.15f.  We also get a ratio
	// of the color from 0 to 1.0 by dividing the height by 256.0f;
	float fColor = -0.15f + (Height(x, y) / 256.0f);

	// Assign this green shade to the current vertex
	glColor3f(0, fColor, 0);
}

//load the terrain texture 
void Terrain::LoadTexture(LPWSTR fileName)
{
	Helper::CreateTexture(m_TerrainTexture, fileName); 
}

//check for camera collition wih the terrain at point (if collide update camera position)
void Terrain::CheckForCollision(Camera *cam, int offset)
{
	//Get the camera position 
	Vector3D currentPos = cam->GetPositionVector(); 
	Vector3D newCamPosition = currentPos; 

	//check if the camera is below the heighground of the terrain at x and z 
	//add offset to avoid seeing through the terrain 
	if (currentPos.m_y < Height((int)currentPos.m_x, (int)currentPos.m_z) + offset)
	{
		//set the new position of the camera so it is above the terrain + offset
		newCamPosition.m_y = (float)Height((int)currentPos.m_x, (int)currentPos.m_z) + offset;

		//get the difference of the y that the camera was pushed back up 
		float temp = newCamPosition.m_y - currentPos.m_y; 

		//get the current view and increase it by the difference the position was moved 
		Vector3D view = cam->GetViewVector(); 
		view.m_y += temp;

		//set the new camera position 
		cam->PositionCamera(newCamPosition, view, Vector3D(0.0, 1.0, 0.0)); 

	}
}

//This sets the current texture coordinate of the terrain, based on the X and Z
void Terrain::SetTextureCoord(float x, float z)
{
	// We want to calculate the texture coordinate for the current vertex.
	// To do this, we just take the current x and y and divide them by the
	// MAP_SIZE of our terrain.  This is of course assuming the height map
	// has the same width and height.  Notice that we negate the v value (IE, (u, v)).
	// This is because we need to flip the texture upside down so it lines up correctly.
	// This simple math we use to find the texture coordinate can be explained easily.
	// We know that our terrain is made up of a grid, and that if we want to stretch
	// the entire texture over top of that grid, we just divide the current section
	// of the grid by the total section of the grid, which gives us a ratio from 0
	// to 1.  This works great because the (u, v) coordinates for a texture map go
	// from (0, 0) in the top left corner to (1, 1) in the bottom right corner.

	// Give OpenGL the current texture coordinate for our height map
	glTexCoord2f((float)x / (float)MAP_SIZE,
		-(float)z / (float)MAP_SIZE);

}


void Terrain::RenderWater()
{
	// This is our function that draws the water over the terrain.
	// We create a simple variable to slowly move the water texture.
	// If you want to change the UV coordinates for the water, just
	// chance the waterUV variable.
	static float move = 0.0f;
	float waterUV = 1.0f;

	// Slowly increase our water texture across the surface
	move += 0.00005f;

	// Turn on texture mapping and bind the water texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_WaterTexture);

	//Render the water surface at the waterHeight 
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, waterUV + move);
	glVertex3f(0.0f, m_waterHeight, 0.0f);
	glTexCoord2f(0.0f, 0.0f + move);
	glVertex3f(0.0f, m_waterHeight, 1000.0f);
	glTexCoord2f(waterUV, 0.0f + move);
	glVertex3f(1000.0f, m_waterHeight, 1000.0f);
	glTexCoord2f(waterUV, waterUV + move);
	glVertex3f(1000.0f, m_waterHeight, 0.0f);
	glEnd();

	// Turn off texturing
	glDisable(GL_TEXTURE_2D);

	// Restore the color of our world to white with full alpha
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


void Terrain::LoadWaterTexture(LPWSTR fileName)
{
	Helper::CreateTexture(m_WaterTexture, fileName); 
	m_bAddWater = true;
}