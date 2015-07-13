#include "Camera.h"
#include <math.h> 
#include <Windows.h> 
#include <gl\GL.h> 
#include <gl\GLU.h> 



Camera::Camera()
{
	Vector3D vZero = { 0.0, 0.0, 0.0 };		// Init a vector to 0 0 0 for our position
	Vector3D vView = { 0.0, 1.0, 0.5 };		// Init a starting view vector (looking up and out the screen) 
	Vector3D vUp =   { 0.0, 0.0, 1.0 };		// Init a standard up vector (Rarely ever changes)

	m_vPosition = vZero;					// Init the position to zero
	m_vView = vView;					// Init the view to a std starting view
	m_vUpVector = vUp;						// Init the UpVector
}

Camera::~Camera()
{

}

void Camera::PositionCamera(Vector3D position, Vector3D view, Vector3D upVector)
{
	m_vPosition = position;					
	m_vView = view;					
	m_vUpVector = upVector;						
}

//Rotate the view around our position we staying 
void Camera::RotateView(float angle, float x, float y, float z)
{
	Vector3D vNewVector; 
	Vector3D vView; 

	//Calculate view vector (the direction we are facing) 
	vView.m_x = m_vView.m_x - m_vPosition.m_x; // This gets the direction of the X 
	vView.m_y = m_vView.m_y - m_vPosition.m_y; // This gets the direction of the Y
	vView.m_z = m_vView.m_z - m_vPosition.m_z; // This gets the direction of the Z

	//Use axis-angle rotation to rotate the camera view around the arbitraty axes
	//Calculate the sin and cosine of the angle once 
	// http://astronomy.swin.edu.au/~pbourke/geometry/rotate/   more info
	float cosTheta = (float) cos(angle);
	float sinTheta = (float) sin(angle); 

	//find the new x position for the new rotated point 
	vNewVector.m_x = (cosTheta + (1 - cosTheta) * x * x) * vView.m_x; 
	vNewVector.m_x += ((1 - cosTheta) * x * y - z * sinTheta) * vView.m_y; 
	vNewVector.m_x += ((1 - cosTheta) * x * z + y * sinTheta) * vView.m_z; 

	// Find the new y position for the new rotated point
	vNewVector.m_y = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.m_x;
	vNewVector.m_y += (cosTheta + (1 - cosTheta) * y * y)		* vView.m_y;
	vNewVector.m_y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.m_z;

	// Find the new z position for the new rotated point
	vNewVector.m_z = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.m_x;
	vNewVector.m_z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.m_y;
	vNewVector.m_z += (cosTheta + (1 - cosTheta) * z * z)		* vView.m_z;


	// Now we just add the newly rotated vector to our position to set
	// our new rotated view of our camera.
	m_vView.m_x = m_vPosition.m_x + vNewVector.m_x;
	m_vView.m_y = m_vPosition.m_y + vNewVector.m_y;
	m_vView.m_z = m_vPosition.m_z + vNewVector.m_z;
 
}

void Camera::MoveCamera(float speed)
{
	// Get the current view vector (the direction we are looking)
	Vector3D vVector = m_vView - m_vPosition;					// Init a vector for our view

	// I snuck this change in here!  We now normalize our view vector when
	// moving throughout the world.  This is a MUST that needs to be done.
	// That way you don't move faster than you strafe, since the strafe vector
	// is normalized too.
	vVector = vVector.Normalize(vVector);

	// Below we are getting the "View Vector".  To get a vector from
	// 2 points, we subtract the first point from the second point.  Basically,
	// what this does is give us a direction that we are looking.  That is
	// how we know which direction we are looking at in 3D. Later we will create
	// functions that will do this stuff for us instead of doing it like this. Read 
	// "Quick Notes" for some more detail on vectors and the camera's view vector.

	//// Get our view vector (The direction we are facing)
	//vVector.m_x = m_vView.m_x - m_vPosition.m_x;		// This gets the direction of the X	
	//vVector.m_y = m_vView.m_y - m_vPosition.m_y;		// This gets the direction of the Y
	//vVector.m_z = m_vView.m_z - m_vPosition.m_z;		// This gets the direction of the Z

	// Below is the code that will move our view and camera position forward or backward.
	// What we are doing is adding our view vector times the speed, plus
	// our current position and view.  You might be thinking that we should just
	// add/subtract the speed to our view and position, and forget about the vector.
	// Well, the reason why we do this is because we want to go in the direction
	// that we are looking at.  It won't work if we just add the constant speed
	// to the X and Z values of our position and view.  Try it and you will see.
	// Sure, if we are looking DIRECTLY down the X or Z axis it would work, but
	// when we start to rotate our view at a unique angle, it won't work.  Trust me.
	// So basically, if we are looking at a 45 degree angle, we move in that direction.
	// If we want to move forward we pass in a positive speed, and if we want to move
	// backwards, just pass in a negative speed.

	m_vPosition.m_x += vVector.m_x * speed;		// Add our acceleration to our position's X
	m_vPosition.m_z += vVector.m_z * speed;		// Add our acceleration to our position's Z
	m_vPosition.m_y += vVector.m_y * speed; 
	m_vView.m_x += vVector.m_x * speed;			// Add our acceleration to our view's X
	m_vView.m_z += vVector.m_z * speed;			// Add our acceleration to our view's Z
	m_vView.m_y += vVector.m_y * speed;

}

