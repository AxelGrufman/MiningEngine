#pragma once

#include <vector>
#include <glad/gl.h>

#include "vertex.h"


class Mesh
{
public:
    Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    bool setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    void bind() const;
    void drawIndexed() const;
    void destroy();

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    GLsizei indexCount = 0;
};
