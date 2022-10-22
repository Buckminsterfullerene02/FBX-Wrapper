#ifndef FBX_WRAPPER_LIBRARY_H
#define FBX_WRAPPER_LIBRARY_H

#include <string>
#include <iostream>
#include <ctime>

#include "fbxsdk.h"
#ifdef snprintf
#undef snprintf
#endif
#include "SMStructs.h"
#include "SKStructs.h"
#include "SKMStructs.h"
#include "AnimSequenceStructs.h"
#include "FbxDataConverter.h"
#include "GenerateSmoothingGroups.h"
#include "JsonDeserializer.h"

#ifdef FBXLIBRARY_EXPORTS
#define FBXLIBRARY_API __declspec(dllexport)
#else
#define FBXLIBRARY_API __declspec(dllimport)
#endif

std::string GetRandomGUID();

std::string GetNameForUVChannel(uint32_t Index);

inline uint32_t GetIndex(FRawStaticIndexBuffer StaticIndexBuffer, uint32_t At) {
    if (StaticIndexBuffer.bIs32Bit) return StaticIndexBuffer.Indices32[At];
    else return StaticIndexBuffer.Indices16[At];
}

inline uint32_t GetBoneIndexByteSize(bool bUse16BitBoneIndex) {
    return bUse16BitBoneIndex ? sizeof(uint16_t) : sizeof(uint8_t);
}

inline uint32_t GetConstantInfluencesVertexStride(bool bUse16BitBoneIndex, uint32_t MaxBoneInfluences) {
    return GetBoneIndexByteSize(bUse16BitBoneIndex) * MaxBoneInfluences;
}

inline float ClampAxis(float Angle) {
    // returns Angle in the range (-360,360)
    Angle = std::fmod(Angle, 360.f);
    if (Angle < 0.f) {
        // shift to [0,360) range
        Angle += 360.f;
    }
    return Angle;
}

inline float NormalizeAxis(float Angle) {
    // returns Angle in the range [0,360)
    Angle = ClampAxis(Angle);
    if (Angle > 180.f) {
        // shift to (-180,180]
        Angle -= 360.f;
    }
    return Angle;
}

// reference
// https://github.com/EpicGames/UnrealEngine/blob/1e5926084bbf386041103735ed6c2ab27bc1c1ee/Engine/Source/Runtime/Core/Private/Math/UnrealMath.cpp#L589
inline FVector Euler(FQuat Quat) {
    float X = Quat.X;
    float Y = Quat.Y;
    float Z = Quat.Z;
    float W = Quat.W;
    const float SingularityTest = Z * X - W * Y;
    const float YawY = 2.f * (W * Z + X * Y);
    const float YawX = (1.f - 2.f * ((Y * Y) + (Z * Z)));

    const float SINGULARITY_THRESHOLD = 0.4999995f;
    const float RAD_TO_DEG = (180.f) / PI;
    FRotator RotatorFromQuat;

    if (SingularityTest < -SINGULARITY_THRESHOLD) {
        RotatorFromQuat.Pitch = -90.f;
        RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * std::atan2(X, W) * RAD_TO_DEG));
    } else if (SingularityTest > SINGULARITY_THRESHOLD) {
        RotatorFromQuat.Pitch = 90.f;
        RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = NormalizeAxis(RotatorFromQuat.Yaw - (2.f * std::atan2(X, W) * RAD_TO_DEG));
    } else {
        RotatorFromQuat.Pitch = std::asin(2.f * (SingularityTest)) * RAD_TO_DEG;
        RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = std::atan2(-2.f * (W * X + Y * Z), (1.f - 2.f * ((X * X) + (Y * Y)))) * RAD_TO_DEG;
    }

    FVector VectorFromRotator;
    VectorFromRotator.X = RotatorFromQuat.Pitch;
    VectorFromRotator.Y = RotatorFromQuat.Yaw;
    VectorFromRotator.Z = RotatorFromQuat.Roll;
    return VectorFromRotator;
}

FbxManager* AllocateFbxManagerForExport();

FbxScene* CreateFbxSceneForFbxManager(FbxManager* manager);

bool ExportFbxSceneToFileByPath(char& OutFileName, FbxScene* Scene, bool bExportAsText, std::string* OutErrorMessage);

int ExportDummyMaterialIntoFbxScene(const std::string& MaterialSlotName, FbxNode* Node);

void AddNodeRecursively(std::vector<FbxNode*>& OutNodeArray, FbxNode* Node);

uint32_t FSkinWeightDataVertexBuffer_GetBoneIndex(const FSkinWeightDataVertexBuffer& VertexBuffer, uint32_t VertexWeightOffset,
                                                  uint32_t VertexInfluenceCount, uint32_t InfluenceIndex);

