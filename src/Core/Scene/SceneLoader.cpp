#include "SceneLoader.h"

#include <fstream>
#include <iostream>

std::optional<SceneData> SceneLoader::load(const std::string& filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "[SceneLoader] Failed to open: " << filePath << '\n';
        return std::nullopt;
    }

    SceneData data;
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

            if (currentSection == "entity")
            {
                data.entities.emplace_back();
            }

            continue;
        }
        
        if (currentSection == "entity" && line.front() == '{' && line.back() == '}')
        {
            std::string componentType = line.substr(1, line.size() - 2);
            auto& entity = data.entities.back();
            entity.components.push_back({componentType, {}});

            continue;
        }
        
        size_t separator = line.find('=');

        if (separator == std::string::npos)
        {
           // std::cerr << "[SceneLoader] Invalid property: " << line << '\n';
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
        else if (currentSection == "entity")
        {
            auto& entity = data.entities.back();

            if (entity.components.empty())
            {
                if (key == "name")
                {
                    entity.name = value;
                }
            }
            else
            {
                entity.components.back().properties[key] = value;
            }
        }
        
    }

    if (data.name.empty())
    {
        std::cerr << "[SceneLoader] Missing required scene data!\n";
        return std::nullopt;
    }

    return data;
}