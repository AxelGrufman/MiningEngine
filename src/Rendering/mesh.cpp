#include "mesh.h"
#include "vertex.h"

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <cstddef>

bool Mesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    destroy();
    indexCount = static_cast<GLsizei>(indices.size());
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    if (VAO == 0 || VBO == 0 || EBO == 0)
    {
        std::cerr << "Failed to setup mesh!\n";
        destroy();
        return false;
    }
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);    
    return true;
}

void Mesh::bind() const
{
    glBindVertexArray(VAO);
}

void Mesh::destroy()
{
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    
    if (EBO != 0)
    {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }

    indexCount = 0;
}

void Mesh::drawIndexed() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

/* old code to genrate cube, sphere, triangel, pyramide

bool Mesh::createPyramid()
{
    destroy();

    float vertices[] = {
                       
        -0.5, -0.5, -0.5,        1.0f, 0.0f, 0.0f, // 0
         0.5, -0.5, -0.5,        1.0f, 0.0f, 0.0f, // 1
         0.5, -0.5,  0.5,        1.0f, 0.0f, 0.0f, // 2
        -0.5, -0.5,  0.5,        1.0f, 0.0f, 0.0f, // 3
         0.0,  0.5,  0.0,        1.0f, 0.0f, 0.0f  // 4 (top)
   };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 1,
        1, 4, 2,
        2, 4, 3
    };
    
    indexCount = static_cast<GLsizei>(sizeof(indices) / sizeof(indices[0]));
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    if (VAO == 0 || VBO == 0 || EBO == 0)
    {
        std::cerr << "Failed to create pyramid mesh!\n";
        destroy();
        return false;
    }
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    return true;
}

bool Mesh::createCube()
{
    destroy();
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, //0
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, //1
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, //2
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, //3
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f, //4
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f, //5
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, //6
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, //7
        
    };
    
    unsigned int indices[] = {
        0, 1, 2, 
        0, 2, 3, 
        0, 3, 4, 
        3, 7, 4, 
        0, 4, 1, 
        1, 4, 5,
        1, 5, 2,
        2, 5, 6,
        2, 6, 3,
        3, 6, 7,
        4, 7, 6,
        4, 6, 5,
        
    };
    
    indexCount = static_cast<GLsizei>(sizeof(indices) / sizeof(indices[0]));
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    if (VAO == 0 || VBO == 0 || EBO == 0)
    {
        std::cerr << "Failed to create cube mesh!\n";
        destroy();
        return false;
    }
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    return true;
}

bool Mesh::createSphere()
{
    destroy();
    
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const int rings = 55;
    const int slices = 50;

    float yStep = 2.0f / (rings - 1);
    float yPos = 1.0f;
    
    for (int i = 0; i < rings; i++)
    {
        int iteration_slices = slices;
        if (i == 0 || i == rings - 1)
        {
            iteration_slices = 1;
        }
        
        for (int j = 0; j < iteration_slices; j++)
        {
            float safeY = glm::clamp(yPos, -1.0f, 1.0f);
            float ringRadius = glm::sin(glm::acos(safeY));
            float angle = glm::radians(360.0f * j / slices);
            
            float x = glm::cos(angle) * ringRadius;
            float y = yPos;
            float z = glm::sin(angle) * ringRadius;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
        yPos -= yStep;
    }
    
    unsigned int topIndex = 0;
    unsigned int bottomIndex = vertices.size() /6 -1;
    unsigned int lastRingStart = 1 + (rings - 3) * slices;
    
    for ( int i = 0; i < rings - 1; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            if (i == 0)
            {
                unsigned int current = j + 1;
                unsigned int next = ((j + 1) % slices) + 1;
                
                indices.push_back(topIndex);
                indices.push_back(next);
                indices.push_back(current);
            }
            else if(i == rings - 2)
            {
                unsigned int current = lastRingStart + j;
                unsigned int next = lastRingStart + ((j + 1) % slices);
                
                indices.push_back(bottomIndex);
                indices.push_back(current);
                indices.push_back(next);
            }
            else
            {
                unsigned int upperRingStart = 1 + (i - 1) * slices;
                unsigned int lowerRingStart = 1 + i * slices;
                
                unsigned int A = upperRingStart + j;
                unsigned int B = upperRingStart + ((j + 1) % slices);

                unsigned int C = lowerRingStart + j;
                unsigned int D = lowerRingStart + ((j + 1) % slices);
                
                indices.push_back(A);
                indices.push_back(B);
                indices.push_back(C);

                indices.push_back(B);
                indices.push_back(D);
                indices.push_back(C);
                
            }
        }
    }
    
    indexCount = static_cast<GLsizei>(indices.size());
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    if (VAO == 0 || VBO == 0 || EBO == 0)
    {
        std::cerr << "Failed to create sphere mesh!\n";
        destroy();
        return false;
    }
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(float),vertices.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(unsigned int),indices.data(),GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    return true;
}

bool Mesh::createTriangle()
{
    destroy();
    
     float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

	
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    if (VAO == 0 || VBO == 0)
    {
        std::cerr << "Failed to create triangle mesh!\n";
        destroy();
        return false;
    }
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    return true;
}

/*
