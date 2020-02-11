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

    std::unordered_map<int, int> vt_map;
    ObjArray* vt_audio_data;

    StringVector Split(const std::string &input, const char &splitter);
    StringVector LoadFile(const char* &filename);
    void ParseObject(const StringVector &model_string);
    StringVector LineToVector(const std::string &line);
    Tuple3f GetTuple(const std::string &line, const std::string &type);
    void AppendMap(const std::string &line);
    void SetVTData(const object &model_data);
};

//====================================================
Object3dModel_new::Object3dModel_new()
{
    vt_audio_data = new ObjArray;
    ParseObject(LoadFile(muse_base));
}

//====================================================
Object3dModel_new::~Object3dModel_new()
{
    delete vt_audio_data;
}

//====================================================
void Object3dModel_new::Load(const char* &filename)
{
    ParseObject(LoadFile(filename));
}

//====================================================
// Load .obj file and write contents to string vector model_file_string
StringVector Object3dModel_new::LoadFile(const char* &filename)
{
    StringVector model_file_string;
    std::string line;
    std::ifstream file(filename);
    if(!file.good())
    {
       std::cout << "ERROR OPENING OBJ FILE" << std::endl;
    }

    while(getline(file, line))
    {
        model_file_string.push_back(line);
    }

    file.close();
    return model_file_string;
}

//====================================================
// Evaluate each line in the model_file_string vector, then set all of the data to OpenGl compliant data
void Object3dModel_new::ParseObject(const StringVector &model_string)
{
    int vcount = 0, tcount = 0, ncount = 0, fcount = 0;
    object model_data;
    for(size_t i = 0; i < model_string.size(); i++)
    {
        std::string line = model_string[i];
        if (line.find("v ") != std::string::npos)
        {

            model_data.vertices.push_back(GetTuple(line, "VERTEX"));
            vcount = vcount +3;
        }
        else if (line.find("vt ") != std::string::npos)
        {
            model_data.texels.push_back(GetTuple(line, "TEXEL"));
            tcount = tcount +3;
        }
        else if (line.find("vn ") != std::string::npos)
        {
            model_data.normals.push_back(GetTuple(line, "NORMAL"));
            ncount = ncount + 2;
        }
        else if (line.find("f ") != std::string::npos)
        {
            model_data.faces.push_back(GetTuple(line, "FACE"));
            AppendMap(line);
            fcount = fcount + 3;
        }
    }
    std::cout << "vertex floats = " << vcount << std::endl;
    std::cout << "texel floats = " << tcount << std::endl;
    std::cout << "normal floats = " << ncount << std::endl;
    std::cout << "face ints = " << fcount << std::endl;
    SetVTData(model_data);
}

//====================================================
Tuple3f Object3dModel_new::GetTuple(const std::string &line, const std::string &type)
{
    float a, b, c;
    StringVector line_data = LineToVector(line);

    if (type == "FACE")
    {
        for(size_t i = 0; i < line_data.size(); i++)
        {
            line_data[i] = Split(line_data[i], '/')[0];
        }
    }

    a = (float)atof(line_data[0].c_str());
    b = (float)atof(line_data[1].c_str());
    // If type "Normal", only print u and v values.
    c = (type == "NORMAL") ? 0 : (float)atof(line_data[2].c_str());

    return std::make_tuple(a, b, c);
}

//====================================================
StringVector Object3dModel_new::LineToVector(const std::string &line)
{
    StringVector split = Split(line, ' ');
    StringVector shift(split.begin() + 1, split.end());
    return shift;
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
void Object3dModel_new::AppendMap(const std::string &line)
{
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
void Object3dModel_new::SetVTData(const object &model_data)
{
    for(size_t i = 0; i < model_data.vertices.size(); i++)
    {   
        Tuple3f vertex = model_data.vertices[i];
        Tuple3f uv = model_data.texels[vt_map.find(i)->second];
        (*vt_audio_data).push_back(std::make_tuple(
            std::get<0>(vertex),
            std::get<1>(vertex),
            std::get<2>(vertex),
            std::get<0>(uv),
            std::get<1>(uv)
        ));
    }
    std::cout << "audio data floats = " << (*vt_audio_data).size() * 5 << std::endl;
}

//====================================================
ObjArray* Object3dModel_new::GetVTData()
{
    return vt_audio_data;
}

// vertex floats = 294918
// texel floats = 296067
// normal floats = 196612
// face ints = 589824
// audio data floats = 491530