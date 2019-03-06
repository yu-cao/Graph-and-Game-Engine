//
// Created by yu cao on 2019-02-27.
//

#ifndef RENDERENGINE_TRIANGLE_H
#define RENDERENGINE_TRIANGLE_H

#include "Vertex.h"

class triangle
{
public:
	triangle() = default;
	triangle(Vertex p0, Vertex p1, Vertex p2);
	~triangle() = default;

	std::string toString();
private:
	Vertex p0, p1, p2;
};


#endif //RENDERENGINE_TRIANGLE_H
