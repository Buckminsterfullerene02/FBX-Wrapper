#ifndef FBX_WRAPPER_SKSTRUCTS_H
#define FBX_WRAPPER_SKSTRUCTS_H

#include "UEStructs.h"

typedef struct {
    std::string Name;
    int ParentIndex;
} FMeshBoneInfo;

typedef struct {
    std::vector<FMeshBoneInfo> RawRefBoneInfo;
    std::vector<FTransform> RawRefBonePose;
} FReferenceSkeleton;

typedef struct {
    FReferenceSkeleton Skeleton;
} FSkeletonStruct;

#endif //FBX_WRAPPER_SKSTRUCTS_H
