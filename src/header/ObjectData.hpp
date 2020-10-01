/** @file ObjectData.hpp
 *  @author Anthony Mesa
 *  @copyright Copyright (C) 2019  Anthony Mesa
 */

#pragma once

#include <string>

class ObjectData
{
  public:

    std::string name = "default";
    float* vertices;
    float* texels;
    float* normals;
    unsigned int* faces;
    float* min_max = new float[] {99999.0, 0.0};
    float magnitude_range;

    ObjectData();
    ObjectData(std::string &object_name);
    ~ObjectData();

  private:

    void LoadObject(std::string &file_name);
    void Prepare(std::ifstream &object_file);
}

ObjectData::ObjectData()
{
  ObjectData("default");
}

/**
*
*
* param@ file_name The filename of the .obj file to be opened
*/
ObjectData::ObjectData(std::string &object_name)
{
  name = object_name;
  std::string file = object_name + ".obj";
  LoadObject(file);
}

ObjectData::~ObjectData()
{
  delete[] this.vertices;
  delete[] this.texels;
  delete[] this.normals;
  delete[] this.faces;
}

void ObjectData::LoadObject(std::string &file_name)
{
  std::ifstream object_file(file_name.c_str());

  PrepareArrays(object_file);
  LoadArrays(object_file);
}

void ObjectData::PrepareArrays(std::ifstream &object_file)
{
  std::string line;
  int* count = new int[4];

  while(getline(object_file, line))
  {
    std::string head = line.substr(0, 2);

    if(head.compare("v ") == 0)
    {
      count[0]++;
    } else if (head.compare("vt"){
      count[1]++;
    } else if (head.compare("vn"){
      count[2]++;
    } else if (head.compare("f "){
      count[3]++;
    }
  }

  this.vertices = new float[count[0] * 3];
  this.texels = new float[count[1] * 2];
  this.normals = new float[count[2] * 3];
  this.faces = new int[(count[3] * 3) * 3];
}

void ObjectData::LoadArrays(std::ifstream &object_file)
{
  std::string line;
  int* count = new int[4];

  while(getline(object_file, line))
  {
    std::string head = line.substr(0, 2);

    if(head.compare("v ") == 0)
    {
      ParseVertex(count[0]++, line);
    } else if (head.compare("vt"){
      ParseTexel(count[1]++, line);
    } else if (head.compare("vn"){
      ParseNormal(count[2]++, line);
    } else if (head.compare("f "){
      ParseFace(count[3]++, line);
    }
  }
}

void ObjectData::ParseVertex(int vertices_index, String line){
  std::vector<std::string> line_items = SplitString(line.substring(2), ' ');

  for(int vector_index = 0; vector_index < line_items.size(); vector_index++){
    this.vertices[(vertices_index * 3) + vector_index] = std::stof(line_items.get(vector_index));
  }
}

void ObjectData::ParseTexel(int texels_index, String line){
  std::vector<std::string> line_items = SplitString(line.substring(2), ' ');

  for(int vector_index = 0; vector_index < line_items.size(); vector_index++){
    this.texels[(texels_index * 2) + vector_index] = std::stof(line_items.get(vector_index));
  }
}

void ObjectData::ParseNormal(int normals_index, String line){
  std::vector<std::string> line_items = SplitString(line.substring(2), ' ');

  for(int vector_index = 0; vector_index < line_items.size(); vector_index++){
    this.normals[(normals_index * 3) + vector_index] = std::stof(line_items.get(vector_index));
  }
}

void ObjectData::ParseFace(int faces_index, String line){
  std::vector<std::string> line_items = SplitString(line.substring(2), ' ');

  for(int point_index = 0; point_index < line_items.size(); point_index++){
    std::vector<std::string> points = SplitString(line_items.get(), '/');

    for(int element_index = 0; element_index < point.length; element_index++){
      faces[(faces_index * 9) + (point_index * 3) + element_index] = std::stof(point.get(element_index));
    }
  }
}

std::vector<std::string> SplitString(std::string &line, char &delimiter)
{
  std::vector<std::string> temp_items;
  int element_index = 0;

  for(size_t i = 0; i < line.length(); i++)
  {
    if(line.at(i) == ' ')
    {
      temp_items.add(line.substring(element_index, i - 1));
      element_index = i + 1;
    }
  }

  return temp_items;
}
