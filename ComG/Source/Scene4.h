#ifndef SCENE_4_H
#define SCENE_4_H

#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"
#include "MatrixStack.h"

class Scene4 : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,
		GEO_CUBE,
		GEO_CIRCLE,
		GEO_RING1,
		GEO_SPHERE,
		GEO_SUN,
		GEO_PLANET1,
		GEO_MOON1,
		GEO_PLANET2,
		GEO_MOON2,
		GEO_PLANET3,
		GEO_MOON3A,
		GEO_MOON3B,
		GEO_PLANET4,
		NUM_GEOMETRY,
	};
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_TOTAL,
	};
public:
	Scene4();
	~Scene4();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
private:
	unsigned m_vertexArrayID;
	Mesh *meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	float rotateAngle;
	float sunRotAngle;
	float planet1RevAngle, planet1RotAngle, moon1RotAngle;
	float planet2RevAngle, planet2RotAngle, moon2RotAngle;
	float planet3RevAngle, planet3RotAngle, moon3aRotAngle, moon3bRotAngle;
	float planet4RevAngle, planet4RotAngle;

	Camera camera;

	MS modelStack, viewStack, projectionStack;
	
};

#endif