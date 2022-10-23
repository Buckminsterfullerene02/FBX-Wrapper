#include <iostream>
#include "JsonDeserializer.h"

FStaticMeshStruct JsonDeserializer::DeserializeSM(std::string Json) {
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

        // Sections
        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["Sections"].size(); ++j) {
            FStaticMeshSection StaticMeshSection{};
            StaticMeshSection.MaterialIndex = JsonObj["RenderData"]["LODs"][i]["Sections"][j]["MaterialIndex"];
            StaticMeshSection.FirstIndex = JsonObj["RenderData"]["LODs"][i]["Sections"][j]["FirstIndex"];
            StaticMeshSection.NumTriangles = JsonObj["RenderData"]["LODs"][i]["Sections"][j]["NumTriangles"];
            StaticMeshSection.MinVertexIndex = JsonObj["RenderData"]["LODs"][i]["Sections"][j]["MinVertexIndex"];
            StaticMeshSection.MaxVertexIndex = JsonObj["RenderData"]["LODs"][i]["Sections"][j]["MaxVertexIndex"];
            StaticMeshSection.bCastShadow = JsonObj["RenderData"]["LODs"][i]["Sections"][j]["bCastShadow"];
            StaticMeshSection.bEnableCollision = JsonObj["RenderData"]["LODs"][i]["Sections"][j]["bEnableCollision"];
            StaticMeshSection.bVisibleInRayTracing = JsonObj["RenderData"]["LODs"][i]["Sections"][j]["bVisibleInRayTracing"];
            StaticMeshLODResources.Sections.emplace_back(StaticMeshSection);
        }

        StaticMeshLODResources.PositionVertexBuffer.NumVertices = JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["NumVertices"];
        StaticMeshLODResources.PositionVertexBuffer.Stride = JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Stride"];

        // Verts
        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Verts"].size(); ++j) {
            FVector FVector{};
            FVector.X = UZ(JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Verts"][j]["X"]);
            FVector.Y = UZ(JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Verts"][j]["Y"]);
            FVector.Z = UZ(JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Verts"][j]["Z"]);
            StaticMeshLODResources.PositionVertexBuffer.Verts.emplace_back(FVector);
        }

        StaticMeshLODResources.VertexBuffer.NumTexCoords = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["NumTexCoords"];
        StaticMeshLODResources.VertexBuffer.Strides = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["Strides"];
        StaticMeshLODResources.VertexBuffer.NumVertices = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["NumVertices"];
        StaticMeshLODResources.VertexBuffer.UseFullPrecisionUVs = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UseFullPrecisionUVs"];
        StaticMeshLODResources.VertexBuffer.UseHighPrecisionTangentBasis = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UseHighPrecisionTangentBasis"];

        // UV
        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"].size(); ++j) {
            FStaticMeshUVItem StaticMeshUVItem{};

            // Vertex tangents
            StaticMeshUVItem.VertexTangentX.VertexTangent.X = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][0]["X"]);
            StaticMeshUVItem.VertexTangentX.VertexTangent.Y = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][0]["Y"]);
            StaticMeshUVItem.VertexTangentX.VertexTangent.Z = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][0]["Z"]);
            StaticMeshUVItem.VertexTangentX.VertexTangent.W = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][0]["W"]);

            StaticMeshUVItem.VertexTangentY.VertexTangent.X = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][1]["X"]);
            StaticMeshUVItem.VertexTangentY.VertexTangent.Y = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][1]["Y"]);
            StaticMeshUVItem.VertexTangentY.VertexTangent.Z = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][1]["Z"]);
            StaticMeshUVItem.VertexTangentY.VertexTangent.W = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][1]["W"]);

            StaticMeshUVItem.VertexTangentZ.VertexTangent.X = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][2]["X"]);
            StaticMeshUVItem.VertexTangentZ.VertexTangent.Y = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][2]["Y"]);
            StaticMeshUVItem.VertexTangentZ.VertexTangent.Z = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][2]["Z"]);
            StaticMeshUVItem.VertexTangentZ.VertexTangent.W = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][2]["W"]);

            FMeshUVFloat UVFloat{};
            UVFloat.UV.X = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["UV"][0]["U"]);
            UVFloat.UV.Y = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["UV"][0]["V"]);
            StaticMeshUVItem.UV.emplace_back(UVFloat);
            // Sometimes there is no second UV set
            if (JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["UV"].size() > 1) {
                UVFloat.UV.X = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["UV"][1]["U"]);
                UVFloat.UV.Y = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["UV"][1]["V"]);
                StaticMeshUVItem.UV.emplace_back(UVFloat);
            }
            StaticMeshUVItem.UV.emplace_back(UVFloat);

            StaticMeshLODResources.VertexBuffer.UV.emplace_back(StaticMeshUVItem);
        }

        StaticMeshLODResources.IndexBuffer.Indices16 = std::vector<uint16_t>(JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices16"].size());
        StaticMeshLODResources.IndexBuffer.Indices32 = std::vector<uint32_t>(JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices32"].size());

        StaticMeshLODResources.IndexBuffer.bIs32Bit = StaticMeshLODResources.IndexBuffer.Indices16.empty();

        StaticMeshStruct.RenderData.LODs.push_back(StaticMeshLODResources);
    }

    std::cout << "finished parsing" << std::endl;
    return StaticMeshStruct;
}

FSkeletonStruct JsonDeserializer::DeserializeSK(std::string Json) {
    return FSkeletonStruct();
}

FSkeletalMeshStruct JsonDeserializer::DeserializeSKM(std::string Json) {
    return FSkeletalMeshStruct();
}

nlohmann::json JsonDeserializer::UZ(nlohmann::json JsonObj) {
    // If JsonObject is a string, check for signed 0
    if (JsonObj.is_string()) {
        if (JsonObj == "+0" || JsonObj == "-0") {
            return 0;
        }
    }
    return JsonObj;
}
