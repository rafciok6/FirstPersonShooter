#ifndef _CAMERA_
#define _CAMERA_

#include "Vector3D.h"
#include "Config.h"

class Camera
{
public: 
	Camera(); 
	~Camera(); 

	//Initialize our camera 
	void PositionCamera(Vector3D position, Vector3D view, Vector3D upVector); 
	void MoveCamera(float speed); 
	//rotate camera when we hit left or right arrow 
	void RotateView(float angle, float x, float y, float z); 

	//allows us to look around using the mouse (first person games) 
	void SetViewByMouse();

	//Move camera left or right 
	void StrafeCamera(float speed);

	//Update the camera 
	void Update(); 

	void Look(); 

	//Check for input if
	void CheckForMovement(); 

	//GET SET
	Vector3D GetPositionVector() { return m_vPosition; }
	Vector3D GetViewVector() { return m_vView; }
	Vector3D GetUpVector() { return m_vUpVector; }

	void SetPosition(float x, float y, float z); 
	void SetViewVector(float x, float y, float z); 
	void SetUpVector(float x, float y, float z);


private: 
	Vector3D m_vPosition; //Camera poistion 
	Vector3D m_vView;	  //The camera view 
	Vector3D m_vUpVector; //The camera up vector

	// The camera's strafe vector
	Vector3D m_vStrafe;

};

#endif