uint8_t FSkinWeightDataVertexBuffer_GetBoneWeight(const FSkinWeightDataVertexBuffer& VertexBuffer, uint32_t VertexWeightOffset,
                                                  uint32_t VertexInfluenceCount, uint32_t InfluenceIndex);

void FSkinWeightLookupVertexBuffer_GetWeightOffsetAndInfluenceCount(const FSkinWeightLookupVertexBuffer& VertexBuffer,
                                                                    uint32_t VertexIndex, uint32_t& OutWeightOffset,
                                                                    uint32_t& OutInfluenceCount);

void FSkinWeightVertexBuffer_GetVertexInfluenceOffsetCount(const FSkinWeightVertexBuffer& Buffer, uint32_t VertexIndex,
                                                           uint32_t& VertexWeightOffset, uint32_t& VertexInfluenceCount);

uint32_t FSkinWeightVertexBuffer_GetBoneIndex(const FSkinWeightVertexBuffer& Buffer,
                                              uint32_t VertexIndex, uint32_t InfluenceIndex);

uint8_t FSkinWeightVertexBuffer_GetBoneWeight(const FSkinWeightVertexBuffer& Buffer,
                                            uint32_t VertexIndex, uint32_t InfluenceIndex);

FSkinWeightInfo FSkinWeightVertexBuffer_GetVertexSkinWeights(const FSkinWeightVertexBuffer& Buffer, uint32_t VertexIndex);

/** Creates bind pose for given fbx node holding mesh or surface */
void CreateBindPose(FbxNode* MeshRootNode);

/** Binds skeletal mesh to skeleton by applying skin weights and skin deformation */
void BindSkeletalMeshToSkeleton(const FSkeletalMeshLODRenderData& SkeletalMeshLOD,
                                       const std::vector<FbxNode*>& BoneNodes, FbxNode* MeshRootNode);

/** Exports common mesh resources into FBX mesh */
void ExportCommonMeshResources(const FStaticMeshVertexBuffer& VertexBuffer,
                               const FPositionVertexBuffer& PositionVertexBuffer, FbxMesh* Mesh);

/** Exports Static Mesh LOD into the target fbx mesh object */
void ExportStaticMesh(const FStaticMeshLODResources& StaticMeshLOD, FStaticMaterial ReferencedMaterials[], FbxMesh* Mesh);

/** Exports skeleton into the fbx scene. Returns root skeleton bone and adds other nodes into the given array */
FbxNode* ExportSkeleton(FbxScene* Scene, const FReferenceSkeleton& Skeleton, std::vector<FbxNode*>& BoneNodes);

/** Exports skeletal mesh LOD into the target fbx mesh */
void ExportSkeletalMesh(const FSkeletalMeshLODRenderData& SkeletalMeshLOD,
                               const std::vector<FSkeletalMaterial>& ReferencedMaterials, FbxMesh* FbxMesh);

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Exports static mesh data into the FBX file using the specified path
     * Keep in mind that not all information is exported, and materials are not exported
     * Material slot names are kept intact during export though, and are filled with dummy materials
     * Until the SM JSON is read to populate them
     */
    FBXLIBRARY_API void* ExportStaticMeshIntoFbxFile(std::string StaticMeshJson, char& OutFileName,
                                                     bool bExportAsText, char* OutErrorMessage);

    /**
     * Exports skeleton itself into the FBX file
     * It does not actually export any geometry or animations, just a bare skeleton
     */
    FBXLIBRARY_API void* ExportSkeletonIntoFbxFile(FSkeletonStruct* SkeletonData, char& OutFileName,
                                                   bool bExportAsText, char* OutErrorMessage);

    /**
     * Exports skeletal mesh into the FBX file
     * Overall behavior is similar to ExportStaticMeshIntoFbxFile, but
     * additional skeletal mesh related data (e.g skeleton, skin weights and binding pose) is exported
     * Animations are exported separately and this function does not handle that
     */
    FBXLIBRARY_API void* ExportSkeletalMeshIntoFbxFile(FSkeletalMeshStruct* SkeletalMeshData, char& OutFileName,
                                                       bool bExportAsText, char* OutErrorMessage);

    /**
     * Exports animation sequence into the FBX file
     * Will export associated skeleton and animation applied to it,
     * but will not export any kind of skeletal meshes
     */
    FBXLIBRARY_API void* ExportAnimSequenceIntoFbxFile(FAnimSequenceStruct* AnimSequenceData, char& OutFileName,
                                                       bool bExportAsText, char* OutErrorMessage);

#ifdef __cplusplus
}
#endif

#endif //FBX_WRAPPER_LIBRARY_H
