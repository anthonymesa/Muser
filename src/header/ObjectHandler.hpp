#pragma once

/*

Author: Anthony Mesa
Date: 10/3/2020

This class provides access to the creation and manipulation of the 3D data
loaded from blender waveform '.obj' files.

*/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include "Mantis.hpp"

/*

This object handler class maintains all of the actual
object data for all of the Muses in a scene. All of
the struct data objects are kept in a vector. When a new
object is created, the index of the object's location
in the vector is returned as that object's ID.

*/

class ObjectHandler
{
public:

	static unsigned int NewObject();

	//i maybe should put this together into a single getdata function that returns a tuple of pointers
	static std::vector<float>* GetVerticesVector(unsigned int object_index);
	static std::vector<float>* GetTexelsVector(unsigned int object_index);
	static std::vector<float>* GetNormalsVector(unsigned int object_index);
	static std::vector<unsigned int>* GetIndicesVector(unsigned int object_index);
	static std::vector<float>* GetMinMax(unsigned int object_index);
	static float GetMagnitudeDiffernce(unsigned int object_index);
	static void ClearAllObjects();
	static void CleanUp();

private:

	struct ObjectData
	{
		std::vector<float> vertices;
		std::vector<float> texels;
		std::vector<float> normals;
		std::vector<unsigned int> faces;
		//make this a tuple one day, dont need vector, only two elements (saves memory)
		std::vector<float> min_max_vertex_magnitude;
		float vertex_magnitude_difference;
	};

	static ObjectData* object;
	static std::vector<ObjectData>* objects;

	static float NormalizeVector(std::tuple<float, float, float> vertex);
	static void WriteDefaultFileToObject();
	static void AppendObjectVertices(std::string& line);
	static void AppendObjectTexels(std::string& line);
	static void AppendObjectNormals(std::string& line);
	static void AppendObjectFaces(std::string& line);
	static void SetMinMaxVertexMagnitude();
	static void SetVertexMagnitudeDifference();
	static std::vector<std::string> SplitString(const std::string& line, const char& delimiter);

	static Mantis* object_log;
	static std::string DEFAULT_OBJ;
};

ObjectHandler::ObjectData* ObjectHandler::object = new ObjectHandler::ObjectData();
std::vector<ObjectHandler::ObjectData>* ObjectHandler::objects = new std::vector<ObjectHandler::ObjectData>();
Mantis* ObjectHandler::object_log = new Mantis("object_log");
std::string ObjectHandler::DEFAULT_OBJ = "test_file_high_res";

unsigned int ObjectHandler::NewObject()
{
	SetMinMaxVertexMagnitude();
	SetVertexMagnitudeDifference();
	WriteDefaultFileToObject();

	ObjectHandler::objects->push_back(*ObjectHandler::object);

	unsigned int object_id = objects->size() - 1;
	return object_id;
}

void ObjectHandler::WriteDefaultFileToObject()
{
	std::string temp_path(std::filesystem::current_path());
	std::string file_name = temp_path.append("/data/" + DEFAULT_OBJ + ".obj");
	std::ifstream file(file_name.c_str());
	std::string line;

	Mantis* vert_log = new Mantis("obj_vert_log");
	Mantis* tex_log = new Mantis("obj_tex_log");
	Mantis* norm_log = new Mantis("obj_norm_log");
	Mantis* face_log = new Mantis("obj_face_log");

	while (getline(file, line))
	{
		std::string head = line.substr(0, 2);

		if (head.compare("v ") == 0)
		{
			std::string values = line.substr(2);
			AppendObjectVertices(values);
			vert_log->Log(values);
		}
		else if (head.compare("vt") == 0)
		{
			std::string values = line.substr(3);
			AppendObjectTexels(values);
			tex_log->Log(values);
		}
		else if (head.compare("vn") == 0)
		{
			std::string values = line.substr(3);
			AppendObjectNormals(values);
			norm_log->Log(values);
		}
		else if (head.compare("f ") == 0)
		{
			std::string values = line.substr(2);
			AppendObjectFaces(values);
			face_log->Log(values);
		}
	}

	delete vert_log;
	delete tex_log;
	delete norm_log;
	delete face_log;

	file.close();
}

// ATTENTION: this method is repeated in ZMesh??? I should fix that
float ObjectHandler::NormalizeVector(std::tuple<float, float, float> vertex)
{
	return std::sqrt(
		std::pow(std::get<0>(vertex), 2) +
		std::pow(std::get<1>(vertex), 2) +
		std::pow(std::get<2>(vertex), 2)
	);
}

