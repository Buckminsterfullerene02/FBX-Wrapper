#ifndef FBX_WRAPPER_UESTRUCTS_H
#define FBX_WRAPPER_UESTRUCTS_H

#include <math.h>

#define PI (3.1415926535897932f)
#define MAX_TOTAL_INFLUENCES 12 // TODO: May change between engine versions (GPUSkinPublicDefs.h)

typedef struct {
    float X;
    float Y;
} FVector2D;

typedef struct {
    float X;
    float Y;
    float Z;
} FVector;

typedef struct {
    float X;
    float Y;
    float Z;
    float W;
} FVector4;

typedef struct {
    float Pitch;
    float Yaw;
    float Roll;
} FRotator;

typedef struct {
    float X;
    float Y;
    float Z;
    float W;
} FQuat;

typedef struct {
    FQuat Rotation;
    FVector Translation;
    FVector Scale3D;
} FTransform;

#endif //FBX_WRAPPER_UESTRUCTS_H
