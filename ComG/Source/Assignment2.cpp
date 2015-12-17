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
	meshList[GEO_TORSO1] = MeshBuilder::GenerateCube("cube", Color(1.000f, 0.498f, 0.314f));
	meshList[GEO_CIRCLE] = MeshBuilder::GenerateCircle("circle", Color(1, 0, 1), 20);
	meshList[GEO_TORSO2] = MeshBuilder::GenerateCube("cube", Color(1.000f, 0.498f, 0.314f));
	meshList[GEO_TORSO3] = MeshBuilder::GenerateCube("cube", Color(1.000f, 0.498f, 0.314f));
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(1.000f, 0.498f, 0.314f), 18, 36);
	meshList[GEO_EYE] = MeshBuilder::GenerateSphere("sphere", Color(0.196f, 0.804f, 0.196f), 18, 36);
	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("lightball", Color(1, 1, 1), 18, 36);

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

	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, -10, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_FLOOR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Body
	modelStack.Scale(2, 2, 2);

	modelStack.PushMatrix(); //Right Chest
	modelStack.Scale(2, 2, 0.2);
	modelStack.Translate(-0.75, 0, 7.5);
	RenderMesh(meshList[GEO_TORSO2], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Left Chest
	modelStack.Scale(2, 2, 0.2);
	modelStack.Translate(0.75, 0, 7.5);
	RenderMesh(meshList[GEO_TORSO2], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Stomach
	modelStack.Scale(3.3, 3.3, 3);
	modelStack.Translate(0, -1, 0);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Head
	modelStack.Scale(2, 2, 2);
	modelStack.Translate(0, 2.3, 0.2);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Eye
	modelStack.Scale(0.5, 0.5, 0.5);
	modelStack.Translate(-2.5, 10, 2.4);
	RenderMesh(meshList[GEO_EYE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Eye
	modelStack.Scale(0.5, 0.5, 0.5);
	modelStack.Translate(2.5, 10, 2.4);
	RenderMesh(meshList[GEO_EYE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Right Shoulder
	modelStack.Scale(1, 1, 1);
	modelStack.Translate(-3, 0, 0);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix(); //Left Shoulder
	modelStack.Scale(1, 1, 1);
	modelStack.Translate(3, 0, 0);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Torso
	modelStack.Scale(3, 3, 2);
	RenderMesh(meshList[GEO_SPHERE], true);
	modelStack.PopMatrix();

}

void Assignment2::Exit()
{
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}