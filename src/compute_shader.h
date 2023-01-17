#pragma once


class ComputeShader{
public:

    unsigned int id;

    ComputeShader(const char* cs);
    ~ComputeShader();

    void bind();


    void dispatch(unsigned int x, unsigned int y, unsigned int z);

};