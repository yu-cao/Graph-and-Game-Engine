//
// Created by yu cao on 2019-02-27.
//

#ifndef RENDERENGINE_MATERIAL_H
#define RENDERENGINE_MATERIAL_H

#include <iostream>
#include <string>

class Material
{
	class Color
	{
	public:
		Color(float flag, float code, float offset);
		Color() = default;
		void SetFlag(float flag) {
			this->flag = flag;
		}
		void SetCode(float code) {
			this->code = code;
		}
		void SetOffset(float offset) {
			this->offset = offset;
		}
		std::string toString() {
			return "flag:"  + std::to_string(flag) + "	" + "code:" + std::to_string(code) + "	" + "offset:" + std::to_string(offset);
		}
	private:
		float flag, code, offset;
	};

	std::string name;
	Color mat_color;

public:
	Material(std::string name,Color mat_color);
	Material() = default;
	~Material() = default;
};


#endif //RENDERENGINE_MATERIAL_H
