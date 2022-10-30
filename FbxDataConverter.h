#ifndef FBX_WRAPPER_FBXDATACONVERTER_H
#define FBX_WRAPPER_FBXDATACONVERTER_H

#include "UEStructs.h"
#include "fbxsdk.h"

FbxVector4 ConvertToFbxPos(FVector Vector);

FbxVector4 ConvertToFbxPos(FVector4 Vector);

FbxVector4 ConvertToFbxRot(FVector Vector);

FbxVector4 ConvertToFbxRot(FQuat Vector);

FbxVector4 ConvertToFbxScale(FVector Vector);

FbxDouble3 ConvertToFbxColor(FColor Color);

#endif //FBX_WRAPPER_FBXDATACONVERTER_H
