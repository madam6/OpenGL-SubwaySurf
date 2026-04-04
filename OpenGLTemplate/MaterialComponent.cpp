#include "MaterialComponent.h"

void MaterialComponent::Apply(const PropertyMap& props)
{
	auto itPos = props.find("ma");
	if (itPos != props.end()) { Ma = ParseVec3(itPos->second);

	auto itScale = props.find("md");
	if (itScale != props.end()) Md = ParseVec3(itScale->second);

	auto itScale = props.find("ms");
	if (itScale != props.end()) Ms = ParseVec3(itScale->second);

	auto itShinines = props.find("shiny");
	if (itShinines != props.end()) shininess = std::stof(itShinines->second);
}
