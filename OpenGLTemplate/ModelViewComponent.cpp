#include "ModelViewComponent.h"
#include "ComponentRegistry.h"
#include "Entity.h"

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("ModelViewComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<ModelViewComponent>();
                    c.Apply(props);
                });

            return true;
        }();
}
void ModelViewComponent::Init()
{
}

void ModelViewComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
}

void ModelViewComponent::Apply(const PropertyMap& props)
{
    auto itPos = props.find("position");
    if (itPos != props.end()) position = ParseVec3(itPos->second);

    auto itScale = props.find("scale");
    if (itScale != props.end()) scale = ParseVec3(itScale->second);
}

void ModelViewComponent::Update(float dt)
{
}
