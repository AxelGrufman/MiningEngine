#pragma once

#include <glad/gl.h>
#include <string>

class Shader
{
public:
    Shader() = default;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool load(const std::string& vertexPath, const std::string& fragmentPath);

    void use() const;

    GLint getUniformLocation(const char* name) const;

    void destroy();

private:
    GLuint programID = 0;
};