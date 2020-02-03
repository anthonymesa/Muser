/** @file Object3dModel.hpp
 *  @author Anthony Mesa
 *  @copyright Copyright (C) 2019  Anthony Mesa
 * 
 */

#pragma once

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <bi_tools.hpp>

typedef std::vector<std::tuple<float, float, float, float, float>> ObjArray;
typedef std::vector<std::string> StringVector;
typedef std::tuple<float, float, float> Tuple3f;

class Object3dModel_new
{
public:

    Object3dModel_new();
    ~Object3dModel_new();
    void Load(const char* &filename);
    ObjArray* GetVTData();

private:

    const char *muse_base = "data/muse_base.obj";
    
    struct object
    {
        std::vector<Tuple3f> vertices;
        std::vector<Tuple3f> texels;
        std::vector<Tuple3f> normals;
        std::vector<Tuple3f> faces;
    };
    object model_data;
    std::unordered_map<int, int> vt_map;
    ObjArray* vt_audio_data;

    StringVector Split(const std::string &input, const char &splitter);
    StringVector LoadFile(const char* &filename);
    void ParseObject(const StringVector &model_string);
    StringVector LineToVector(const std::string &line);
    Tuple3f GetTuple(const std::string &line, const std::string &type);
    void AppendMap(const std::string &line);
    void SetVTData();
};

//====================================================
Object3dModel_new::Object3dModel_new()
{
    model_data = new object;
    vt_audio_data = new ObjArray;
    ParseObject(LoadFile(muse_base));
    bug("parsed object");
}

//====================================================
Object3dModel_new::~Object3dModel_new()
{
    delete vt_audio_data;
    delete model_data;
}

//====================================================
void Object3dModel_new::Load(const char* &filename)
{
    ParseObject(LoadFile(filename));
}

//====================================================
// Load .obj file and write contents to buffer
StringVector Object3dModel_new::LoadFile(const char* &filename)
{
    StringVector model_file_string;
    std::string line;
    std::ifstream file(filename);
    if(!file.good())
    {
       std:: cout << "ERROR OPENING OBJ FILE" << std::endl;
    }

    while(getline(file, line))
    {
        model_file_string.push_back(line);
    }

    file.close();
    bug("loaded file");
    return model_file_string;
}

//====================================================
// !!! See if you dont have to return an object, and rather can just update the actual object
void Object3dModel_new::ParseObject(const StringVector &model_string)
{
    for(size_t i = 0; i < model_string.size(); i++)
    {
        std::string line = model_string[i];
        bug("got line");
        if (line.find("v ") != std::string::npos)
        {
            (*model_data).vertices.push_back(GetTuple(line, "VERTEX"));
        }
        else if (line.find("vt ") != std::string::npos)
        {
            (*model_data).texels.push_back(GetTuple(line, "TEXEL"));
        }
        else if (line.find("vn ") != std::string::npos)
        {
            (*model_data).normals.push_back(GetTuple(line, "NORMAL"));
        }
        else if (line.find("f ") != std::string::npos)
        {
            (*model_data).faces.push_back(GetTuple(line, "FACE"));
            AppendMap(line);
        }
    }
    bug("set model_data");
    SetVTData();
    bug("set VTdata");
}

//====================================================
StringVector Object3dModel_new::Split(const std::string &input, const char &splitter)
{
    StringVector result;
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
StringVector Object3dModel_new::LineToVector(const std::string &line)
{
    StringVector split = Split(line, ' ');
    StringVector shift(split.begin() + 1, split.end());
    return shift;
}

//====================================================
Tuple3f Object3dModel_new::GetTuple(const std::string &line, const std::string &type)
{
    float a, b, c;
    StringVector line_data = LineToVector(line);

    bug("Got " + type);

    if (type == "FACE")
    {
        for(size_t i = 0; i < line_data.size(); i++)
        {
            line_data[i] = Split(line_data[i], '/')[0];
        }
    }

    a = (float)atof(line_data[0].c_str());
    b = (float)atof(line_data[1].c_str());
    c = (type == "NORMAL") ? 0 : (float)atof(line_data[2].c_str());

    return std::make_tuple(a, b, c);
}

//====================================================
void Object3dModel_new::AppendMap(const std::string &line)
{
    bug("appended map");
    StringVector face_data = LineToVector(line);
    for(size_t i = 0; i < face_data.size(); i++)
    {
        StringVector vtn = Split(face_data[i], '/');
        int vertex_index = atol(vtn[0].c_str());
        int uv_index = atol(vtn[1].c_str());
        vt_map[vertex_index - 1] = uv_index - 1;
    }
}

//====================================================
void Object3dModel_new::SetVTData()
{
    for(size_t i = 0; i < (*model_data).vertices.size(); i++)
    {   
        Tuple3f vertex = (*model_data).vertices[i];
        Tuple3f uv = (*model_data).texels[vt_map.find(i)->second];
        (*vt_audio_data).push_back(std::make_tuple(
            std::get<0>(vertex),
            std::get<1>(vertex),
            std::get<2>(vertex),
            std::get<0>(uv),
            std::get<1>(uv)
        ));
    }
}

//====================================================
ObjArray* Object3dModel_new::GetVTData()
{
    return vt_audio_data;
}
