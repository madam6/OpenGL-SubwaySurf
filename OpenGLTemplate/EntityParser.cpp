#include "EntityParser.h"
#include "Common.h"
#include "ComponentRegistry.h"
#include <sstream>

PropertyMap ParseArgs(const std::string& args)
{
    PropertyMap result;

    std::istringstream ss(args);
    std::string token;

    while (ss >> token)
    {
        auto pos = token.find(':');
        if (pos == std::string::npos) continue;

        std::string key = token.substr(0, pos);
        std::string value = token.substr(pos + 1);

        result[key] = value;
    }

    return result;
}

std::shared_ptr<Entity> EntityParser::Create(const std::vector<std::string>& lines)
{
    auto entity = std::make_shared<Entity>();

    for (const auto& line : lines)
    {
        if (line.empty()) continue;

        if (line.find("EntityName:") == 0)
        {
            entity->m_Name = line.substr(11);
            continue;
        }

        std::istringstream ss(line);
        std::string componentName;
        ss >> componentName;
        if (componentName.empty()) continue;

        std::string args;
        std::getline(ss, args);
        if (!args.empty() && args[0] == ' ')
        {
            args.erase(0, 1);
        }
        PropertyMap props = ParseArgs(args);

        auto* factory = ComponentRegistry::Instance().Get(componentName);
        if (factory)
        {
            (*factory)(*entity, props);
        }
        else
        {
            std::cerr << "Unknown component: " << componentName << std::endl;
        }
    }

    return entity;
}