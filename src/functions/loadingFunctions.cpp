
#include <loadingFunctions.h>
//Object Loading Information
//Referenced from https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
// and http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// When exporting from blender, keep everything default except:
//       Make sure to include Normals
//       Include UVs
//       Triangulate Faces
//       Don't 'Write Materials'

// filename - Blender .obj file (see file formatting specifics above)
// All vectors passed by reference and filled in function
// vertices - list of, in order, verticies for object. In triangles
// UVs - Texture mapping coordinates, indexed with the above vertices
// normals - index with the above vertices
// number - Total number of points in vertices (should be vertices.length())
void load_obj(const char* filename, std::vector<vmath::vec4> &vertices, std::vector<vmath::vec2> &uvs, std::vector<vmath::vec4> &normals, GLuint &number)
{
    //File to load in
    std::ifstream in(filename, std::ios::in);

    //Check to make sure file opened
    if (!in) {
        char buf[50];
        sprintf(buf, "OBJ file not found!");
        MessageBoxA(NULL, buf, "Error in loading obj file", MB_OK);
    }

    //Temp vectors to hold data
    //These will need to be indexed into the output vectors based on face info
    std::vector<vmath::vec4> tempVert; // from vertices lines 'v <x> <y> <z>'
    //This might need to be a more complicated structure, we can get away with strict indexing
    std::vector<GLuint> tempFace; // from face line 'f <v1>/<t1>/<n1> <v2>/<t2>/<n2> <v3>/<t3>/<n3>', should be indexes
    std::vector<vmath::vec2> tempUVs; // from texture line 'vt <x> <y>'
    std::vector<vmath::vec4> tempNorm; // from a normal line 'vn <x> <y> <z>' //Should this be a vec3 or vec4?

    std::string line;       // Complete line pulled from file
    std::string sub = "";   // Substring working space
    GLfloat num;            // Temp floats from line processing
    while (std::getline(in, line)) { //grab every line of the file
        //Depending on what is in the line, parse differently

        if (line.substr(0,2) == "v ") {
            //Process vertex line
            sub = line.substr(2); //Isolate substring without 'v'

            //vector to be filled
            vmath::vec4 tVec(0.0f,0.0f,0.0f,1.0f);

            //Expect 3 numbers, iterate three times
            //Isolate number and clip substring [x,y,z,1]
            tVec[0] = stof(parseAndClip(sub," "));
            tVec[1] = stof(parseAndClip(sub," "));
            tVec[2] = stof(parseAndClip(sub," "));

            //Push vec4 onto verticies vector
            tempVert.push_back(tVec);

        } else if (line.substr(0,2) == "f ") {
            //Process face line
            // Faces line f 14/25/9 60/19/9 56/97/9 : f <vertex1>/<texture1>/<normal1> <vertex2>/<texture2>/<normal2> <vertex3>/<texture3>/<normal3>
            sub = line.substr(2); //Current sub string of line
            //Expect 3 number sets, loop three times
            for(int i = 0; i < 3; i++) {
                //Isolate next triangle set
                //First before / /
                tempFace.push_back( stoi(parseAndClip(sub,"/")) ); //sub_part 0 == vertex index

                //second value in triplet (between the / and /)
                tempFace.push_back( stoi(parseAndClip(sub,"/")) ); //sub_part 1 == texture coordinate index

                //third value in triplet (after both /, " " up next)
                tempFace.push_back( stoi(parseAndClip(sub," ")) ); //sub_part 2 == normal index
            }
            //Already pushed info onto faces

        } else if (line.substr(0,3) == "vt ") {
            //Process Texture mappings
            sub = line.substr(3); //Isolate substring without 'vt '

            vmath::vec2 tUV(0.0f,0.0f);
            //Expect 2 numbers, loop twice
            tUV[0] = stof(parseAndClip(sub," ")); //x
            tUV[1] = stof(parseAndClip(sub," ")); //y

            //Push vec2 onto texture vector
            tempUVs.push_back(tUV);

        } else if (line.substr(0,3) == "vn ") {
            //Process normal line
            sub = line.substr(3); //Isolate substring without 'vt '

            //4D? is w:0?
            vmath::vec4 tNorm(0.0f,0.0f,0.0f,0.0f);

            //Expect 3 numbers, loop thrice
            //Isolate number and clip substring
            tNorm[0] = stof(parseAndClip(sub," "));
            tNorm[1] = stof(parseAndClip(sub," "));
            tNorm[2] = stof(parseAndClip(sub," "));

            //Push normal onto tempNormal vector
            tempNorm.push_back(tNorm);

        } else {
            //other kind of line, ignoring
        }
    } //end of line parsing

    //Clear out output vectors (just to be safe)
    vertices.clear();
    uvs.clear();
    normals.clear();
    number = 0;

    //At this point out temp vectors are full of data
    // tempVert, tempUVs and tempNorm are indexed (starting at 0) in file order
    // tempFace correlates everything together in sets of 9 values (three triplets)
    // Using the data in tempFace the output vectors will be filled with the correct (in order) data

    //                   0    1    2    3    4    5    6    7    8
    // Faces striping: <v1>/<t1>/<n1> <v2>/<t2>/<n2> <v3>/<t3>/<n3>
    //Because the data in tempFace is striped buy sets of three triplets, step forward by 9 each time
    for(int i = 0; i < tempFace.size(); i += 9 ){
        //Pull data into vertices
        //                                   VVV Index offset pattern
        //                          VVV Holds vertex index to pull from tempVery (offset from starting at 1 to 0)
        //                 VVV Indexed vertex info
        vertices.push_back(tempVert[tempFace[i+0]-1]); //v1
        vertices.push_back(tempVert[tempFace[i+3]-1]); //v2
        vertices.push_back(tempVert[tempFace[i+6]-1]); //v3

        //Pull data into uvs
        uvs.push_back(tempUVs[tempFace[i+1]-1]); //uv1
        uvs.push_back(tempUVs[tempFace[i+4]-1]); //uv2
        uvs.push_back(tempUVs[tempFace[i+7]-1]); //uv3

        //Pull data into normals
        normals.push_back(tempNorm[tempFace[i+2]-1]); //n1
        normals.push_back(tempNorm[tempFace[i+5]-1]); //n1
        normals.push_back(tempNorm[tempFace[i+8]-1]); //n1

        number++; //Sanity Check to make sure things line up
    }

}

