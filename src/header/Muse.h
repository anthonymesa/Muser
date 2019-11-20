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
        int spectrogram_size = 9999;
        unsigned char** spectrogram;
        int arrayX, arrayY;

        void CreateSpectrogramArray()
        {
            spectrogram = new unsigned char*[spectrogram_size];
            for (int i = 0; i < spectrogram_size; i++)
            {
                spectrogram[i] = new unsigned char[spectrogram_size];
            }
        }

        double GetMinimumTopologyValue()
        {
            float x, y, z;
            double value, min_value = 0;

            for (size_t i = 0; i < muse_object.object_array.size(); i++)
            {
                x = std::get<0>(muse_object.object_array[i]);
                y = std::get<1>(muse_object.object_array[i]);
                z = std::get<2>(muse_object.object_array[i]);
                value = double (sqrt(pow((x - 0), 2) + pow((y - 0), 2) + pow((z - 0), 2)));
                if (value < min_value){
                    min_value = value;
                }
            }
            return min_value;
        }

        double GetMaximumTopologyValue()
        {
            float x, y, z;
            double value, max_value = 9999999999;

            for (size_t i = 0; i < muse_object.object_array.size(); i++)
            {
                x = std::get<0>(muse_object.object_array[i]);
                y = std::get<1>(muse_object.object_array[i]);
                z = std::get<2>(muse_object.object_array[i]);
                value = double (sqrt(pow((x - 0), 2) + pow((y - 0), 2) + pow((z - 0), 2)));
                if (value > max_value){
                    max_value = value;
                }
            }
            return max_value;
        }

        double GetTopologyValue(int i)
        {
            float x, y, z;
            x = std::get<0>(muse_object.object_array[i]);
            y = std::get<1>(muse_object.object_array[i]);
            z = std::get<2>(muse_object.object_array[i]);
            return double (sqrt(pow((x - 0), 2) + pow((y - 0), 2) + pow((z - 0), 2)));
        }

        void PopulateSpectrogramArray()
        {
            double minimum = GetMinimumTopologyValue();
            double maximum = GetMaximumTopologyValue();

            for (int i = 0; i < muse_object.object_array.size(); i++)
            {
                arrayX = int ((std::get<3>(muse_object.object_array[i])) * 100000)/1000;
                arrayY = int ((std::get<4>(muse_object.object_array[i])) * 100000)/1000;

                *((*(spectrogram + arrayY)) + arrayX) = char (((GetTopologyValue(i) - minimum) * 255) / (maximum - minimum));
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
            image_file << "P3\n";
            image_file << "9999 9999\n";
            image_file << "255\n";

            char* progress = SetSpectrogramProgress();

            for (int row = 0; row < spectrogram_size; row++)
            {   
                for (int element = 0; element < spectrogram_size; element++)
                {
                    image_file << (int)*((*(spectrogram + row)) + element) << " ";
                    UpdateSpectorgramProgress(progress, row, element);
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
            filename += ".ppm"; 
            int n = filename.length(); 
            char filename_char[n + 1]; 
            strcpy(filename_char, filename.c_str()); 
            CreateSpectrogramArray();
            PopulateSpectrogramArray();
            GeneratePpmFile(filename_char);
        }

        ~Muse()
        {
            //delete muse_object;
            for (int i = 0; i < spectrogram_size; i++)
            {
                delete[] spectrogram[i];
            }
            delete[] spectrogram;
        }
};