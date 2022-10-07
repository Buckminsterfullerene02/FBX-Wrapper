#ifndef FBX_WRAPPER_SMSTRUCTS_H
#define FBX_WRAPPER_SMSTRUCTS_H

#include <vector>
#include "UEStructs.h"

typedef struct {
    FVector Position;
} FPositionVertex;

typedef struct {
    char* name;
} FStaticMeshStruct;

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
    float U;
    float V;
} FMeshUVFloat;

typedef struct {
    uint32_t Data;
    float X;
    float Y;
    float Z;
    float W;
} FPackedNormal;

typedef struct {
    std::vector<FPackedNormal> Normal;
    std::vector<FMeshUVFloat> UV;
} FStaticMeshUVItem;

typedef struct {
    int NumTexCoords;
    bool UseFullPrecisionUVs;
    bool UseHighPrecisionTangentBasis;
    FPackedNormal VertexTangent;
    FStaticMeshUVItem VertexUV;
} FStaticMeshVertexBuffer;

typedef struct {
    int NumVertices;
    FPositionVertexBuffer PositionVertexBuffer;
    FStaticMeshVertexBuffer StaticMeshVertexBuffer;
} FStaticMeshVertexBuffers;

typedef struct {
    std::vector<uint16_t> Indices16;
    std::vector<uint32_t> Indices32;
    /* FRawStaticIndexBuffer.cs
     * if (is32bit)
            {
                var count = (int)len / 4;
                Indices32 = new uint[count];
                for (int i = 0; i < count; i++) {
                    Indices32[i] = reader.ReadUInt32();
                }
            }
            else
            {
                var count = (int)len / 2;
                Indices32 = new uint[count];
                for (int i = 0; i < count; i++) {
                    Indices32[i] = reader.ReadUInt16();
                }
            }
     */
    uint32_t GetIndex(uint32_t At) {
        return Indices32[At];
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

#endif //FBX_WRAPPER_SMSTRUCTS_H
