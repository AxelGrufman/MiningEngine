#pragma once

#include <glad/gl.h>

class Mesh
{
public:
    Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    bool createTriangle();
    bool createPyramid();
    bool createCube();
    bool createSphere();
    void bind() const;
    void destroy();
    void drawIndexed() const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    
    GLsizei indexCount  = 0;
};