#version 450 core                                                 

in vec4 texture_coordinates; // From vertex shader, this will map through the sampler cube   
uniform samplerCube cube_texture;                                                            
out vec4 color;                                                   
                                                                  
void main(void)                                                   
{
    // Use cube_texture to interpolate the correct texture color
    color = texture(cube_texture, vec3(texture_coordinates));
}                               
