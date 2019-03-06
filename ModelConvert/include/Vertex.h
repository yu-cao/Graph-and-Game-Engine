//
// Created by yu cao on 2019-02-27.
//

#ifndef RENDERENGINE_VERTEX_H
#define RENDERENGINE_VERTEX_H
#include "Vec3.h"

class Vertex
{
public:
	Vertex(Vec3 postion, Vec3 normal);
	Vertex() = default;
	~Vertex(){};
	Vec3 getPosition();
	Vec3 getNormal();

private:
	Vec3 position;
	Vec3 normal;
};


#endif //RENDERENGINE_VERTEX_H
