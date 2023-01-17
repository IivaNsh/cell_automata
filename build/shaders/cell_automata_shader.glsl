#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgInput;
layout(rgba32f, binding = 1) uniform image2D imgOutput;



void main(){
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    
    float self_v = imageLoad(imgInput, texelCoord).x; 


    float n1 = imageLoad(imgInput, texelCoord + ivec2(-1,-1)).x;
    float n2 = imageLoad(imgInput, texelCoord + ivec2(-1, 0)).x;
    float n3 = imageLoad(imgInput, texelCoord + ivec2(-1, 1)).x;
    float n4 = imageLoad(imgInput, texelCoord + ivec2( 0, 1)).x;
    float n5 = imageLoad(imgInput, texelCoord + ivec2( 1, 1)).x;
    float n6 = imageLoad(imgInput, texelCoord + ivec2( 1, 0)).x;
    float n7 = imageLoad(imgInput, texelCoord + ivec2( 1,-1)).x;
    float n8 = imageLoad(imgInput, texelCoord + ivec2( 0,-1)).x;


    float sum_v = n1 + n2 + n3 + n4 + n5 + n6 + n7 + n8;
    
    if(self_v>0){
        if(sum_v < 2 || sum_v > 3){
            self_v = 0;
        }
    }
    else{
        if(sum_v == 3){
            self_v = 1;
        }
    }
    imageStore(imgOutput, texelCoord, vec4(self_v, self_v, self_v, 1));
}
