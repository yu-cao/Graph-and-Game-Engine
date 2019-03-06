//
// Created by yu cao on 2019-02-27.
//

#include "triangle.h"

triangle::triangle(Vertex p0, Vertex p1, Vertex p2)
{
	this->p0 = p0;
	this->p1 = p1;
	this->p2 = p2;
}

std::string triangle::toString()
{
	std::string P0 = "P0:" + std::to_string(p0.getNormal().x()) + "," + std::to_string(p0.getNormal().y()) + "," +
					 std::to_string(p0.getNormal().z()) + "," + std::to_string(p0.getPosition().x()) + "," +
					 std::to_string(p0.getPosition().y()) + "," + std::to_string(p0.getPosition().z()) + "  ";
	std::string P1 = "P1:" + std::to_string(p1.getNormal().x()) + "," + std::to_string(p1.getNormal().y()) + "," +
					 std::to_string(p1.getNormal().z()) + "," + std::to_string(p1.getPosition().x()) + "," +
					 std::to_string(p1.getPosition().y()) + "," + std::to_string(p1.getPosition().z()) + "  ";
	std::string P2 = "P2:" + std::to_string(p2.getNormal().x()) + "," + std::to_string(p2.getNormal().y()) + "," +
					 std::to_string(p2.getNormal().z()) + "," + std::to_string(p2.getPosition().x()) + "," +
					 std::to_string(p2.getPosition().y()) + "," + std::to_string(p2.getPosition().z());
	return P0 + P1 + P2;
}