void ObjectHandler::AppendObjectVertices(std::string& values)
{
	std::vector<std::string> line_items = SplitString(values, ' ');

	object_log->Log("------------------------");
	object_log->Log("vertex coords: ");
	object_log->Log(values);

	//appending vertices
	ObjectHandler::object->vertices.push_back(std::stof(line_items.at(0)));
	ObjectHandler::object->vertices.push_back(std::stof(line_items.at(1)));
	ObjectHandler::object->vertices.push_back(std::stof(line_items.at(2)));

	//ATTENTION: this following code should be broken up into functions

	//turn into tuple for normalizing distance
	std::tuple<float, float, float> distance {
		std::stof(line_items.at(0)),
		std::stof(line_items.at(1)),
		std::stof(line_items.at(2)),
	};

	float distance_normalized = ObjectHandler::NormalizeVector(distance);

	object_log->Log("Normalized distance: " + std::to_string(distance_normalized));
	object_log->Log("Current min and max: ");
	object_log->Log(ObjectHandler::object->min_max_vertex_magnitude);

	//set min max accordingly
	if (distance_normalized < ObjectHandler::object->min_max_vertex_magnitude.at(0)) {
		ObjectHandler::object->min_max_vertex_magnitude.at(0) = distance_normalized;
	}
	if (distance_normalized > ObjectHandler::object->min_max_vertex_magnitude.at(1)) {
		ObjectHandler::object->min_max_vertex_magnitude.at(1) = distance_normalized;
	}

	object_log->Log("New min and max: ");
	object_log->Log(ObjectHandler::object->min_max_vertex_magnitude);

	//set min max difference accordingly
	ObjectHandler::object->vertex_magnitude_difference =
		ObjectHandler::object->min_max_vertex_magnitude.at(1) -
		ObjectHandler::object->min_max_vertex_magnitude.at(0);

	object_log->Log("Vertex magnitude difference: " + 
		std::to_string(ObjectHandler::object->vertex_magnitude_difference));
}

void ObjectHandler::AppendObjectTexels(std::string& line)
{
	std::vector<std::string> line_items = SplitString(line, ' ');

	ObjectHandler::object->texels.push_back(std::stof(line_items.at(0)));
	ObjectHandler::object->texels.push_back(std::stof(line_items.at(1)));
}

void ObjectHandler::AppendObjectNormals(std::string& line)
{
	std::vector<std::string> line_items = SplitString(line, ' ');

	ObjectHandler::object->normals.push_back(std::stof(line_items.at(0)));
	ObjectHandler::object->normals.push_back(std::stof(line_items.at(1)));
	ObjectHandler::object->normals.push_back(std::stof(line_items.at(2)));
}

void ObjectHandler::AppendObjectFaces(std::string& line)
{
	std::vector<std::string> line_items = SplitString(line, ' ');
	for (auto face : line_items)
	{
		std::vector<std::string> face_data = SplitString(face, '/');
		ObjectHandler::object->faces.push_back(std::stof(face_data.at(0)));
		ObjectHandler::object->faces.push_back(std::stof(face_data.at(1)));
		ObjectHandler::object->faces.push_back(std::stof(face_data.at(2)));
	}
}

void ObjectHandler::SetMinMaxVertexMagnitude()
{
	std::vector<float> temp;
	temp.push_back(9999.0);
	temp.push_back(0);

	ObjectHandler::object->min_max_vertex_magnitude = temp;
}

void ObjectHandler::SetVertexMagnitudeDifference()
{
	ObjectHandler::object->vertex_magnitude_difference = 0;
}

std::vector<std::string> ObjectHandler::SplitString(const std::string& line, const char& delimiter)
{
	std::vector<std::string> result;
	int element_index = 0;
	for (size_t i = 0; i < line.size(); i++)
	{
		if (line[i] == delimiter)
		{
			result.push_back(line.substr(element_index, i - element_index));
			element_index = i + 1;
		}
	}
	result.push_back(line.substr(element_index));
	return result;
}

std::vector<float>* ObjectHandler::GetVerticesVector(unsigned int object_index)
{
	return &ObjectHandler::objects->at(object_index).vertices;
}

std::vector<float>* ObjectHandler::GetNormalsVector(unsigned int object_index)
{
	return &ObjectHandler::objects->at(object_index).normals;
}

std::vector<float>* ObjectHandler::GetTexelsVector(unsigned int object_index)
{
	return &ObjectHandler::objects->at(object_index).texels;
}

std::vector<unsigned int>* ObjectHandler::GetIndicesVector(unsigned int object_index)
{
	return &ObjectHandler::objects->at(object_index).faces;
}

std::vector<float>* ObjectHandler::GetMinMax(unsigned int object_index)
{
	return &ObjectHandler::objects->at(object_index).min_max_vertex_magnitude;
}

float ObjectHandler::GetMagnitudeDiffernce(unsigned int object_index)
{
	return ObjectHandler::objects->at(object_index).vertex_magnitude_difference;
}

void ObjectHandler::ClearAllObjects()
{
	ObjectHandler::objects->clear();
}

void ObjectHandler::CleanUp()
{
	delete ObjectHandler::object;
	delete ObjectHandler::objects;
	delete ObjectHandler::object_log;
}