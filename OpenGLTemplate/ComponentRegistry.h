#include "Common.h"
#include "Entity.h"

#pragma once
class ComponentRegistry
{
public:
    using Factory = std::function<void(Entity&, const PropertyMap&)>;

    static ComponentRegistry& Instance()
    {
        static ComponentRegistry instance;
        return instance;
    }

    void Register(const std::string& name, Factory factory)
    {
        m_Factories[name] = factory;
    }

    Factory* Get(const std::string& name)
    {
        auto it = m_Factories.find(name);
        if (it != m_Factories.end())
        {
            return &it->second;
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, Factory> m_Factories;
};