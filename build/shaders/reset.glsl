#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

void main(){
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    imageStore(imgOutput, texelCoord, vec4(0,0,0,1));
}
