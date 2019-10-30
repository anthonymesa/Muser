#pragma once

/*
 * Class Muse
 * 
 * Contains all the methods and variables relevant to displaying and rendering a 3d mesh to audio.
 * 
 */

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

#define SPEC_SZ 9999

class Muse
{

    private:
        struct vertexUvPair
        {
            std::tuple<float, float, float> vertex;
            std::tuple<float, float> uv;
        };

        std::vector<vertexUvPair> baseMuserData;
        //int spectrogramBase[999999][999999] = {0};
        unsigned char spectrogram[SPEC_SZ][SPEC_SZ];
        int arrayX, arrayY;

        std::vector<std::string> split(std::string input, char splitter)
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

        std::vector<vertexUvPair> parseObj(const std::string &filename)
        {
            // load path
            std::ifstream muse(filename);

            // if path doesnt exist, throw exception
            if (!muse.good())
            {
                throw std::runtime_error("There is no .obj file with name " + filename + " in resources folder");
            }

            std::cout << "\n" << "Opened File..." << "\n";

            // declare vertex and uv vector arrays
            std::vector<std::tuple<float, float, float>> verticies;
            std::vector<std::tuple<float, float>> uvs;
            std::string line;

            bool v = false, vt = false, f = false;
            float x, y, z;
            std::unordered_map<int, int> pointerMap;

            // for each line in file parse accordingly
            while (getline(muse, line))
            {
                // if the parsed line begins in v, vt, or f...
                if (line.find("v ") != std::string::npos)
                {
                    if (v == false) {
                        std::cout << "Reading all Verticies...\n";
                        v = true;
                    }
                    // uses split function, removes the first value, and then pushes the three values to the verticies Vector
                    std::vector<std::string> vSplit = split(line, ' ');
                    std::vector<std::string> vShift(vSplit.begin() + 1, vSplit.end());
                    x = (float)atof(vShift[0].c_str());
                    y = (float)atof(vShift[1].c_str());
                    z = (float)atof(vShift[2].c_str());
                    verticies.push_back(std::make_tuple(x, y, z));
                }
                else if (line.find("vt ") != std::string::npos)
                {
                    if (vt == false) {
                        std::cout << "Reading all Texture Coordinates...\n";
                        vt = true;
                    }
                    std::vector<std::string> vtSplit = split(line, ' ');
                    std::vector<std::string> vtShift(vtSplit.begin() + 1, vtSplit.end());
                    x = (float)atof(vtShift[0].c_str());
                    y = (float)atof(vtShift[1].c_str());
                    uvs.push_back(std::make_tuple(x, y));
                }
                else if (line.find("f ") != std::string::npos)
                {
                    if (f == false) {
                        std::cout << "Reading all Faces...\n";
                        f = true;
                    }
                    std::vector<std::string> pointerList = split(line, ' ');
                    std::vector<std::string> pointerListShift(pointerList.begin() + 1, pointerList.end());
                    for (std::string& each : pointerListShift)
                    {
                        // for each line in pointerlistshift, split each element into pieces and assign to a map
                        std::vector<std::string> pointer = split(each, '/');
                        std::vector<std::string> pointerShift(pointer.begin(), pointer.begin() + 2);
                        int key = atol(pointerShift[0].c_str());
                        int value = atol(pointerShift[1].c_str());
                        pointerMap[key - 1] = value - 1;
                    }
                }
            }

            // sets variable to display the amount of vertex/uv's processed in file
            int mapCount = distance(pointerMap.begin(), pointerMap.end());

            std::ofstream muserBase;
            muserBase.open("res/base.mus");

            // for each element in the verticies array, using the pointer map, find the corresponding element from the uvs array,
            // and set both objects to variables that are then passed to museVertTexDataLine
            std::vector<vertexUvPair> lines;
            for (size_t i = 0; i < verticies.size(); i++) {
                std::tuple<float, float, float> vertex = verticies[i];
                std::tuple<float, float> uv = uvs[pointerMap[i]];

                muserBase << std::get<0>(vertex) << " " << std::get<1>(vertex) << " " << std::get<2>(vertex) << " " << std::get<0>(uv) << " " << std::get<1>(uv) << "\n";

                vertexUvPair line;
                line.vertex = vertex;
                line.uv = uv;

                lines.push_back(line);
            }
            muserBase.close();
            std::cout << "\n" << "Using map of " << mapCount << " entries, created new struct with " << lines.size() << " entries" << "\n";
            return lines;
        }

