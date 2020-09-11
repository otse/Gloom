#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "types.h"

// https://learnopengl.com/Getting-started/Textures

struct Texture
{
    friend Texture *GetProduceTexture(const char *);

    GLuint tid;

    int size;
    const unsigned char *buf = nullptr;

    const string path;

protected:
    Texture(const string &);
    ~Texture();

    void Load();
};

#endif