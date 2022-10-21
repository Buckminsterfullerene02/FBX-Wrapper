#include "JsonDeserializer.h"

FStaticMeshStruct JsonDeserializer::Deserialize(std::string Json) {
    nlohmann::json JsonObj = nlohmann::json::parse(Json);
    FStaticMeshStruct StaticMeshStruct{};

    StaticMeshStruct.Name = JsonObj["Name"];

    for (int i = 0; i < JsonObj["StaticMaterials"].size(); ++i) {
        FStaticMaterial StaticMaterial{};
        StaticMaterial.MaterialSlotName = JsonObj["StaticMaterials"][i]["MaterialSlotName"];
        StaticMeshStruct.StaticMaterials.push_back(StaticMaterial);
    }

    for (int i = 0; i < JsonObj["RenderData"]["LODs"].size(); ++i) {
        FStaticMeshLODResources StaticMeshLODResources{};

        StaticMeshLODResources.Sections = std::vector<FStaticMeshSection>(JsonObj["RenderData"]["LODs"][i]["Sections"].size());



        StaticMeshLODResources.IndexBuffer.Indices16 = std::vector<uint16_t>(JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices16"].size());
        StaticMeshLODResources.IndexBuffer.Indices32 = std::vector<uint32_t>(JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices32"].size());

        StaticMeshStruct.RenderData.LODs.push_back(StaticMeshLODResources);
    }

    return StaticMeshStruct;
}