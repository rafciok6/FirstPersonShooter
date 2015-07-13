#ifndef _VECTOR3D_ 
#define _VECTOR3D_


class Vector3D
{
public: 
	Vector3D(); 
	Vector3D(float x, float y, float z); 
	~Vector3D(); 

	//cross product of two vectors 
	Vector3D CrossProduct(Vector3D v1, Vector3D v2); 
	//calculate length (magnitude of the vector) 
	float Magnitude(Vector3D v); 
	//Returns normalize vector (vector exactly of the length 1) 
	Vector3D Normalize(Vector3D v); 

	Vector3D operator-(Vector3D&); 
	Vector3D operator-(float);


	float m_x, m_y, m_z;

private: 
};

#endif