//allows us to look around using the mouse (first person games) 
void Camera::SetViewByMouse()
{
	POINT mousePos;									// This is a window structure that holds an X and Y
	int middleX = SCREEN_WIDTH >> 1;				// This is a binary shift to get half the width
	int middleY = SCREEN_HEIGHT >> 1;				// This is a binary shift to get half the height
	float angleY = 0.0f;							// This is the direction for looking up or down
	float angleZ = 0.0f;							// This will be the value we need to rotate around the Y axis (Left and Right)
	static float currentRotX = 0.0f;

	// Get the mouse's current X,Y position
	GetCursorPos(&mousePos);

	// Now that we got the mouse position, we want to put the mouse position
	// back at the middle of the screen.  We pass in half of our screen width and height.
	// The >> operator is a binary shift.  So, we are shifting our width and height 
	// to the right by 1.  If you do the binary math, it is the same thing as dividing by 2, 
	// but extremely faster.  The reason why we put the cursor in the middle of the 
	// screen each time is we can get a delta (difference) of how far we move 
	// each frame, so we know how much to rotate the camera.

	// If our cursor is still in the middle, we never moved... so don't update the screen.
	if ((mousePos.x == middleX) && (mousePos.y == middleY)) return;

	// Set the mouse position to the middle of our window
	SetCursorPos(middleX, middleY);

	// Now we need to get the direction (or VECTOR) that the mouse moved.
	// To do this, it's a simple subtraction.  Just take the middle point,
	// and subtract the new point from it:  VECTOR = P1 - P2;   with P1
	// being the middle point (400, 300) and P2 being the new mouse location.
	// Look below at the *Quick Notes* for more information and examples.
	// After we get the X and Y delta (or direction), I divide by 1000 to bring the number
	// down a bit, otherwise the camera would move lightning fast!

	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	angleY = (float)((middleX - mousePos.x)) / 1000.0f;
	angleZ = (float)((middleY - mousePos.y)) / 1000.0f;

	static float lastRotX = 0.0f;
	lastRotX = currentRotX; // We store off the currentRotX and will use it in when the angle is capped

	// Here we keep track of the current rotation (for up and down) so that
	// we can restrict the camera from doing a full 360 loop.
	currentRotX += angleZ;

	// If the current rotation (in radians) is greater than 1.0, we want to cap it.
	if (currentRotX > 1.0f)
	{
		currentRotX = 1.0f;

		// Rotate by remaining angle if there is any
		if (lastRotX != 1.0f)
		{
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			Vector3D vAxis = vAxis.CrossProduct(m_vView - m_vPosition, m_vUpVector);
			vAxis = vAxis.Normalize(vAxis);

			// Rotate the camera by the remaining angle (1.0f - lastRotX)
			RotateView(1.0f - lastRotX, vAxis.m_x, vAxis.m_y, vAxis.m_z);
		}
	}
	// Check if the rotation is below -1.0, if so we want to make sure it doesn't continue.
	else if (currentRotX < -1.0f)
	{
		currentRotX = -1.0f;

		// Rotate by the remaining angle if there is any
		if (lastRotX != -1.0f)
		{
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			Vector3D vAxis = vAxis.CrossProduct(m_vView - m_vPosition, m_vUpVector);
			vAxis = vAxis.Normalize(vAxis);

			// Rotate the camera by ( -1.0f - lastRotX)
			RotateView(-1.0f - lastRotX, vAxis.m_x, vAxis.m_y, vAxis.m_z);
		}
	}
	// Otherwise, we can rotate the view around our position
	else
	{
		// To find the axis we need to rotate around for up and down
		// movements, we need to get a perpendicular vector from the
		// camera's view vector and up vector.  This will be the axis.
		// Before using the axis, it's a good idea to normalize it first.
		Vector3D vAxis = vAxis.CrossProduct(m_vView - m_vPosition, m_vUpVector);
		vAxis = vAxis.Normalize(vAxis);

		// Rotate around our perpendicular axis
		RotateView(angleZ, vAxis.m_x, vAxis.m_y, vAxis.m_z);
	}

	// Always rotate the camera around the y-axis
	RotateView(angleY, 0, 1, 0);
}


