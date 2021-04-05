#pragma once

#include <sb7.h>
#include <vmath.h>
#include <string>
#include <fstream>
#include <vector>

//Function set to help load .obj files into a form ready for OpenGL

//File parsing helper
//Pull off the first element of sub up to delim
// Ex: sub |0.877342 0.081279 -0.329742| delim: " "
// returns |0.877342|
// changes sub (via reference) to |0.081279 -0.329742|  
std::string parseAndClip(std::string& sub, std::string delim);

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
void load_obj(const char* filename, std::vector<vmath::vec4> &vertices, std::vector<vmath::vec2> &uvs, std::vector<vmath::vec4> &normals, GLuint &number);