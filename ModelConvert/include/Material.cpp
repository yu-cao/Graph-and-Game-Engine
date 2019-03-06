//
// Created by yu cao on 2019-02-27.
//

#include "Material.h"

Material::Material(std::string name, Material::Color mat_color)
{
	this->name = name;
	this->mat_color = mat_color;
}

Material::Color::Color(float flag, float code, float offset)
{
	this->flag = flag;
	this->code = code;
	this->offset = offset;
}