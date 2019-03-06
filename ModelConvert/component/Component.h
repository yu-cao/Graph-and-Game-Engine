//
// Created by yu cao on 2019-02-27.
//

#ifndef RENDERENGINE_COMPONENT_H
#define RENDERENGINE_COMPONENT_H

#include <string>

class Component
{
public:
	virtual std::string getName()
	{
		return name;
	}
	virtual int getType()
	{
		return type;
	}
	virtual std::string toString() = 0;
	virtual void getInfo() = 0;

protected:
	int type;
	std::string name;
};

#endif //RENDERENGINE_COMPONENT_H
