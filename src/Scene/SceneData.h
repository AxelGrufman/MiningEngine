#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct SceneObjectData
{
    std::string name;
    std::string meshPath;
    std::string texturePath = "none";

    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
};

struct SceneData
{
    std::string name;

    glm::vec3 player1Start{0.0f};
    glm::vec3 player1Rotation{0.0f};

    glm::vec3 player2Start{0.0f};
    glm::vec3 player2Rotation{0.0f};

    glm::vec3 goalPosition{0.0f};

    std::vector<SceneObjectData> objects;
};