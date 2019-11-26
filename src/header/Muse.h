#pragma once

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cmath>
#include <Object3dModel.h>

class Muse
{
    private:
        Object3dModel muse_object;
        int spectrogram_size = 1000;
        unsigned char** spectrogram;
        int arrayX, arrayY;

        void CreateSpectrogramArray()
        {
            spectrogram = new unsigned char*[spectrogram_size];
            for (int i = 0; i < spectrogram_size; i++)
            {
                spectrogram[i] = new unsigned char[spectrogram_size];
            }

            for (int i = 0; i < spectrogram_size; i++)
            {   
                for (int j = 0; j < spectrogram_size; j++)
                {
                    spectrogram[i][j] = '\0';
                }
            }
        }

        double GetMinimumTopologyValue()
        {
            float x, y, z;
            double value, min_value = 9999999999;

            for (size_t i = 0; i < (*muse_object.obj_vertex_uv_coordinates).size(); i++)
            {
                x = std::get<0>((*muse_object.obj_vertex_uv_coordinates)[i]);
                y = std::get<1>((*muse_object.obj_vertex_uv_coordinates)[i]);
                z = std::get<2>((*muse_object.obj_vertex_uv_coordinates)[i]);
                value = double (sqrt(pow((x - 0), 2) + pow((y - 0), 2) + pow((z - 0), 2))) + .00001;
                if (value < min_value){
                    min_value = value;
                }
            }
            return min_value;
        }

        double GetMaximumTopologyValue()
        {
            float x, y, z;
            double value, max_value = 0;

            for (size_t i = 0; i < (*muse_object.obj_vertex_uv_coordinates).size(); i++)
            {
                x = std::get<0>((*muse_object.obj_vertex_uv_coordinates)[i]);
                y = std::get<1>((*muse_object.obj_vertex_uv_coordinates)[i]);
                z = std::get<2>((*muse_object.obj_vertex_uv_coordinates)[i]);
                value = double (sqrt(pow((x - 0), 2) + pow((y - 0), 2) + pow((z - 0), 2))) + .00001;
                if (value > max_value){
                    max_value = value;
                }
            }
            return max_value;
        }

        double GetTopologyValue(int i)
        {
            float x, y, z;
            x = std::get<0>((*muse_object.obj_vertex_uv_coordinates)[i]);
            y = std::get<1>((*muse_object.obj_vertex_uv_coordinates)[i]);
            z = std::get<2>((*muse_object.obj_vertex_uv_coordinates)[i]);
            return double (sqrt(pow((x - 0), 2) + pow((y - 0), 2) + pow((z - 0), 2))) + .00001;
        }

        void PopulateSpectrogramArray()
        {
            double minimum = GetMinimumTopologyValue();
            double maximum = GetMaximumTopologyValue();

            for (int i = 0; i < (*muse_object.obj_vertex_uv_coordinates).size(); i++)
            {
                arrayX = int (std::get<3>((*muse_object.obj_vertex_uv_coordinates)[i]) * spectrogram_size);
                arrayY = int (std::get<4>((*muse_object.obj_vertex_uv_coordinates)[i]) * spectrogram_size);

                spectrogram[arrayY][arrayX] = char (((GetTopologyValue(i) - minimum) / (maximum - minimum)) * 255);
                //spectrogram[arrayY][arrayX] = char (GetTopologyValue(i) * 255);
            }
        }

        char* SetSpectrogramProgress()
        {
            char progress[] = "__________";
            return progress;
        }

        void UpdateSpectorgramProgress(char* progress, int row, int element)
        {
            for (int k = 0; k < (int)(((double)(element + 1)/9999) * 10); k++){
                progress[k] = '#';
            }
            std::cout << "\rWriting file - " << row << ": [" << progress << "]" << std::flush;
        }

        void GeneratePpmFile(const char* filename)
        {
            std::ofstream image_file(filename);
            image_file << "P2\n";
            image_file << spectrogram_size << " " << spectrogram_size << "\n";
            image_file << "255\n";

            //char* progress = SetSpectrogramProgress();

            for (int i = 0; i < spectrogram_size; i++)
            {   
                for (int j = 0; j < spectrogram_size; j++)
                {
                    image_file << int (spectrogram[i][j]) << " ";
                    //UpdateSpectorgramProgress(progress, i, j);
                }
                image_file << std::endl;
            }
            image_file.close();
        }

    public:

        Muse()
        {
        }

        void ImportFromObj(const char* model)
        {
            muse_object.Load(model);
        }

        void ExportToObj()
        {
        }

        void RenderToSpectrogram(std::string &filename)
        {
            filename = ("temp/" + filename + ".ppm"); 
            int file_length = filename.length() + 1;
            char* filename_char;
            filename_char = new char[file_length]; 
            strcpy(filename_char, filename.c_str()); 
            CreateSpectrogramArray();
            PopulateSpectrogramArray();
            GeneratePpmFile(filename_char);
            delete[] filename_char;
        }

        ~Muse()
        {
            for (int i = 0; i < spectrogram_size; i++)
            {
                delete[] spectrogram[i];
            }
            delete[] spectrogram;
        }
};