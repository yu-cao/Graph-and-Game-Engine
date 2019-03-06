//
// Created by yu cao on 2019-02-27.
//

#ifndef RENDERENGINE_MODELSECTION_H
#define RENDERENGINE_MODELSECTION_H

#include <vector>
#include "Material.h"
#include "triangle.h"

class ModelSection
{
public:
	ModelSection(Material* mat);
	~ModelSection() = default;
	void addTriangle(triangle tri);
	int getVertexCount();
	Material* getMaterial();
	std::vector<triangle> getTriandles();

private:
	Material* material;
	std::vector<triangle> triangles;
};

#endif //RENDERENGINE_MODELSECTION_H
