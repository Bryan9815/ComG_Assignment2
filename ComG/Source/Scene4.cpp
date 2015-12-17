#include "Scene4.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"

#include "Application.h"
#include "MeshBuilder.h"

Scene4::Scene4()
{
}

Scene4::~Scene4()
{
}

void Scene4::Init()
{
	// Init VBO here

	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	//Enable back face culling
	glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//Load vertex and fragment shaders
	m_programID = LoadShaders("Shader//TransformVertexShader.vertexshader", "Shader//SimpleFragmentShader.fragmentshader");
	// Use our shader
	glUseProgram(m_programID);
	// Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");

	//variable to rotate geometry
	rotateAngle = 0;
	planet1RotAngle = planet1RevAngle = moon1RotAngle = 0;

	//Initialize camera settings
	camera.Init(Vector3(40, 30, 30), Vector3(0, 0, 0), Vector3(0, 1, 0));

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 0));
	meshList[GEO_CIRCLE] = MeshBuilder::GenerateCircle("circle", Color(1, 0, 1), 20);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 18, 36);
	meshList[GEO_SUN] = MeshBuilder::GenerateSphere("planet1", Color(1, 1, 0), 18, 36);
	meshList[GEO_PLANET1] = MeshBuilder::GenerateSphere("planet1", Color(.2f, .4f, 1), 18, 36);
	meshList[GEO_MOON1] = MeshBuilder::GenerateSphere("moon1", Color(.8f, 1, .3f), 18, 36);
	meshList[GEO_RING1] = MeshBuilder::GenerateRing("ring1", Color(0.902f, 0.902f, 0.980f), 0.8f, 20);
	meshList[GEO_PLANET2] = MeshBuilder::GenerateSphere("planet2", Color(0.957f, 0.643f, 0.376f), 18, 36);
	meshList[GEO_MOON2] = MeshBuilder::GenerateSphere("moon2", Color(0.847f, 0.749f, 0.847f), 18, 36);
	meshList[GEO_PLANET3] = MeshBuilder::GenerateSphere("planet3", Color(0.863f, 0.078f, 0.235f), 18, 36);
	meshList[GEO_MOON3A] = MeshBuilder::GenerateSphere("moon3a", Color(0.580f, 0, 0.827f), 18, 36);
	meshList[GEO_MOON3B] = MeshBuilder::GenerateSphere("moon3b", Color(0.580f, 0, 0.827f), 18, 36);
	meshList[GEO_PLANET4] = MeshBuilder::GenerateSphere("planet4", Color(0, 0.5f, 0), 18, 36);

	Mtx44 projection;
	projection.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);
}

static float ROT_LIMIT = 45.f;
static float SCALE_LIMIT = 5.f;

