#ifndef FBX_WRAPPER_SKMSTRUCTS_H
#define FBX_WRAPPER_SKMSTRUCTS_H

#include "SKStructs.h"

typedef struct {
    std::vector<int> InfluenceBones;
    std::vector<int> InfluenceWeights;
} FSkinWeightInfo;

typedef struct {
    FRawStaticIndexBuffer IndexBuffer;
} FMultisizeIndexContainer;

typedef struct {
    short MaterialIndex;
    int NumTriangles;
    int BaseIndex;
    uint32_t BaseVertexIndex;
    int NumVertices;
    std::vector<short> BoneMap;
} FSkelMeshSection;

typedef struct {
    uint8_t* DataPtr;
} FSkinWeightLookupVertexBuffer;

typedef struct {
    uint8_t* DataPtr;
    uint32_t MaxBoneInfluences;
    bool bVariableBonesPerVertex;
    bool bUse16BitBoneIndex;
} FSkinWeightDataVertexBuffer;

typedef struct {
    FSkinWeightDataVertexBuffer* DataVertexBuffer;
    FSkinWeightLookupVertexBuffer* LookupVertexBuffer;
} FSkinWeightVertexBuffer;

typedef struct {
    FStaticMeshVertexBuffer StaticVertexBuffer;
    FPositionVertexBuffer PositionVertexBuffer;
    FSkinWeightVertexBuffer SkinWeightVertexBuffer;
    FMultisizeIndexContainer Indices;
    std::vector<FSkelMeshSection> RenderSections;
} FSkeletalMeshLODRenderData;

typedef struct {
    std::vector<FSkeletalMeshLODRenderData> LODRenderData;
} FSkeletalMeshRenderData;

typedef struct {
    std::string MaterialSlotName;
} FSkeletalMaterial;

typedef struct {
    std::string Name;
    FReferenceSkeleton RefSkeleton;
    FSkeletalMeshRenderData SkeletalMeshRenderData;
    std::vector<FSkeletalMaterial> Materials;
} FSkeletalMeshStruct;

#endif //FBX_WRAPPER_SKMSTRUCTS_H