//File parsing helper
//Pull off the first element of sub up to delim
// Ex: sub |0.877342 0.081279 -0.329742| delim: " "
// returns |0.877342|
// changes sub (via reference) to |0.081279 -0.329742|
std::string parseAndClip(std::string& sub, std::string delim){
    unsigned int curIndex = sub.find(delim); //Defined once to speed up
    //Grab substring upto first delimeter
    std::string ret = sub.substr(0,curIndex);
    //delete everything upto delimeter
    sub = sub.substr(curIndex+1);
    return ret;
}


void load_BMP(std::string file, unsigned char* &texture_data, unsigned int &tWidth, unsigned int &tHeight){
    //If you are curious why so many unsigned chars: https://stackoverflow.com/questions/75191/what-is-an-unsigned-char

    //Set up some function variables
    //Input file stream
    std::ifstream tFile;
    unsigned int tDataSize = 0;

    //BitMap File infomation
    // reference: https://en.wikipedia.org/wiki/BMP_file_format
    // reference: http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
    // It is important to note that we only need to know the size of data (width and height) and where the data starts
    // Asssuming data offset (tStartOfData) is valid, and so are the height and width, this should work with any version
    //Bitmap header / DIB Header info holders
    char bmpFileHeader[14]; // We need this to grab the last four bytes (data offset)
    char bmpDIBHeader[12]; //We only need to grab the first few lines of this
    unsigned int tStartOfData; //Location of start of pixel data (likely 54 bytes)
    //unsigned int tHeight; //Size of the texture data
    //unsigned int tWidth;  //Size of the texture data
    // Based on how the file format works this is going to need to be a power of two (likely) or there will be padding involved

    //Attempt to open the file
    tFile.open(file,std::ifstream::in | std::ifstream::binary);
    if(!tFile){
        //Check to see if file is open
        char buf[50];
        sprintf(buf, "One of the texture files was found!");
        MessageBoxA(NULL, buf, "Error in loading texture file", MB_OK);
        return;
    }

    //Read in the BitMap file header, this will be used to isolate the data offset (where the pixels start)
    tFile.read(bmpFileHeader,14); //Read in 14 bytes

    //Seek forward 14 bytes from the beginning (should already be there, just to be safe)
    tFile.seekg(14,std::ios_base::beg);

    //Read just enough from the infoHeader to get the width and height info
    tFile.read(bmpDIBHeader, 12);

    //Extract relevant info from headers
    tWidth = charToUInt(&bmpDIBHeader[4]);//second thing (4 bytes in)
    tHeight = charToUInt(&bmpDIBHeader[8]);//third thing (4 more bytes in)
    tStartOfData = charToUInt(&bmpFileHeader[10]); //0xA offset from start (10 bytes in)

    //Calculate the needed OUTPUT size of the data (width x height x 4) 4 because rgb+alpha
    tDataSize = tWidth * tHeight * 4;
    //allocate memory for data
    texture_data = new unsigned char [tDataSize];

    //Zero out array, just in case
    for(int i = 0; i < tDataSize; i++){
        texture_data[i] = 0;
    }

    //Move file pointer to the correct location based on header size
    tFile.seekg(tStartOfData,std::ios_base::beg);

    // At this point we can read every pixel of the image
    int i = 0;
    int j = 0; //Index variables
    char colors[3]; //Temp hold location for color data
    for (; i < tWidth * tHeight; i++) { //Loop over all 'pixels'
            // We load an RGB value from the file
            tFile.read(colors,3);

            // And store it
            texture_data[j+0] = colors[2]; //Red component
            texture_data[j+1] = colors[1]; //Green componet
            texture_data[j+2] = colors[0]; //Blue component
            texture_data[j+3] = 255; //Add alpha element
            j += 4; // Go to the next position
    }

    // Closes the file stream
    tFile.close();

    return;
}

