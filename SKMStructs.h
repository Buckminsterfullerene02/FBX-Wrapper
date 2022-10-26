#ifndef FBX_WRAPPER_SKMSTRUCTS_H
#define FBX_WRAPPER_SKMSTRUCTS_H

#include "SKStructs.h"

typedef struct {
    std::vector<int> BoneIndex;
    std::vector<float> BoneWeight;
} FSkinWeightInfo;

typedef struct {
    FPackedNormal VertexTangentX;
    FPackedNormal VertexTangentY;
    FPackedNormal VertexTangentZ;
} FSkelMeshNormal;

typedef struct {
    std::vector<FMeshUVFloat> UV;
    FVector Pos;
    FSkelMeshNormal Normal;
    FSkinWeightInfo Influences;
} FSkelMeshVertexBuffer;

typedef struct {
    int NumVertices;
    bool bUseFullPrecisionUVs;
    bool bExtraBoneInfluences;
    bool bUseVertsHalf;
    std::vector<FSkelMeshVertexBuffer> Verts;
} FVertexBufferGPUSkin;

typedef struct {
    int MaterialIndex;
    int NumTriangles;
    int BaseIndex;
    uint32_t BaseVertexIndex;
    int NumVertices;
    bool bUse16BitBoneIndex;
    uint32_t MaxBoneInfluences;
    std::vector<int> BoneMap;
} FSkelMeshSection;

typedef struct {
    std::vector<FSkelMeshSection> Sections;
    FRawStaticIndexBuffer Indices;
    int NumTexCoords;
    FVertexBufferGPUSkin VertexBufferGPUSkin;
} FStaticLODModel;

typedef struct {
    std::string MaterialSlotName;
} FSkeletalMaterial;

typedef struct {
    std::string Name;
    FReferenceSkeleton RefSkeleton;
    std::vector<FStaticLODModel> LODModels;
    std::vector<FSkeletalMaterial> Materials;
} FSkeletalMeshStruct;

#endif //FBX_WRAPPER_SKMSTRUCTS_H
