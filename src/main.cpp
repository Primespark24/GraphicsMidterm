/*
 * Kyle Shepard and Brycen Martin
 *
 * Midterm Project - MC Maze
 *
 * Based on work by Graham Sellers and OpenGL SuperBible7 and Scott Griffith
 * Also: https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
 *       http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
 *       https://antongerdelan.net/opengl/cubemaps.html
 *       https://learnopengl.com/Getting-started/Camera
 */

#include <vectorFunctions.h>
#include <sb7.h>
#include <shader.h>
#include <vmath.h>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <ctime>

#include <loadingFunctions.h>
#include <skybox.h>
#include <maze.h>

//Needed for file loading (also vector)
#include <string>
#include <fstream>

// For error checking
#include <vector>
#include <cassert>
#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

class test_app : public sb7::application{

    private:
        //Scene Rendering Information
        GLuint rendering_program; //Program reference for scene generation
        GLuint vertex_array_object;

        //Uniform attributes for Scene Render
        GLuint transform_ID; //Dynamic transform of object
        GLuint perspec_ID;   //Perspective transform
        GLuint toCam_ID;     //World to Camera transform
        GLuint vertex_ID;    //This will be mapped to different objects as we load them

        // Structure to hold all the object info
        struct obj_t{
            //Data for object loaded from file
            std::vector<vmath::vec4> vertices;
            std::vector<vmath::vec4> normals;
            std::vector<vmath::vec2> uv;
            vmath::vec3 scale;
            vmath::vec3 world_origin;
            vmath::vec3 max; //maximum x, y, and z values for bounding box
            vmath::vec3 min; //minimum x, y, and z values for bounding box
            GLuint vertNum; //This should be the same as vertivies.size()

            //Handle from OpenGL set up
            GLuint vertices_buffer_ID;

            //Object to World transforms
            vmath::mat4 obj2world;
        };

        //Hold all of our objects
        std::vector<obj_t> objects;



        //Data for Skycube
        GLuint sc_program; //Program refernce

        GLuint sc_vertex_array_object;
        GLuint sc_map_texture;

        //TODO:: Rename these better names
        GLuint sc_Camera;
        GLuint sc_Perspective;

        std::vector<vmath::vec4> skycube_vertices; //List of skycube vertexes

        //decide how fast camera should translate/rotate on keypresses
        GLfloat move_speed;
        GLfloat pan_speed;

        GLfloat pitch;
        GLfloat yaw;

        GLint last_x;
        GLint last_y;
        bool first_mouse;

        bool autoRotate = false;

        // Camera Stuff
        struct camera_t{ //Keep all of our camera stuff together
            bool collision;
            float camera_near;   //Near clipping mask
            float camera_far;    //Far clipping mask
            float fovy;          //Field of View in y
            float aspect;        //Aspect ratio (w/h)
            float pitch;
            float yaw;
            vmath::mat4 proj_Matrix; //Collection of the above

            vmath::vec3 position; //Current world coordinates of the camera
            vmath::vec3 forward; //unit vector of forward direction of the camera
            vmath::vec3 up; //unit vector of up direction of the camera

            vmath::mat4 view_mat; //World to Camera matrix
            vmath::mat4 view_mat_no_translation; //World to Camera matrix with no translation

            vmath::vec3 advance(GLfloat move_speed){
                return vmath::normalize(vmath::vec3(forward[0], 0.0f, forward[2])) * move_speed;
            }

            vmath::vec3 strafe(GLfloat move_speed){
                vmath::vec3 direction = vmath::cross(forward, up);
                return vmath::normalize(vmath::vec3(direction[0], 0.0f, direction[2])) * move_speed;
            }
        } camera;

        //Utility to update project matrix and view matrix of a camera_t
        void calcProjection(camera_t &cur){
            cur.aspect = static_cast<float>(info.windowWidth) / static_cast<float>(info.windowHeight); //Maybe this will keep it updated?
            cur.proj_Matrix = vmath::perspective(cur.fovy,cur.aspect, cur.camera_near, cur.camera_far);
        }