        void loadBaseMuserData(const std::string &input)
        {
            // Declare file object defaultMuser from input
            std::ifstream defaultMuser(input);

            // If base.mus doesnt exist, generate a new one
            if (!defaultMuser.good()) {
                try
                {
                    parseObj("res/muserBase.obj");
                    std::ifstream defaultMuser(input);
                }
                catch (std::exception& exeption)
                {
                    std::cout << exeption.what() << std::endl;
                }
            }

            float x, y, z, u, v;

            // String to hold the info read from the muser object
            std::string defaultMuserLine;
            // Declare struct of vertexes and uvs
            vertexUvPair baseMuserStruct;

            // reads each line in loaded file object 'defaultMuser'
            while (getline(defaultMuser, defaultMuserLine)) {

                // Split current line defaultMuserLine into declared array of strings
                std::vector<std::string> loadedData = split(defaultMuserLine, ' ');

                // apply values from line array into x, y, and z
                x = (float)atof(loadedData[0].c_str());
                y = (float)atof(loadedData[1].c_str());
                z = (float)atof(loadedData[2].c_str());

                std::tuple<float, float, float> baseMusVertecies;

                // apply valuse of x, y, and z to a tuple
                baseMusVertecies = std::make_tuple(x, y, z);

                // apply values from line array into u and v
                u = (float)atof(loadedData[3].c_str());
                v = (float)atof(loadedData[4].c_str());

                std::tuple<float, float> baseMusUvs;

                // apply valuse of u and v to a tuple
                baseMusUvs = std::make_tuple(u, v);

                // apply tuples to both elements in museVertTexDataLine struct
                baseMuserStruct.vertex = baseMusVertecies;
                baseMuserStruct.uv = baseMusUvs;

                // add struct baseMuserStruct to baseMuserData array of vertex[3]uv[2] structs
                baseMuserData.push_back(baseMuserStruct);
            }
        }

    public:

        Muse()
        {
            loadBaseMuserData("res/base.mus");
        }

        void renderToSpectrogram(std::string &nameOfFile)
        {
            std::ofstream spectrogramImage;
            
            for (int i = 0; i < baseMuserData.size(); i++){
                arrayX = int ((std::get<0>(baseMuserData[i].uv)) * 100000)/1000;
                arrayY = int ((std::get<1>(baseMuserData[i].uv)) * 100000)/1000;
                spectrogram[arrayX][arrayY] = ((std::get<0>(baseMuserData[i].vertex + std::get<1>(baseMuserData[i].vertex + std::get<2>(baseMuserData[i].vertex)/3) * 255;
                //spectrogram[arrayX][arrayY] = (float (int (pow((pow((std::get<0>(baseMuserData[i].vertex) - 0), 2) + pow((std::get<1>(baseMuserData[i].vertex) - 0), 2) + pow((std::get<2>(baseMuserData[i].vertex) - 0), 2)), .5) * 10 + 0.5)) / 10);
            }
            
            spectrogramImage.open(nameOfFile + ".ppm");
            spectrogramImage << "P3\n";
            spectrogramImage << "9999 9999\n";
            spectrogramImage << "255";

            for (int i = 0; i < sizeof(spectrogram)/sizeof(spectrogram[0]); i++){
                for (int j = 0; j < sizeof(spectrogram[0])/sizeof(int); j++){
                    spectrogramImage << spectrogram[i][j] << " ";
                }
            }
        }
};