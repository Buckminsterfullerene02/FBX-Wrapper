#include "FbxDataConverter.h"

FbxVector4 ConvertToFbxPos(FVector Vector) {
    FbxVector4 Out{};
    Out[0] = Vector.X;
    Out[1] = -Vector.Y;
    Out[2] = Vector.Z;
    return Out;
}

FbxVector4 ConvertToFbxPos(FVector4 Vector) {
    FbxVector4 Out{};
    Out[0] = Vector.X;
    Out[1] = -Vector.Y;
    Out[2] = Vector.Z;
    return Out;
}

FbxVector4 ConvertToFbxRot(FVector Vector) {
    FbxVector4 Out{};
    Out[0] = Vector.X;
    Out[1] = -Vector.Y;
    Out[2] = -Vector.Z;
    return Out;
}

FbxVector4 ConvertToFbxScale(FVector Vector) {
    FbxVector4 Out{};
    Out[0] = Vector.X;
    Out[1] = Vector.Y;
    Out[2] = Vector.Z;
    return Out;
}

FbxDouble3 ConvertToFbxColor(FColor Color) {
    FbxDouble3 Out;
    Out[0] = Color.R / 255.f;
    Out[1] = Color.G / 255.f;
    Out[2] = Color.B / 255.f;
    return Out;
}
