#ifndef OBJECTLOADER_HPP
#define OBJECTLOADER_HPP

class ObjectLoader
{
public:
	ObjectLoader(void);
	float* CalculateNormals(float* coord1,float* coord2,float* coord3);

	int Load(char* filename);

	void Draw();
	void Release();
	void Update();

	float* normals;
	float* faces;
	float* vertexBuffer;
	long totalPoints;
	long totalTriangles;

private:
	Vector3 pos;
	GLfloat rot;

};

#endif