void Scene4::Update(double dt)
{
	camera.Update(dt);

	if (Application::IsKeyPressed('1')) //enable back face culling
		glEnable(GL_CULL_FACE);
	if (Application::IsKeyPressed('2')) //disable back face culling
		glDisable(GL_CULL_FACE);
	if (Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	if (Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode

	rotateAngle += (float)(10 * dt);
	sunRotAngle += (float)(5 * dt);
	planet1RotAngle += (float)(5 * dt);
	planet1RevAngle += (float)(2 * dt);
	moon1RotAngle += (float)(50 * dt);
	planet2RotAngle += (float)(5 * dt);
	planet2RevAngle += (float)(2 * dt);
	moon2RotAngle += (float)(50 * dt);
	planet3RotAngle += (float)(5 * dt);
	planet3RevAngle += (float)(2 * dt);
	moon3aRotAngle += (float)(50 * dt);
	moon3bRotAngle += (float)(50 * dt);
	planet4RotAngle += (float)(5 * dt);
	planet4RevAngle += (float)(2 * dt);
}

void Scene4::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set view matrix using camera settings
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);

	modelStack.LoadIdentity();

	Mtx44 MVP;

	modelStack.PushMatrix();
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_AXES]->Render();
	modelStack.PopMatrix();

	modelStack.PushMatrix();						//sun
	modelStack.Rotate(sunRotAngle, 1, 0, 0);	//revolution about planet
	modelStack.Scale(7, 7, 7);
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_SUN]->Render();
	modelStack.PopMatrix();

	modelStack.PushMatrix();						//planet1 system

	modelStack.Rotate(planet1RevAngle, 0, 1, 0);		//revolution about centre
	modelStack.Translate(30, 0, 0);					//offset from centre

	modelStack.PushMatrix();						//moon
	modelStack.Rotate(moon1RotAngle, 1, 0, 0);	//revolution about planet
	modelStack.Translate(0, 10, 0);					//offset from planet
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_MOON1]->Render();
	modelStack.PopMatrix();

	modelStack.PushMatrix();						//planet1 itself
	modelStack.Rotate(planet1RotAngle, 0, 1, 0);	//rotation about its own axis
	modelStack.Scale(5, 5, 5);						//planet size
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_PLANET1]->Render();
	modelStack.PopMatrix();

	modelStack.PushMatrix();						//planet1's ring
	modelStack.Scale(7, 7, 7);
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_RING1]->Render();
	modelStack.PopMatrix();

	modelStack.PopMatrix(); //planet1 system

	modelStack.PushMatrix();						//planet2 system

	modelStack.Rotate(5 * planet2RevAngle, 0, 1, 0);		//revolution about centre
	modelStack.Translate(10, 0, 0);					//offset from centre

	modelStack.PushMatrix();						//moon
	modelStack.Rotate(moon2RotAngle, 1, 0, 0);	//revolution about planet
	modelStack.Translate(0, 3, 0);					//offset from planet
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_MOON2]->Render();
	modelStack.PopMatrix();

	modelStack.PushMatrix();						//planet2 itself
	modelStack.Rotate(10 * planet2RotAngle, 0, 1, 0);	//rotation about its own axis
	modelStack.Scale(2, 2, 2);						//planet size
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_PLANET2]->Render();
	modelStack.PopMatrix();

	modelStack.PopMatrix(); //planet2 system

	modelStack.PushMatrix();						//planet3 system

	modelStack.Rotate(planet3RevAngle, 0, 1, 0);		//revolution about centre
	modelStack.Translate(-30, 0, 0);					//offset from centre

	modelStack.PushMatrix();						//moon
	modelStack.Rotate(moon3aRotAngle, 1, 0, 0);	//revolution about planet
	modelStack.Translate(0, 7, 0);					//offset from planet
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_MOON3A]->Render();
	modelStack.PopMatrix();

	modelStack.PushMatrix();						//moon
	modelStack.Rotate(moon3bRotAngle, 1, 0, 0);	//revolution about planet
	modelStack.Translate(0, -7, 0);					//offset from planet
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_MOON3B]->Render();
	modelStack.PopMatrix();

	modelStack.PushMatrix();						//planet3 itself
	modelStack.Rotate(planet3RotAngle, 0, 1, 0);	//rotation about its own axis
	modelStack.Scale(5, 5, 5);						//planet size
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_PLANET3]->Render();
	modelStack.PopMatrix();

	modelStack.PopMatrix(); //planet3 system

	modelStack.PushMatrix();						//planet4 system

	modelStack.Rotate(10 * planet4RevAngle, 0, 1, 0);		//revolution about centre
	modelStack.Translate(-15, 0, 0);					//offset from centre

	modelStack.PushMatrix();						//planet4 itself
	modelStack.Rotate(10 * planet4RotAngle, 0, 1, 0);	//rotation about its own axis
	modelStack.Scale(2, 2, 2);						//planet size
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	meshList[GEO_PLANET4]->Render();
	modelStack.PopMatrix();

	modelStack.PopMatrix(); //planet4 system
}

void Scene4::Exit()
{
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}