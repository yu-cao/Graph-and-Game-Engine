//
// Created by yu cao on 2019-02-27.
//

#include "ModelSection.h"

ModelSection::ModelSection(Material *mat)
{
	this->material = mat;
}

void ModelSection::addTriangle(triangle tri)
{
	triangles.push_back(tri);
}

int ModelSection::getVertexCount()
{
	return triangles.size() * 3;
}

Material *ModelSection::getMaterial()
{
	return material;
}

std::vector<triangle> ModelSection::getTriandles()
{
	return std::vector<triangle>(triangles);
}


