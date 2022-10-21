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
} FPositionVertexBuffer;

typedef struct {
    FVector2D UV;
} FMeshUVFloat;

typedef struct {
    uint32_t Data;
    FVector4 VertexTangent;
} FPackedNormal;

typedef struct {
    FPackedNormal VertexTangentX;
    FPackedNormal VertexTangentY;
    FPackedNormal VertexTangentZ;
    std::vector<FMeshUVFloat> UV;
} FStaticMeshUVItem;

typedef struct {
    int NumTexCoords;
    int Strides;
    int NumVertices;
    bool UseFullPrecisionUVs;
    bool UseHighPrecisionTangentBasis;
    std::vector<FStaticMeshUVItem> UV;
} FStaticMeshVertexBuffer;

typedef struct {
    std::vector<uint16_t> Indices16;
    std::vector<uint32_t> Indices32;
    bool bIs32Bit;
} FRawStaticIndexBuffer;

typedef struct {
    std::vector<FStaticMeshSection> Sections;
    FPositionVertexBuffer PositionVertexBuffer;
    FStaticMeshVertexBuffer VertexBuffer;
    FRawStaticIndexBuffer IndexBuffer;
} FStaticMeshLODResources;

typedef struct {
    std::string MaterialSlotName;
} FStaticMaterial;

typedef struct {
    std::vector<FStaticMeshLODResources> LODs;
} FStaticMeshRenderData;

typedef struct {
    std::string Name;
    FStaticMeshRenderData RenderData;
    std::vector<FStaticMaterial> StaticMaterials;
} FStaticMeshStruct;

#endif //FBX_WRAPPER_SMSTRUCTS_H