void Camera::CheckForMovement()
{
	if ((GetKeyState(VK_UP) & 0x80) || (GetKeyState('W') & 0x80)) //0x80 means that button is down 
	{
		//move camera forward 
		MoveCamera(CAMERA_SPEED);
	}
	if ((GetKeyState(VK_DOWN) & 0x80) || (GetKeyState('S') & 0x80))
	{
		//move camera down 
		MoveCamera(-CAMERA_SPEED);
	}
	if ((GetKeyState(VK_LEFT) & 0x80) || (GetKeyState('A') & 0x80))
	{
		StrafeCamera(-CAMERA_SPEED); 
	}
	if ((GetKeyState(VK_RIGHT) & 0x80) || (GetKeyState('D') & 0x80))
	{
		StrafeCamera(CAMERA_SPEED);
	}
}

void Camera::Update()
{
	// Below we calculate the strafe vector every time we update
	// the camera.  This is because many functions use it so we might
	// as well calculate it only once.  

	// Initialize a variable for the cross product result
	Vector3D vCross =vCross.CrossProduct(m_vView - m_vPosition, m_vUpVector);

	// Normalize the strafe vector
	m_vStrafe = m_vStrafe.Normalize(vCross);

	// Move the camera's view by the mouse
	SetViewByMouse();

	// This checks to see if the keyboard was pressed
	CheckForMovement();
}

void Camera::Look()
{
	// Give openGL our camera position, then camera view, then camera up vector
	gluLookAt(m_vPosition.m_x, m_vPosition.m_y, m_vPosition.m_z,
		m_vView.m_x, m_vView.m_y, m_vView.m_z,
		m_vUpVector.m_x, m_vUpVector.m_y, m_vUpVector.m_z);
}


//Move camera left or right 
void Camera::StrafeCamera(float speed)
{
	// Strafing is quite simple if you understand what the cross product is.
	// If you have 2 vectors (say the up vVector and the view vVector) you can
	// use the cross product formula to get a vVector that is 90 degrees from the 2 vectors.
	// For a better explanation on how this works, check out the OpenGL "Normals" tutorial.
	// In our new Update() function, we set the strafing vector (m_vStrafe).  Due
	// to the fact that we need this vector for many things including the strafing
	// movement and camera rotation (up and down), we just calculate it once.
	//
	// Like our MoveCamera() function, we add the strafing vector to our current position 
	// and view.  It's as simple as that.  It has already been calculated in Update().

	// Add the strafe vector to our position
	m_vPosition.m_x += m_vStrafe.m_x * speed;
	m_vPosition.m_z += m_vStrafe.m_z * speed;

	// Add the strafe vector to our view
	m_vView.m_x += m_vStrafe.m_x * speed;
	m_vView.m_z += m_vStrafe.m_z * speed;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_vPosition.m_x = x; 
	m_vPosition.m_y = y; 
	m_vPosition.m_z = z;
}

void Camera::SetViewVector(float x, float y, float z)
{
	m_vView.m_x = x; 
	m_vView.m_y = y; 
	m_vView.m_z = z; 
}

void Camera::SetUpVector(float x, float y, float z)
{
	m_vUpVector.m_x = x; 
	m_vUpVector.m_y = y; 
	m_vUpVector.m_z = z;
}