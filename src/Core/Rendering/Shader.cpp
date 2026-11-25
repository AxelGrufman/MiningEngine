#include "Shader.h"

#include <fstream>
#include <iostream>

namespace
{
    bool loadShaderSource(const std::string& path, std::string& output)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Failed to open shader: " << path << '\n';
            return false;
        }

        output.clear();

        std::string line;

        while (std::getline(file, line))
        {
            output += line;
            output += '\n';
        }

        return true;
    }

    GLuint compileShader(GLenum shaderType, const std::string& shaderSource)
    {
        GLuint shader = glCreateShader(shaderType);
        const char* source = shaderSource.c_str();
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        GLint compileStatus = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            std::cerr << "Shader compilation failed!\n";
            GLchar infoLog[512] = {};
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << infoLog << '\n';
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader)
    {
        GLuint shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        glLinkProgram(shaderProgram);
	
        GLint linkStatus = GL_FALSE;
	
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE)
        {
            std::cerr << "Program linking failed!\n";
            GLchar infoLog[512] = {};
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << infoLog << '\n';
            glDeleteProgram(shaderProgram);
            return 0;
        }
        return shaderProgram;
    }
}

void Shader::use() const
{
    glUseProgram(programID);
}

GLint Shader::getUniformLocation(const char* name) const
{
    return glGetUniformLocation(programID, name);
}

void Shader::destroy()
{
    if (programID != 0)
    {
        glDeleteProgram(programID);
        programID = 0;
    }
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath)
{
    destroy();

    std::string vertexSource;
    std::string fragmentSource;

    if (!loadShaderSource(vertexPath,vertexSource) || !loadShaderSource(fragmentPath,fragmentSource))
    {
        return false;
    }

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);

    if (vertexShader == 0)
        return false;

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return false;
    }

    GLuint newProgram = createShaderProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (newProgram == 0)
        return false;

    programID = newProgram;

    return true;
}

