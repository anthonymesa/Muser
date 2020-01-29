/** @file Object3dModel.h
 *  @author Anthony Mesa
 *  @copyright Copyright (C) 2019  Anthony Mesa
 */

#pragma once

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <bi_tools.hpp>

typedef std::vector<std::tuple<float, float, float, float, float>> OBJarray;

class Object3dModel
{
public:

    Object3dModel();
    ~Object3dModel();

    void Load(const char* filename);
    OBJarray* GetObjArray();
    void GetOpenGLVerticies();
    void GetOpenGLIndicies();
    
private:

    std::string default_object = "data/default_muse.obj";
    OBJarray* obj_vertex_uv_coordinates;
    std::ofstream log;
    struct verticies_uvs_map
    {
        std::unordered_map<int, int> pointer_map;
        std::vector<std::tuple<float, float, float>> vertex_coordinates_list;
        std::vector<std::tuple<float, float>> uv_coordinates_list;
    };

    std::vector<std::string> Split(std::string input, char splitter);
    verticies_uvs_map ParseObj(const char* filename);
    std::tuple<float, float, float> VertexFromLine(const std::string &file_line);
    std::tuple<float, float> UvFromLine(const std::string &file_line);
    void GeneratePointerMap(const std::string &file_line, std::unordered_map<int, int> &pointer_map);
    void MapVertexToUv(const std::string &element_in_line, std::unordered_map<int, int> &pointer_map);
    void SetDefaultMuseArray(verticies_uvs_map &obj_info);
    void SetOpenGLVerticies();
    void SetOpenGLIndicies();

    float* vertex_opengl_array;
    float* index_opengl_array;
};

//====================================================
Object3dModel::Object3dModel()
{
    obj_vertex_uv_coordinates = new OBJarray;
    log.open("temp/log.txt");
    verticies_uvs_map obj_info = ParseObj(default_object);
    SetOpenglData(default_object, obj_info.vertex_coordinates_list.size(), obj_info.uv_coordinates_list.size());
    SetDefaultMuseArray(obj_info);
}

//====================================================
void Object3dModel::Load(const char* filename)
{
    verticies_uvs_map obj_info = ParseObj(filename);
    SetOpenglData(filename, obj_info.vertex_coordinates_list.size(), obj_info.uv_coordinates_list.size());
    SetDefaultMuseArray(obj_info);
}

//====================================================
OBJarray* Object3dModel::GetObjArray()
{
    return obj_vertex_uv_coordinates;
}

//====================================================
Object3dModel::~Object3dModel()
{
    delete obj_vertex_uv_coordinates;
}

//====================================================
std::vector<std::string> Object3dModel::Split(std::string input, char splitter)
{
    std::vector<std::string> result;
    int start = 0;
    for (size_t i = 0; i < input.size(); i++)
    {
        if (input[i] == splitter)
        {
            result.push_back(input.substr(start, i - start));
            start = i + 1;
        }
    }
    result.push_back(input.substr(start));
    return result;
}

//====================================================
Object3dModel::verticies_uvs_map Object3dModel::ParseObj(const char* filename)
{
    verticies_uvs_map obj_info;
    std::ifstream object_file(filename);
    std::string object_read_line;
    bool v = false, vt = false, f = false;

    log << "Reading .obj file:" << std::endl;
    while (getline(object_file, object_read_line))
    {
        if (object_read_line.find("v ") != std::string::npos)
        {  
            if (v == false){
                log << "Loading .obj vertexes..." << std::endl;
                v = true;
            }
            obj_info.vertex_coordinates_list.push_back(VertexFromLine(object_read_line));
        }
        else if (object_read_line.find("vt ") != std::string::npos)
        {
            if (vt == false){
                log << "Loading .obj texture coordinates..." << std::endl;
                vt = true;
            }
            obj_info.uv_coordinates_list.push_back(UvFromLine(object_read_line));
        }
        else if (object_read_line.find("f ") != std::string::npos)
        {
            if (f == false){
                log << "Mapping vertexes to texture coordinates..." << std::endl;
                f = true;
            }
            GeneratePointerMap(object_read_line, obj_info.pointer_map);
        }
    }

    log << "Creating pointer map log..." << std::endl;
    for (const auto &[ key, value ] : obj_info.pointer_map)
    {
        log << "Key: " << key << "| Value: " << value << std::endl;
    }
    log << "Map log complete..." << std::endl;
    log << "Processed " << obj_info.vertex_coordinates_list.size() << " vertexes coordinates." << std::endl;
    log<< "Processed " << obj_info.uv_coordinates_list.size() << " uv coordinates." << std::endl;
    log << "Mapped " << obj_info.pointer_map.size() << " vertexes to uv coordinates." << std::endl;
    object_file.close();
    if (!(object_file.good()))
    {
        log << "closed file..." << std::endl;
    }
    return obj_info;
}

