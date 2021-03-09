#pragma once

/*

Author: Anthony Mesa
Date: 10/3/2020

This class provides access to the creation and manipulation of the 3D data
loaded from blender waveform '.obj' files.

*/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>

/*
 * Things to add:
 *
 *		1. I could make a vector of the buffers to allow for more than one
 */

class AudioHandler
{
public:
	static void InitializeBuffer(const int size_x, const int size_y);
	static float* GetBuffer();
	static void SetBuffer(float* new_buffer);
	static int RenderToWav();
	static void SetBufferValueAt(std::tuple<int, int> coord, int value);
	static void CleanUp();
	static void ToImage(std::string&& filename);

	//these arent const, and maybe i should do this differently so that they cant be changed outside the 
	//class. I could make this a default 1000 or 44100 but
	//i think at first I should make it a variable buffer size, I may need to fix this later
	static int BUFFER_WIDTH;
	static int BUFFER_HEIGHT;

private:

	//ATTENTION: should i use std::array?
	static float* audio_buffer;

	static void GeneratePpmFile(const char* filename);
};

float* AudioHandler::audio_buffer = NULL;
int BUFFER_WIDTH = 0;
int BUFFER_HEIGHT = 0;

//ATTENTION: the buffer needs to be initialized before it can be set using zmesh so that buffer size is created first
void AudioHandler::InitializeBuffer(const int size_x, const int size_y)
{
	AudioHandler::BUFFER_HEIGHT = size_y;
	AudioHandler::BUFFER_WIDTH = size_x;

	int temp_size = size_x * size_y;
	//vs is throwing a possible overflow error for the dynamic array size, may need to address that in the future?
	AudioHandler::audio_buffer = new float[size_x * size_y];
	for (int i = 0; i < (size_x * size_y); i++)
	{
		AudioHandler::audio_buffer[i] = 0;
	}
}

float* AudioHandler::GetBuffer()
{
	return AudioHandler::audio_buffer;
}

void AudioHandler::SetBuffer(float* new_buffer)
{
	delete[] AudioHandler::audio_buffer;
	AudioHandler::audio_buffer = new_buffer;
}

void AudioHandler::SetBufferValueAt(std::tuple<int, int> coord, int value)
{
	int x = std::get<0>(coord);
	int y = std::get<1>(coord);
	AudioHandler::audio_buffer[(y * AudioHandler::BUFFER_HEIGHT) + x] = value;
}

int AudioHandler::RenderToWav()
{
	return 0;
}

void AudioHandler::CleanUp()
{
	delete[] AudioHandler::audio_buffer;
}

void AudioHandler::ToImage(std::string&& filename)
{
	std::string filename_string = ("temp/" + filename + ".ppm");
	//int file_length = filename_string.length() + 1;
	char* filename_char = const_cast<char*>(filename_string.c_str());
	//filename_char = new char[file_length];
	//have to copy string to char* because ofstream neads it in GeneratePpmFile
	//std::strcpy(filename_char, filename.c_str());
	GeneratePpmFile(filename_char);
}

void AudioHandler::GeneratePpmFile(const char* filename)
{
	std::ofstream image_file(filename);
	image_file << "P2\n";
	image_file << AudioHandler::BUFFER_WIDTH << " " << AudioHandler::BUFFER_HEIGHT << "\n";
	image_file << "255\n";

	for (int i = 0; i < AudioHandler::BUFFER_HEIGHT; i++)
	{
		for (int j = 0; j < AudioHandler::BUFFER_WIDTH; j++)
		{
			image_file << int(AudioHandler::audio_buffer[(i * AudioHandler::BUFFER_HEIGHT) + j]) << " ";
		}
		image_file << std::endl;
	}
	image_file.close();
}