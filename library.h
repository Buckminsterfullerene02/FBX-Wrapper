#ifndef FBX_WRAPPER_LIBRARY_H
#define FBX_WRAPPER_LIBRARY_H

#include <string>
#include <iostream>
#include "fbxsdk.h"
#include "SMStructs.h"
#include "SKStructs.h"
#include "FbxDataConverter.h"
#include "GenerateSmoothingGroups.h"

#ifdef FBXLIBRARY_EXPORTS
#define FBXLIBRARY_API __declspec(dllexport)
#else
#define FBXLIBRARY_API __declspec(dllimport)
#endif


std::string GetNameForUVChannel(uint32_t Index);
FbxManager* AllocateFbxManagerForExport();
FbxScene* CreateFbxSceneForFbxManager(FbxManager* manager);
bool ExportFbxSceneToFileByPath(char& OutFileName, FbxScene* Scene, bool bExportAsText, std::string* OutErrorMessage);
int ExportDummyMaterialIntoFbxScene(const std::string& MaterialSlotName, FbxNode* Node);
void AddNodeRecursively(std::vector<FbxNode*>& OutNodeArray, FbxNode* Node);

/** Exports common mesh resources into FBX mesh */
void ExportCommonMeshResources(const FStaticMeshVertexBuffers& VertexBuffers, FbxMesh* Mesh);

/** Exports Static Mesh LOD into the target fbx mesh object */
void ExportStaticMesh(const FStaticMeshLODResources& StaticMeshLOD, FStaticMaterial ReferencedMaterials[], FbxMesh* Mesh);

FbxNode* ExportSkeleton(FbxScene* Scene, const FReferenceSkeleton& Skeleton, std::vector<FbxNode*>& BoneNodes);

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Exports static mesh data into the FBX file using the specified path
     * Keep in mind that not all information is exported, and materials are not exported
     * Material slot names are kept intact during export though, and are filled with dummy materials
     * Until the SM JSON is read to populate them
     */
    FBXLIBRARY_API void* ExportStaticMeshIntoFbxFile(FStaticMeshStruct* StaticMeshData, char& OutFileName,
                                                     bool bExportAsText, char* OutErrorMessage);
    FBXLIBRARY_API void* ExportSkeletonIntoFbxFile(void* FSkeletonStruct, char& OutFileName,
                                                   bool bExportAsText, char* OutErrorMessage);
    FBXLIBRARY_API void* ExportSkeletalMeshIntoFbxFile(void* SkeletalMeshData, char& OutFileName,
                                                       bool bExportAsText, char* OutErrorMessage);
    FBXLIBRARY_API void* ExportAnimSequenceIntoFbxFile(void* AnimSequenceData, char& OutFileName,
                                                       bool bExportAsText, char* OutErrorMessage);

#ifdef __cplusplus
}
#endif

#endif //FBX_WRAPPER_LIBRARY_H
