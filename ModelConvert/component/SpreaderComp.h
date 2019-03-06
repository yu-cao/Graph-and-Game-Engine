//
// Created by yu cao on 2019-02-27.
//

#ifndef RENDERENGINE_SPREADERCOMP_H
#define RENDERENGINE_SPREADERCOMP_H

#include "Component.h"
#include "../utils/InputStream.h"

class SpreaderComp : public Component
{
private:
	int biome;
	int strength;
	int distance;

public:
	SpreaderComp(std::string name, int type)
	{
		this->name = name;
		this->type = type;
	};

	void getInfo()
	{
		this->biome = InputStream::getInt("Enter biome ID: ");
		this->strength = InputStream::getInt("Enter strength: ");
		this->distance = InputStream::getInt("Enter range: ");
	}

	std::string toString()
	{
		return Component::getName() + "	" +
			   "biome:" + std::to_string(biome) + "	" +
			   "strength:" + std::to_string(strength) + "	" +
			   "distance:" + std::to_string(distance);
	}
};

#endif //RENDERENGINE_SPREADERCOMP_H
