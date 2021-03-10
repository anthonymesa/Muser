#pragma once

/*
*
*	Author: Anthony Mesa
*	Date: 10/3/2020
*
*	This class provides access to the creation and manipulation of muse objects.
*
*	Muse objects (structs of Muse data) are stored in a vector collection for easy access;
*
*	The Muse object serves two main purposes, a container for the actual 3d data representing the muse,
*	as well as a container for the "spectrogram" array data that is used to convert the geometry to
*	an audio file.
*
*/

#include <string>
#include <vector>
#include <set>

#include "ObjectHandler.hpp"
#include "AudioHandler.hpp"
#include "ZMesh.hpp"

class MuseHandler
{
public:

	static unsigned int NewMuse(const char* name);
	static float* GetVerticesOpenGl(unsigned int muse_index);
	static std::string GetName(unsigned int muse_index);
	static void CleanUp();
	static void RenderToAudio(unsigned int muse_index);
	static void RenderToImage(unsigned int muse_index, int x, int y);
	static void Remove(std::string muse_name);

private:

	struct MuseData
	{
		std::string name;

		// This muse's corresponding object data
		unsigned int object_data;
	};

	template<typename T, typename tag> struct newtype
	{
		explicit newtype(const T& v) : value(v) {}
		T value;
	};

	// struct muse_index {};

	// typedef newtype<int, muse_index> MUSINT;

	static MuseData* muse;
	static std::vector<MuseData>* muses;
};

/*
*	These created objects are freed in CleanUp().
*/
MuseHandler::MuseData* MuseHandler::muse = new MuseHandler::MuseData();
// I should make this an array of pointers instead
std::vector<MuseHandler::MuseData>* MuseHandler::muses = new std::vector<MuseHandler::MuseData>();

/*
*	Uses the muse struct as a template. After filling out the MuseData muse struct (which is overwritten
*	at each new muse initialization) the struct is appended to the vector of currently
*	in-use muses.
*
*	@returns Int value of vector index of newly appended muse.
*	@param name Unique string name for new muaase being created.
*/
unsigned int MuseHandler::NewMuse(const char* name)
{
	MuseHandler::muse->name = name;
	MuseHandler::muse->object_data = ObjectHandler::NewObject();
	MuseHandler::muses->push_back(*MuseHandler::muse);

	unsigned int muse_id = muses->size() - 1;
	return muse_id;
}

/* 
*	Gets the unique string name of desired muse in muses vector.
*
*	@param muse_index Vector index to retrieve desired muse object
*/
std::string MuseHandler::GetName(unsigned int muse_index)
{
	return MuseHandler::muses->at(muse_index).name;
}

/*
*	Provides method to return OpenGl vertices from muse.
* 
*	@returns Pointer to desired muse's object_data's vertices vector to use as OpengGl vertices array.
*/
float* MuseHandler::GetVerticesOpenGl(unsigned int muse_index)
{
	//get the pointer to the data vector
	std::vector<float>* temp = ObjectHandler::GetVerticesVector(MuseHandler::muses->at(muse_index).object_data);
	//return the pointer to the array form of the vector and return it
	return &(*temp)[0];
}

void MuseHandler::RenderToAudio(unsigned int muse_index)
{
	//process buffer data using zmesh algorithm
	// then set audio buffer to new data
	ZMesh::InterpolateMuseSurfaceData(MuseHandler::muses->at(muse_index).object_data);
	int success = AudioHandler::RenderToWav();
}

void MuseHandler::RenderToImage(unsigned int muse_index, int x, int y)
{
	//ATTENTION: Should I initialize this buffer somewhere else?
	AudioHandler::InitializeBuffer(x, y);
	ZMesh::InterpolateMuseSurfaceData(MuseHandler::muses->at(muse_index).object_data);
	AudioHandler::ToImage(MuseHandler::muses->at(muse_index).name.c_str());
}

/*
*	This should be called when we are completely done with the MuseHandler.
*/
void MuseHandler::CleanUp()
{
	ObjectHandler::CleanUp();
	AudioHandler::CleanUp();
	delete MuseHandler::muse;
	delete MuseHandler::muses;
}