#include <iostream>
#include "JsonDeserializer.h"

FStaticMeshStruct JsonDeserializer::DeserializeSM(std::string Json) {
    nlohmann::json JsonObj = nlohmann::json::parse(Json);
    FStaticMeshStruct StaticMeshStruct{};

    StaticMeshStruct.Name = JsonObj["Name"];

    for (int i = 0; i < JsonObj["StaticMaterials"].size(); ++i) {
        FStaticMaterial StaticMaterial{};
        StaticMaterial.MaterialSlotName = JsonObj["StaticMaterials"][i]["MaterialSlotName"];
        StaticMeshStruct.StaticMaterials.emplace_back(StaticMaterial);
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

        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices16"].size(); ++j) {
            StaticMeshLODResources.IndexBuffer.Indices16.emplace_back(JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices16"][j]);
        }
        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices32"].size(); ++j) {
            StaticMeshLODResources.IndexBuffer.Indices32.emplace_back(JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices32"][j]);
        }
        StaticMeshLODResources.IndexBuffer.bIs32Bit = StaticMeshLODResources.IndexBuffer.Indices16.empty();

        StaticMeshStruct.RenderData.LODs.push_back(StaticMeshLODResources);
    }

    std::cout << "finished parsing" << std::endl;
    return StaticMeshStruct;
}

FReferenceSkeleton JsonDeserializer::DeserializeSK(std::string Json) {
    nlohmann::json JsonObj = nlohmann::json::parse(Json);
    FReferenceSkeleton SkeletonStruct{};

    for (int i = 0; i < JsonObj["Skeleton"]["FinalRefBoneInfo"].size(); ++i) {
        FMeshBoneInfo MeshBoneInfo{};
        MeshBoneInfo.Name = JsonObj["Skeleton"]["FinalRefBoneInfo"][i]["Name"];
        MeshBoneInfo.ParentIndex = JsonObj["Skeleton"]["FinalRefBoneInfo"][i]["ParentIndex"];
        SkeletonStruct.RawRefBoneInfo.emplace_back(MeshBoneInfo);
    }

    for (int i = 0; i < JsonObj["Skeleton"]["FinalRefBonePose"].size(); ++i) {
        FTransform Transform{};

        Transform.Translation.X = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Translation"]["X"]);
        Transform.Translation.Y = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Translation"]["Y"]);
        Transform.Translation.Z = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Translation"]["Z"]);

        Transform.Rotation.X = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Rotation"]["X"]);
        Transform.Rotation.Y = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Rotation"]["Y"]);
        Transform.Rotation.Z = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Rotation"]["Z"]);
        Transform.Rotation.W = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Rotation"]["W"]);

        Transform.Scale3D.X = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Scale3D"]["X"]);
        Transform.Scale3D.Y = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Scale3D"]["Y"]);
        Transform.Scale3D.Z = UZ(JsonObj["Skeleton"]["FinalRefBonePose"][i]["Scale3D"]["Z"]);

        SkeletonStruct.RawRefBonePose.emplace_back(Transform);
    }

    return SkeletonStruct;
}

FSkeletalMeshStruct JsonDeserializer::DeserializeSKM(std::string Json) {
    nlohmann::json JsonObj = nlohmann::json::parse(Json);
    FSkeletalMeshStruct SkeletalMeshStruct{};

    SkeletalMeshStruct.Name = JsonObj["Name"];

    SkeletalMeshStruct.RefSkeleton = DeserializeSK(Json);

    for (int i = 0; i < JsonObj["LODModels"].size(); ++i) {
        FStaticLODModel StaticLODModel{};

        // Sections
        for (int j = 0; j < JsonObj["LODModels"][i]["Sections"].size(); ++j) {
            FSkelMeshSection SkelMeshSection{};
            SkelMeshSection.MaterialIndex = JsonObj["LODModels"][i]["Sections"][j]["MaterialIndex"];
            SkelMeshSection.BaseIndex = JsonObj["LODModels"][i]["Sections"][j]["BaseIndex"];
            SkelMeshSection.NumTriangles = JsonObj["LODModels"][i]["Sections"][j]["NumTriangles"];
            SkelMeshSection.BaseVertexIndex = JsonObj["LODModels"][i]["Sections"][j]["BaseVertexIndex"];
            SkelMeshSection.NumVertices = JsonObj["LODModels"][i]["Sections"][j]["NumVertices"];

            // Bone map
            for (int k = 0; k < JsonObj["LODModels"][i]["Sections"][j]["BoneMap"].size(); ++k) {
                SkelMeshSection.BoneMap.emplace_back(JsonObj["LODModels"][i]["Sections"][j]["BoneMap"][k]);
            }

            StaticLODModel.Sections.emplace_back(SkelMeshSection);
        }

        // Indices
        StaticLODModel.Indices.Indices16 = std::vector<uint16_t>(JsonObj["RenderData"]["LODs"][i]["Indices"]["Indices16"].size());
        StaticLODModel.Indices.Indices32 = std::vector<uint32_t>(JsonObj["RenderData"]["LODs"][i]["Indices"]["Indices32"].size());
        StaticLODModel.Indices.bIs32Bit = StaticLODModel.Indices.Indices16.empty();

        //
    }


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
