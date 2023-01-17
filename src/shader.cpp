#include "shader.h"
#include "gl_deps.h"
#include "read_file.h"
#include <iostream>

Shader::Shader(const char* vs, const char* fs){



    int success;
    char infoLog[512];
    std::string shader_string;

    // vertex shader
    //
    //
    //open shader files
    shader_string = readfile(vs); 
    const char* vertex_shader_content = shader_string.c_str();
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_content, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    // fragment shader

    //open shader files
    shader_string = readfile(fs); 
    const char* fragment_shader_content = shader_string.c_str();
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_content, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    // check for linking errors
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


}


Shader::~Shader(){
    glDeleteProgram(id);
}

void Shader::bind(){
    glUseProgram(id);
}

void Shader::setUniformTexure( const char* name, unsigned int location, unsigned int texture_id)
{
    glUniform1i(glGetUniformLocation(id, name), location); 
}


