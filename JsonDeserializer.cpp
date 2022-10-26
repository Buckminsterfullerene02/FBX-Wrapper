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

        // Verts
        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Verts"].size(); ++j) {
            FVector FVector{};
            FVector.X = UZ(JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Verts"][j]["X"]);
            FVector.Y = UZ(JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Verts"][j]["Y"]);
            FVector.Z = UZ(JsonObj["RenderData"]["LODs"][i]["PositionVertexBuffer"]["Verts"][j]["Z"]);
            StaticMeshLODResources.PositionVertexBuffer.Verts.emplace_back(FVector);
        }

        StaticMeshLODResources.VertexBuffer.NumTexCoords = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["NumTexCoords"];
        StaticMeshLODResources.VertexBuffer.NumVertices = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["NumVertices"];
        StaticMeshLODResources.VertexBuffer.bUseFullPrecisionUVs = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["bUseFullPrecisionUVs"];
        StaticMeshLODResources.VertexBuffer.bUseHighPrecisionTangentBasis = JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["bUseHighPrecisionTangentBasis"];

        // UV
        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"].size(); ++j) {
            FStaticMeshUVItem StaticMeshUVItem{};

            // Vertex tangents
            FPackedNormal VertexTangentX;
            VertexTangentX.VertexTangent.X = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][0]["X"]);
            VertexTangentX.VertexTangent.Y = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][0]["Y"]);
            VertexTangentX.VertexTangent.Z = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][0]["Z"]);
            VertexTangentX.VertexTangent.W = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][0]["W"]);
            StaticMeshUVItem.VertexTangentX = VertexTangentX;

            FPackedNormal VertexTangentY;
            VertexTangentY.VertexTangent.X = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][1]["X"]);
            VertexTangentY.VertexTangent.Y = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][1]["Y"]);
            VertexTangentY.VertexTangent.Z = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][1]["Z"]);
            VertexTangentY.VertexTangent.W = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][1]["W"]);
            StaticMeshUVItem.VertexTangentY = VertexTangentY;

            FPackedNormal VertexTangentZ;
            VertexTangentZ.VertexTangent.X = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][2]["X"]);
            VertexTangentZ.VertexTangent.Y = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][2]["Y"]);
            VertexTangentZ.VertexTangent.Z = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][2]["Z"]);
            VertexTangentZ.VertexTangent.W = UZ(JsonObj["RenderData"]["LODs"][i]["VertexBuffer"]["UV"][j]["Normal"][2]["W"]);
            StaticMeshUVItem.VertexTangentZ = VertexTangentZ;

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

        // Indices
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

    // LODModels
    for (int i = 0; i < JsonObj["LODModels"].size(); ++i) {
        FStaticLODModel StaticLODModel{};

        StaticLODModel.NumTexCoords = JsonObj["LODModels"][i]["NumTexCoords"];

        // Sections
        for (int j = 0; j < JsonObj["LODModels"][i]["Sections"].size(); ++j) {
            FSkelMeshSection SkelMeshSection{};
            SkelMeshSection.MaterialIndex = JsonObj["LODModels"][i]["Sections"][j]["MaterialIndex"];
            SkelMeshSection.BaseIndex = JsonObj["LODModels"][i]["Sections"][j]["BaseIndex"];
            SkelMeshSection.NumTriangles = JsonObj["LODModels"][i]["Sections"][j]["NumTriangles"];
            SkelMeshSection.BaseVertexIndex = JsonObj["LODModels"][i]["Sections"][j]["BaseVertexIndex"];
            SkelMeshSection.NumVertices = JsonObj["LODModels"][i]["Sections"][j]["NumVertices"];
            SkelMeshSection.bUse16BitBoneIndex = JsonObj["LODModels"][i]["Sections"][j]["bUse16BitBoneIndex"];
            SkelMeshSection.MaxBoneInfluences = JsonObj["LODModels"][i]["Sections"][j]["MaxBoneInfluences"];

            // Bone map
            for (int k = 0; k < JsonObj["LODModels"][i]["Sections"][j]["BoneMap"].size(); ++k) {
                SkelMeshSection.BoneMap.emplace_back(JsonObj["LODModels"][i]["Sections"][j]["BoneMap"][k]);
            }

            StaticLODModel.Sections.emplace_back(SkelMeshSection);
        }

        // Indices
        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices16"].size(); ++j) {
            StaticLODModel.Indices.Indices16.emplace_back(JsonObj["RenderData"]["LODs"][i]["Indices"]["Indices16"][j]);
        }
        for (int j = 0; j < JsonObj["RenderData"]["LODs"][i]["IndexBuffer"]["Indices32"].size(); ++j) {
            StaticLODModel.Indices.Indices32.emplace_back(JsonObj["RenderData"]["LODs"][i]["Indices"]["Indices32"][j]);
        }
        StaticLODModel.Indices.bIs32Bit = StaticLODModel.Indices.Indices16.empty();

        // Vertex buffers
        StaticLODModel.VertexBufferGPUSkin.bExtraBoneInfluences = JsonObj["LODModels"][i]["VertexBufferGPUSkin"]["bExtraBoneInfluences"];
        StaticLODModel.VertexBufferGPUSkin.bUseFullPrecisionUVs = JsonObj["LODModels"][i]["VertexBufferGPUSkin"]["bUseFullPrecisionUVs"];

        std::string VertsType = JsonObj["LODModels"][i]["VertexBuffer"]["VertsHalf"].empty() ? "VertsFloat" : "VertsHalf";
        StaticLODModel.VertexBufferGPUSkin.NumVertices = JsonObj["LODModels"][i]["VertexBufferGPUSkin"][VertsType].size();
        int t = JsonObj["LODModels"][i]["VertexBufferGPUSkin"][VertsType].size();
        for (int j = 0; j < JsonObj["LODModels"][i]["VertexBufferGPUSkin"][VertsType].size(); ++j) {
            // UV
            FMeshUVFloat MeshUVFloat{};
            for (int k = 0; k < JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["UV"].size(); ++k) {
                FVector2D UV;
                UV.X = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["UV"][k]["U"]);
                UV.Y = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["UV"][k]["V"]);
                MeshUVFloat.UV = UV;
            }
            StaticLODModel.VertexBufferGPUSkin.Verts[j].UV.emplace_back(MeshUVFloat);

            // Pos
            StaticLODModel.VertexBufferGPUSkin.Verts[j].Pos.X = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Pos"]["X"]);
            StaticLODModel.VertexBufferGPUSkin.Verts[j].Pos.Y = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Pos"]["Y"]);
            StaticLODModel.VertexBufferGPUSkin.Verts[j].Pos.Z = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Pos"]["Z"]);

            // Normals
            FPackedNormal VertexTangentX;
            VertexTangentX.VertexTangent.X = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][0]["X"]);
            VertexTangentX.VertexTangent.Y = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][0]["Y"]);
            VertexTangentX.VertexTangent.Z = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][0]["Z"]);
            VertexTangentX.VertexTangent.W = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][0]["W"]);
            StaticLODModel.VertexBufferGPUSkin.Verts[j].Normal.VertexTangentX = VertexTangentX;

            FPackedNormal VertexTangentY;
            VertexTangentY.VertexTangent.X = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][1]["X"]);
            VertexTangentY.VertexTangent.Y = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][1]["Y"]);
            VertexTangentY.VertexTangent.Z = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][1]["Z"]);
            VertexTangentY.VertexTangent.W = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][1]["W"]);
            StaticLODModel.VertexBufferGPUSkin.Verts[j].Normal.VertexTangentY = VertexTangentY;

            FPackedNormal VertexTangentZ;
            VertexTangentZ.VertexTangent.X = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][2]["X"]);
            VertexTangentZ.VertexTangent.Y = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][2]["Y"]);
            VertexTangentZ.VertexTangent.Z = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][2]["Z"]);
            VertexTangentZ.VertexTangent.W = UZ(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Normal"][2]["W"]);
            StaticLODModel.VertexBufferGPUSkin.Verts[j].Normal.VertexTangentZ = VertexTangentZ;

            // Influences
            FSkinWeightInfo Influences{};
            for (int k = 0; k < JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Infs"]["BoneIndex"].size(); ++k) {
                Influences.BoneIndex.emplace_back(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Infs"]["BoneIndex"][k]);
                Influences.BoneWeight.emplace_back(JsonObj["LODModels"][i]["VertexBuffer"][VertsType][j]["Infs"]["BoneWeight"][k]);
            }
            StaticLODModel.VertexBufferGPUSkin.Verts[j].Influences = Influences;
        }
    }

    // Materials
    for (int i = 0; i < JsonObj["Materials"].size(); ++i) {
        FSkeletalMaterial SkeletalMaterial{};
        SkeletalMaterial.MaterialSlotName = JsonObj["Materials"][i]["MaterialSlotName"];
        SkeletalMeshStruct.Materials.emplace_back(SkeletalMaterial);
    }

    return FSkeletalMeshStruct();
}

// Unsign zeros
nlohmann::json JsonDeserializer::UZ(nlohmann::json JsonObj) {
    // If JsonObject is a string, check for signed 0
    if (JsonObj.is_string()) {
        if (JsonObj == "+0" || JsonObj == "-0") {
            return 0;
        }
    }
    return JsonObj;
}
