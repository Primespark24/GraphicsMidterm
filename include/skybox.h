/*
* Skybox Utility Functions
* Whitworth University, CS357, Computer Graphics
* Developed by Scott Griffith, Spring 2021
*
* Based on work from:
* Kent Jones, Whitworth
* Dr. Anton Gerdelan, https://github.com/capnramses
*                     https://antongerdelan.net/opengl/cubemaps.html
*
* Functions to help load and set up a skycube!
*/
#pragma once

#include <sb7.h>   //OpenGL commands and utilities
#include <vector>  //Vertex holder
#include <string>  //passing in file names
#include <vmath.h> //Graphics utilities
#include <loadingFunctions.h>

//If you want more inspiring sky boxes: http://www.humus.name/index.php?page=Textures

//Fill a soon-to-be vertex list that represents a cube
// Unit cube centered on (0,0,0)
// Winding so normals are pointing 'in'
void createCube(std::vector<vmath::vec4> &vertices);

//based on sub point positions, fill up vertices array
// 0 1 2 3 corresponds to a cube face with the corners 0 1 2 and 3
// winding order matters here:
// we need to make sure the 'faces' of our cube are 'in'
// based on unit CUBE_VERTICES
void subPoints(int a, int b, int c, int d,std::vector<vmath::vec4> &vertices);

//Define the points of a unit cube
const vmath::vec4 CUBE_VERTICES[] = {
    vmath::vec4(-0.5, -0.5,  0.5, 1.0),
    vmath::vec4(-0.5,  0.5,  0.5, 1.0),
    vmath::vec4( 0.5,  0.5,  0.5, 1.0),
    vmath::vec4( 0.5, -0.5,  0.5, 1.0),
    vmath::vec4(-0.5, -0.5, -0.5, 1.0),
    vmath::vec4(-0.5,  0.5, -0.5, 1.0),
    vmath::vec4( 0.5,  0.5, -0.5, 1.0),
    vmath::vec4( 0.5, -0.5, -0.5, 1.0)
};

//Texture loading function
//Assumes that you have already set up the texture_ID as a Texture Cube Map
// Directory should hold six files: "sc_<side>.bmp"
// texture_ID -> GL handle for location of the texture
void loadCubeTextures(std::string directory, GLuint texture_ID);

//loadCubeTextures helper function
//pulls in individual files data and assigns it to a specific texture maping
// texture_ID -> GL handle for location of the texture
// side       -> ex: GL_TEXTURE_CUBE_MAP_POSITIVE_X
//               Which texture side is being uploaded
void loadCubeSide(GLint texture_ID, GLenum side, std::string file);
