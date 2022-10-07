#ifndef FBX_WRAPPER_LIBRARY_H
#define FBX_WRAPPER_LIBRARY_H

#include <string>
#include <iostream>
#include "fbxsdk.h"
#include "SMStructs.h"
#include "FbxDataConverter.h"

#ifdef FBXLIBRARY_EXPORTS
#define FBXLIBRARY_API __declspec(dllexport)
#else
#define FBXLIBRARY_API __declspec(dllimport)
#endif

std::string GetNameForUVChannel(uint32_t Index);
FbxManager* AllocateFbxManagerForExport();
FbxScene* CreateFbxSceneForFbxManager(FbxManager* manager);
bool ExportFbxSceneToFileByPath(char& OutFileName, FbxScene* Scene, bool bExportAsText, std::string* OutErrorMessage);
void ExportCommonMeshResources(const FStaticMeshVertexBuffers& VertexBuffers, FbxMesh* Mesh);
void ExportStaticMesh(const FStaticMeshLODResources& StaticMeshLOD, FStaticMaterial ReferencedMaterials[], FbxMesh* Mesh);

#ifdef __cplusplus
extern "C" {
#endif

    FBXLIBRARY_API void* ExportStaticMeshIntoFbxFile(FStaticMeshStruct* StaticMeshData, const char* OutFileName,
                                                     bool bExportAsText, const char* OutErrorMessage);
    FBXLIBRARY_API void* ExportSkeletonIntoFbxFile(void* SkeletonData, const char* OutFileName,
                                                     bool bExportAsText, char* OutErrorMessage);
    FBXLIBRARY_API void* ExportSkeletalMeshIntoFbxFile(void* SkeletalMeshData, const char* OutFileName,
                                                     bool bExportAsText, char* OutErrorMessage);
    FBXLIBRARY_API void* ExportAnimSequenceIntoFbxFile(void* AnimSequenceData, const char* OutFileName,
                                                     bool bExportAsText, char* OutErrorMessage);

#ifdef __cplusplus
}
#endif

#endif //FBX_WRAPPER_LIBRARY_H
