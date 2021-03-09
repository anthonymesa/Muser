#pragma once

#include <iostream>
#include <map>
#include <string>
#include <set>
#include <vector>
#include "ObjectHandler.hpp"
#include "AudioHandler.hpp"
#include "Mantis.hpp"

#define DEBUG

class ZMesh
{
public:

	static void InterpolateMuseSurfaceData(const unsigned int &object_index);

private:

    static unsigned int current_object_index;
	struct ZMeshPoint
	{
		float vertex[3];
		float texel[2];
		float normal[3];
		float raw_magnitude;
		float real_magnitude;
		float color_magnitude;
		std::vector<std::vector<unsigned int>> shared_faces;
		std::set<std::tuple<int, int>> shared_nodes;
	};

	//using a pointer so that the map is just vectors and pointers instead of vectors and structs
	//also we are doing this because this map could be 90k points large and we dont want this on stack
	static ZMeshPoint* z_mesh_point_temp;
    //while the zmeshpoint is a single object for template purposes, the map takes the actual zmeshpoint data.
    //this is because if we only pushed the pointer we would have the same pointer over and over or need to make
    // new pointers which would be redundant.
	static std::map<std::tuple<int, int>, ZMeshPoint>* z_mesh;

    static Mantis* zmesh_log;
    static Mantis* algo_log;

    static void GenerateMesh();
    static void PopulateZMeshPoint(const int &texel_index);
    static int GetVertexIndex(const int &texel_index, int &faces_list_size, std::vector<unsigned int>* &faces);
    static void SetTexel(const int &texel_index);
    static void SetVertex(const int &vertex_index);
    static float NormalizeVector(std::tuple<float, float, float> vertex);
    static void SetRawMagnitude();
    static void SetRealMagnitude();
    static void SetColorMagnitude();
    static std::vector<std::vector<unsigned int>> GetSharedFaces(const int& texel_index, const int& faces_list_size, std::vector<unsigned int>*& faces);
    static void PopulateSharedFaces(std::vector<std::vector<unsigned int>>& shared_faces);

    //ATTENTION: these three functions are redundant and could probably be meshed into a single function with a switch for 
    //the relative values that make these functions different
    static void SetPointA(std::vector<unsigned int>& line_data, std::vector<std::vector<unsigned int>>& shared_faces, const int& vector_index);
    static void SetPointB(std::vector<unsigned int>& line_data, std::vector<std::vector<unsigned int>>& shared_faces, const int& vector_index);
    static void SetPointC(std::vector<unsigned int>& line_data, std::vector<std::vector<unsigned int>>& shared_faces, const int& vector_index);

    //ATTENTION: this needs to change from color mag to audio magnitude when that ball gets rolling
    static int GetColorValue(int v_i);

    static void PopulateSharedNodes(std::vector<std::vector<unsigned int>>& shared_faces);

    static std::vector<unsigned int> GetFace(std::tuple<float, float> buffer_coords, std::vector<std::vector<unsigned int>> shared_faces);
    static void CalculatePixelColor(std::vector<unsigned int> face, std::tuple<int, int> buffer_coords);

    static float AreaTriangle(std::tuple<float, float> a, std::tuple<float, float> b, std::tuple<float, float> c);
    static float AreaKey(std::tuple<float, float> a, std::tuple<float, float> b, std::tuple<float, float> c, std::tuple<float, float> d);
};

ZMesh::ZMeshPoint* ZMesh::z_mesh_point_temp = new ZMesh::ZMeshPoint();
std::map<std::tuple<int, int>, ZMesh::ZMeshPoint>* ZMesh::z_mesh = new std::map<std::tuple<int, int>, ZMesh::ZMeshPoint>();

//apparently though these are from an outside class, I have to reimport them here?
int AudioHandler::BUFFER_WIDTH;
int AudioHandler::BUFFER_HEIGHT;
unsigned int ZMesh::current_object_index;

Mantis* ZMesh::zmesh_log = new Mantis("zmesh_log");
Mantis* ZMesh::algo_log = new Mantis("algorithm_log");

