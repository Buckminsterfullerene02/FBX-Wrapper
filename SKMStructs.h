#ifndef FBX_WRAPPER_SKMSTRUCTS_H
#define FBX_WRAPPER_SKMSTRUCTS_H

typedef struct {
    FRawStaticIndexBuffer IndexBuffer;
} FMultisizeIndexContainer;

typedef struct {
    short MaterialIndex;
    int NumTriangles;
    int BaseIndex;
} FSkelMeshSection;

typedef struct {
    FStaticMeshVertexBuffers StaticVertexBuffers;
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
    char Name;
    FReferenceSkeleton RefSkeleton;
    FSkeletalMeshRenderData SkeletalMeshRenderData;
    std::vector<FSkeletalMaterial> Materials;
} FSkeletalMeshStruct;

#endif //FBX_WRAPPER_SKMSTRUCTS_H
