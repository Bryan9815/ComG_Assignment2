#include "Assignment2.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"

#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"

Assignment2::Assignment2()
{
}

Assignment2::~Assignment2()
{
}

void Assignment2::Init()
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

	m_programID = LoadShaders("Shader//Shading.vertexshader", "Shader//Shading.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	glUseProgram(m_programID);

	light[0].position.Set(0, 20, 0);
	light[0].color.Set(1, 1, 1);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.f;

	//Make sure you pass uniform parameters after glUseProgram()
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);

	//variable to rotate geometry
	rotateAngle = 0;

	//Initialize camera settings
	camera.Init(Vector3(40, 30, 30), Vector3(0, 0, 0), Vector3(0, 1, 0));

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_FLOOR] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1));
	meshList[GEO_CUBEBODY] = MeshBuilder::GenerateCube("cube", Color(0.992f, 0.643f, 0.039f));
	meshList[GEO_CIRCLE] = MeshBuilder::GenerateCircle("circle", Color(1, 0, 1), 20);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(0.992f, 0.643f, 0.039f), 18, 36);
	meshList[GEO_EYE] = MeshBuilder::GenerateSphere("eye", Color(0.196f, 0.804f, 0.196f), 18, 36);
	meshList[GEO_IRIS] = MeshBuilder::GenerateSphere("iris", Color(0, 0, 0), 18, 36);
	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("lightball", Color(1, 1, 1), 18, 36);
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(0.992f, 0.643f, 0.039f), 36);
	meshList[GEO_CONEBODY] = MeshBuilder::GenerateCone("neck", Color(0.992f, 0.643f, 0.039f), 36);
	meshList[GEO_CONENAIL] = MeshBuilder::GenerateCone("cone", Color(1.0f, 1.0f, 1.0f), 36);

	Mtx44 projection;
	projection.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);
}

static float ROT_LIMIT = 45.f;
static float SCALE_LIMIT = 5.f;
static float LSPEED = 10.f;

void Assignment2::Update(double dt)
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

	if (Application::IsKeyPressed('I'))
	{
		light[0].position.z -= (float)(LSPEED * dt);
	}
	if (Application::IsKeyPressed('K'))
	{
		light[0].position.z += (float)(LSPEED * dt);
	}
	if (Application::IsKeyPressed('J'))
	{
		light[0].position.x -= (float)(LSPEED * dt);
	}
	if (Application::IsKeyPressed('L'))
	{
		light[0].position.x += (float)(LSPEED * dt);
	}
	if (Application::IsKeyPressed('O'))
	{
		light[0].position.y -= (float)(LSPEED * dt);
	}
	if (Application::IsKeyPressed('P'))
	{
		light[0].position.y += (float)(LSPEED * dt);
	}

	rotateAngle += (float)(10 * dt);

}

void Assignment2::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	mesh->Render();
}

