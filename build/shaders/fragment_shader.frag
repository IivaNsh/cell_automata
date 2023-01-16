#version 430 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D tex;

uniform vec3 color;

void main()
{   
    FragColor = vec4(texture(tex, texCoords).xyz * color, 1);
    //FragColor = vec4(1,0,0,1);
}
