#ifndef FBX_WRAPPER_SMSTRUCTS_H
#define FBX_WRAPPER_SMSTRUCTS_H

#include <vector>
#include "UEStructs.h"

typedef struct {
    FVector Position;
} FPositionVertex;

typedef struct {
    int MaterialIndex;
    int FirstIndex;
    int NumTriangles;
    int MinVertexIndex;
    int MaxVertexIndex;
    bool bEnableCollision;
    bool bCastShadow;
    bool bForceOpaque;
    bool bVisibleInRayTracing;
} FStaticMeshSection;

typedef struct {
    uint32_t NumVertices;
    uint32_t Stride;
    uint32_t Data; // TODO: Figure out wtf this is
    std::vector<FVector> Verts;
    const FVector& VertexPosition(uint32_t VertexIndex) const
    {
        return ((FPositionVertex*)(Data + VertexIndex * Stride))->Position;
    }
} FPositionVertexBuffer;

typedef struct {
    FVector2D UV;
} FMeshUVFloat;

typedef struct {
    FVector4 VertexTangent;
} FPackedNormal;

typedef struct {
    FPackedNormal VertexTangentX;
    FPackedNormal VertexTangentY;
    FPackedNormal VertexTangentZ;
    std::vector<FMeshUVFloat> VertexUV;
} FStaticMeshUVItem;

typedef struct {
    int NumTexCoords;
    int NumVertices;
    bool UseFullPrecisionUVs;
    bool UseHighPrecisionTangentBasis;
    std::vector<FStaticMeshUVItem> Vertex;
} FStaticMeshVertexBuffer;

typedef struct {
    int NumVertices;
    FPositionVertexBuffer PositionVertexBuffer;
    FStaticMeshVertexBuffer StaticMeshVertexBuffer;
} FStaticMeshVertexBuffers;

typedef struct {
    std::vector<uint16_t> Indices16;
    std::vector<uint32_t> Indices32;
    bool bIs32Bit;
    uint32_t GetIndex(uint32_t At) {
        if (bIs32Bit) return Indices32[At];
        else return Indices16[At];
    }
} FRawStaticIndexBuffer;

typedef struct {
    std::vector<FStaticMeshSection> Sections;
    FStaticMeshVertexBuffers VertexBuffers;
    FRawStaticIndexBuffer IndexBuffer;
} FStaticMeshLODResources;

typedef struct {
    std::string MaterialSlotName;

} FStaticMaterial;

typedef struct {
    std::vector<FStaticMeshLODResources> LODs;
} FStaticMeshRenderData;

typedef struct {
    char* Name;
    FStaticMeshRenderData RenderData;
    std::vector<FStaticMaterial> StaticMaterials;
} FStaticMeshStruct;

#endif //FBX_WRAPPER_SMSTRUCTS_H
