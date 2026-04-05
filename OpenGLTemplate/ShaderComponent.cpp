#include "ShaderComponent.h"
#include "ComponentRegistry.h"
#include "Entity.h"

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("ShaderComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<ShaderComponent>();
                    c.Apply(props);
                });

            return true;
        }();
}
void ShaderComponent::Init()
{
}
void ShaderComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
}
void ShaderComponent::Update(float dt)
{
}
void ShaderComponent::Apply(const PropertyMap& props)
{
	auto it = props.find("shader");
	if (it != props.end())
	{
		shaderName = it->second;
	}
}
