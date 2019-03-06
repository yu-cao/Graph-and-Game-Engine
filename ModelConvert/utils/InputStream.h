//
// Created by yu cao on 2019-02-27.
//

#ifndef RENDERENGINE_INPUTSTREAM_H
#define RENDERENGINE_INPUTSTREAM_H

#include <iostream>
#include <fstream>
#include <string>

class InputStream
{
public:
	InputStream() = default;
	~InputStream() = default;
	static bool getBool(std::string message);
	static float getFloat(std::string message);
	static std::string getString(std::string message);
	static int getInt(std::string message);
};


#endif //RENDERENGINE_INPUTSTREAM_H
