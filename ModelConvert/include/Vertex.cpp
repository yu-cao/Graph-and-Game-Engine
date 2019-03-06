//
// Created by yu cao on 2019-02-27.
//

#include "Vertex.h"

Vertex::Vertex(Vec3 position, Vec3 normal)
{
	this->position = position;
	this->normal = normal;
}

Vec3 Vertex::getPosition()
{
	return position;
}

Vec3 Vertex::getNormal()
{
	return normal;
}
