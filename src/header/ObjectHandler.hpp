#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class ObjectHandler
{
public:

	static unsigned int NewObject(std::string &name);

	static void PrintFirstVertex(int object_index);

	static float* GetVerticesOpenGl();
	static unsigned int* GetIndicesOpenGl();

	static void Clear();

private:

	static struct Data
	{
		std::string name;
		std::vector<float> vertices;
		std::vector<float> texels;
		std::vector<float> normals;
		std::vector<unsigned int> faces;
		std::vector<float> min_max;
		float magnitude_range;
	} object;

	static std::vector<Data>* objects;

	static void GetVertex(std::string &line);
	static void GetTexel(std::string& line);
	static void GetNormal(std::string& line);
	static void GetFace(std::string& line);
	static float GetMagnitudeRange();

	static std::vector<std::string> SplitString(const std::string& line, const char delimiter);
};

std::vector<ObjectHandler::Data>* ObjectHandler::objects = new std::vector<ObjectHandler::Data>();

unsigned int ObjectHandler::NewObject(std::string &name)
{
	std::string file_name = "default.obj";
	std::ifstream file(file_name.c_str());
	std::string line;

	object.name = name;

	while (getline(file, line))
	{
		std::string head = line.substr(0, 2);

		if (head.compare("v ") == 0)
		{
			GetVertex(line);
		}
		else if (head.compare("vt") == 0)
		{
			GetTexel(line);
		}
		else if (head.compare("vn") == 0)
		{
			GetNormal(line);
		}
		else if (head.compare("f ") == 0)
		{
			GetFace(line);
		}
	}

	object.min_max.push_back(9999999.0);
	object.min_max.push_back(0);

	object.magnitude_range = GetMagnitudeRange();
	
	file.close();

	objects->push_back(object);

	unsigned int object_id = objects->size() - 1;
	return object_id;
}

void ObjectHandler::GetVertex(std::string& line)
{
	std::vector<std::string> line_items = SplitString(line, ' ');

	object.vertices.push_back(std::stof(line_items.at(0)));
	object.vertices.push_back(std::stof(line_items.at(1)));
	object.vertices.push_back(std::stof(line_items.at(2)));
}

void ObjectHandler::GetTexel(std::string& line)
{
	std::vector<std::string> line_items = SplitString(line, ' ');

	object.texels.push_back(std::stof(line_items.at(0)));
	object.texels.push_back(std::stof(line_items.at(1)));
}

void ObjectHandler::GetNormal(std::string& line)
{
	std::vector<std::string> line_items = SplitString(line, ' ');

	object.normals.push_back(std::stof(line_items.at(0)));
	object.normals.push_back(std::stof(line_items.at(1)));
	object.normals.push_back(std::stof(line_items.at(2)));
}

void ObjectHandler::GetFace(std::string& line)
{

}

float ObjectHandler::GetMagnitudeRange()
{
	return 0;
}

std::vector<std::string> SplitString(const std::string &line, const char delimiter)
{
	std::vector<std::string> temp_items;
	int element_index = 0;

	for (size_t i = 0; i < line.length(); i++)
	{
		if (line.at(i) == ' ')
		{
			temp_items.push_back(line.substr(element_index, i - 1));
			element_index = i + 1;
		}
	}

	return temp_items;
}

float* ObjectHandler::GetVerticesOpenGl()
{
	return &object.vertices[0];
}

// needs fixed so that it isnt throwing back whole faces, just the vertex indices
unsigned int* ObjectHandler::GetIndicesOpenGl()
{
	return &object.faces[0];
}

void  ObjectHandler::PrintFirstVertex(int object_index)
{
	std::cout << "x" << std::endl;
}
