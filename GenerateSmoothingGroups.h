/****************************************************************************************

   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

//
// The example illustrates how to:
//        1) get normals of mesh.
//        2) get smoothing info of mesh.
//        3) compute smoothing info from normals.
//        4) convert hard/soft edges info to smoothing group info.
//
//Background knowledge:
//There are two kinds of smoothing info:
//1. Smoothing groups info which is saved by polygon. It usually come from 3ds Max, because 3ds Max can set smoothing groups for polygon.
//2. Hard/soft edges info which is saved by edge. It usually come from Maya, because Maya can set hard/soft edges.
//
//steps:
// 1. initialize FBX sdk object.
// 2. load fbx scene form the specified file.
// 3. Get root node of the scene.
// 4. Recursively traverse each node in the scene.
// 5. Get normals of mesh, according to different mapping mode and reference mode.
// 6. Recursively traverse each node in the scene.
// 7. Computing smoothing info from normals or convert smoothing info
// 8. Get smoothing info of mesh, according to different mapping mode and reference mode.
// 9. Destroy all objects.
//

#ifndef FBX_WRAPPER_GENERATESMOOTHINGGROUPS_H
#define FBX_WRAPPER_GENERATESMOOTHINGGROUPS_H

#include <fbxsdk.h>

class GenerateSmoothingGroups {
public:
    //get mesh normals info
    static void GetNormals(FbxNode* pNode);

    //set pCompute true to compute smoothing from normals by default
    //set pConvertToSmoothingGroup true to convert hard/soft edge info to smoothing group info by default
    static void GetSmoothing(FbxManager* pSdkManager, FbxNode* pNode,
                             bool pCompute = false, bool pConvertToSmoothingGroup = false);
};


#endif //FBX_WRAPPER_GENERATESMOOTHINGGROUPS_H
