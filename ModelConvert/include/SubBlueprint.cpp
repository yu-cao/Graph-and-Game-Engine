//
// Created by yu cao on 2019-02-27.
//

#include "SubBlueprint.h"

void SubBlueprint::addSection(ModelSection *section)
{
	sections.push_back(section);
}

void SubBlueprint::setMin(Vec3 min)
{
	this->min = min;
}

void SubBlueprint::setMax(Vec3 max)
{
	this->max = max;
}

Vec3 SubBlueprint::getMin()
{
	return min;
}

Vec3 SubBlueprint::getMax()
{
	return max;
}

int SubBlueprint::getVertexCount()
{
	int res = 0;
	for (auto &section : sections)
		res += section->getVertexCount();
	return res;
}

//Cost is high, need improve
std::vector<ModelSection *> SubBlueprint::getSection()
{
	const std::vector<ModelSection*> unmodifiedSection = sections;
	return unmodifiedSection;
}

