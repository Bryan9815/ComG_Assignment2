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
	rotateThigh = 0;
	turnLeft = 0;
	turnRight = 0;

	//variable to translate geometry
	walkForward = 0;
	walkBackward = 0;

	//Initialize camera settings
	camera.Init(Vector3(40, 30, 30), Vector3(0, 0, 0), Vector3(0, 1, 0));

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_FLOOR] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1));
	meshList[GEO_CUBEBODY] = MeshBuilder::GenerateCube("cube", Color(0.992f, 0.643f, 0.039f));
	meshList[GEO_CUBEDGV] = MeshBuilder::GenerateCube("cubedgv", Color(1, 1, 1));
	meshList[GEO_CUBESB] = MeshBuilder::GenerateCube("cubedgv", Color(0.392f, 0.373f, 0.314f));
	meshList[GEO_CUBESG] = MeshBuilder::GenerateCube("cubedgv", Color(0.02f, 0.369f, 0.059f));
	meshList[GEO_CIRCLE] = MeshBuilder::GenerateCircle("circle", Color(1, 0, 1), 20);
	meshList[GEO_SPHEREBODY] = MeshBuilder::GenerateSphere("sphere", Color(0.992f, 0.643f, 0.039f), 18, 36);
	meshList[GEO_SPHEREKC] = MeshBuilder::GenerateSphere("sphere", Color(0.275f, 0.267f, 0.224f), 18, 36);
	meshList[GEO_EYE] = MeshBuilder::GenerateSphere("eye", Color(0.196f, 0.804f, 0.196f), 18, 36);
	meshList[GEO_IRIS] = MeshBuilder::GenerateSphere("iris", Color(0, 0, 0), 18, 36);
	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("lightball", Color(1, 1, 1), 18, 36);
	meshList[GEO_CYLINDERAGU] = MeshBuilder::GenerateCylinder("cylinder", Color(0.992f, 0.643f, 0.039f), 36);
	meshList[GEO_CYLINDERDGV] = MeshBuilder::GenerateCylinder("cylinder", Color(1, 1, 1), 36);
	meshList[GEO_CYLINDERKC] = MeshBuilder::GenerateCylinder("cylinder", Color(0.275f, 0.267f, 0.224f), 36);
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
	bool thighForward = 0;
	if (Application::IsKeyPressed('W'))
	{
		if (rotateThigh >= -30 && thighForward == false)
		{
			rotateThigh -= 1;
			if (rotateThigh <= -30)
			{
				thighForward = true;
			}
			else
			{
				thighForward = false;
			}
		}
		if (rotateThigh <= 0 && thighForward == true)
		{
			rotateThigh += 1;
			if (rotateThigh >= 0)
			{
				thighForward = false;
			}
			else
			{
				thighForward = true;
			}
		}
	}
	if (Application::IsKeyPressed('W'))
	{
		walkForward += (float)(10 * dt);
	}
	if (Application::IsKeyPressed('S'))
	{
		walkBackward -= (float)(10 * dt);
	}
	if (Application::IsKeyPressed('D'))
	{
		turnRight -= 2;
	}
	if (Application::IsKeyPressed('A'))
	{
		turnLeft += 2;
	}
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
	modelStack.Translate(0.f, -20.f, 0.f);
	modelStack.Scale(1000.f, 1000.f, 1000.f);
	RenderMesh(meshList[GEO_FLOOR], true);
	modelStack.PopMatrix(); //Floor

	modelStack.Scale(2.f, 2.f, 2.f);
	modelStack.PushMatrix(); //Body
	
	modelStack.Rotate(turnRight, 0, 1, 0);
	modelStack.Rotate(turnLeft, 0, 1, 0);

	modelStack.Translate(0, 0, walkForward);
	modelStack.Translate(0, 0, walkBackward);

	modelStack.PushMatrix(); //Neck
	modelStack.PushMatrix(); //Head

	modelStack.PushMatrix(); //Right Ear
	modelStack.Translate(-1.75f, 5.5f, 0.f);
	modelStack.Rotate(300.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(60.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.55f, 0.25f);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Right Ear

	modelStack.PushMatrix(); //Left Ear
	modelStack.Translate(1.75f, 5.5f, 0.f);
	modelStack.Rotate(300.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(300.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.55f, 0.25f);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Left Ear

	modelStack.PushMatrix(); //Right Eyelid
	
	modelStack.PushMatrix(); //Right Eye
	modelStack.Translate(-1.f, 5.35f, 1.25f);
	modelStack.Scale(1.f, 0.75f, 0.5f);
	RenderMesh(meshList[GEO_EYE], true);

	modelStack.PushMatrix(); //Right Iris
	modelStack.Translate(-0.05f, 0.1f, 0.f);
	modelStack.Rotate(15.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1.f, 0.75f, 0.75f);
	RenderMesh(meshList[GEO_IRIS], true);
	modelStack.PopMatrix(); //Right Iris
	modelStack.PopMatrix(); //Right Eye

	modelStack.Translate(-1.25f, 6.35f, 1.25f);
	modelStack.Rotate(20.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(10.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.35f, 0.25f, 0.75f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Right Eyelid

	modelStack.PushMatrix(); //Left Eyelid

	modelStack.PushMatrix(); //Left Eye
	modelStack.Translate(1.f, 5.35f, 1.25f);
	modelStack.Scale(1.f, 0.75f, 0.5f);
	RenderMesh(meshList[GEO_EYE], true);

	modelStack.PushMatrix(); //Left Iris
	modelStack.Translate(0.05f, 0.1f, 0.f);
	modelStack.Rotate(345.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1.f, 0.75f, 0.75f);
	RenderMesh(meshList[GEO_IRIS], true);
	modelStack.PopMatrix(); //Left Iris
	modelStack.PopMatrix(); //Left Eye

	modelStack.Translate(1.25f, 6.35f, 1.25f);
	modelStack.Rotate(20.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(350.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.35f, 0.25f, 0.75f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Left Eyelid

	modelStack.PushMatrix(); //Mouth1
	modelStack.PushMatrix(); //Mouth2

	modelStack.PushMatrix(); //Right Nostril
	modelStack.Translate(-0.5f, 4.8f, 5.65f);
	modelStack.Rotate(45.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(15.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.55f, 0.25f);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Right Nostril

	modelStack.PushMatrix(); //Left Nostril
	modelStack.Translate(0.5f, 4.8f, 5.65f);
	modelStack.Rotate(45.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(345.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.55f, 0.25f);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Left Nostril

	modelStack.Translate(0.f, 4.5f, 1.6f);
	modelStack.Rotate(100.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1.05f, 4.f, 1.f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Mouth2

	modelStack.Translate(0.f, 5.f, 1.7f);
	modelStack.Rotate(100.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1.f, 4.f, 1.f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Mouth1

	modelStack.Translate(0.f, 5.f, 0.7f);
	modelStack.Scale(2.f, 2.f, 2.f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Head

	modelStack.PushMatrix(); //Throat

	modelStack.Translate(0.f, 3.5f, 0.5f);
	modelStack.Rotate(80.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.75f, 3.5f, 1.f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Throat

	modelStack.Translate(0.f, 2.655f, 0.2f);
	modelStack.Rotate(192.5f, 1.f, 0.f, 0.f);
	modelStack.Scale(1.f, 2.f, 1.45f);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Neck

	modelStack.PushMatrix(); //Right Shoulder
	modelStack.PushMatrix(); //Right Upper Arm
	modelStack.PushMatrix(); //Right Elbow
	modelStack.PushMatrix(); //Right Forearm

	modelStack.PushMatrix(); //Right Hand Right Finger

	modelStack.PushMatrix(); //Right Hand Right Fingernail
	modelStack.Translate(-4.515f, -2.545f, 3.15f);
	modelStack.Rotate(147.5f, 1.f, 0.f, 0.f);
	modelStack.Rotate(1.5f, 0.f, 1.f, 0.f);
	modelStack.Rotate(11.5f, 0.f, 0.f, 1.f);
	modelStack.Rotate(-1.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Hand Right Fingernail

	modelStack.Translate(-4.255f, -1.375f, 2.35f);
	modelStack.Rotate(145.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(10.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Hand Right Finger

	modelStack.PushMatrix(); //Right Hand Middle Fingernail
	modelStack.Translate(-3.935f, -2.005f, 3.7f);
	modelStack.Rotate(141.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(-1.25f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Hand Middle Fingernail

	modelStack.PushMatrix(); //Right Hand Middle Finger
	modelStack.Translate(-3.95f, -0.975f, 2.75f);
	modelStack.Rotate(135.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Hand Middle Finger

	modelStack.PushMatrix(); //Right Hand Left Finger

	modelStack.PushMatrix(); //Right Hand Left Fingernail
	modelStack.Translate(-3.655f, -2.525f, 3.15f);
	modelStack.Rotate(148.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Hand Left Fingernail

	modelStack.Translate(-3.655f, -1.335f, 2.35f);
	modelStack.Rotate(145.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Hand Left Finger

	modelStack.Translate(-3.95f, 0.f, 1.25f);
	modelStack.Rotate(135.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.75f, 2.f, 0.75f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Forearm

	modelStack.Translate(-3.95f, 0.f, 1.25f);
	modelStack.Rotate(45.f, 0.f, 1.f, 1.f);
	modelStack.Scale(1.f, 0.75f, 0.75f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Right Elbow

	modelStack.Translate(-2.75f, 1.25f, 0.f);
	modelStack.Rotate(135.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(35.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.75, 2.f, 0.75f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Upper Arm

	modelStack.Translate(-2.5f, 1.5f, 0.f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Right Shoulder

	modelStack.PushMatrix(); //Left Shoulder
	modelStack.PushMatrix(); //Left Upper Arm
	modelStack.PushMatrix(); //Left Elbow
	modelStack.PushMatrix(); //Left Forearm

	modelStack.PushMatrix(); //Left Hand Right Finger

	modelStack.PushMatrix(); //Left Hand Right Fingernail
	modelStack.Translate(3.655f, -2.525f, 3.15f);
	modelStack.Rotate(148.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Hand Right Fingernail

	modelStack.Translate(3.655f, -1.335f, 2.35f);
	modelStack.Rotate(145.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Hand Right Finger

	modelStack.PushMatrix(); //Left Hand Middle Fingernail
	modelStack.Translate(3.935f, -2.005f, 3.7f);
	modelStack.Rotate(141.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(1.25f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Hand Middle Fingernail

	modelStack.PushMatrix(); //Left Hand Middle Finger
	modelStack.Translate(3.95f, -0.975f, 2.75f);
	modelStack.Rotate(135.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Hand Middle Finger

	modelStack.PushMatrix(); //Left Hand Left Finger

	modelStack.PushMatrix(); //Left Hand Left Fingernail
	modelStack.Translate(4.515f, -2.545f, 3.15f);
	modelStack.Rotate(146.5f, 1.f, 0.f, 0.f);
	modelStack.Rotate(1.5f, 0.f, 1.f, 0.f);
	modelStack.Rotate(348.5f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Hand Left Fingernail

	modelStack.Translate(4.255f, -1.375f, 2.35f);
	modelStack.Rotate(145.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(350.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.375f, 1.f, 0.375f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Hand Left Finger

	modelStack.Translate(3.95f, 0.f, 1.25f);
	modelStack.Rotate(135.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.75f, 2.f, 0.75f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Forearm

	modelStack.Translate(3.95f, 0.f, 1.25f);
	modelStack.Rotate(315.f, 0.f, 1.f, 1.f);
	modelStack.Scale(1.f, 0.75f, 0.75f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Left Elbow

	modelStack.Translate(2.75f, 1.25f, 0.f);
	modelStack.Rotate(135.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(325.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.75f, 2.f, 0.75f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Upper Arm

	modelStack.Translate(2.5f, 1.5f, 0.f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Left Shoulder

	modelStack.PushMatrix(); //Right Thigh
	modelStack.PushMatrix(); //Right Knee
	modelStack.PushMatrix(); //Right Shin
	modelStack.PushMatrix(); //Right Foot

	modelStack.PushMatrix(); //Right Foot Right Toe
	modelStack.PushMatrix(); //Right Foot Right Toenail
	modelStack.Translate(-2.4f, -8.95f, 3.9f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 3.f, 0.65f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Foot Right Toenail

	modelStack.Translate(-2.4f, -8.95f, 2.1f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 1.f, 0.65f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Foot Right Toe
	
	modelStack.PushMatrix(); //Right Foot Middle Toe
	modelStack.PushMatrix(); //Right Foot Middle Toenail
	modelStack.Translate(-1.6f, -8.95f, 3.9f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 3.f, 0.65f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Foot Middle Toenail

	modelStack.Translate(-1.6f, -8.95f, 2.1f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 1.f, 0.75f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Foot Middle Toe

	modelStack.PushMatrix(); //Right Foot Left Toe
	modelStack.PushMatrix(); //Right Foot Left Toenail
	modelStack.Translate(-0.8f, -8.95f, 3.9f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 3.f, 0.65f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Right Foot Left Toenail

	modelStack.Translate(-0.8f, -8.95f, 2.1f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 1.f, 0.65f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Foot Left Toe

	modelStack.PushMatrix(); //Right Heel
	modelStack.Translate(-1.6f, -9.f, -0.25f);
	modelStack.Scale(0.5f, 0.5f, 1.5f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Right Heel

	modelStack.Translate(-1.6f, -9.f, 0.2f);
	modelStack.Rotate(270.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1.5f, 4.f, 0.75f);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Right Foot

	modelStack.Translate(-1.6f, -8.8f, 0.5f);
	modelStack.Scale(0.9f, 3.f, 1.f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Shin

	modelStack.Translate(-1.55f, -5.7f, 0.5f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Right Knee

	modelStack.Translate(-1.55f, -5.5f, 0.5f);
	modelStack.Rotate(335, 1.f, 0.f, 0.f);
	modelStack.Rotate(350.f, 0.f, 0.f, 1.f);
	modelStack.Scale(1.f, 3.f, 1.f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Right Thigh

	modelStack.PushMatrix(); //Left Thigh
	modelStack.PushMatrix(); //Left Knee
	modelStack.PushMatrix(); //Left Shin
	modelStack.PushMatrix(); //Left Foot

	modelStack.PushMatrix(); //Left Foot Right Toe
	modelStack.PushMatrix(); //Left Foot Right Toenail
	modelStack.Translate(0.8f, -8.95f, 3.7f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 3.f, 0.65f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Foot Right Toenail

	modelStack.Translate(0.8f, -8.95f, 1.9f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 1.f, 0.65f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Foot Right Toe

	modelStack.PushMatrix(); //Left Foot Middle Toe
	modelStack.PushMatrix(); //Left Foot Middle Toenail
	modelStack.Translate(1.6f, -8.95f, 3.7f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 3.f, 0.65f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Foot Middle Toenail

	modelStack.Translate(1.6f, -8.95f, 1.9f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 1.f, 0.75f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Foot Middle Toe

	modelStack.PushMatrix(); //Left Foot Left Toe
	modelStack.PushMatrix(); //Left Foot Left Toenail
	modelStack.Translate(2.4f, -8.95f, 3.7f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 3.f, 0.65f);
	RenderMesh(meshList[GEO_CONENAIL], true);
	modelStack.PopMatrix(); //Left Foot Left Toenail

	modelStack.Translate(2.4f, -8.95f, 1.9f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.65f, 1.f, 0.65f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Foot Left Toe

	modelStack.PushMatrix(); //Left Heel
	modelStack.Translate(1.6f, -9.f, -0.45f);
	modelStack.Scale(0.5f, 0.5f, 1.5f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Left Heel

	modelStack.Translate(1.6f, -9.f, 0.f);
	modelStack.Rotate(270.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1.5f, 4.f, 0.75f);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Left Foot

	modelStack.Translate(1.6f, -8.8f, 0.2f);
	modelStack.Scale(0.9f, 3.f, 1.f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Shin

	modelStack.Translate(1.6f, -5.7f, 0.3f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Left Knee

	modelStack.Translate(1.55f, -5.5f, 0.2f);
	modelStack.Rotate(335.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(10.f, 0.f, 0.f, 1.f);
	modelStack.Scale(1.f, 3.f, 1.f);
	RenderMesh(meshList[GEO_CYLINDERAGU], true);
	modelStack.PopMatrix(); //Left Thigh

	modelStack.PushMatrix(); //Stomach
	modelStack.PushMatrix(); //Tail

	modelStack.Translate(0.f, -2.f, -3.f);
	modelStack.Rotate(270.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1.5f, 3.5f, 1.5f);
	RenderMesh(meshList[GEO_CONEBODY], true);
	modelStack.PopMatrix(); //Tail

	modelStack.Translate(0.f, -2.f, 0.f);
	modelStack.Scale(2.75f, 2.5f, 2.5f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Stomach

	modelStack.PushMatrix(); //Right Chest
	modelStack.Translate(-1.f, 0.675f, 0.75f);
	modelStack.Rotate(330.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1.5f, 1.25f, 1.f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Right Chest

	modelStack.PushMatrix(); //Left Chest
	modelStack.Translate(1.f, 0.675f, 0.75f);
	modelStack.Rotate(30.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1.5f, 1.25f, 1.f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Left Chest

	modelStack.Scale(2.5f, 3.f, 2.f);
	RenderMesh(meshList[GEO_SPHEREBODY], true);
	modelStack.PopMatrix(); //Body

	modelStack.PushMatrix(); //DigiviceBodyW1
	modelStack.Translate(0.f, 0.f, 5.f);

	modelStack.PushMatrix(); //DigiviceBodyW2
	modelStack.PushMatrix(); //DigiviceBodyW3
	modelStack.PushMatrix(); //DigiviceBodyO1
	modelStack.PushMatrix(); //DigiviceBodyO2
	modelStack.PushMatrix(); //DigiviceBodyO1T1
	modelStack.PushMatrix(); //DigiviceBodyO2T1
	modelStack.PushMatrix(); //DigiviceRoundButton
	modelStack.PushMatrix(); //DigiviceRightOvalButton
	modelStack.PushMatrix(); //DigiviceMiddleOvalButton
	modelStack.PushMatrix(); //DigiviceLeftOvalButton
	modelStack.PushMatrix(); //DigiviceScreenB
	modelStack.PushMatrix(); //DigiviceScreenG
	modelStack.PushMatrix(); //DigivicePopOutW
	modelStack.PushMatrix(); //DigivicePopOutO
	modelStack.PushMatrix(); //DigiviceKeychainJointR1
	modelStack.PushMatrix(); //DigiviceKeychainJointL1
	modelStack.PushMatrix(); //DigiviceKeychainLinkR
	modelStack.PushMatrix(); //DigiviceKeychainLinkL
	modelStack.PushMatrix(); //DigiviceKeychainJointR2
	modelStack.PushMatrix(); //DigiviceKeychainJointR2
	modelStack.PushMatrix(); //DigiviceKeychainLinkMiddle

	modelStack.Translate(0.2f, -1.4f, 0.8f);
	modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.05f, 0.35f, 0.05f);
	RenderMesh(meshList[GEO_CYLINDERKC], true);
	modelStack.PopMatrix(); //DigiviceKeychainLinkMiddle

	modelStack.Translate(0.15f, -1.4f, 0.8f);
	modelStack.Scale(0.1f, 0.05f, 0.05f);
	RenderMesh(meshList[GEO_SPHEREKC], true);
	modelStack.PopMatrix(); //DigiviceKeychainJointL2

	modelStack.Translate(-0.15f, -1.4f, 0.8f);
	modelStack.Scale(0.1f, 0.05f, 0.05f);
	RenderMesh(meshList[GEO_SPHEREKC], true);
	modelStack.PopMatrix(); //DigiviceKeychainJointR2

	modelStack.Translate(0.2f, -1.4f, 0.8f);
	modelStack.Scale(0.045f, 0.3f, 0.05f);
	RenderMesh(meshList[GEO_CYLINDERKC], true);
	modelStack.PopMatrix(); //DigiviceKeychainLinkL

	modelStack.Translate(-0.2f, -1.4f, 0.8f);
	modelStack.Scale(0.045f, 0.3f, 0.05f);
	RenderMesh(meshList[GEO_CYLINDERKC], true);
	modelStack.PopMatrix(); //DigiviceKeychainLinkR

	modelStack.Translate(0.15f, -1.1f, 0.8f);
	modelStack.Scale(0.1f, 0.05f, 0.065f);
	RenderMesh(meshList[GEO_SPHEREKC], true);
	modelStack.PopMatrix(); //DigiviceKeychainJointL1

	modelStack.Translate(-0.15f, -1.1f, 0.8f);
	modelStack.Scale(0.1f, 0.05f, 0.065f);
	RenderMesh(meshList[GEO_SPHEREKC], true);
	modelStack.PopMatrix(); //DigiviceKeychainJointR1

	modelStack.Translate(0.f, -1.f, 0.75f);
	modelStack.Rotate(45.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.2f, 0.3f, 0.25f);
	RenderMesh(meshList[GEO_CUBEBODY], true);
	modelStack.PopMatrix(); //DigivicePopOutO

	modelStack.Translate(0.f, -1.05f, 0.65f);
	modelStack.Rotate(45.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.25f, 0.5f, 0.4f);
	RenderMesh(meshList[GEO_CUBEDGV], true);
	modelStack.PopMatrix(); //DigivicePopOutW

	modelStack.Translate(0.f, 0.5f, -0.02f);
	modelStack.Rotate(330.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.7f, 0.675f, 0.1f);
	RenderMesh(meshList[GEO_CUBESG], true);
	modelStack.PopMatrix(); //DigiviceScreenG

	modelStack.Translate(0.f, 0.5f, -0.03f);
	modelStack.Rotate(330.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.999f, 0.75f, 0.1f);
	RenderMesh(meshList[GEO_CUBESB], true);
	modelStack.PopMatrix(); //DigiviceScreenB

	modelStack.Translate(0.2f, -0.25f, 0.275f);
	modelStack.Rotate(60.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(330.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.05f, 0.2f, 0.15f);
	RenderMesh(meshList[GEO_CYLINDERDGV], true);
	modelStack.PopMatrix(); //DigiviceLeftOvalButton

	modelStack.Translate(0.f, -0.2f, 0.25f);
	modelStack.Rotate(60.f, 1.f, 0.f, 0.f);
	//modelStack.Rotate(30, 0, 0, 1);
	modelStack.Scale(0.05f, 0.2f, 0.15f);
	RenderMesh(meshList[GEO_CYLINDERDGV], true);
	modelStack.PopMatrix(); //DigiviceMiddleOvalButton

	modelStack.Translate(-0.2f, -0.25f, 0.275f);
	modelStack.Rotate(60.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(30.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.05f, 0.2f, 0.15f);
	RenderMesh(meshList[GEO_CYLINDERDGV], true);
	modelStack.PopMatrix(); //DigiviceRightOvalButton

	modelStack.Translate(0.f, -0.45f, 0.3f);
	modelStack.Rotate(60.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.25f, 0.25f, 0.25f);
	RenderMesh(meshList[GEO_CYLINDERDGV], true);
	modelStack.PopMatrix(); //DigiviceBodyRoundButton

	modelStack.Translate(0.f, -0.95f, 0.47f);
	modelStack.Scale(0.75f, 0.4f, 0.55f);
	RenderMesh(meshList[GEO_CUBEBODY], true);
	modelStack.PopMatrix(); //DigiviceBodyO2T1

	modelStack.Translate(0.f, -0.05f, 0.05f);
	modelStack.Rotate(330.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.75f, 1.9f, 0.5f);
	RenderMesh(meshList[GEO_CUBEBODY], true);
	modelStack.PopMatrix(); //DigiviceBodyO1T1

	modelStack.Translate(0.f, -0.98f, 0.45f);
	modelStack.Scale(0.9f, 0.45f, 0.55f);
	RenderMesh(meshList[GEO_CUBEBODY], true);
	modelStack.PopMatrix(); //DigiviceBodyO2

	modelStack.Translate(0.f, -0.01f, 0.02f);
	modelStack.Rotate(330.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.9f, 1.985f, 0.5f);
	RenderMesh(meshList[GEO_CUBEBODY], true);
	modelStack.PopMatrix(); //DigiviceBodyO1

	modelStack.Translate(0.4975f, -1.2f, 0.44f);
	modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.25f, 0.999f, 0.275f);
	RenderMesh(meshList[GEO_CYLINDERDGV], true);
	modelStack.PopMatrix(); //DigiviceBodyW3

	modelStack.Translate(0.f, -0.995f, 0.44f);
	modelStack.Scale(1.f, 0.5f, 0.55f);
	RenderMesh(meshList[GEO_CUBEDGV], true);
	modelStack.PopMatrix(); //DigiviceBodyW2

	modelStack.Rotate(330.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1.f, 2.f, 0.5f);
	RenderMesh(meshList[GEO_CUBEDGV], true);
	modelStack.PopMatrix(); //DigiviceBodyW1
}

void Assignment2::Exit()
{
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}