void Assignment2::Render()
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

	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);

	RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix(); //Lightsource
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix(); //Lightsource

	modelStack.PushMatrix(); //Floor
	modelStack.Translate(0, -20, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_FLOOR], true);
	modelStack.PopMatrix(); //Floor

	modelStack.Scale(2, 2, 2);
	modelStack.PushMatrix(); //Body

	modelStack.PushMatrix(); //Neck
	modelStack.PushMatrix(); //Head

	modelStack.PushMatrix(); //Right Ear
	modelStack.Translate(-1.75, 5.5, 0);
	modelStack.Rotate(300, 1, 0, 0);
	modelStack.Rotate(60, 0, 0, 1);
	modelStack.Scale(0.5, 0.55, 0.25);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Right Ear

	modelStack.PushMatrix(); //Left Ear
	modelStack.Translate(1.75, 5.5, 0);
	modelStack.Rotate(300, 1, 0, 0);
	modelStack.Rotate(300, 0, 0, 1);
	modelStack.Scale(0.5, 0.55, 0.25);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Left Ear

	modelStack.PushMatrix(); //Right Eyelid
	
	modelStack.PushMatrix(); //Right Eye
	modelStack.Translate(-1, 5.35, 1.25);
	modelStack.Scale(1, 0.75, 0.5);
	RenderMesh(meshList[GEO_EYE], true);

	modelStack.PushMatrix(); //Right Iris
	modelStack.Translate(-0.05, 0.1, 0);
	modelStack.Rotate(15, 0, 1, 0);
	modelStack.Scale(1, 0.75, 0.75);
	RenderMesh(meshList[GEO_IRIS], true);
	modelStack.PopMatrix(); //Right Iris
	modelStack.PopMatrix(); //Right Eye

	modelStack.Translate(-1.25, 6.35, 1.25);
	modelStack.Rotate(20, 1, 0, 0);
	modelStack.Rotate(10, 0, 1, 0);
	modelStack.Scale(0.35, 0.25, 0.75);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Right Eyelid

	modelStack.PushMatrix(); //Left Eyelid

	modelStack.PushMatrix(); //Left Eye
	modelStack.Translate(1, 5.35, 1.25);
	modelStack.Scale(1, 0.75, 0.5);
	RenderMesh(meshList[GEO_EYE], true);

	modelStack.PushMatrix(); //Left Iris
	modelStack.Translate(0.05, 0.1, 0);
	modelStack.Rotate(345, 0, 1, 0);
	modelStack.Scale(1, 0.75, 0.75);
	RenderMesh(meshList[GEO_IRIS], true);
	modelStack.PopMatrix(); //Left Iris
	modelStack.PopMatrix(); //Left Eye

	modelStack.Translate(1.25, 6.35, 1.25);
	modelStack.Rotate(20, 1, 0, 0);
	modelStack.Rotate(350, 0, 1, 0);
	modelStack.Scale(0.35, 0.25, 0.75);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Left Eyelid

	modelStack.PushMatrix(); //Mouth1
	modelStack.PushMatrix(); //Mouth2

	modelStack.PushMatrix(); //Right Nostril
	modelStack.Translate(-0.5, 4.8, 5.65);
	modelStack.Rotate(45, 1, 0, 0);
	modelStack.Rotate(15, 0, 0, 1);
	modelStack.Scale(0.5, 0.55, 0.25);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Right Nostril

	modelStack.PushMatrix(); //Left Nostril
	modelStack.Translate(0.5, 4.8, 5.65);
	modelStack.Rotate(45, 1, 0, 0);
	modelStack.Rotate(345, 0, 0, 1);
	modelStack.Scale(0.5, 0.55, 0.25);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Left Nostril

	modelStack.Translate(0, 4.5, 1.6);
	modelStack.Rotate(100, 1, 0, 0);
	modelStack.Scale(1.05, 4, 1);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Mouth2

	modelStack.Translate(0, 5, 1.7);
	modelStack.Rotate(100, 1, 0, 0);
	modelStack.Scale(1, 4, 1);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Mouth1

	modelStack.Translate(0, 5, 0.7);
	modelStack.Scale(2, 2, 2);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Head

	modelStack.PushMatrix(); //Throat

	modelStack.Translate(0, 3.5, 0.5);
	modelStack.Rotate(80, 1, 0, 0);
	modelStack.Scale(0.75, 3.5, 1);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Throat

	modelStack.Translate(0, 2.655, 0.2);
	modelStack.Rotate(192.5, 1, 0, 0);
	modelStack.Scale(1, 2, 1.45);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Neck

	modelStack.PushMatrix(); //Right Shoulder
	modelStack.PushMatrix(); //Right Upper Arm
	modelStack.PushMatrix(); //Right Elbow
	modelStack.PushMatrix(); //Right Forearm

	modelStack.PushMatrix(); //Right Hand Right Finger

	modelStack.PushMatrix(); //Right Hand Right Fingernail
	modelStack.Translate(-4.515, -2.545, 3.15);
	modelStack.Rotate(147.5, 1, 0, 0);
	modelStack.Rotate(1.5, 0, 1, 0);
	modelStack.Rotate(11.5, 0, 0, 1);
	modelStack.Rotate(-1, 1, 0, 0);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Hand Right Fingernail

	modelStack.Translate(-4.255, -1.375, 2.35);
	modelStack.Rotate(145, 1, 0, 0);
	modelStack.Rotate(10, 0, 0, 1);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Hand Right Finger

	modelStack.PushMatrix(); //Right Hand Middle Fingernail
	modelStack.Translate(-3.935, -2.005, 3.7);
	modelStack.Rotate(141, 1, 0, 0);
	modelStack.Rotate(-1.25, 0, 0, 1);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Hand Middle Fingernail

	modelStack.PushMatrix(); //Right Hand Middle Finger
	modelStack.Translate(-3.95, -0.975, 2.75);
	modelStack.Rotate(135, 1, 0, 0);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Hand Middle Finger

	modelStack.PushMatrix(); //Right Hand Left Finger

	modelStack.PushMatrix(); //Right Hand Left Fingernail
	modelStack.Translate(-3.655, -2.525, 3.15);
	modelStack.Rotate(148, 1, 0, 0);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Hand Left Fingernail

	modelStack.Translate(-3.655, -1.335, 2.35);
	modelStack.Rotate(145, 1, 0, 0);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Hand Left Finger

	modelStack.Translate(-3.95, 0, 1.25);
	modelStack.Rotate(135, 1, 0, 0);
	modelStack.Scale(0.75, 2, 0.75);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Forearm

	modelStack.Translate(-3.95, 0, 1.25);
	modelStack.Rotate(45, 0, 1, 1);
	modelStack.Scale(1, 0.75, 0.75);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Right Elbow

	modelStack.Translate(-2.75, 1.25, 0);
	modelStack.Rotate(135, 1, 0, 0);
	modelStack.Rotate(35, 0, 0, 1);
	modelStack.Scale(0.75, 2, 0.75);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Upper Arm

	modelStack.Translate(-2.5, 1.5, 0);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Right Shoulder

	modelStack.PushMatrix(); //Left Shoulder
	modelStack.PushMatrix(); //Left Upper Arm
	modelStack.PushMatrix(); //Left Elbow
	modelStack.PushMatrix(); //Left Forearm

	modelStack.PushMatrix(); //Left Hand Right Finger

	modelStack.PushMatrix(); //Left Hand Right Fingernail
	modelStack.Translate(3.655, -2.525, 3.15);
	modelStack.Rotate(148, 1, 0, 0);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Hand Right Fingernail

	modelStack.Translate(3.655, -1.335, 2.35);
	modelStack.Rotate(145, 1, 0, 0);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Hand Right Finger

	modelStack.PushMatrix(); //Left Hand Middle Fingernail
	modelStack.Translate(3.935, -2.005, 3.7);
	modelStack.Rotate(141, 1, 0, 0);
	modelStack.Rotate(1.25, 0, 0, 1);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Hand Middle Fingernail

	modelStack.PushMatrix(); //Left Hand Middle Finger
	modelStack.Translate(3.95, -0.975, 2.75);
	modelStack.Rotate(135, 1, 0, 0);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Hand Middle Finger

	modelStack.PushMatrix(); //Left Hand Left Finger

	modelStack.PushMatrix(); //Left Hand Left Fingernail
	modelStack.Translate(4.515, -2.545, 3.15);
	modelStack.Rotate(146.5, 1, 0, 0);
	modelStack.Rotate(1.5, 0, 1, 0);
	modelStack.Rotate(-11.5, 0, 0, 1);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Hand Left Fingernail

	modelStack.Translate(4.255, -1.375, 2.35);
	modelStack.Rotate(145, 1, 0, 0);
	modelStack.Rotate(-10, 0, 0, 1);
	modelStack.Scale(0.375, 1, 0.375);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Hand Left Finger

	modelStack.Translate(3.95, 0, 1.25);
	modelStack.Rotate(135, 1, 0, 0);
	modelStack.Scale(0.75, 2, 0.75);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Forearm

	modelStack.Translate(3.95, 0, 1.25);
	modelStack.Rotate(315, 0, 1, 1);
	modelStack.Scale(1, 0.75, 0.75);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Left Elbow

	modelStack.Translate(2.75, 1.25, 0);
	modelStack.Rotate(135, 1, 0, 0);
	modelStack.Rotate(325, 0, 0, 1);
	modelStack.Scale(0.75, 2, 0.75);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Upper Arm

	modelStack.Translate(2.5, 1.5, 0);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Left Shoulder

	modelStack.PushMatrix(); //Right Thigh
	modelStack.PushMatrix(); //Right Knee
	modelStack.PushMatrix(); //Right Shin
	modelStack.PushMatrix(); //Right Foot

	modelStack.PushMatrix(); //Right Foot Right Toe
	modelStack.PushMatrix(); //Right Foot Right Toenail
	modelStack.Translate(-2.4, -8.95, 3.9);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 3, 0.65);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Foot Right Toenail

	modelStack.Translate(-2.4, -8.95, 2.1);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 1, 0.65);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Foot Right Toe
	
	modelStack.PushMatrix(); //Right Foot Middle Toe
	modelStack.PushMatrix(); //Right Foot Middle Toenail
	modelStack.Translate(-1.6, -8.95, 3.9);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 3, 0.65);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Foot Middle Toenail

	modelStack.Translate(-1.6, -8.95, 2.1);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 1, 0.75);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Foot Middle Toe

	modelStack.PushMatrix(); //Right Foot Left Toe
	modelStack.PushMatrix(); //Right Foot Left Toenail
	modelStack.Translate(-0.8, -8.95, 3.9);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 3, 0.65);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Foot Left Toenail

	modelStack.Translate(-0.8, -8.95, 2.1);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 1, 0.65);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Foot Left Toe

	modelStack.PushMatrix(); //Right Heel
	modelStack.Translate(-1.6, -9, -0.25);
	modelStack.Scale(0.5, 0.5, 1.5);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Right Heel

	modelStack.Translate(-1.6, -9, 0.2);
	modelStack.Rotate(270, 1, 0, 0);
	modelStack.Scale(1.5, 4, 0.75);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Right Foot

	modelStack.Translate(-1.6, -8.8, 0.5);
	modelStack.Scale(0.9, 3, 1);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Shin

	modelStack.Translate(-1.55, -5.7, 0.5);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Right Knee

	modelStack.Translate(-1.55, -5.5, 0.5);
	modelStack.Rotate(335, 1, 0, 0);
	modelStack.Rotate(350, 0, 0, 1);
	modelStack.Scale(1, 3, 1);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Right Thigh

	modelStack.PushMatrix(); //Left Thigh
	modelStack.PushMatrix(); //Left Knee
	modelStack.PushMatrix(); //Left Shin
	modelStack.PushMatrix(); //Left Foot

	modelStack.PushMatrix(); //Left Foot Right Toe
	modelStack.PushMatrix(); //Left Foot Right Toenail
	modelStack.Translate(0.8, -8.95, 3.7);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 3, 0.65);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Foot Right Toenail

	modelStack.Translate(0.8, -8.95, 1.9);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 1, 0.65);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Foot Right Toe

	modelStack.PushMatrix(); //Left Foot Middle Toe
	modelStack.PushMatrix(); //Left Foot Middle Toenail
	modelStack.Translate(1.6, -8.95, 3.7);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 3, 0.65);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Foot Middle Toenail

	modelStack.Translate(1.6, -8.95, 1.9);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 1, 0.75);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Foot Middle Toe

	modelStack.PushMatrix(); //Left Foot Left Toe
	modelStack.PushMatrix(); //Left Foot Left Toenail
	modelStack.Translate(2.4, -8.95, 3.7);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 3, 0.65);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Foot Left Toenail

	modelStack.Translate(2.4, -8.95, 1.9);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(0.65, 1, 0.65);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Foot Left Toe

	modelStack.PushMatrix(); //Left Heel
	modelStack.Translate(1.6, -9, -0.45);
	modelStack.Scale(0.5, 0.5, 1.5);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Left Heel

	modelStack.Translate(1.6, -9, 0);
	modelStack.Rotate(270, 1, 0, 0);
	modelStack.Scale(1.5, 4, 0.75);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Left Foot

	modelStack.Translate(1.6, -8.8, 0.2);
	modelStack.Scale(0.9, 3, 1);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Shin

	modelStack.Translate(1.6, -5.7, 0.3);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Left Knee

	modelStack.Translate(1.55, -5.5, 0.2);
	modelStack.Rotate(335, 1, 0, 0);
	modelStack.Rotate(10, 0, 0, 1);
	modelStack.Scale(1, 3, 1);
	RenderMesh(meshList[GEO_CYLINDER], true);
	modelStack.PopMatrix(); //Left Thigh

	modelStack.PushMatrix(); //Stomach
	modelStack.PushMatrix(); //Tail

	modelStack.Translate(0, -2, -3);
	modelStack.Rotate(270, 1, 0, 0);
	modelStack.Scale(1.5, 3.5, 1.5);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Tail

	modelStack.Translate(0, -2, 0);
	modelStack.Scale(2.75, 2.5, 2.5);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Stomach

	modelStack.PushMatrix(); //Right Chest
	modelStack.Translate(-1, 0.675, 0.75);
	modelStack.Rotate(330, 0, 1, 0);
	modelStack.Scale(1.5, 1.25, 1);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Right Chest

	modelStack.PushMatrix(); //Left Chest
	modelStack.Translate(1, 0.675, 0.75);
	modelStack.Rotate(30, 0, 1, 0);
	modelStack.Scale(1.5, 1.25, 1);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Left Chest

	modelStack.Scale(2.5, 3, 2);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix(); //Body
}

void Assignment2::Exit()
{
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}