//ATTENTION: currently this returns void, but it needs to return a float pointer at some point
//This is because I want to keep this class from directly accessing and editing the audio buffer.
void ZMesh::InterpolateMuseSurfaceData(const unsigned int& object_index)
{
    ZMesh::current_object_index = object_index;
    zmesh_log->Log("Current mesh index: " + std::to_string(ZMesh::current_object_index));
    ZMesh::GenerateMesh();
    zmesh_log->Log("Mesh Generated...");

    //float* mesh_buffer = new float[AudioHandler::BUFFER_HEIGHT * AudioHandler::BUFFER_WIDTH];

    algo_log->Log("Beginning algorithm...");
    algo_log->Log("Length of mesh buffer: ");
    algo_log->Log(AudioHandler::BUFFER_HEIGHT * AudioHandler::BUFFER_WIDTH);

    std::tuple<int, int> anchor_node;
    std::tuple<int, int> traverse_node;

    for (int y = 0; y < AudioHandler::BUFFER_HEIGHT; y++)
    {
        std::tuple<int, int> key = { 0, y };

        algo_log->Log("Current key: ");
        algo_log->Log(key);

        //if first element of row is a point
        if ((*ZMesh::z_mesh).find(key) != (*ZMesh::z_mesh).end()) {
            anchor_node = std::make_tuple(0, y);
            algo_log->Log("Key was found in mesh...");
            algo_log->Log("Anchor node set: ");
            algo_log->Log(anchor_node);
        }

        traverse_node = anchor_node;
        algo_log->Log("initializeing traverse node to anchor node.");
        algo_log->Log("iterating accross width...");

        for (int x = 0; x < AudioHandler::BUFFER_WIDTH; x++)
        {
            std::tuple<int, int> buffer_coords = { x, y };

            //algo_log->Log("Working buffer pixel: ");
            //algo_log->Log(buffer_coords);

            //if element on the horizontal is a point
            if ((*ZMesh::z_mesh).find(buffer_coords) != (*ZMesh::z_mesh).end())
            {

                algo_log->Log("working pixel is a texel point...");
                traverse_node = buffer_coords;
                algo_log->Log("traverse node set to current working pixel...");
                algo_log->Log(traverse_node);

                std::vector<unsigned int> face = GetFace(buffer_coords, (*ZMesh::z_mesh)[traverse_node].shared_faces);
                CalculatePixelColor(face, buffer_coords);
            }
            else             //if neither, use the local faces to calculate the value
            {
                algo_log->Log("working pixel is not a texel point, checking if in local faces...");
                std::vector<unsigned int> face = GetFace(buffer_coords, (*ZMesh::z_mesh)[traverse_node].shared_faces);

                if (face.size() != 0)
                {
                    algo_log->Log("Found in local faces...");
                    CalculatePixelColor(face, buffer_coords);
                }
                else
                {
                    algo_log->Log("Not found in local faces, exceeded face boundary, finding next traverse node...");
                    std::set<std::tuple<int, int>> local_nodes = (*ZMesh::z_mesh).at(traverse_node).shared_nodes;
                    
                    //ATTENTION: trying out C++ 11 feature here
                    //if face can be found in the shared faces of a surrounding node
                    for (auto node : local_nodes)
                    {
                        face = GetFace(buffer_coords, (*ZMesh::z_mesh)[node].shared_faces);

                        if (face.size() != 0)
                        {
                            CalculatePixelColor(face, buffer_coords);
                            algo_log->Log("Found face in new node boundary: ");
                            algo_log->Log(node);
                            traverse_node = node;
                        }
                    }
                }
            }
        }
    }

    delete zmesh_log;
    delete algo_log;

    delete z_mesh_point_temp;
    delete z_mesh;
}

