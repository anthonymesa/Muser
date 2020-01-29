/** @file Muse.h
 *  @author Anthony Mesa
 *  @copyright Copyright (C) 2019  Anthony Mesa
 */

#pragma once

#include <cmath>
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <Object3dModel.hpp>
#include <AudioFile/AudioFile.h>
#include <bi_tools.hpp>

class Muse
{
public:

    Muse();
    ~Muse();

    void ImportFromObj(const char* model);
    void ExportToObj();
    void RenderToSpectrogram(std::string &filename);
    int RenderToAudio(const char *filename);

    void SetSampleRate(const int &value);
    void SetNumberChannels(const int &value);
    void SetLengthInSamples(const int &value);
    void SetSpectrogramSize(const int &value);

private:
    
    Object3dModel muse_object;
    int spectrogram_size = 1000;
    unsigned char** spectrogram;
    int arrayX, arrayY;
    const int MAX_HERTZ = 15000;
    const int MIN_HERTZ = 30;
    int numChannels = 0;
    int numSamplesPerChannel = 0;
    double sampleRate = 0;

    void CreateSpectrogramArray();
    void PopulateSpectrogramArray();
    void GeneratePpmFile(const char* filename);

    double GetMinimumTopologyValue();
    double GetMaximumTopologyValue();
    double GetTopologyValue(int i);

    double GetHertzRange();
    float GetElementInLine(const size_t &element, const int &row);
    double DistanceToOrigin(const float &x, const float &y, const float &z);
    char ChangeDistanceToValue(const int &i, const double &min, const double &max, const int &goal);
    double Frequency(const int &row);
    double Amplitude(const int &row, const int &sample_index);
    double CalculateSampleAtRow(const int &row, const int &sample_index);
    bool ElementHasValue(const int &row, const int &sample_index);
    float* GetVertexs();
    float* GetIndexs();
};

//====================================================
Muse::Muse()
{
    
}

//====================================================
Muse::~Muse()
{
    for (int i = 0; i < spectrogram_size; i++)
    {
        delete[] spectrogram[i];
    }
    delete[] spectrogram;
}

//====================================================
void Muse::ImportFromObj(const char* model)
{
    muse_object.Load(model);
}

//====================================================
void Muse::ExportToObj()
{
}

//====================================================
void Muse::RenderToSpectrogram(std::string &filename)
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

//====================================================
int Muse::RenderToAudio(const char *filename)
{
    AudioFile<double> audioFile;
    AudioFile<double>::AudioBuffer buffer;
    numChannels = 1;
    numSamplesPerChannel = 48000;
    sampleRate = 48000;
    buffer.resize(1);
    buffer[0].resize(48000);

    for (int s = 0; s < numSamplesPerChannel; s++)
    {
        double sample = 0;
        for (int r = 0; r < spectrogram_size; r++)
        {
            if (ElementHasValue(r, s))
            {
                sample += CalculateSampleAtRow(r, s);
            }
        }
        buffer[0][s] = sample;
    }
    audioFile.setAudioBuffer(buffer);
    if(!audioFile.save(filename, AudioFileFormat::Wave))
    {
        return -1;
    }

    return 0;
}

//====================================================
void Muse::CreateSpectrogramArray()
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

//====================================================
// float Muse::GetElementInLine(const int &element, const int &row)
// {
//     return std::get<element>((*muse_object.GetObjArray())[row]);
// }

//====================================================
double Muse::GetMinimumTopologyValue()
{
    float x, y, z;
    double value, min_value = 9999999999;

    for (size_t i = 0; i < (*muse_object.GetObjArray()).size(); i++)
    {
        x = std::get<0>((*muse_object.GetObjArray())[i]);
        y = std::get<1>((*muse_object.GetObjArray())[i]);
        z = std::get<2>((*muse_object.GetObjArray())[i]);
        value = DistanceToOrigin(x, y, z) + .00001;
        min_value = (value < min_value) ? value : min_value;
    }
    return min_value;
}

