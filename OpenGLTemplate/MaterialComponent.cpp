#include "MaterialComponent.h"
#include "ComponentRegistry.h"
#include "Entity.h"

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("MaterialComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<MaterialComponent>();
                    c.Apply(props);
                });

            return true;
        }();
}

void MaterialComponent::Init()
{
}

void MaterialComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
}

void MaterialComponent::Update(float dt)
{
}

void MaterialComponent::Apply(const PropertyMap& props)
{
    auto itPos = props.find("ma");
    if (itPos != props.end()) Ma = ParseVec3(itPos->second);

    auto itScale = props.find("md");
    if (itScale != props.end()) Md = ParseVec3(itScale->second);

    auto itMs = props.find("ms");
    if (itMs != props.end()) Ms = ParseVec3(itMs->second);

    auto itShininess = props.find("shiny");
    if (itShininess != props.end()) shininess = std::stof(itShininess->second);
}