#ifndef FBX_WRAPPER_UESTRUCTS_H
#define FBX_WRAPPER_UESTRUCTS_H

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
