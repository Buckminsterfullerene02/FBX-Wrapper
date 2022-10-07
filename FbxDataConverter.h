#ifndef FBX_WRAPPER_FBXDATACONVERTER_H
#define FBX_WRAPPER_FBXDATACONVERTER_H

#include "UEStructs.h"
#include "fbxsdk.h"

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

#endif //FBX_WRAPPER_FBXDATACONVERTER_H
