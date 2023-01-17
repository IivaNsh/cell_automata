#pragma once 


class Texture{
public:

    unsigned int id;

    Texture(unsigned int resolution);
    ~Texture();

    void display_bind(unsigned int location);
    void compute_bind(unsigned int location);
};