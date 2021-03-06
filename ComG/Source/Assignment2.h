#ifndef ASSIGNMENT_2_H
#define ASSIGNMENT_2_H

#include "Scene.h"
#include "Camera.h"
#include "Camera2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"

class Assignment2 : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_FLOOR,
		GEO_CUBEBODY,
		GEO_CUBEDGV,
		GEO_CUBESB,
		GEO_CUBESG,
		GEO_CIRCLE,
		GEO_RING1,
		GEO_SPHEREBODY,
		GEO_SPHEREKC,
		GEO_EYE,
		GEO_IRIS,
		GEO_LIGHTBALL,
		GEO_CONEBODY,
		GEO_CONENAIL,
		GEO_CYLINDERAGU,
		GEO_CYLINDERDGV,
		GEO_CYLINDERKC,
		NUM_GEOMETRY,
	};
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		U_TOTAL,
	};
public:
	Assignment2();
	~Assignment2();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
private:
	void RenderMesh(Mesh *mesh, bool enableLight);

	unsigned m_vertexArrayID;
	Mesh *meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	float rotateThigh;
	float walkForward;
	float walkBackward;
	float turnRight;
	float turnLeft;

	Camera2 camera;

	MS modelStack, viewStack, projectionStack;

	Light light[1];
};

#endif