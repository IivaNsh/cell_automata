#include "texture.h"
#include "gl_deps.h"

Texture::Texture(unsigned int resolution){

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL);
    //glGenerateMipmap(texture);

}

Texture::~Texture(){
    glDeleteTextures(1, &id);
}

void Texture::compute_bind(unsigned int location)
{
    glBindImageTexture(location, id , 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
     
}


void Texture::display_bind(unsigned int location){
    
    glActiveTexture(GL_TEXTURE0 + location);   

    glBindTexture(GL_TEXTURE_2D, id);
}