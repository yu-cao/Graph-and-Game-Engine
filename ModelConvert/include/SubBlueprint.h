//
// Created by yu cao on 2019-02-27.
//

#ifndef RENDERENGINE_SUBBLUEPRINT_H
#define RENDERENGINE_SUBBLUEPRINT_H

#include <vector>
#include "ModelSection.h"

class SubBlueprint
{
public:
	SubBlueprint() = default;
	~SubBlueprint() = default;
	void addSection(ModelSection* section);
	void setMin(Vec3 min);
	void setMax(Vec3 max);
	Vec3 getMin();
	Vec3 getMax();
	int getVertexCount();
	std::vector<ModelSection*> getSection();

private:
	std::vector<ModelSection *> sections;
	Vec3 min, max;
};


#endif //RENDERENGINE_SUBBLUEPRINT_H
