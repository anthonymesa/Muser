#pragma once

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

class Object3dModel
{
    private:
        struct verticies_uvs_map
        {
            std::unordered_map<int, int> pointer_map;
            std::vector<std::tuple<float, float, float>> vertex_coordinates_list;
            std::vector<std::tuple<float, float>> uv_coordinates_list;
        };

        //  Splits string into multiple strings
        //
        std::vector<std::string> Split(std::string input, char splitter)
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

        //  Parses obj file and returns struct containing the pointer map, list of all verticies and list of all uvs
        //
        verticies_uvs_map ParseObj(const char* filename)
        {
            verticies_uvs_map obj_info;
            std::ifstream object_file(filename);
            std::string object_read_line;

            while (getline(object_file, object_read_line))
            {
                if (object_read_line.find("v ") != std::string::npos)
                {
                    obj_info.vertex_coordinates_list.push_back(VertexFromLine(object_read_line));
                }
                else if (object_read_line.find("vt ") != std::string::npos)
                {
                    obj_info.uv_coordinates_list.push_back(UvFromLine(object_read_line));
                }
                else if (object_read_line.find("f ") != std::string::npos)
                {
                    GeneratePointerMap(object_read_line, obj_info.pointer_map);
                }
            }

            object_file.close();
            return obj_info;
        }

        //  Makes an XYZ tuple with data from 'v' line of OBJ
        //
        std::tuple<float, float, float> VertexFromLine(const std::string &file_line)
        {
            float x, y, z;
            std::vector<std::string> v_split = Split(file_line, ' ');
            std::vector<std::string> v_shift(v_split.begin() + 1, v_split.end());
            x = (float)atof(v_shift[0].c_str());
            y = (float)atof(v_shift[1].c_str());
            z = (float)atof(v_shift[2].c_str());
            return std::make_tuple(x, y, z);
        }

        //  Makes an UV tuple with data from 'vt' line of OBJ
        //
        std::tuple<float, float> UvFromLine(const std::string &file_line)
        {
            float u, v;
            std::vector<std::string> vt_split = Split(file_line, ' ');
            std::vector<std::string> vt_shift(vt_split.begin() + 1, vt_split.end());
            u = (float)atof(vt_shift[0].c_str());
            v = (float)atof(vt_shift[1].c_str());
            return std::make_tuple(u, v);
        }

        //  Populates the entire vertex -> uv pointer map
        //
        void GeneratePointerMap(const std::string &file_line, std::unordered_map<int, int> &pointer_map)
        {
            std::vector<std::string> pointer_list = Split(file_line, ' ');
            std::vector<std::string> pointer_list_shift(pointer_list.begin() + 1, pointer_list.end());
            for (std::string& each : pointer_list_shift)
            {
                MapVertexToUv(each, pointer_map);
            }
        }

        //  Maps a uv value to a vertex value within the pointer map
        //
        void MapVertexToUv(const std::string &element_in_line, std::unordered_map<int, int> &pointer_map)
        {
            std::vector<std::string> pointer = Split(element_in_line, '/');
            std::vector<std::string> pointer_shift(pointer.begin(), pointer.begin() + 2);
            int vertex_index = atol(pointer_shift[0].c_str());
            int uv_index = atol(pointer_shift[1].c_str());
            pointer_map[vertex_index - 1] = uv_index - 1;
        }

        void SetDefaultMuseArray(verticies_uvs_map &obj_info)
        {
            for(size_t i = 0; i < obj_info.vertex_coordinates_list.size(); i++)
            {
                std::tuple<float, float, float> vertex = obj_info.vertex_coordinates_list[i];
                std::tuple<float, float> uv = obj_info.uv_coordinates_list[obj_info.pointer_map[i]];
                std::get<0>(object_array[i]) = std::get<0>(vertex);
                std::get<1>(object_array[i]) = std::get<1>(vertex);
                std::get<2>(object_array[i]) = std::get<2>(vertex);
                std::get<3>(object_array[i]) = std::get<0>(uv);
                std::get<4>(object_array[i]) = std::get<1>(uv);
            }
        }

    public:
        std::vector<std::tuple<float, float, float, float, float>> object_array;
        
        Object3dModel()
        {
            verticies_uvs_map obj_info = ParseObj("data/muse_default.obj");
            SetDefaultMuseArray(obj_info);
        }

        void Load(const char* filename)
        {
            verticies_uvs_map obj_info = ParseObj(filename);
            SetDefaultMuseArray(obj_info);
        }

        // ~Object_3dModel(){

        // }
};