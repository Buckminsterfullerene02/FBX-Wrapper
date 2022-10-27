#ifndef FBX_WRAPPER_UESTRUCTS_H
#define FBX_WRAPPER_UESTRUCTS_H

#include <math.h>

#define PI (3.1415926535897932f)

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

typedef struct {
    float R;
    float G;
    float B;
} FColor;

#endif //FBX_WRAPPER_UESTRUCTS_H
