#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* MeshBuilder::GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* MeshBuilder::GenerateQuad(const std::string &meshName, Color color);
	static Mesh* MeshBuilder::GenerateCube(const std::string &meshName, Color color);
	static Mesh* MeshBuilder::GenerateCircle(const std::string &meshName, Color color, unsigned numSlices = 36);
	static Mesh* MeshBuilder::GenerateRing(const std::string &meshName, Color color, float innerRadius, unsigned numSlices = 36);
	static Mesh* MeshBuilder::GenerateSphere(const std::string &meshName, Color color, unsigned numSlices = 18, unsigned numStacks = 36);
	static Mesh* MeshBuilder::GenerateCylinder(const std::string &meshName, Color color, unsigned numSlices);
	static Mesh* MeshBuilder::GenerateCone(const std::string &meshName, Color color, unsigned numSlices);
};

#endif