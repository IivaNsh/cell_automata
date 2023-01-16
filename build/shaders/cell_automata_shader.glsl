#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

void main(){
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    
    float self_v = imageLoad(imgOutput, texelCoord).x; 
    float left_v = imageLoad(imgOutput, texelCoord + ivec2(-1, 0)).x;
    float top_v = imageLoad(imgOutput, texelCoord + ivec2( 0, 1)).x;
    float right_v = imageLoad(imgOutput, texelCoord + ivec2( 1, 0)).x;
    float bottom_v = imageLoad(imgOutput, texelCoord + ivec2( 0,-1)).x;
    
    float sum_v = left_v + top_v + right_v + bottom_v;
    
    if(self_v==1){
        if(sum_v >= 3){
            self_v = 0;
        }
    }
    else{
        if(sum_v == 2){
            self_v = 1;
        }
    }
    imageStore(imgOutput, texelCoord, vec4(self_v, self_v, self_v, 1));
}
