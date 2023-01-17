#pragma once

class Shader{
public:

    unsigned int id;

    Shader(const char* vs, const char* fs);
    ~Shader();

    void bind();

    void setUniformTexure( const char* name, unsigned int location, unsigned int texture_id);

};