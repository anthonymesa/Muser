/** @file Object3dModel.hpp
 *  @author Anthony Mesa
 *  @copyright Copyright (C) 2019  Anthony Mesa
 * 
 * Need: 
 * 
 *      Vertex array for OpenGl, float[]
 *      Index array for OpenGl, float[]
 *      Array of matched vertexes and texels for audio processing, float[]
 * 
 * Functionality:
 * 
 *      I don't want people to be able to open random .obj files.
 *          - by Splitting the obj file into multiple files without the prefix, obj files cant be used
 *      I do want people to be able to save their muse and open in another editing program.
 *      I don't want to add vertices, texels, normals or faces to the muse.
 *      I may not want the files to be read directly ( I may want to encrypt them in some way ).
 *      I do want to have a base file that is read on startup.
 */

#pragma once

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <bi_tools.hpp>

typedef std::vector<std::tuple<float, float, float, float, float>> VectorTuple5f;

class ObjGl
{
    public:

        ObjGl();
        ObjGl(std::string &file_name);
        ~ObjGl();
        float* GetMuseVertices();
        float* GetMuseTexels();
        float* GetMuseNormals();
        unsigned int* GetMuseFaces();
        VectorTuple5f* GetVtData();
        float *GetVnData();

    private:

        VectorTuple5f vt_data; 
        float* vn_data;                    // Data to be passed for spectrogram generation
        std::string default_muse_folder = "data/default_muse";
        std::unordered_map<int, int> vt_map{};
        std::unordered_map<int, int> vn_map{};

        const std::size_t vertices_size = 294918;
        const std::size_t texels_size = 197378;
        const std::size_t normals_size = 294918;
        const std::size_t faces_size = 589824;

        struct muse_data
        {
            float *vertices;    // OpenGl array with 98,306 vertices of 3 floats, roughly 1.179672 megabytes.
            float *texels;        // OpenGl array with 98,689 vertices of 2 floats, roughly 0.789512 megabytes.
            float *normals;      // OpenGl array with 98,306 vertices of 3 floats, roughly 1.179672 megabytes.
            unsigned int *faces;   // OpenGl indices array with 196,608 faces of 3 ints, roughly 2.359296 megabytes.
        } muse;

        void LoadObject(std::string &file_name);
        void LoadVertices(std::ifstream &vertex_file);
        void LoadTexels(std::ifstream &texel_file);
        void LoadNormals(std::ifstream &normal_file);
        void LoadFaces(std::ifstream &face_file);
        std::vector<std::string> Split(const std::string &input, const char &Splitter);
        void SetVtData();
        void SetVnData();
};

ObjGl::ObjGl() : ObjGl(default_muse_folder)
{

}

ObjGl::ObjGl(std::string &file_name)
{
    std::cout << "Loading " << file_name << std::endl;
    muse.vertices = new float[vertices_size];
    muse.texels = new float[texels_size];
    muse.normals = new float[normals_size];
    muse.faces = new unsigned int[faces_size];
    vn_data = new float[294918 * 6];
    LoadObject(file_name);
}

ObjGl::~ObjGl(){
    delete[] muse.vertices;
    delete[] muse.texels;
    delete[] muse.normals;
    delete[] muse.faces;
    delete[] vn_data;
}

// Obj data is loaded as 4 seperate .mus files in a folder, then parsed individually.
void ObjGl::LoadObject(std::string &file_name){
    std::string vertex_file_path = file_name + "/vertex.mus";
    std::string texel_file_path = file_name + "/texel.mus";
    std::string normal_file_path = file_name + "/normal.mus";
    std::string face_file_path = file_name + "/face.mus";

    std::cout << face_file_path << std::endl;

    std::ifstream vertex_file(vertex_file_path.c_str());
    std::ifstream texel_file(texel_file_path.c_str());
    std::ifstream normal_file(normal_file_path.c_str());
    std::ifstream face_file(face_file_path.c_str());

    LoadVertices(vertex_file);
    LoadTexels(texel_file);
    LoadNormals(normal_file);
    LoadFaces(face_file);

    SetVtData();
    SetVnData();
}

void ObjGl::LoadVertices(std::ifstream &vertex_file)
{
    std::string line;
    std::vector<std::string> temp;
    int count = 0;
    std::cout << sizeof(unsigned int) << ' ' << sizeof(GLuint) << std::endl;
    while(getline(vertex_file, line))
    {
        if (line == "") {
            continue;
        }
        temp = Split(line, ' ');
        if (temp.size() != 3) {
            std::cout << "not 3 bro" << std::endl;
        }
        for(int i = 0; i < 3; i++)
        {
            muse.vertices[(count * 3) + i] = (float)atof(temp[i].c_str());
        }
        count++;
    } 
    std::cout << "LoadVerticies() done" << std::endl;
}

