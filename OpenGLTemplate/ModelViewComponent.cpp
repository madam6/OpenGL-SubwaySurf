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

    auto itRot = props.find("rotation");
    if (itRot != props.end())
    {
        glm::vec3 euler = ParseVec3(itRot->second);
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(euler.x), glm::vec3(1, 0, 0));
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(euler.y), glm::vec3(0, 1, 0));
        glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(euler.z), glm::vec3(0, 0, 1));
        orientation = rotZ * rotY * rotX;
    }
}

void ModelViewComponent::Update(float dt)
{
}
