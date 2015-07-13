#include "Vector3D.h"
#include <math.h> 


Vector3D::Vector3D()
{

}
Vector3D::Vector3D(float x, float y, float z)
{
	m_x = x; 
	m_y = y; 
	m_z = z; 
}
Vector3D::~Vector3D()
{

}

//cross product of two vectors 
Vector3D Vector3D::CrossProduct(Vector3D v1, Vector3D v2)
{
	Vector3D vNormal; 
	// If we are given 2 vectors (the view and up vector) then we have a plane define.  
	// The cross product finds a vector that is perpendicular to that plane, 
	// which means its point is straight out of the plane at a 90 degree angle.
	// The equation for the cross product is simple, but difficult at first to memorize:

	//The X value for the vector is: (v1.y * V2.z) - (V1.z * V2.y) 
	vNormal.m_x = ((v1.m_y * v2.m_z) - (v1.m_z * v2.m_y)); 

	//The Y value for the vector is: (v1.z * V2.x) - (V1.x * V2.z) 
	vNormal.m_y = ((v1.m_z * v2.m_x) - (v1.m_x * v2.m_z));


	//The Z value for the vector is: (v1.x * V2.y) - (V1.y * V2.x) 
	vNormal.m_z = ((v1.m_x * v2.m_y) - (v1.m_y * v2.m_x));

	// *IMPORTANT* This is not communitive. You can not change the order of this or
	// else it won't work correctly.  It has to be exactly like that.  Just remember,
	// If you are trying to find the X, you don't use the X value of the 2 vectors, and
	// it's the same for the Y and Z.  You notice you use the other 2, but never that axis.
	// If you look at the camera rotation tutorial, you will notice it's the same for rotations.

	// So why do I need the cross product to do a first person view?  Well, we need
	// to find the axis that our view has to rotate around.  Rotating the camera left
	// and right is simple, the axis is always (0, 1, 0).  Rotating around the camera
	// up and down is different because we are constantly going in and out of axises.
	// We need to find the axis that our camera is on, and that is why we use the cross
	// product.  By taking the cross product between our view vector and up vector,
	// we get a perpendicular vector to those 2 vectors, which is our desired axis.
	// Pick up a linear algebra book if you don't already have one, you'll need it.

	// Return the cross product
	return vNormal;

}

//calculate length (magnitude of the vector) 
float Vector3D::Magnitude(Vector3D v)
{
	// This will give us the magnitude or "norm" as some say, of our normal.
	// The magnitude has to do with the length of the vector.  We use this
	// information to normalize a vector, which gives it a length of 1.
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2)   Where V is the vector

	return (float)sqrt((v.m_x * v.m_x) +
		(v.m_y * v.m_y) +
		(v.m_z * v.m_z));
}

//Returns normalize vector (vector exactly of the length 1) 
Vector3D Vector3D::Normalize(Vector3D v)
{
	// What's this function for you might ask?  Well, since we are using the cross
	// product formula, we need to make sure our view vector is normalized.  
	// For a vector to be normalized, it means that it has a length of 1.
	// For instance, a vector (2, 0, 0) would be (1, 0, 0) once normalized.
	// Most equations work well with normalized vectors.  If in doubt, normalize.

	// Get the magnitude of our normal
	float magnitude = Magnitude(v);

	// Now that we have the magnitude, we can divide our vector by that magnitude.
	// That will make our vector a total length of 1.  
	// This makes it easier to work with too.
	v = Vector3D( v.m_x / magnitude, v.m_y / magnitude, v.m_z / magnitude);

	// Finally, return our normalized vector
	return v;
}

Vector3D Vector3D::operator-(Vector3D&vec)
{
	Vector3D result; 

	result.m_x = m_x - vec.m_x; 
	result.m_y = m_y - vec.m_y;
	result.m_z = m_z - vec.m_z;
	return result; 
}

Vector3D Vector3D::operator-(float x)
{
	Vector3D result;

	result.m_x = m_x - x;
	result.m_y = m_y - x;
	result.m_z = m_z - x;
	return result;
}
