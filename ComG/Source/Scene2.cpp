#include "Scene2.h"
#include "GL\glew.h"
#include "Mtx44.h"
#include "Application.h"
#include "shader.hpp"


Scene2::Scene2()
{
}

Scene2::~Scene2()
{
}

void Scene2::Init()
{
	// Init VBO here
	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Generate buffers
	glGenBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glGenBuffers(NUM_GEOMETRY, &m_colorBuffer[0]);

	// An array of 3 vectors which represents 3 vertices
	static const GLfloat vertex_buffer_data[] = 
	{
		-0.2f, 1.0f, 0.0f, // vertex 0 of triangle
		0.2f, 1.0f, 1.0f, // vertex 1 of triangle
		0.0f, 0.3f, 0.1f, // vertex 2 of triangle
		/*0.3f, 0.7f, 0.0f,
		0.5f, 0.8f, 0.9f,
		0.5f, 0.0f, 0.0f,*/
	};

	// Set the current active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	// Transfer vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	// An array of 3 vectors which represents the colors of the 3 vertices
	static const GLfloat color_buffer_data[] = 
	{
		1.0f, 1.0f, 0.0f, //color 0 of triangle
		1.0f, 1.0f, 1.0f, //color 1 of triangle
		1.0f, 0.3f, 0.1f, //color 2 of triangle
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer[GEO_TRIANGLE_1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

	// Load vertex and fragment shaders
	m_programID = LoadShaders(
		"Shader//TransformVertexShader.vertexshader",
		"Shader//SimpleFragmentShader.fragmentshader");
	// Use our shader
	glUseProgram(m_programID);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");

	rotateAngle = 0.5f;
	translateX = 0.5f;
	scaleAll = 0.5f;
}

void Scene2::Update(double dt)
{
	if (Application::IsKeyPressed(VK_SPACE))
	{
		// Rotation
		if (rotateAngle > 60)
		{
			rotateAngle = 1;
		}
		rotateAngle += (float)(10 * dt);

		// Translation
		if (translateX > 20)
		{
			translateX = 1;
		}
		translateX += (float)(10 * dt);

		// Scaling
		if (scaleAll > 5)
		{
			scaleAll = 1;
		}
		scaleAll += (float)(2 * dt);
	}
}

void Scene2::Render()
{
	Mtx44 translate, rotate, scale;
	Mtx44 model;
	Mtx44 view;
	Mtx44 projection;
	Mtx44 MVP;

	translate.SetToIdentity();
	rotate.SetToIdentity();
	scale.SetToIdentity();
	model.SetToIdentity();
	view.SetToIdentity(); // no need camera for now, set it at World's origin
	projection.SetToOrtho(-40, +40, -30, +30, -10, +10); // Our world is a cube defined by these boundaries

	//1st triangle
	scale.SetToScale(2, 2, 2);
	rotate.SetToRotation(rotateAngle, 0, 0, 1);
	translate.SetToTranslation(0.5, 0, 0);
	model = translate * rotate * scale; // scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // Remember, matrix multiplication is the other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); // update the shader with new MVP

	// Render VBO here
	// Clear color buffer & depth every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(0); // 1st attribute buffer : vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer
		(
		0,                      // attribute 0. Must match the layout in the shader. Usually 0 is for vertex
		3,                      // size
		GL_FLOAT,       // type
		GL_FALSE,       // normalized?
		0,                      // stride
		0                       // array buffer offset
		);
	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3vertices = 1 triangle
	//till here

	//Second triangle
	scale.SetToScale(1, 1, 1);
	rotate.SetToRotation(100, 0, 0, 1);
	translate.SetToTranslation(translateX, 15, 5);
	model = translate * rotate * scale; // scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // Remember, matrix multiplication is the other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); // update the shader with new MVP

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer
		(
		0,                      // attribute 0. Must match the layout in the shader. Usually 0 is for vertex
		3,                      // size
		GL_FLOAT,       // type
		GL_FALSE,       // normalized?
		0,                      // stride
		0                       // array buffer offset
		);
	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3vertices = 1 triangle
	//till here

	//third triangle
	scale.SetToScale(scaleAll, scaleAll, scaleAll);
	rotate.SetToRotation(0, 0, 0, 1);
	translate.SetToTranslation(15, 1, 5);
	model = translate * rotate * scale; // scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // Remember, matrix multiplication is the other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); // update the shader with new MVP

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer
		(
		0,                      // attribute 0. Must match the layout in the shader. Usually 0 is for vertex
		3,                      // size
		GL_FLOAT,       // type
		GL_FALSE,       // normalized?
		0,                      // stride
		0                       // array buffer offset
		);
	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3vertices = 1 triangle
	//till here

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

void Scene2::Exit()
{
	// Cleanup VBO here
	glDeleteBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glDeleteBuffers(NUM_GEOMETRY, &m_colorBuffer[0]);
	glDeleteBuffers(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}