//====================================================
double Muse::GetMaximumTopologyValue()
{
    float x, y, z;
    double value, max_value = 0;

    for (size_t i = 0; i < (*muse_object.GetObjArray()).size(); i++)
    {
        x = std::get<0>((*muse_object.GetObjArray())[i]);
        y = std::get<1>((*muse_object.GetObjArray())[i]);
        z = std::get<2>((*muse_object.GetObjArray())[i]);
        value = DistanceToOrigin(x, y, z) + .00001;
        max_value = (value > max_value) ? value : max_value;
    }
    return max_value;
}

//====================================================
double Muse::GetTopologyValue(int i)
{
    float x, y, z;
    x = std::get<0>((*muse_object.GetObjArray())[i]);
    y = std::get<1>((*muse_object.GetObjArray())[i]);
    z = std::get<2>((*muse_object.GetObjArray())[i]);
    return DistanceToOrigin(x, y, z) + .00001;
}

//====================================================
void Muse::PopulateSpectrogramArray()
{
    double min = GetMinimumTopologyValue();
    double max = GetMaximumTopologyValue();

    for (int i = 0; i < (*muse_object.GetObjArray()).size(); i++)
    {
        arrayX = int (std::get<3>((*muse_object.GetObjArray())[i]) * spectrogram_size);
        arrayY = int (std::get<4>((*muse_object.GetObjArray())[i]) * spectrogram_size);

        spectrogram[arrayY][arrayX] = ChangeDistanceToValue(i, min, max, 255);
    }
}

//====================================================
void Muse::GeneratePpmFile(const char* filename)
{
    std::ofstream image_file(filename);
    image_file << "P2\n";
    image_file << spectrogram_size << " " << spectrogram_size << "\n";
    image_file << "255\n";

    for (int i = 0; i < spectrogram_size; i++)
    {   
        for (int j = 0; j < spectrogram_size; j++)
        {
            image_file << int (spectrogram[i][j]) << " ";
        }
        image_file << std::endl;
    }
    image_file.close();
}

//====================================================
double Muse::GetHertzRange()
{
    return (MAX_HERTZ - MIN_HERTZ);
}

//====================================================
void Muse::SetSampleRate(const int &value)
{
    sampleRate = value;
}

//====================================================
void Muse::SetNumberChannels(const int &value)
{
    numChannels = value;
}
                                                      
//====================================================
void Muse::SetLengthInSamples(const int &value)
{
    numSamplesPerChannel = value;
}

//====================================================
void Muse::SetSpectrogramSize(const int &value)
{
    spectrogram_size = value;
}

//====================================================
double Muse::DistanceToOrigin(const float &x, const float &y, const float &z)
{
    return double (sqrt(pow((x - 0), 2) + pow((y - 0), 2) + pow((z - 0), 2)));
}

// Using 255 because ppm image export. If not generating ppm, any goal is fine.
//====================================================
char Muse::ChangeDistanceToValue(const int &i, const double &min, const double &max, const int &goal)
{
    return char (((GetTopologyValue(i) - min) / (max - min)) * goal);
}

// Frequency is calculated from row that current element belongs to.
//====================================================
double Muse::Frequency(const int &row)
{
    return ((GetHertzRange() * row) / spectrogram_size) + MIN_HERTZ;
}

// Amplitude is calculated from value in current element.
//====================================================
double Muse::Amplitude(const int &row, const int &sample_index)
{
    return ((100 * spectrogram[row][sample_index/(numSamplesPerChannel/spectrogram_size)]) / 255);
}

//====================================================
double Muse::CalculateSampleAtRow(const int &row, const int &sample_index)
{
    return (sin((2 * M_PI) * ((double (sample_index) / double (numSamplesPerChannel)) * Frequency(row))) * Amplitude(row, sample_index));
}

// Only want to calculate samples if element is greater than "\0".
//====================================================
bool Muse::ElementHasValue(const int &row, const int &sample_index)
{
    return ((spectrogram[row][sample_index/(numSamplesPerChannel/spectrogram_size)] == '\0') ? false : true );
}

float* Muse::GetVerticies()
{
    return muse_object.GetOpenGLVerticies();
}

float* Muse::GetIndicies()
{
    return muse_object.GetOpenGLIndicies();
}