//====================================================
std::tuple<float, float, float> Object3dModel::VertexFromLine(const std::string &file_line)
{
    float x, y, z;
    std::vector<std::string> v_split = Split(file_line, ' ');
    std::vector<std::string> v_shift(v_split.begin() + 1, v_split.end());
    x = (float)atof(v_shift[0].c_str());
    y = (float)atof(v_shift[1].c_str());
    z = (float)atof(v_shift[2].c_str());
    return std::make_tuple(x, y, z);
}

//====================================================
std::tuple<float, float> Object3dModel::UvFromLine(const std::string &file_line)
{
    float u, v;
    std::vector<std::string> vt_split = Split(file_line, ' ');
    std::vector<std::string> vt_shift(vt_split.begin() + 1, vt_split.end());
    u = (float)atof(vt_shift[0].c_str());
    v = (float)atof(vt_shift[1].c_str());
    return std::make_tuple(u, v);
}

//====================================================
void Object3dModel::GeneratePointerMap(const std::string &file_line, std::unordered_map<int, int> &pointer_map)
{
    std::vector<std::string> pointer_list = Split(file_line, ' ');
    std::vector<std::string> pointer_list_shift(pointer_list.begin() + 1, pointer_list.end());
    for (std::string& each : pointer_list_shift)
    {
        MapVertexToUv(each, pointer_map);
    }
}

//====================================================
void Object3dModel::MapVertexToUv(const std::string &element_in_line, std::unordered_map<int, int> &pointer_map)
{
    std::vector<std::string> pointer = Split(element_in_line, '/');
    std::vector<std::string> pointer_shift(pointer.begin(), pointer.begin() + 2);
    int vertex_index = atol(pointer_shift[0].c_str());
    int uv_index = atol(pointer_shift[1].c_str());
    pointer_map[vertex_index - 1] = uv_index - 1;
}

//====================================================
void Object3dModel::SetDefaultMuseArray(verticies_uvs_map &obj_info)
{
    log << "Printing Vertex/Coordinate list..." << std::endl;
    for(size_t i = 0; i < obj_info.vertex_coordinates_list.size(); i++)
    {   
        std::tuple<float, float, float> vertex = obj_info.vertex_coordinates_list[i];
        std::tuple<float, float> uv = obj_info.uv_coordinates_list[obj_info.pointer_map.find(i)->second];
        log << std::get<0>(vertex) << " " << std::get<1>(vertex) << " " << std::get<2>(vertex) << " " << std::get<0>(uv) << " " << std::get<1>(uv) << std::endl;
        (*obj_vertex_uv_coordinates).push_back(std::make_tuple(
            std::get<0>(vertex),
            std::get<1>(vertex),
            std::get<2>(vertex),
            std::get<0>(uv),
            std::get<1>(uv)
        ));
    }
}

void Object3dModel::SetOpenGLVerticiess()
{

    vertex_opengl_array = [];
}

void Object3dModel::SetOpenGLIndicies()
{
    index_opengl_array = [];
}

void Object3dModel::SetOpenglData(const char* filename, int point_amount, int index_amount)
{
    std::ifstream object_file(filename);
    std::string object_read_line;
    float points[] = new float[point_amount];
    float indexs[] = new float[index_amount];

    while (getline(object_file, object_read_line))
    {
        if (object_read_line.find("v ") != std::string::npos)
        {
            
        }
        else if (object_read_line.find("f ") != std::string::npos)
        {
            indexes.push_back();
        }
    }

    SetOpenGLVerticies(points);
    SetOpenGLIndicies();
}