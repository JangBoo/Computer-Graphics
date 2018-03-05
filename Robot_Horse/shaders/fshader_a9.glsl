#version 330 core

in  vec4 color;
out vec4 fColor;

uniform vec4 horse_color;

void main() 
{ 
    fColor = horse_color;
} 

