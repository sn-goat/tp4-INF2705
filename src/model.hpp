#pragma once

#include <glbinding/gl/gl.h>

using namespace gl;

class Model
{
public:
    void load(const char* path);
    void load(float* vertices, size_t verticesSize, unsigned int* elements, size_t elementsSize);
    
    ~Model();
    
    void draw();

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei count_ = 0;
};