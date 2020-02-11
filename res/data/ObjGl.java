class ObjGl {

    float[] ObjArray;
    Vector<char> temp_string = new Vector<char>;
    Vector<float> data = new Vector<float>;
    Vector<char> line = new Vector<char>;
    int count = 1;
    int key_value;
    Vector<float> vertex_data, texel_data, normal_data;
    float[] vertex_data_array, texel_data_array, normal_data_array;
    int vector_vertex_index, vector_normal_index;
    map vertex_texel_map;

    public static void main(String args[]){

        //new file is opened

        //file buffer is written to character 'file' c.string

        c.string buffer = c.string.length()

        // for every element in the file string
        for(int i = 0; i < buffer.length(); i++)
        {
            // if current character is vftn set state and move on, else, evalute character
            switch(buffer[0])
            {
                case 'f': data_index = 4;
                    break;
                case 't': data_index = 2;
                    break;
                case 'n': data_index = 3;
                    break;
                case 'v': data_index = 1;
                    break;
                default:
                    if(data_index = 4)
                    {
                        evaluateFace();               
                    }
                    else
                    {
                        evaluateNonFace();
                    }
            }   
        }

        convertDataToOpenGl();
        convertDataToSpectrogram();
    }

    public static void evaluateFace()
    {
        if(buffer[0] != ' ')&&(buffer[0] != '/')
        {
            temp_string.push_back();
        }
        if(buffer[0] == '/')&&(temp_string.isntClear)
        {
            if(count == 2)
            {
                vertex_texel_map[key: key_value] = buffer[0];
                count = 1;
            }
            else
            {
                key_value = temp_string.convertToFloat();
                count++;
            }
        }
        if(buffer[0] == ' ')
        {
            temp_string.clear();
        }  
    }

    public static void evaluateNonFace()
    {
        if(buffer[0] != ' ')
        {
            temp_string.push_back();
        }
        if(buffer[0] == ' ')&&(temp_string.isntClear)
        {
            if(data_index == 2)
            {
                writeData(2);
            }
            else
            {
                writeData(3);
            }
        }
    }

    public static void writeData(int n)
    {
        if(count == n)
        {
            temp_string.clear();
            count = 1;
        }
        else
        {
            switch(data_index)
            {
                case 1: vertex_data.push_back(temp_string.convertToFloat();
                    break;
                case 2: texel_data.push_back(temp_string.convertToFloat();
                    break;
                case 3: normal_data.push_back(temp_string.convertToFloat();
                    break;
            }
            temp_string.clear();
            count++;
        }
    }

    public static void convertDataToOpenGl()
    {
        vertex_data_array = new float[vertex_data.size()];
        std::copy(vertex_data.begin(), vertex_data.end(), vertex_data_array);
        texel_data_array = new float[texel_data_array.size()];
        std::copy(texel_data.begin(), texel_data.end(), texel_data_array);
        normal_data_array = new float[normal_data_array.size()];
        std::copy(normal_data.begin(), normal_data.end(), normal_data_array);
    }

    public static void convertDataToSpectrogram()
    {
        ObjArray = new float[vertex_texel_map.size() * 5];

        for(const auto &kvPair : vertex_texel_map)
        {
            vector_vertex_index = kvPair.first;  //line 4
            vector_normal_index = kvPair.second;  //line 4

            //get array_vertex_index = ((vector_vertex_index - 1) * 3)
            //get array_nornal_index = ((vector_normal_index - 1) * 2)
            //get OBJArray index = (vector_vertex_index - 1) * 5)
            
            //objarraay [OBJArray index + 1] = vertex data array[arrayvertex index + 1]
            //objarraay [OBJArray index + 2] = vertex data array[arrayvertex index + 2]
            //objarraay [OBJArray index + 3] = vertex data array[arrayvertex index + 3]
            //objarraay [OBJArray index + 4] = normal data array[arrat normal index + 1]
            //objarraay [OBJArray index + 5] = normal data array[arrat normal index + 2]

            //  normal data array [
            //        <float>, <float>,
            //        <float>, <float>,
            //        <float>, <float>, 
            //        <float>, <float>, ]


            //  vertex data array [
            //        <float>, <float>, <float>, 
            //        <float>, <float>, <float>,
            //        <float>, <float>, <float>, 
            //        <float>, <float>, <float>, ]
            
            //  ObjArray [
            //        <float>, <float>, <float>, <float>, <float>, 
            //        <float>, <float>, <float>, <float>, <float>, 
            //        <float>, <float>, <float>, <float>, <float>, 
            //        <float>, <float>, <float>...]

        }
    }
}