        void calcView(camera_t &cur){

            cur.view_mat = vmath::lookat(cur.position, autoRotate ? vmath::vec3(0.0f, 0.0f, 0.0f) : cur.position + cur.forward, vmath::vec3(0.0f, 1.0f, 0.0f)); //Based on position and focus location
            cur.view_mat_no_translation = cur.view_mat;
            //Removing the tranlational elements for skybox
            cur.view_mat_no_translation[3][0] = 0;
            cur.view_mat_no_translation[3][1] = 0;
            cur.view_mat_no_translation[3][2] = 0;
        }

    public:

    void init(){
        // Set up appropriate title
        static const char title[] = "A Maze-ing";
        sb7::application::init();
        memcpy(info.title, title, sizeof(title));

        //16:9 aspect ratio
        info.windowWidth = 1600;
        info.windowHeight = 900;
    }

    void startup(){
        //////////////////////
        // Load Object Info //
        //////////////////////

        //create maze floor
        for (unsigned x = 0; x < MazeWidth; x++){
            for (unsigned y = 0; y < MazeHeight; y++){
                //flatten to 1D index
                unsigned i = (x * MazeWidth) + y;
                // generate new cube
                objects.push_back(obj_t());
                load_obj(".\\bin\\media\\cube.obj", objects[i].vertices, objects[i].uv, objects[i].normals, objects[i].vertNum);

                //test place in line
                objects[i].world_origin = vmath::vec3(static_cast<float>(x), -1.0f, static_cast<float>(y));
                objects[i].scale = vmath::vec3(0.5f, 0.5f, 0.5f);
                // level.push_back({ vmath::vec3(x, 0, y), vmath::vec3(1) });
            }
        }


        ////////////////////////////////
        //Set up Object Scene Shaders //
        ////////////////////////////////
        GLuint shaders[2];

        //Load scene rendering based shaders
        //These need to be co-located with main.cpp in src
        shaders[0] = sb7::shader::load(".\\src\\vs.glsl", GL_VERTEX_SHADER);
        compiler_error_check(shaders[0]);
        shaders[1] = sb7::shader::load(".\\src\\fs.glsl", GL_FRAGMENT_SHADER);
        compiler_error_check(shaders[1]);
        //Put together scene rendering program from the two loaded shaders
        rendering_program = sb7::program::link_from_shaders(shaders, 2, true);
        GL_CHECK_ERRORS

        /////////////////////////////////
        // Transfer Object Into OpenGL //
        /////////////////////////////////

        //Set up vao
        glUseProgram(rendering_program); //TODO:: This might not be necessary (because of the above link_from_shaders)
        glCreateVertexArrays(1,&vertex_array_object);
        glBindVertexArray(vertex_array_object);

        std::srand(std::time(nullptr));

        std::vector<vmath::vec3> initMaze = GenerateMaze();
        initMaze.erase(std::remove(initMaze.begin(), initMaze.end(), vmath::vec3(0, 0, 1)), initMaze.end());
        initMaze.erase(std::remove(initMaze.begin(), initMaze.end(), vmath::vec3(MazeWidth - 1, MazeHeight - 1, 3)), initMaze.end());
        // std::ostringstream oss;
        // oss << initMaze.size();
        // errorBoxString(oss.str());
        std::vector<obj_t> maze = convertMazeToWorld(initMaze);
        //insert into objects and build
        objects.reserve(objects.size() + maze.size());
        objects.insert(objects.end(), maze.begin(), maze.end());

        for(int i = 0; i < objects.size(); i++){
            //For each object in objects, set up openGL buffers
            glGenBuffers(1,&objects[i].vertices_buffer_ID); //Create the buffer id for this object
            glBindBuffer( GL_ARRAY_BUFFER, objects[i].vertices_buffer_ID);
            glBufferData( GL_ARRAY_BUFFER,
                objects[i].vertices.size() * sizeof(objects[i].vertices[0]), //Size of element * number of elements
                objects[i].vertices.data(),                                   //Actual data
                GL_STATIC_DRAW);                                               //Set to static draw (read only)

            //calculate and store bounding box
            std::vector<float> xs;
            std::vector<float> ys;
            std::vector<float> zs;

            for(int j = 0; j < objects[i].vertices.size(); j++){
                vmath::vec4 vertex = objects[i].vertices[j] * vmath::scale(objects[i].scale);
                xs.push_back(vertex[0]);
                ys.push_back(vertex[1]);
                zs.push_back(vertex[2]);
            }

            //check vector contents
            // errorBoxString(toString(xs));
            // errorBoxString(toString(zs));

            std::tuple<float, float> xMinMax = minMax(xs);
            std::tuple<float, float> yMinMax = minMax(ys);
            std::tuple<float, float> zMinMax = minMax(zs);

            // std::ostringstream oss;
            // oss << std::get<1>(xMinMax);
            // errorBoxString(oss.str());

            objects[i].min = vmath::vec3(std::get<0>(xMinMax), std::get<0>(yMinMax), std::get<0>(zMinMax));
            objects[i].max = vmath::vec3(std::get<1>(xMinMax), std::get<1>(yMinMax), std::get<1>(zMinMax));

            //If we needed to load the UVs or Normals, this would be where.
        }

        GL_CHECK_ERRORS
        ////////////////////////////////////
        // Grab IDs for rendering program //
        ////////////////////////////////////
        transform_ID = glGetUniformLocation(rendering_program,"transform");
        perspec_ID = glGetUniformLocation(rendering_program,"perspective");
        toCam_ID = glGetUniformLocation(rendering_program,"toCamera");
        vertex_ID = glGetAttribLocation(rendering_program,"obj_vertex");

        ///////////////////////////
        //Set up Skycube shaders //
        ///////////////////////////
        // Placeholders for loaded shaders
        GLuint sc_shaders[2];

        //Load Skycube based shaders
        //These need to be co-located with main.cpp in src
        sc_shaders[0] = sb7::shader::load(".\\src\\sc_vs.glsl", GL_VERTEX_SHADER);
        compiler_error_check(sc_shaders[0]);
        sc_shaders[1] = sb7::shader::load(".\\src\\sc_fs.glsl", GL_FRAGMENT_SHADER);
        compiler_error_check(sc_shaders[1]);

        //Put together Sky cube program from the two loaded shaders
        sc_program = sb7::program::link_from_shaders(sc_shaders, 2, true);
        GL_CHECK_ERRORS

        /////////////////////
        //Load Skycube info//
        /////////////////////
        //skycube_vertices holds triangle form of a cube
        createCube(skycube_vertices);
        //Set up Vertex Array Object and associated Vertex Buffer Object
        GLuint vBufferObject; //We only need this to associate with sc_vertex_array_object
        glGenBuffers(1,&vBufferObject); //Create the buffer id
        glBindBuffer( GL_ARRAY_BUFFER, vBufferObject);
        glBufferData( GL_ARRAY_BUFFER,
                skycube_vertices.size() * sizeof(skycube_vertices[0]), //Size of element * number of elements
                skycube_vertices.data(),                               //Actual data
                GL_STATIC_DRAW);                                       //Set to static draw (read only)

        glGenVertexArrays(1, &sc_vertex_array_object); // Get ID for skycube vao
        glBindVertexArray(sc_vertex_array_object);
        glEnableVertexAttribArray(0); //Enable Vertex Attribute Array
        glBindBuffer( GL_ARRAY_BUFFER, vBufferObject);
        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, NULL); //Linking the buffer filled above to a vertex attribute
        GL_CHECK_ERRORS

