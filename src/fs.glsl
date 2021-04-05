#version 450 core                                                 

in vec4 vs_color;                                                                  
out vec4 color;                                                   
                                                                  
void main(void)                                                   
{                     
    // color = vec4(vec3((gl_FragCoord.z)), 1.0); //This will shade things based on the z 'depth'
    color = vs_color;                             
}                                                                 