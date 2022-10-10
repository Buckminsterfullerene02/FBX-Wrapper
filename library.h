#ifndef FBX_WRAPPER_LIBRARY_H
#define FBX_WRAPPER_LIBRARY_H

#include <string>
#include <iostream>
#include <ctime>

#include "fbxsdk.h"
#include "SMStructs.h"
#include "SKStructs.h"
#include "SKMStructs.h"
#include "AnimSequenceStructs.h"
#include "FbxDataConverter.h"
#include "GenerateSmoothingGroups.h"

#ifdef FBXLIBRARY_EXPORTS
#define FBXLIBRARY_API __declspec(dllexport)
#else
#define FBXLIBRARY_API __declspec(dllimport)
#endif

std::string GetRandomGUID();
std::string GetNameForUVChannel(uint32_t Index);
FbxManager* AllocateFbxManagerForExport();
FbxScene* CreateFbxSceneForFbxManager(FbxManager* manager);
bool ExportFbxSceneToFileByPath(char& OutFileName, FbxScene* Scene, bool bExportAsText, std::string* OutErrorMessage);
int ExportDummyMaterialIntoFbxScene(const std::string& MaterialSlotName, FbxNode* Node);
void AddNodeRecursively(std::vector<FbxNode*>& OutNodeArray, FbxNode* Node);

/** Binds skeletal mesh to skeleton by applying skin weights and skin deformation */
void BindSkeletalMeshToSkeleton(const FSkeletalMeshLODRenderData& SkeletalMeshLOD,
                                       const std::vector<FbxNode*>& BoneNodes, FbxNode* MeshRootNode);

/** Exports common mesh resources into FBX mesh */
void ExportCommonMeshResources(const FStaticMeshVertexBuffers& VertexBuffers, FbxMesh* Mesh);

/** Exports Static Mesh LOD into the target fbx mesh object */
void ExportStaticMesh(const FStaticMeshLODResources& StaticMeshLOD, FStaticMaterial ReferencedMaterials[], FbxMesh* Mesh);

/** Exports skeleton into the fbx scene. Returns root skeleton bone and adds other nodes into the given array */
FbxNode* ExportSkeleton(FbxScene* Scene, const FReferenceSkeleton& Skeleton, std::vector<FbxNode*>& BoneNodes);

/** Exports skeletal mesh LOD into the target fbx mesh */
void ExportSkeletalMesh(const FSkeletalMeshLODRenderData& SkeletalMeshLOD,
                               const std::vector<FSkeletalMaterial>& ReferencedMaterials, FbxMesh* FbxMesh);

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

    /**
     * Exports skeleton itself into the FBX file
     * It does not actually export any geometry or animations, just a bare skeleton
     */
    FBXLIBRARY_API void* ExportSkeletonIntoFbxFile(FSkeletonStruct* SkeletonData, char& OutFileName,
                                                   bool bExportAsText, char* OutErrorMessage);

    /**
     * Exports skeletal mesh into the FBX file
     * Overall behavior is similar to ExportStaticMeshIntoFbxFile, but
     * additional skeletal mesh related data (e.g skeleton, skin weights and binding pose) is exported
     * Animations are exported separately and this function does not handle that
     */
    FBXLIBRARY_API void* ExportSkeletalMeshIntoFbxFile(FSkeletalMeshStruct* SkeletalMeshData, char& OutFileName,
                                                       bool bExportAsText, char* OutErrorMessage);

    /**
     * Exports animation sequence into the fbx file
     * Will export associated skeleton and animation applied to it,
     * but will not export any kind of skeletal meshes
     */
    FBXLIBRARY_API void* ExportAnimSequenceIntoFbxFile(FAnimSequenceStruct* AnimSequenceData, char& OutFileName,
                                                       bool bExportAsText, char* OutErrorMessage);

#ifdef __cplusplus
}
#endif

#endif //FBX_WRAPPER_LIBRARY_H