        //Set up texture information
        glActiveTexture(GL_TEXTURE0);     //Set following data to GL_TEXTURE0
        glGenTextures(1,&sc_map_texture); //Grab texture ID
        //Call a file loading function to load in textures for skybox
        loadCubeTextures(".\\bin\\media\\Skycube\\",sc_map_texture);
        GL_CHECK_ERRORS

        //Get uniform handles for perspective and camera matrices
        sc_Perspective = glGetUniformLocation(sc_program,"perspective");
        sc_Camera= glGetUniformLocation(sc_program,"toCamera");
        GL_CHECK_ERRORS

        /////////////////////
        // Camera Creation //
        /////////////////////
        camera.collision = true;   //enable collision
        camera.camera_near = 0.1f; //Near Clipping Plane
        camera.camera_far = 100.0f; //Far Clipping Plane
        camera.fovy       = 67.0f; //Field of view in the y direction (x defined by aspect)
        //Initial camera details
        camera.position = vmath::vec3(0.0f, 0.5f, 5.0f); //Starting camera at position (0,0,5)
        camera.forward = vmath::vec3(0.0f, 0.0f, -1.0f); //Camera is looking at origin
        camera.up = vmath::vec3(0.0f, 1.0f, 0.0f); //Camera is looking at origin
        pitch = 0.0f;
        yaw = -90.0f;


        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        move_speed = 0.2f;
        pan_speed = 0.2f;