void ObjGl::LoadTexels(std::ifstream &texel_file)
{
    std::string line;
    std::vector<std::string> temp;
    int count = 0;
    while(getline(texel_file, line))
    {
        temp = Split(line, ' ');
        for(int i = 0; i < 2; i++)
        {
            muse.texels[(count * 2) + i] = (float)atof(temp[i].c_str());
        }
        count++;
    }
    std::cout << "LoadTexels() done" << std::endl;
}

void ObjGl::LoadNormals(std::ifstream &normal_file)
{
    std::string line;
    std::vector<std::string> temp;
    int count = 0;
    while(getline(normal_file, line))
    {
        temp = Split(line, ' ');
        for(int i = 0; i < 3; i++)
        {
            muse.normals[(count * 3) + i] = (float)atof(temp[i].c_str());
        }
        count++;
    }
    std::cout << "LoadNormals() done" << std::endl;
}

void ObjGl::LoadFaces(std::ifstream &face_file)
{
    std::string line;
    std::vector<std::string> temp;
    int count = 0;
    if (!face_file) {
        std::cout << "file done fucked up" << std::endl;
    }
    while(getline(face_file, line))
    {
        temp = Split(line, ' ');

        for(int i = 0; i < 3; i++)
        {
            int v = (int)atoi(Split(temp[i], '/')[0].c_str());
            int t = (int)atoi(Split(temp[i], '/')[1].c_str());
            int n = (int)atoi(Split(temp[i], '/')[2].c_str());

            vt_map[v - 1] = t - 1;
            vn_map[v - 1] = n - 1;
            muse.faces[(count * 3) + i] = v - 1;
        }
        count++;
    }
    std::cout << "LoadFaces() done" << std::endl;
}

std::vector<std::string> ObjGl::Split(const std::string &input, const char &splitter)
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

void ObjGl::SetVtData()
{
    for(int i = 0; i < (vertices_size / 3); i++)
    {   
        float v_x = muse.vertices[(i * 3)];
        float v_y = muse.vertices[(i * 3) + 1];
        float v_z = muse.vertices[(i * 3) + 2];
        if (vt_map.count(i) <= 0) {
            std::cout << "Missing vertex" << std::endl;
            throw std::runtime_error("Missing vertex");
        }
        float t_u = muse.texels[((vt_map[i]) * 2)];
        float t_v = muse.texels[((vt_map[i]) * 2) + 1];
        vt_data.push_back(std::make_tuple(
            v_x, v_y, v_z,
            t_u, t_v
        ));
    }
    std::cout << "SetVtData()" << std::endl;
}

void ObjGl::SetVnData()
{
    unsigned int counter = 0;
    for(int i = 0; i < (vertices_size / 3); i++)
    {   
        float v_x = muse.vertices[(i * 3)];
        float v_y = muse.vertices[(i * 3) + 1];
        float v_z = muse.vertices[(i * 3) + 2];
        if (vn_map.count(i) <= 0) {
            std::cout << "Missing normal" << std::endl;
            throw std::runtime_error("Missing normal");
        }
        float n_x = muse.normals[((vn_map[i]) * 3)];
        float n_y = muse.normals[((vn_map[i]) * 3) + 1];
        float n_z = muse.normals[((vn_map[i]) * 3) + 2];
        vn_data[counter + 0] = v_x;
        vn_data[counter + 1] = v_y;
        vn_data[counter + 2] = v_z;
        vn_data[counter + 3] = n_x;
        vn_data[counter + 4] = n_y;
        vn_data[counter + 5] = n_z;
        counter += 6;
    }
    std::cout << "SetVnData()" << std::endl;
}

VectorTuple5f* ObjGl::GetVtData()
{
    return &vt_data;
}

float *ObjGl::GetVnData()
{
    return vn_data;
}

float* ObjGl::GetMuseVertices()
{
    return muse.vertices;
}

float* ObjGl::GetMuseTexels()
{
    return muse.texels;
}

float* ObjGl::GetMuseNormals()
{
    return muse.normals;
}

unsigned int* ObjGl::GetMuseFaces()
{
    return muse.faces;
}