//ATTENTION: this function will need to change name/etc to reflect audio buffer usage
void ZMesh::CalculatePixelColor(std::vector<unsigned int> face, std::tuple<int, int> buffer_coords)
{
    float color_value = 0;

    //using arrays here because tuples are too verbose and each should only have three elents
    int a[] = {
        static_cast<int>(face[0]), 
        static_cast<int>(face[1]),
        static_cast<int>(face[2])
    };
    int b[] = {
        static_cast<int>(face[3]),
        static_cast<int>(face[4]),
        static_cast<int>(face[5])
    };
    int c[] = {
        static_cast<int>(face[6]),
        static_cast<int>(face[7]),
        static_cast<int>(face[8])
    };

    //vector subtractions
    int d[] = {b[0] - a[0], b[1] - a[1], b[2] - a[2]};
    int e[] = {c[0] - a[0], c[1] - a[1], c[2] - a[2]};

    //getting cross product
    float n[] = {
        static_cast<float>((d[1] * e[2]) - (d[2] * e[1])),
        static_cast<float>((d[2] * e[0]) - (d[0] * e[2])),
        static_cast<float>((d[0] * e[1]) - (d[1] * e[0]))
    };

    color_value = -(-((a[0] - std::get<0>(buffer_coords)) * (-n[0] / -n[2])) - ((a[1] - std::get<1>(buffer_coords)) * (n[1] / n[2])) - a[2]);
    if (color_value < 0)
    {
        std::cout << "color value less than 0: " << color_value << std::endl;
        std::cout << "texel position: " << std::get<0>(buffer_coords) << " " << std::get<1>(buffer_coords) << std::endl;
        for (auto i : face)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
    AudioHandler::SetBufferValueAt(buffer_coords, (int)color_value);
}

//ATTENTION: changed the way this returns. shouldnt be an issue though
std::vector<unsigned int> ZMesh::GetFace(std::tuple<float, float> z_mesh_key, std::vector<std::vector<unsigned int>> shared_faces)
{
    std::vector<unsigned int> face;
    for (int i = 0; i < shared_faces.size(); i++)
    {
        std::tuple<float, float> a = { shared_faces[i][0], shared_faces[i][1] };
        std::tuple<float, float> b = { shared_faces[i][3], shared_faces[i][4] };
        std::tuple<float, float> c = { shared_faces[i][6], shared_faces[i][7] };

        float area_normal = AreaTriangle(a, b, c);
        float area_key = AreaKey(a, b, c, z_mesh_key);

        if (area_key <= area_normal)
        {
            face = shared_faces[i];
        }
    }

    return face;
}

//ATTENTION: theres gotta be a better way to do this than with tuples.
float ZMesh::AreaTriangle(std::tuple<float, float> a, std::tuple<float, float> b, std::tuple<float, float> c)
{
    return std::abs((std::get<0>(a) * (std::get<1>(b) - std::get<1>(c)) + std::get<0>(b) * (std::get<1>(c) -
        std::get<1>(a)) + std::get<0>(c) * (std::get<1>(a) - std::get<1>(b))) / 2);
}

float ZMesh::AreaKey(std::tuple<float, float> a, std::tuple<float, float> b, std::tuple<float, float> c, std::tuple<float, float> d)
{
    float area_1 = std::abs((std::get<0>(a) * (std::get<1>(b) - std::get<1>(d)) + std::get<0>(b) * (std::get<1>(d) -
        std::get<1>(a)) + std::get<0>(d) * (std::get<1>(a) - std::get<1>(b))) / 2);

    float area_2 = std::abs((std::get<0>(a) * (std::get<1>(c) - std::get<1>(d)) + std::get<0>(c) * (std::get<1>(d) -
        std::get<1>(a)) + std::get<0>(d) * (std::get<1>(a) - std::get<1>(c))) / 2);

    float area_3 = std::abs((std::get<0>(b) * (std::get<1>(c) - std::get<1>(d)) + std::get<0>(c) * (std::get<1>(d) -
        std::get<1>(b)) + std::get<0>(d) * (std::get<1>(b) - std::get<1>(c))) / 2);

    return area_1 + area_2 + area_3;
}

void ZMesh::GenerateMesh()
{
    #ifdef DEBUG
        zmesh_log->Log("Generating mesh...");
    #endif

    //ATTENTION: this right here is why object handler returns the vectors instead of just array pointers (like I do for opengl),
    //because I "think" i need to know the size of the array. after I have fully implemented
    // this zmesh algorithm i may need to revisit this
    std::vector<float>* texels = ObjectHandler::GetTexelsVector(ZMesh::current_object_index);

    int texels_amt = ((*texels).size() / 2); //number of texels * 2 (x and y values)
    
    #ifdef DEBUG
        zmesh_log->Log("Number of texels: " + std::to_string(texels_amt));
        zmesh_log->Log("Iterating over texels...");
    #endif

    for (int texel_index = 0; texel_index < texels_amt; texel_index++)
    {
        zmesh_log->Log("==============================");
        zmesh_log->Log("Evaluating texel at index: " + std::to_string(texel_index + 1));
        zmesh_log->Log("Raw texel: " + 
            std::to_string((*texels)[(texel_index * 2) + 0]) + " " + 
            std::to_string((*texels)[(texel_index * 2) + 1]));

        int x = (int)(AudioHandler::BUFFER_WIDTH * (*texels)[(texel_index * 2) + 0]);
        int y = (int)(AudioHandler::BUFFER_HEIGHT * (*texels)[(texel_index * 2) + 1]);
        
        std::tuple<int, int> buffer_coords = { x, y };
        zmesh_log->Log("Texel buffer coordinate: ");
        zmesh_log->Log(buffer_coords);

        ZMesh::PopulateZMeshPoint(texel_index);

        //THIS ISSUE IS LAST
        (*ZMesh::z_mesh)[buffer_coords] = (*ZMesh::z_mesh_point_temp);

        zmesh_log->Log("Zmesh map updated with z_mesh_point value at key:");
        zmesh_log->Log(buffer_coords);
    }
}

void ZMesh::PopulateZMeshPoint(const int &texel_index)
{
    zmesh_log->Log("------------------------------");
    zmesh_log->Log("Populating zmesh point...");

    //not shure if i should keep the faces getting here or in their respective methods
    std::vector<unsigned int>* faces = ObjectHandler::GetIndicesVector(ZMesh::current_object_index);

    int faces_list_size = ((*faces).size() / 9);

    zmesh_log->Log("Number of faces: " + std::to_string(faces_list_size));

    int vertex_index = GetVertexIndex(texel_index, faces_list_size, faces);
    
    //set the current values for the vertex used by the zmesh point
    ZMesh::SetTexel(texel_index);
    ZMesh::SetVertex(vertex_index);
    ZMesh::SetRawMagnitude();
    ZMesh::SetRealMagnitude();
    ZMesh::SetColorMagnitude();
    std::vector<std::vector<unsigned int>> shared_faces = ZMesh::GetSharedFaces(texel_index, faces_list_size, faces);

    ZMesh::PopulateSharedFaces(shared_faces);
    ZMesh::PopulateSharedNodes(shared_faces);
}

int ZMesh::GetVertexIndex(const int &texel_index, int &faces_list_size, std::vector<unsigned int>* &faces)
{
    zmesh_log->Log("Matching vertex from texel index " + std::to_string(texel_index + 1));

    int vertex_index = 0;

    for (int faces_index = 0; faces_index < faces_list_size; faces_index++) { // runs from faces_index = 0 to faces_index = 1728
        if ((*faces)[(faces_index * 9) + 1] == (texel_index + 1)) { // if the value at faces_index + 1, 4, 7 (the location in each face info of the texel index) = the current (texel index + 1) [there are no zeros in the data]
            vertex_index = (*faces)[(faces_index * 9) + 0];
        }
        else if ((*faces)[(faces_index * 9) + 4] == (texel_index + 1)) {
            vertex_index = (*faces)[(faces_index * 9) + 3];
        }
        else if ((*faces)[faces_index + 7] == (texel_index + 1)) {
            vertex_index = (*faces)[(faces_index * 9) + 6];
        }
    }

    zmesh_log->Log("Vertex index is: " + std::to_string(vertex_index));

    return (vertex_index - 1);
}

void ZMesh::SetTexel(const int &texel_index)
{
    std::vector<float>* texels = ObjectHandler::GetTexelsVector(ZMesh::current_object_index);

    ZMesh::z_mesh_point_temp->texel[0] = (*texels)[(texel_index * 2) + 0];
    ZMesh::z_mesh_point_temp->texel[1] = (*texels)[(texel_index * 2) + 1];

    zmesh_log->Log("Set texel to " + 
        std::to_string(ZMesh::z_mesh_point_temp->texel[0]) + " " +
        std::to_string(ZMesh::z_mesh_point_temp->texel[1])
    );
}

void ZMesh::SetVertex(const int &vertex_index)
{
    std::vector<float>* vertices = ObjectHandler::GetVerticesVector(ZMesh::current_object_index);

    ZMesh::z_mesh_point_temp->vertex[0] = (*vertices)[(vertex_index * 3) + 0];
    ZMesh::z_mesh_point_temp->vertex[1] = (*vertices)[(vertex_index * 3) + 1];
    ZMesh::z_mesh_point_temp->vertex[2] = (*vertices)[(vertex_index * 3) + 2];

    zmesh_log->Log("Set vertex to " +
        std::to_string(ZMesh::z_mesh_point_temp->vertex[0]) + " " +
        std::to_string(ZMesh::z_mesh_point_temp->vertex[1]) + " " +
        std::to_string(ZMesh::z_mesh_point_temp->vertex[2])
    );
}

float ZMesh::NormalizeVector(std::tuple<float, float, float> vertex)
{
    return std::sqrt(
        std::pow(std::get<0>(vertex), 2) +
        std::pow(std::get<1>(vertex), 2) +
        std::pow(std::get<2>(vertex), 2)
    );
}

void ZMesh::SetRawMagnitude()
{
    std::tuple<float, float, float> point_vertex = {
        ZMesh::z_mesh_point_temp->vertex[0],
        ZMesh::z_mesh_point_temp->vertex[1],
        ZMesh::z_mesh_point_temp->vertex[2]
    };

    ZMesh::z_mesh_point_temp->raw_magnitude = ZMesh::NormalizeVector(point_vertex);

    zmesh_log->Log("Set raw magnitude to " +
        std::to_string(ZMesh::z_mesh_point_temp->raw_magnitude));
}

void ZMesh::SetRealMagnitude()
{
    std::vector<float>* min_max = ObjectHandler::GetMinMax(ZMesh::current_object_index);

    float mag_diff = ObjectHandler::GetMagnitudeDiffernce(ZMesh::current_object_index);

    zmesh_log->Log("magnitude difference: " + std::to_string(mag_diff));

    ZMesh::z_mesh_point_temp->real_magnitude = (ZMesh::z_mesh_point_temp->raw_magnitude - (*min_max)[0]) / mag_diff;
   
    zmesh_log->Log("Set real magnitude to " +
        std::to_string(ZMesh::z_mesh_point_temp->real_magnitude));
}

//ATTENTION: Currently this magnitude is set up specifically for color, but this will need to change to audio magnitude
void ZMesh::SetColorMagnitude()
{
    ZMesh::z_mesh_point_temp->color_magnitude = 255 * ZMesh::z_mesh_point_temp->real_magnitude;

    zmesh_log->Log("Set color magnitude to " +
        std::to_string(ZMesh::z_mesh_point_temp->color_magnitude));
}

std::vector<std::vector<unsigned int>> ZMesh::GetSharedFaces(const int &texel_index, const int &faces_list_size, std::vector<unsigned int>*& faces)
{
    zmesh_log->Log("Getting shared faces...");

    //no pointer here because it is small enough to stay on stack
    std::vector<std::vector<unsigned int>> shared_faces;

    // refrence: faces vector is a vector of unsigned ints, given this we know:
    //
    // number of faces = (faces vector / 9)
    //
    // for each face listed in the faces data vector of (of three data points of three
    // values (9 face data values in total)), check each datapoint, and if that face 
    // there exists a least one data point that has the same texel index as the current
    // anchor cell, then push a vector of the 9 unsigned ints representing that face to the
    // vector to the shared faces vector 
    for (int faces_index = 0; faces_index < faces_list_size; faces_index++) // runs from faces_index = 0 to faces_index = 1728
    {
        // vector of 9 values representing that face
        std::vector<unsigned int> face;

        //zmesh_log->Log("Checking for shared face at face index " + std::to_string(faces_index + 1));

        if ((*faces)[(faces_index * 9) + 1] == ((texel_index) + 1))
        {
            zmesh_log->Log("Found shared face at face index " + std::to_string(faces_index + 1));

            for (int i = 0; i < 9; i++)
            {
                face.push_back((*faces)[(faces_index * 9) + i]);
            }
            shared_faces.push_back(face);
        }
        else if ((*faces)[(faces_index * 9) + 4] == ((texel_index) + 1))
        {
            zmesh_log->Log("Found shared face at face index " + std::to_string(faces_index + 1));

            for (int i = 0; i < 9; i++)
            {
                face.push_back((*faces)[(faces_index * 9) + i]);
            }
            shared_faces.push_back(face);
        }
        else if ((*faces)[(faces_index * 9) + 7] == ((texel_index) + 1))
        {
            zmesh_log->Log("Found shared face at face index " + std::to_string(faces_index + 1));

            for (int i = 0; i < 9; i++)
            {
                face.push_back((*faces)[(faces_index * 9) + i]);
            }
            shared_faces.push_back(face);
        }

        if (face.size() != 0)
        {
            zmesh_log->Log(face);
        }
    }

    zmesh_log->Log("\nListing shared faces...");
    zmesh_log->Log(shared_faces);

    return shared_faces;
}

void ZMesh::PopulateSharedFaces(std::vector<std::vector<unsigned int>> &shared_faces)
{

    zmesh_log->Log("Converting shared faces data format...");
    //here we are changing the shared faces data from [[vtn] [vtn] [vtn] to [[tx ty vm] [tx ty vm] [tx ty vm]]
    std::vector<std::vector<unsigned int>> shared_faces_data;

    for (int vector_index = 0; vector_index < shared_faces.size(); vector_index++) { // for each array of ints (shared_face arrays) in the vector
        std::vector<unsigned int> line_data;

        zmesh_log->Log(line_data);
        //pointers to pass? passing by reference should be fine
        ZMesh::SetPointA(line_data, shared_faces, vector_index);
        zmesh_log->Log(line_data);
        ZMesh::SetPointB(line_data, shared_faces, vector_index);
        zmesh_log->Log(line_data);
        ZMesh::SetPointC(line_data, shared_faces, vector_index);
        zmesh_log->Log(line_data);

        shared_faces_data.push_back(line_data);
    }

    zmesh_log->Log("Shared faces info updated...");
    zmesh_log->Log(shared_faces_data);
    ZMesh::z_mesh_point_temp->shared_faces = shared_faces_data;
}

void ZMesh::SetPointA(std::vector<unsigned int>& line_data, std::vector<std::vector<unsigned int>>& shared_faces, const int& vector_index)
{
    std::vector<float>* texels = ObjectHandler::GetTexelsVector(ZMesh::current_object_index);

    //havent tried accessing a double vector as a 2d array, hopefully this works
    int v_i = shared_faces[vector_index][0];
    int t_i = shared_faces[vector_index][1];
    line_data.push_back((int)(AudioHandler::BUFFER_WIDTH * (*texels)[((t_i - 1) * 2) + 0]));
    line_data.push_back((int)(AudioHandler::BUFFER_HEIGHT * (*texels)[((t_i - 1) * 2) + 1]));
    line_data.push_back(GetColorValue(v_i -1));
}

void ZMesh::SetPointB(std::vector<unsigned int>& line_data, std::vector<std::vector<unsigned int>>& shared_faces, const int& vector_index)
{
    std::vector<float>* texels = ObjectHandler::GetTexelsVector(ZMesh::current_object_index);
    int v_i = shared_faces[vector_index][3];
    int t_i = shared_faces[vector_index][4];
    line_data.push_back((int)(AudioHandler::BUFFER_WIDTH * (*texels)[((t_i - 1) * 2) + 0]));
    line_data.push_back((int)(AudioHandler::BUFFER_HEIGHT * (*texels)[((t_i - 1) * 2) + 1]));
    line_data.push_back(GetColorValue(v_i - 1));
}

void ZMesh::SetPointC(std::vector<unsigned int>& line_data, std::vector<std::vector<unsigned int>>& shared_faces, const int& vector_index)
{
    std::vector<float>* texels = ObjectHandler::GetTexelsVector(ZMesh::current_object_index);
    //havent tried accessing a double vector as a 2d array, hopefully this works
    int v_i = shared_faces[vector_index][6];
    int t_i = shared_faces[vector_index][7];
    line_data.push_back((int)(AudioHandler::BUFFER_WIDTH * (*texels)[((t_i - 1) * 2) + 0]));
    line_data.push_back((int)(AudioHandler::BUFFER_HEIGHT * (*texels)[((t_i - 1) * 2) + 1]));
    line_data.push_back(GetColorValue(v_i - 1));
}

int ZMesh::GetColorValue(int v_i)
{
    std::vector<float>* vertices = ObjectHandler::GetVerticesVector(ZMesh::current_object_index);
    std::vector<float>* min_max = ObjectHandler::GetMinMax(ZMesh::current_object_index);
    float mag_diff = ObjectHandler::GetMagnitudeDiffernce(ZMesh::current_object_index);

    std::tuple<float, float, float> vertex = {
        (*vertices)[(v_i * 3) + 0],
        (*vertices)[(v_i * 3) + 1],
        (*vertices)[(v_i * 3) + 2]
    };

    float temp_raw_mag = ZMesh::NormalizeVector(vertex);
    float temp_real_mag = (temp_raw_mag - (*min_max)[0]) / mag_diff;
    float color_magnitude = 255 * temp_real_mag;

    zmesh_log->Log("Getting color value..." + std::to_string(color_magnitude));

    return (int)color_magnitude;
}

void ZMesh::PopulateSharedNodes(std::vector<std::vector<unsigned int>>& shared_faces)
{
    zmesh_log->Log("Populating shared nodes...");

    std::vector<float>* texels = ObjectHandler::GetTexelsVector(ZMesh::current_object_index);

    //ATTENTION: theres an issue here regarding float and int. i removed the int casts below but might need them for reasons later
    std::set<std::tuple<int, int>> shared_nodes;

    for (int vector_index = 0; vector_index < shared_faces.size(); vector_index++) { // for each array of ints (shared_face arrays) in the vector

        int t_i = shared_faces[vector_index][1];
        std::tuple<int, int> node_1 = {
            (int)(AudioHandler::BUFFER_WIDTH * (*texels)[((t_i - 1) * 2) + 0]),
            (int)(AudioHandler::BUFFER_HEIGHT * (*texels)[((t_i - 1) * 2) + 1])
        };

        t_i = shared_faces[vector_index][4];
        std::tuple<int, int> node_2 = {
            (int)(AudioHandler::BUFFER_WIDTH * (*texels)[((t_i - 1) * 2) + 0]),
            (int)(AudioHandler::BUFFER_HEIGHT * (*texels)[((t_i - 1) * 2) + 1])
        };

        t_i = shared_faces[vector_index][7];
        std::tuple<int, int> node_3 = {
            (int)(AudioHandler::BUFFER_WIDTH * (*texels)[((t_i - 1) * 2) + 0]),
            (int)(AudioHandler::BUFFER_HEIGHT * (*texels)[((t_i - 1) * 2) + 1])
        };

        shared_nodes.insert(node_1);
        shared_nodes.insert(node_2);
        shared_nodes.insert(node_3);
    }

    std::tuple<int, int> root_node = {
        (int)(ZMesh::z_mesh_point_temp->texel[0] * AudioHandler::BUFFER_WIDTH),
        (int)(ZMesh::z_mesh_point_temp->texel[1] * AudioHandler::BUFFER_HEIGHT)
    };

    shared_nodes.erase(root_node);

    ZMesh::z_mesh_point_temp->shared_nodes = shared_nodes;
    zmesh_log->Log(shared_nodes);
}
