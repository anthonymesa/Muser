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
        ~ObjGl();
        float* GetMuseVertices();
        float* GetMuseTexels();
        float* GetMuseNormals();
        int* GetMuseFaces();
        VectorTuple5f* GetVtData();

    private:

        VectorTuple5f vt_data;                     // Data to be passed for spectrogram generation
        std::string default_muse_folder = "data/Default_muse";
        std::unordered_map<int, int> vt_map;

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
        };

        muse_data muse;

        // Load the object folder and load individual object files.
        void LoadObject(std::string &file_name);

        // Populate the arrays with the file data
        void LoadVertices(std::ifstream &vertex_file);
        void LoadTexels(std::ifstream &texel_file);
        void LoadNormals(std::ifstream &normal_file);
        void LoadFaces(std::ifstream &face_file);

        // Split string into vector of strings using delimiter.
        std::vector<std::string> Split(const std::string &input, const char &Splitter);

        void SetVtData();
};

ObjGl::ObjGl()
{
    bug("created ObjGl");
    muse.vertices = new float[vertices_size];
    muse.texels = new float[texels_size];
    muse.normals = new float[normals_size];
    muse.faces = new unsigned int[faces_size];
    bug("created the four arrays");
    LoadObject(default_muse_folder);
    bug("loaded the objects");
}

ObjGl::~ObjGl(){
    delete[] muse.vertices;
    delete[] muse.texels;
    delete[] muse.normals;
    delete[] muse.faces;
}

void ObjGl::LoadObject(std::string &file_name){
    std::string vertex_file_path = file_name + "/vertex.mus";
    std::string texel_file_path = file_name + "/texel.mus";
    std::string normal_file_path = file_name + "/normal.mus";
    std::string face_file_path = file_name + "/face.mus";

    std::ifstream vertex_file(vertex_file_path.c_str());
    std::ifstream texel_file(texel_file_path.c_str());
    std::ifstream normal_file(normal_file_path.c_str());
    std::ifstream face_file(face_file_path.c_str());

    bug("created ifstreams");
    LoadVertices(vertex_file);
    LoadTexels(texel_file);
    LoadNormals(normal_file);
    LoadFaces(face_file);

    vertex_file.close();
    texel_file.close();
    normal_file.close();
    face_file.close();
    
    bug("closed the files");

    SetVtData();
}


void ObjGl::LoadVertices(std::ifstream &vertex_file)
{
    std::string line;
    std::vector<std::string> temp;
    int count = 0;
    bug("loading vertices");
    while(getline(vertex_file, line))
    {
        temp = Split(line, ' ');
        for(int i = 0; i < 3; i++)
        {
            muse.vertices[(count * 3) + i] = (float)atof(temp[i].c_str());
        }
        count++;
    } 
    bug("finished loading vertices");
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
}

void ObjGl::LoadFaces(std::ifstream &face_file)
{
    std::string line;
    std::vector<std::string> temp;
    int count = 0;
    while(getline(face_file, line))
    {
        temp = Split(line, ' ');

        for(int i = 0; i < 3; i++)
        {
            int v = (int)atoi(Split(temp[i], '/')[0].c_str());
            int t = (int)atoi(Split(temp[i], '/')[1].c_str());
                
            vt_map[v - 1] = t - 1;
            muse.faces[(count * 3) + i] = v;
        }
        count++;
    }
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
    bug("setting vt data");
    for(int i = 0; i < (vertices_size / 3); i++)
    {   
        bug("iterating through the array");
        float v_x = muse.vertices[(i * 3)];
        bug("one");
        float v_y = muse.vertices[(i * 3) + 1];
        bug("two");
        float v_z = muse.vertices[(i * 3) + 2];
        bug("three");
        float t_u = muse.texels[((vt_map.find(i)->second) * 3)];
        bug("four");
        float t_v = muse.texels[((vt_map.find(i)->second) * 3) + 1];

        bug("finished getting vvvtt");
        vt_data.push_back(std::make_tuple(
            v_x, v_y, v_z,
            t_u, t_v
        ));
    }
}

VectorTuple5f* ObjGl::GetVtData()
{
    return &vt_data;
}