unsigned int charToUInt(char * loc){
    // Data coming in like:
    // loc[0] : 0x00
    // loc[1] : 0x02
    // loc[2] : 0x00
    // loc[3] : 0x00
    // output 00 00 02 00 = 0x00000200
    unsigned int base = 0;

    //                  VVVVVVVV Casting does work, but it is going from 1 byte (char) to 4 bytes int
    //                           based on how 1's compliment works, 1111 get padded if the most significant bit is 1
    //                                          VVVVV Mask off all those ones that might show up
    base = (static_cast<unsigned int>(loc[3]) & 0x0FF); //Start with the 'highest' byte
    base = base << 8; //Shift over to make room for the next

    base += (static_cast<unsigned int>(loc[2]) & 0x0FF); //Rinse and repeat
    base = base << 8;

    base += (static_cast<unsigned int>(loc[1]) & 0x0FF);
    base = base << 8;

    base += (static_cast<unsigned int>(loc[0]) & 0x0FF);

    return base;
}

/*
This needs to be in ./src/functions/loadingFunctions.cpp
with a corresponding .h file in ./include/loadingFunctions.cpp

Edit your CMakeLists.txt:

add_library(sb7
            src/sb7/sb7.cpp
            src/sb7/sb7color.cpp
            src/sb7/sb7ktx.cpp
            src/sb7/sb7object.cpp
            src/sb7/sb7shader.cpp
            src/sb7/sb7textoverlay.cpp
            src/sb7/gl3w.c
            src/functions/loadingFunctions.cpp     <<<<< Add these lines
            src/functions/skybox.cpp               <<<<<
)


*/
