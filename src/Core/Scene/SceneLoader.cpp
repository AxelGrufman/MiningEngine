#include "SceneLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>

static bool parseVec3(const std::string& text, glm::vec3& result)
{
    std::istringstream stream(text);
    char comma1, comma2;

    if (!(stream >> result.x >> comma1 >> result.y >> comma2 >> result.z))
        return false;

    if (comma1 != ',' || comma2 != ',')
        return false;

    stream >> std::ws;
    return stream.eof();
}

std::optional<SceneData> SceneLoader::load(const std::string& filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "[SceneLoader] Failed to open: " << filePath << '\n';
        return std::nullopt;
    }

    SceneData data;
    bool hasPlayer1Start = false;
    bool hasPlayer2Start = false;
    bool hasGoal = false;
    
    std::string line;
    std::string currentSection;
    
    auto trim = [](const std::string& text) -> std::string
    {
        size_t first = text.find_first_not_of(" \t\r\n");

        if (first == std::string::npos)
            return "";

        size_t last = text.find_last_not_of(" \t\r\n");

        return text.substr(first, last - first + 1);
    };

    while (std::getline(file, line))
    {
        line = trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        if (line.front() == '[' && line.back() == ']')
        {
            currentSection = line.substr(1, line.size() - 2);

            std::cout << "[SceneLoader] Section: " << currentSection << '\n';

            if (currentSection == "object")
            {
                data.objects.emplace_back();
            }

            continue;
        }
        
        size_t separator = line.find('=');

        if (separator == std::string::npos)
        {
            std::cerr << "[SceneLoader] Invalid property: " << line << '\n';
            continue;
        }

        std::string key = trim(line.substr(0, separator));
        std::string value = trim(line.substr(separator + 1));

        if (currentSection == "scene")
        {
            if (key == "name")
            {
                data.name = value; 
            }
        }
        else if (currentSection == "player1")
        {
            if (key == "position")
            {
                if (!parseVec3(value, data.player1Start))
                {
                    std::cerr << "[SceneLoader] Invalid Player1 position\n";
                    return std::nullopt;
                }
                hasPlayer1Start = true;
            }
            else if (key == "rotation")
            {
                if (!parseVec3(value, data.player1Rotation))
                {
                    std::cerr << "[SceneLoader] Invalid Player1 rotation\n";
                    return std::nullopt;
                }
            }
        }
        else if (currentSection == "player2")
        {
            if (key == "position")
            {
                if (!parseVec3(value, data.player2Start))
                {
                    std::cerr << "[SceneLoader] Invalid Player2 position\n";
                    return std::nullopt;
                }
                hasPlayer2Start = true;
            }
            else if (key == "rotation")
            {
                if (!parseVec3(value, data.player2Rotation))
                {
                    std::cerr << "[SceneLoader] Invalid Player2 rotation\n";
                    return std::nullopt;
                }
            }
        }
        else if (currentSection == "goal")
        {
            if (key == "position")
            {
                if (!parseVec3(value, data.goalPosition))
                {
                    std::cerr << "[SceneLoader] Invalid goal position\n";
                    return std::nullopt;
                }
                hasGoal = true;
            }
        }
        else if (currentSection == "object")
        {
            if (data.objects.empty())
                return std::nullopt;

            SceneObjectData& object = data.objects.back();

            if (key == "name")
            {
                object.name = value;
            }
            else if (key == "mesh")
            {
                object.meshPath = value;
            }
            else if (key == "texture")
            {
                object.texturePath = value;
            }
            else if (key == "position")
            {
                if (!parseVec3(value, object.position))
                {
                    std::cerr << "[SceneLoader] Invalid position for object: " << object.name << '\n';
                    return std::nullopt;
                }
            }
            else if (key == "rotation")
            {
                if (!parseVec3(value, object.rotation))
                {
                    std::cerr << "[SceneLoader] Invalid rotation for object: " << object.name << '\n';
                    return std::nullopt;
                }
            }
            else if (key == "scale")
            {
                if (!parseVec3(value, object.scale))
                {
                    std::cerr << "[SceneLoader] Invalid scale for object: " << object.name << '\n';
                    return std::nullopt;
                }
            }
        }
        
    }

    if (data.name.empty() || !hasPlayer1Start || !hasPlayer2Start || !hasGoal)
    {
        std::cerr << "[SceneLoader] Missing required scene data!\n";
        return std::nullopt;
    }

    return data;
}