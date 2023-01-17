#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgInput;
layout(rgba32f, binding = 1) uniform image2D imgOutput;

uniform ivec2 putPos;


void main(){
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    imageStore(imgOutput, texelCoord, imageLoad(imgInput, texelCoord));

}