        //Now that we have parameters set, calculate the Projection and View information for this camera
        calcProjection(camera); //Calculate the projection matrix used by this camera
        calcView(camera); //Calculate the View matrix for camera

        //Link locations to Uniforms
        glUseProgram(sc_program);
        glUniformMatrix4fv(sc_Perspective,1,GL_FALSE,camera.proj_Matrix);
        glUniformMatrix4fv(sc_Camera,1,GL_FALSE,camera.view_mat_no_translation);
        GL_CHECK_ERRORS

        // General openGL settings
        //src:: https://github.com/capnramses/antons_opengl_tutorials_book/tree/master/21_cube_mapping
        glEnable( GL_DEPTH_TEST );          // enable depth-testing
        glDepthFunc( GL_LESS );             // depth-testing interprets a smaller value as "closer"
        glEnable( GL_CULL_FACE );           // cull face
        glCullFace( GL_BACK );              // cull back face
        glFrontFace( GL_CCW );              // set counter-clock-wise vertex order to mean the front
        glClearColor( 0.2, 0.2, 0.2, 1.0 ); // grey background to help spot mistakes

        //End of set up check
        GL_CHECK_ERRORS
    }

    void shutdown(){
        //Clean up Buffers
        glDeleteVertexArrays(1, &sc_vertex_array_object);
        glDeleteTextures(1,&sc_map_texture);
        glDeleteProgram(sc_program);
    }

    void render(double curTime){

        glViewport( 0, 0, info.windowWidth, info.windowHeight ); //Set Viewport information

        //if Auto rotate flag is set, update the position of the camera
        if(autoRotate){
            camera.position = vmath::vec3(static_cast<float>(cos(curTime/10.0) * 5.0),
                                          0.0f,
                                          static_cast<float>(sin(curTime/10.0) * 5.0) );
        }

        //recalculate the View matrix for camera
        calcView(camera);

        //Clear output
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        runtime_error_check(1);

        //Draw the skyCube!
        drawSkyCube(curTime);

        runtime_error_check(2);

        /////////////////////////////////////////////////////////////////////////////////
        // This would be where you want to call another program to render your 'stuff' //
        // This could also go into a function for organizational ease                  //
        /////////////////////////////////////////////////////////////////////////////////

        //Set up obj->world transforms for each object (these could be modified for animation)

        // objects[0].world_origin = vmath::vec3(static_cast<float>(cos(curTime)), 0.0f, static_cast<float>(sin(curTime)));


        for(int i = 0; i < objects.size(); i++ ){
            objects[i].obj2world =
                vmath::translate(objects[i].world_origin) *
                vmath::scale(objects[i].scale);
            //render loop, go through each object and render it!
            glUseProgram(rendering_program); //activate the render program
            glBindVertexArray(vertex_array_object); //Select base vao

            //Copy over all the transforms
            glUniformMatrix4fv(transform_ID, 1,GL_FALSE, objects[i].obj2world); //Load in transform for this object
            //TODO::These might only need to be loaded once (for all objects)
            glUniformMatrix4fv(perspec_ID, 1,GL_FALSE, camera.proj_Matrix); //Load camera projection
            glUniformMatrix4fv(toCam_ID, 1,GL_FALSE, camera.view_mat); //Load in view matrix for camera

            //link to object buffer
            glEnableVertexAttribArray(vertex_ID); //Recall the vertex ID
            glBindBuffer(GL_ARRAY_BUFFER,objects[i].vertices_buffer_ID);//Link object buffer to vertex_ID
            glVertexAttribPointer( //Index into the buffer
                    vertex_ID, //Attribute in question
                    4,         //Number of elements per vertex call (vec4)
                    GL_FLOAT,  //Type of element
                    GL_FALSE,  //Normalize? Nope
                    0,         //No stride (steps between indexes)
                    0);       //initial offset

            glDrawArrays( GL_TRIANGLES, 0, objects[i].vertices.size());
        }

        runtime_error_check(4);
    }

    //adapted from 3dmaze library
    std::vector<obj_t> convertMazeToWorld(std::vector<vmath::vec3> maze){
        std::vector<obj_t> result;

        for (unsigned i = 0; i < maze.size(); i++) {
            result.push_back(obj_t());
            load_obj(".\\bin\\media\\cube.obj", result[i].vertices, result[i].uv, result[i].normals, result[i].vertNum);

            //check orientation of wall
            switch ((unsigned)maze[i][2]) {
                case 0:
                    result[i].world_origin = vmath::vec3(maze[i][0] - 0.5f + WallSize / 2.0f, 0.5f, maze[i][1]);
                    result[i].scale = vmath::vec3(WallSize, 1.0f, 1.0f);
                    break;
                case 1:
                    result[i].world_origin = vmath::vec3(maze[i][0], 0.5f, maze[i][1] - 0.5f + WallSize / 2);
                    result[i].scale = vmath::vec3(1.0f, 1.0f, WallSize);
                    break;
                case 2:
                    result[i].world_origin = vmath::vec3(maze[i][0] + 0.5f - WallSize / 2, 0.5f, maze[i][1]);
                    result[i].scale = vmath::vec3(WallSize, 1.0f, 1.0f);
                    break;
                case 3:
                    result[i].world_origin = vmath::vec3(maze[i][0], 0.5f, maze[i][1] + 0.5f - WallSize / 2);
                    result[i].scale = vmath::vec3(1.0f, 1.0f, WallSize);
                    break;
            }
            result[i].scale *= vmath::vec3(0.5f, 1.0f, 0.5f);
        }

        return result;
    }

    void drawSkyCube(double curTime){

        glDepthMask( GL_FALSE ); //Used to force skybox 'into' the back, making sure everything is rendered over it
        glUseProgram( sc_program ); //Select the skycube program
        glUniformMatrix4fv( sc_Perspective, 1, GL_FALSE, camera.proj_Matrix); //Update the projection matrix (if needed)
        glUniformMatrix4fv( sc_Camera, 1, GL_FALSE, camera.view_mat_no_translation); //Update the projection matrix (if needed)
        glActiveTexture( GL_TEXTURE0 ); //Make sure we are using the CUBE_MAP texture we already set up
        glBindTexture( GL_TEXTURE_CUBE_MAP, sc_map_texture ); //Link to the texture
        glBindVertexArray( sc_vertex_array_object ); // Set up the vertex array
        glDrawArrays( GL_TRIANGLES, 0, skycube_vertices.size() ); //Start drawing triangles
        glDepthMask( GL_TRUE ); //Turn depth masking back on

        runtime_error_check();
    }

    void onResize(int w, int h) {
        info.windowWidth = w;
        info.windowHeight = h;
        //Recalculate the projection matrix used by camera
        calcProjection(camera);
    }

    //adapted from https://learnopengl.com/Getting-started/Camera
    void onMouseMove(int x, int y){
        if (first_mouse)
        {
            last_x = x;
            last_y = y;
            first_mouse = false;
        }

        float xoffset = static_cast<float>(x - last_x);
        float yoffset = static_cast<float>(last_y - y);
        last_x = x;
        last_y = y;

        xoffset *= pan_speed;
        yoffset *= pan_speed;

        yaw   += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;
        // pitch = 0.0f; // filter out y so it doesn't look broken ;)
        camera.forward = vmath::normalize(
            vmath::vec3(
                cos(vmath::radians(yaw)) * cos(vmath::radians(pitch)),
                sin(vmath::radians(pitch)),
                sin(vmath::radians(yaw)) * cos(vmath::radians(pitch))
            )
        );
    }

    //before moving check if collision will happen. if so, just stop
    void move(vmath::vec3 direction, short sign){
        //do collision detection
        //iterate through all walls
        vmath::vec3 dest = camera.position + direction;
        float dest_x = dest[0];
        float dest_y = dest[1];
        float dest_z = dest[2];

        bool collides = false;
        float player_width = 0.15f;
        float player_height = 0.5f;

        for(int i = 0; i < objects.size(); i++){

            float min_x = objects[i].min[0] + objects[i].world_origin[0];
            float max_x = objects[i].max[0] + objects[i].world_origin[0];
            float min_y = objects[i].min[1] + objects[i].world_origin[1];
            float max_y = objects[i].max[1] + objects[i].world_origin[1];
            float min_z = objects[i].min[2] + objects[i].world_origin[2];
            float max_z = objects[i].max[2] + objects[i].world_origin[2];

            if(
                ((dest_x) < max_x + player_width) && (dest_x > min_x - player_width) &&
                ((dest_y) < max_y + player_height) && (dest_y > min_y - player_height) &&
                ((dest_z) < max_z + player_width) && (dest_z > min_z - player_width) &&
                camera.collision
            ) { //if x + z collides
                // char buf[150];
                // sprintf(buf, "dest_x = %.3f, dest_y = %.3f, dest_z = %.3f\nmin_x = %.3f, max_x = %.3f, min_y = %.3f, max_y = %.3f, min_z = %.3f, max_z = %.3f",
                //             dest_x, dest_y, dest_z,
                //             min_x, max_x, min_y, max_y, min_z, max_z
                //         );
                // errorBox(buf);
                collides = true;
                break;
            }
        }
        if(!collides){
            camera.position += (direction * sign);
        }
    }

    void onKey(int key, int action) {
        //If something did happen
        if (action) {
            switch (key) { //Select an action
                case 'W': //go toward camera direction
                    // camera.position += camera.advance(move_speed);
                    move(camera.advance(move_speed), 1);
                    break;
                case 'A': //strafe left of camera direction
                    move(camera.strafe(move_speed), -1);
                    break;
                case 'S': //back away from camera direction
                    move(camera.advance(move_speed), -1);
                    break;
                case 'D': //strafe right of camera direction
                    move(camera.strafe(move_speed), 1);
                    break;
                case GLFW_KEY_UP:
                    camera.position[1]++;
                    break;
                case GLFW_KEY_DOWN:
                    camera.position[1]--;
                    break;
                case 'C':
                    autoRotate = !autoRotate;
                    break;
                case 'Z': //Reset
                    camera.position = vmath::vec3(0.0f, 0.0f, 5.0f); //Starting camera at position (0,0,5)
                    // camera.focus = vmath::vec3(0.0f, 0.0f, 0.0f); //Camera is looking in the +y direction
                    break;
                case 'N':
                    camera.collision = !camera.collision;
                    break;
                case 'X': //Info
                    char buf[50];
                    sprintf(buf, "Current Camera Pos:(%.3f,%.3f,%.3f)\nForward:(%.3f,%.3f,%.3f)\nUp:(%.3f,%.3f,%.3f)\nPitch:%.3f Yaw:%.3f\nCollision Enabled:%d",
                                       camera.position[0],camera.position[1],camera.position[2],
                                       camera.forward[0],camera.forward[1],camera.forward[2],
                                       camera.up[0], camera.up[1], camera.up[2],
                                       pitch, yaw, camera.collision
                                    );
                    MessageBoxA(NULL, buf, "Diagnostic Printout", MB_OK);
                    break;
                case 'M': //Info
                    char buf2[200];
                    sprintf(buf2, "Current Not translation mat \n[ %.3f, %.3f, %.3f, %.3f] \n[ %.3f, %.3f, %.3f, %.3f] \n[ %.3f, %.3f, %.3f, %.3f] \n[ %.3f, %.3f, %.3f, %.3f]",
                                       camera.view_mat_no_translation[0][0],camera.view_mat_no_translation[0][1],camera.view_mat_no_translation[0][2],camera.view_mat_no_translation[0][3],
                                       camera.view_mat_no_translation[1][0],camera.view_mat_no_translation[1][1],camera.view_mat_no_translation[1][2],camera.view_mat_no_translation[1][3],
                                       camera.view_mat_no_translation[2][0],camera.view_mat_no_translation[2][1],camera.view_mat_no_translation[2][2],camera.view_mat_no_translation[2][3],
                                       camera.view_mat_no_translation[3][0],camera.view_mat_no_translation[3][1],camera.view_mat_no_translation[3][2],camera.view_mat_no_translation[3][3]);
                    MessageBoxA(NULL, buf2, "Diagnostic Printout", MB_OK);
                    break;
            }
        }

    }

    void errorBoxString(std::string str){
        char buf[str.length()];
        strcpy(buf, str.c_str());
        MessageBoxA(NULL, buf, "Diagnostic Printout", MB_OK);
    }

    void errorBox(const char* msg){
        char buf[50];
        strcpy(buf, msg);
        // sprintf(buf, "Current Camera Pos:(%.3f,%.3f,%.3f)\nForward:(%.3f,%.3f,%.3f)\nUp:(%.3f,%.3f,%.3f)\nPitch:%.3f Yaw:%.3f",
        //                     camera.position[0],camera.position[1],camera.position[2],
        //                     camera.forward[0],camera.forward[1],camera.forward[2],
        //                     camera.up[0], camera.up[1], camera.up[2],
        //                     pitch, yaw
        //                 );
        MessageBoxA(NULL, buf, "Diagnostic Printout", MB_OK);
    }

    void runtime_error_check(GLuint tracker = 0)
    {
        GLenum err = glGetError();

        if (err) {
            char buf[50];
            sprintf(buf, "Error(%d) = %x", tracker, err);
            MessageBoxA(NULL, buf, "Error running program!", MB_OK);
        }
    }

    void compiler_error_check(GLuint shader)
    {
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

            if(maxLength == 0){
                char buf[50];
                sprintf(buf, "Possibly shaders not found!", maxLength);
                MessageBoxA(NULL, buf, "Error in shader compilation", MB_OK);
            } else {
                // Provide the infolog in whatever manor you deem best.
                MessageBoxA(NULL, &errorLog[0], "Error in shader compilation", MB_OK);
            }

            // Exit with failure.
            glDeleteShader(shader); // Don't leak the shader.
        }
    }
};


DECLARE_MAIN(test_app);
