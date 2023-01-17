#include "compute_shader.h"
#include "gl_deps.h"
#include "read_file.h"
#include <iostream>

ComputeShader::ComputeShader(const char* cs){

    int success;
    char infoLog[512];
    std::string shader_string;

    //compute shader

    unsigned int compute_shader_code;

    shader_string = readfile(cs); 
    const char* compute_shader_content = shader_string.c_str();

    compute_shader_code = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader_code, 1, &compute_shader_content, NULL);
    glCompileShader(compute_shader_code);
    // check for shader compile errors
    glGetShaderiv(compute_shader_code, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(compute_shader_code, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // shader Program
    id = glCreateProgram();
    glAttachShader(id, compute_shader_code);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR::COMPUTE_SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(compute_shader_code);

}


ComputeShader::~ComputeShader(){
    glDeleteProgram(id);
}



void ComputeShader::bind(){
    glUseProgram(id);
}





void ComputeShader::dispatch(unsigned int x, unsigned int y, unsigned int z){
    glDispatchCompute(x, y, z);
    // make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}