/*
* Skybox Utility Functions
* Whitworth University, CS357, Computer Graphics
* Developed by Scott Griffith, Spring 2021
*
* Based on work from:
* Kent Jones, Whitworth
* Dr. Anton Gerdelan, https://github.com/capnramses 
*                     https://antongerdelan.net/opengl/cubemaps.html                 
*/
#include <skybox.h>
#include <fstream>

void createCube(std::vector<vmath::vec4> &vertices){
    //We need to enumerate all of the different sides of a cube
    //Each one will have two triangles that make up the side
    //The goal is to make this a unit cube that we can scale up later
    subPoints(1,0,3,2,vertices); //Side 1
    subPoints(2,3,7,6,vertices); //Side 2

    subPoints(3,0,4,7,vertices);
    subPoints(6,5,1,2,vertices);

    subPoints(4,5,6,7,vertices);
    subPoints(5,4,0,1,vertices);

}

void subPoints(int a, int b, int c, int d,std::vector<vmath::vec4> &vertices){
    //Triangle 1
    vertices.push_back(CUBE_VERTICES[a]);
    //vertices.push_back(CUBE_VERTICES[b]); //This is has to do with the winding order (this will be CW)
    vertices.push_back(CUBE_VERTICES[c]);

    vertices.push_back(CUBE_VERTICES[b]); //This is CCW winding

    //Triangle 2
    vertices.push_back(CUBE_VERTICES[a]);
    //vertices.push_back(CUBE_VERTICES[c]); //This is has to do with the winding order (this will be CW)
    vertices.push_back(CUBE_VERTICES[d]);

    vertices.push_back(CUBE_VERTICES[c]); //This is CCW winding
}

void loadCubeTextures(std::string directory, GLuint texture_ID){
    //Load each side in from the file
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, directory+".\\panorama_0.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, directory+".\\panorama_2.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, directory+".\\panorama_5.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, directory+".\\panorama_4.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_POSITIVE_X, directory+".\\panorama_1.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, directory+".\\panorama_3.bmp");

    // Set standard parameters for the cube map texture mapping
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );   
}

void loadCubeSide(GLint texture_ID, GLenum side, std::string file){
    //If you are curious why so many unsigned chars: https://stackoverflow.com/questions/75191/what-is-an-unsigned-char
    // Bind the next call to this CUBE_MAP
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ID);

    //Set up some function variables
    //Input file stream
    std::ifstream tFile;
    //Memory location of where we will put the texture data
    unsigned char *texture_data; 
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
    unsigned int tHeight; //Size of the texture data
    unsigned int tWidth;  //Size of the texture data 
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

    // Load the image data into a buffer to send to the GPU
	glTexImage2D( side, // Which side are you loading in (should be an enum)
                     0, // Level of detail, 0 base level
               GL_RGBA, // Internal (target) format of data, in this case Red, Gree, Blue, Alpha
                tWidth, // Width of texture data (max is 1024, but maybe more)
               tHeight, // Height of texture data
                     0, //border (must be zero)
               GL_RGBA, //Format of input data (in this case we added the alpha when reading in data)
      GL_UNSIGNED_BYTE, //Type of data being passed in
          texture_data); // Finally pointer to actual data to be passed in
    
    // Free the memory we used to load the texture
	delete[] texture_data;

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

    base = loc[3]; //Start with the 'highest' byte
    base = base << 8; //Shift over to make room for the next

    base += loc[2]; //Rinse and repeat
    base = base << 8;

    base += loc[1];
    base = base << 8;

    base += loc[0];

    return base;   
}