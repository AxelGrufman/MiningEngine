#pragma once

#include <string>
#include <glad/gl.h>

class Texture
{
public:
    bool load(const std::string& filepath);
    void destroy();
    void bind() const;
    
private:
    GLuint textureID = 0;
};