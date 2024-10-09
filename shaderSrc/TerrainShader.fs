#version 330 core

in vec3 gl_Color;

out vec4 FragColor;

void main()
{
   
    FragColor = vec4(gl_Color, 1);    
 
}