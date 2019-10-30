#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

class Muse
{
    private:

        struct vertexUvPair
        {
            std::tuple<float, float, float> vertex;
            std::tuple<float, float> uv;
        }

        int spectrogramBase[999999][999999] = {0};
        int spectrogram[999999][999999];
        
        /*
        *  Splits a string given a character to split with.
        *
        *  @param input String to splits
        *  @param splitter Character to split with
        *  @return result An array of String objects
        */
        vector<string> split(string input, char splitter)
        {
            vector<string> result;
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

        /*
        *  Takes in a local .obj file and parses the data into vector arrays of vertex(x, y, z) and uv(u, v) data, writes to base.mus
        *
        *  parseObj and loadBaseMuserData seem to be similar in what they do, but this method is important because the information in
        *  the .obj file is not ordered. Later, an ordered .obj may be used in which case this method may not be needed.
        * 
        *  @params filename Path to local .obj file
        *  @return lines 
        */
        std::vector<vertexUvPair> parseObj(std::string &filename) {
            // load path
            ifstream muse(filename);

            // if path doesnt exist, throw exception
            if (!muse.good())
            {
                throw std::runtime_error("There is no .obj file with name " + filename + " in resources folder");
            }

            cout << "\n" << "Opened File..." << "\n";

            // declare vertex and uv vector arrays
            vector<tuple<float, float, float>> verticies;
            vector<tuple<float, float>> uvs;
            string line;

            bool v = false, vt = false, f = false;
            float x, y, z;
            unordered_map<int, int> pointerMap;

            // for each line in file parse accordingly
            while (getline(muse, line))
            {
                // if the parsed line begins in v, vt, or f...
                if (line.find("v ") != string::npos)
                {
                    if (v == false) {
                        cout << "Reading all Verticies...\n";
                        v = true;
                    }
                    // uses split function, removes the first value, and then pushes the three values to the verticies Vector
                    vector<string> vSplit = split(line, ' ');
                    vector<string> vShift(vSplit.begin() + 1, vSplit.end());
                    x = (float)atof(vShift[0].c_str());
                    y = (float)atof(vShift[1].c_str());
                    z = (float)atof(vShift[2].c_str());
                    verticies.push_back(make_tuple(x, y, z));
                }
                else if (line.find("vt ") != string::npos)
                {
                    if (vt == false) {
                        cout << "Reading all Texture Coordinates...\n";
                        vt = true;
                    }
                    vector<string> vtSplit = split(line, ' ');
                    vector<string> vtShift(vtSplit.begin() + 1, vtSplit.end());
                    x = (float)atof(vtShift[0].c_str());
                    y = (float)atof(vtShift[1].c_str());
                    uvs.push_back(make_tuple(x, y));
                }
                else if (line.find("f ") != string::npos)
                {
                    if (f == false) {
                        cout << "Reading all Faces...\n";
                        f = true;
                    }
                    vector<string> pointerList = split(line, ' ');
                    vector<string> pointerListShift(pointerList.begin() + 1, pointerList.end());
                    for (string& each : pointerListShift)
                    {
                        // for each line in pointerlistshift, split each element into pieces and assign to a map
                        vector<string> pointer = split(each, '/');
                        vector<string> pointerShift(pointer.begin(), pointer.begin() + 2);
                        int key = atol(pointerShift[0].c_str());
                        int value = atol(pointerShift[1].c_str());
                        pointerMap[key - 1] = value - 1;
                    }
                }
            }

            // sets variable to display the amount of vertex/uv's processed in file
            int mapCount = distance(pointerMap.begin(), pointerMap.end());

            ofstream muserBase;
            muserBase.open("res/base.mus");

            // for each element in the verticies array, using the pointer map, find the corresponding element from the uvs array,
            // and set both objects to variables that are then passed to museVertTexDataLine
            std::vector<vertexUvPair> lines;
            for (size_t i = 0; i < verticies.size(); i++) {
                tuple<float, float, float> vertex = verticies[i];
                tuple<float, float> uv = uvs[pointerMap[i]];

                muserBase << std::get<0>(vertex) << " " << std::get<1>(vertex) << " " << std::get<2>(vertex) << " " << std::get<0>(uv) << " " << std::get<1>(uv) << "\n";

                museVertTexDataLine line;
                line.vertex = vertex;
                line.uv = uv;

                lines.push_back(line);
            }
            muserBase.close();
            cout << "\n" << "Using map of " << mapCount << " entries, created new struct with " << lines.size() << " entries" << "\n";
            return lines;
        }

        /*
        *  Takes in local base.mus file and parses the data into an array of the vertex/uv information in base.mus
        * 
        *  @params filename Path to local .obj file
        *  @return lines 
        */
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
            museVertTexDataLine baseMuserStruct;
            // Declare array of structs
            std::vector<museVertTexDataLine> baseMuserData;

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

        Muse(){
            loadBaseMuserData("res/base.mus");
        }

        void renderToSpectrogram(std::string &nameOfFile){
            for (int = i; i < baseMuserData.size(); i++){
                arrayX = baseMuserData[i].uv.u;
                arrayY = baseMuserData[i].uv.v;
                spectrogram[arrayX][arrayY] = (float (int (pow((pow((baseMuserData[i].vertex.x - 0), 2) + pow((baseMuserData[i].vertex.y - 0), 2) + pow((baseMuserData[i].vertex.z - 0), 2)), .5) * 10 + 0.5)) / 10);
            }

            ifstream spectrogramImage("spectrogram.ppm")
            spectrogramImage << "P3\n";
            spectrogramImage << "999999 999999\n"
            spectrogramImage << "255"
            for (int = i; i < spectrogram.size(); i++)
        }
};