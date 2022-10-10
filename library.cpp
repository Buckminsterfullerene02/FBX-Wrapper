#include "library.h"



std::string GetRandomGUID() {
    const char Digits[] = "0123456789";
    std::string Ret;
    Ret.reserve(32);
    for (int i = 0; i < 32; ++i) {
        Ret += Digits[rand() % (sizeof(Digits) - 1)];
    }
    return Ret;
}

std::string GetNameForUVChannel(uint32_t Index) {
    if (Index == 0) return "uv";
    return "uv" + std::to_string(Index + 1);
}

FbxManager* AllocateFbxManagerForExport() {
    FbxManager* FbxManager = FbxManager::Create();

    //Initialize root I/O settings for created manager
    FbxIOSettings* IOSettings = FbxIOSettings::Create(FbxManager, IOSROOT);

    //IOSettings->SetBoolProp(EXP_FBX_MATERIAL, true);
    IOSettings->SetBoolProp(EXP_FBX_TEXTURE, true);
    IOSettings->SetBoolProp(EXP_FBX_SHAPE, true);
    IOSettings->SetBoolProp(EXP_FBX_GOBO, true);
    IOSettings->SetBoolProp(EXP_FBX_ANIMATION, true);
    IOSettings->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    FbxManager->SetIOSettings(IOSettings);
    return FbxManager;
}

FbxScene* CreateFbxSceneForFbxManager(FbxManager* manager) {
    FbxScene* Scene = FbxScene::Create(manager, "");

    // create scene info
    FbxDocumentInfo* SceneInfo = FbxDocumentInfo::Create(manager, "SceneInfo");
    SceneInfo->mTitle = "Cooked Asset Serializer FBX Exporter";
    SceneInfo->mComment = "We don't take any responsibility for misuse of this wrapper.";

    SceneInfo->Original_ApplicationName.Set("Cooked Asset Serializer");
    Scene->SetSceneInfo(SceneInfo);

    auto FrontVector = (FbxAxisSystem::EFrontVector)-FbxAxisSystem::eParityOdd;
    const FbxAxisSystem UnrealZUp(FbxAxisSystem::eZAxis, FrontVector, FbxAxisSystem::eRightHanded);
    Scene->GetGlobalSettings().SetAxisSystem(UnrealZUp);
    Scene->GetGlobalSettings().SetOriginalUpAxis(UnrealZUp);
    Scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::cm);
    Scene->GetGlobalSettings().SetTimeMode(FbxTime::eDefaultMode);

    return Scene;
}

bool ExportFbxSceneToFileByPath(char& OutFileName, FbxScene* Scene, bool bExportAsText, std::string* OutErrorMessage) {
    FbxManager* RootManager = Scene->GetFbxManager();
    FbxExporter* FbxExporter = FbxExporter::Create(RootManager, "");
    FbxIOSettings* IOSettings = RootManager->GetIOSettings();

    int FileFormat;
    IOSettings->SetBoolProp(EXP_ASCIIFBX, bExportAsText);
    if (bExportAsText) {
        FileFormat = RootManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX ascii (*.fbx)");
    } else {
        FileFormat = RootManager->GetIOPluginRegistry()->GetNativeWriterFormat();
    }

    const FbxString FbxFileName = OutFileName;
    bool bSuccess = FbxExporter->Initialize(FbxFileName, FileFormat, IOSettings);

    if (!bSuccess) {
        if (OutErrorMessage) {
            *OutErrorMessage = FbxExporter->GetStatus().GetErrorString();
        }
        return false;
    }

    //Export scene with fbx mesh we created from static mesh
    bSuccess = FbxExporter->Export(Scene);
    if (!bSuccess) {
        if (OutErrorMessage) {
            *OutErrorMessage = FbxExporter->GetStatus().GetErrorString();
        }
        return false;
    }
    return true;
}

int ExportDummyMaterialIntoFbxScene(const std::string& MaterialSlotName, FbxNode* Node) {
    //Create dummy material
    //MaterialSlotName will be either current material name or predefined name
    FbxScene* Scene = Node->GetScene();
    FbxSurfaceMaterial* DummySectionMaterial = FbxSurfaceMaterial::Create(Scene, MaterialSlotName.c_str());
    return Node->AddMaterial(DummySectionMaterial);
}

void AddNodeRecursively(std::vector<FbxNode*>& OutNodeArray, FbxNode* Node) {
    if (Node) {
        AddNodeRecursively(OutNodeArray, Node->GetParent());
        // Add unique
        if (std::find(OutNodeArray.begin(), OutNodeArray.end(), Node) == OutNodeArray.end()) {
            OutNodeArray.push_back(Node);
        }
    }
}

uint32_t FSkinWeightDataVertexBuffer_GetBoneIndex(const FSkinWeightDataVertexBuffer& VertexBuffer, uint32_t VertexWeightOffset,
                                                uint32_t VertexInfluenceCount, uint32_t InfluenceIndex) {
    //if (InfluenceIndex < VertexInfluenceCount) {
    //	const uint8* BoneData = VertexBuffer.GetData() + VertexWeightOffset;
    //	if (VertexBuffer.Use16BitBoneIndex()) {
    //		FBoneIndex16* BoneIndex16Ptr = (FBoneIndex16*)BoneData;
    //		return BoneIndex16Ptr[InfluenceIndex];
    //	}
    //	return BoneData[InfluenceIndex];
    //}
    return 0;
}

uint8_t FSkinWeightDataVertexBuffer_GetBoneWeight(const FSkinWeightDataVertexBuffer& VertexBuffer, uint32_t VertexWeightOffset,
                                                uint32_t VertexInfluenceCount, uint32_t InfluenceIndex) {
    //if (InfluenceIndex < VertexInfluenceCount){
    //	const uint8* BoneData = VertexBuffer.GetData() + VertexWeightOffset;
    //	uint32 BoneWeightOffset = VertexBuffer.GetBoneIndexByteSize() * VertexInfluenceCount;
    //	return BoneData[BoneWeightOffset + InfluenceIndex];
    //}
    return 0;
}

void FSkinWeightLookupVertexBuffer_GetWeightOffsetAndInfluenceCount(const FSkinWeightLookupVertexBuffer& VertexBuffer,
                                                                    uint32_t VertexIndex, uint32_t& OutWeightOffset,
                                                                    uint32_t& OutInfluenceCount) {
    //uint32 Offset = VertexIndex * 4;
    //uint32 DataUInt32 = *((const uint32*)(&VertexBuffer.GetData()[Offset]));
    //OutWeightOffset = DataUInt32 >> 8;
    //OutInfluenceCount = DataUInt32 & 0xff;
}

//Copied from FSkinWeightVertexBuffer methods because they are not exported by the engine
void FSkinWeightVertexBuffer_GetVertexInfluenceOffsetCount(const FSkinWeightVertexBuffer& Buffer, uint32_t VertexIndex,
                                                           uint32_t& VertexWeightOffset, uint32_t& VertexInfluenceCount) {
    if (Buffer.DataVertexBuffer->bVariableBonesPerVertex) {
        FSkinWeightLookupVertexBuffer_GetWeightOffsetAndInfluenceCount(*Buffer.LookupVertexBuffer, VertexIndex,
                                                                       VertexWeightOffset, VertexInfluenceCount);
    } else {
        VertexWeightOffset = Buffer.DataVertexBuffer->GetConstantInfluencesVertexStride() * VertexIndex;
        VertexInfluenceCount = Buffer.DataVertexBuffer->MaxBoneInfluences;
    }
}

uint32_t FSkinWeightVertexBuffer_GetBoneIndex(const FSkinWeightVertexBuffer& Buffer, uint32_t VertexIndex,
                                              uint32_t InfluenceIndex) {
    uint32_t VertexWeightOffset = 0;
    uint32_t VertexInfluenceCount = 0;
    FSkinWeightVertexBuffer_GetVertexInfluenceOffsetCount(Buffer, VertexIndex, VertexWeightOffset, VertexInfluenceCount);
    return FSkinWeightDataVertexBuffer_GetBoneIndex(*Buffer.DataVertexBuffer, VertexWeightOffset, VertexInfluenceCount,
                                                    InfluenceIndex);
}

uint8_t FSkinWeightVertexBuffer_GetBoneWeight(const FSkinWeightVertexBuffer& Buffer, uint32_t VertexIndex,
                                              uint32_t InfluenceIndex) {
    uint32_t VertexWeightOffset = 0;
    uint32_t VertexInfluenceCount = 0;
    FSkinWeightVertexBuffer_GetVertexInfluenceOffsetCount(Buffer, VertexIndex, VertexWeightOffset, VertexInfluenceCount);
    return FSkinWeightDataVertexBuffer_GetBoneWeight(*Buffer.DataVertexBuffer, VertexWeightOffset, VertexInfluenceCount,
                                                     InfluenceIndex);
}

FSkinWeightInfo FSkinWeightVertexBuffer_GetVertexSkinWeights(const FSkinWeightVertexBuffer& Buffer, uint32_t VertexIndex) {
    FSkinWeightInfo OutVertex;
    for (int InfluenceIdx = 0; InfluenceIdx < MAX_TOTAL_INFLUENCES; InfluenceIdx++) {
        OutVertex.InfluenceBones[InfluenceIdx] = FSkinWeightVertexBuffer_GetBoneIndex(Buffer, VertexIndex, InfluenceIdx);
        OutVertex.InfluenceWeights[InfluenceIdx] = FSkinWeightVertexBuffer_GetBoneWeight(Buffer, VertexIndex, InfluenceIdx);
    }
    return OutVertex;
}

void CreateBindPose(FbxNode* MeshRootNode) {
    FbxScene* Scene = MeshRootNode->GetScene();

    // In the bind pose, we must store all the link's global matrix at the time of the bind.
    // Plus, we must store all the parent(s) global matrix of a link, even if they are not
    // themselves deforming any model.

    // Create a bind pose with the link list
    std::vector<FbxNode*> ClusteredFbxNodes;

    FbxNodeAttribute* NodeAttribute = MeshRootNode->GetNodeAttribute();
    if (NodeAttribute) {
        int SkinCount = 0;
        int ClusterCount = 0;

        if (NodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
        NodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
        NodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch) {

            FbxGeometry* GeometryAttribute = (FbxGeometry*) NodeAttribute;
            SkinCount = GeometryAttribute->GetDeformerCount(FbxDeformer::eSkin);

            //Go through all the skins and count them
            //then go through each skin and get their cluster count
            for(int i = 0; i < SkinCount; i++) {
                FbxSkin* Skin = (FbxSkin*) GeometryAttribute->GetDeformer(i, FbxDeformer::eSkin);
                ClusterCount += Skin->GetClusterCount();
            }
        }

        //if we found some clusters we must add the node
        if (ClusterCount) {
            //Again, go through all the skins get each cluster link and add them
            for (int i = 0; i < SkinCount; i++) {
                FbxGeometry* GeometryAttribute = (FbxGeometry*) NodeAttribute;
                FbxSkin* Skin = (FbxSkin*) GeometryAttribute->GetDeformer(i, FbxDeformer::eSkin);

                for (int j = 0; j < Skin->GetClusterCount(); j++) {
                    FbxNode* SkinClusterNode = Skin->GetCluster(j)->GetLink();
                    AddNodeRecursively(ClusteredFbxNodes, SkinClusterNode);
                }
            }
            // Add the patch to the pose
            ClusteredFbxNodes.push_back(MeshRootNode);
        }
    }

    // Now create a bind pose with the link list
    if (ClusteredFbxNodes.size()) {
        // A pose must be named. Arbitrarily use the name of the patch node.
        FbxPose* Pose = FbxPose::Create(Scene, MeshRootNode->GetName());

        // default pose type is rest pose, so we need to set the type as bind pose
        Pose->SetIsBindPose(true);

        for (FbxNode* FbxNode : ClusteredFbxNodes) {
            FbxMatrix BindMatrix = FbxNode->EvaluateGlobalTransform();
            Pose->Add(FbxNode, BindMatrix);
        }

        // Add the pose to the scene
        Scene->AddPose(Pose);
    }
}

void BindSkeletalMeshToSkeleton(const FSkeletalMeshLODRenderData& SkeletalMeshLOD,
                                const std::vector<FbxNode*>& BoneNodes, FbxNode* MeshRootNode) {
    FbxScene* Scene = MeshRootNode->GetScene();
    FbxAMatrix MeshMatrix = MeshRootNode->EvaluateGlobalTransform();

    FbxGeometry* MeshAttribute = (FbxGeometry*) MeshRootNode->GetNodeAttribute();
    FbxSkin* Skin = FbxSkin::Create(Scene, "");
    const FSkinWeightVertexBuffer& SkinWeightVertexBuffer = SkeletalMeshLOD.SkinWeightVertexBuffer;

    const int BoneCount = BoneNodes.size();
    for (int BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex) {
        FbxNode* BoneNode = BoneNodes[BoneIndex];

        // Create the deforming cluster
        FbxCluster* CurrentCluster = FbxCluster::Create(Scene, "");
        CurrentCluster->SetLink(BoneNode);
        CurrentCluster->SetLinkMode(FbxCluster::eTotalOne);

        //We need to do it per-section because bone indices of vertex are local to the section they are contained in
        //So we can mesh bone index from section local bone index and then apply transform to this bone
        for (const FSkelMeshSection& RenderSection : SkeletalMeshLOD.RenderSections) {
            const uint32_t BaseVertexIndex = RenderSection.BaseVertexIndex;
            const uint32_t MaxVertexIndex = BaseVertexIndex + RenderSection.NumVertices;

            for (uint32_t VertexIndex = BaseVertexIndex; VertexIndex < MaxVertexIndex; VertexIndex++) {
                //Retrieve influences for this exact vertex
                FSkinWeightInfo WeightInfo = FSkinWeightVertexBuffer_GetVertexSkinWeights(SkinWeightVertexBuffer, VertexIndex);

                for (int InfluenceIndex = 0; InfluenceIndex < MAX_TOTAL_INFLUENCES; InfluenceIndex++) {
                    const int InfluenceBone = RenderSection.BoneMap[WeightInfo.InfluenceBones[InfluenceIndex]];
                    const float InfluenceWeight = WeightInfo.InfluenceWeights[InfluenceIndex] / 255.f;

                    if (InfluenceBone == BoneIndex && InfluenceWeight > 0.0f) {
                        CurrentCluster->AddControlPointIndex(VertexIndex, InfluenceWeight);
                    }
                }
            }
        }

        // Now we have the Patch and the skeleton correctly positioned,
        // set the Transform and TransformLink matrix accordingly.
        CurrentCluster->SetTransformMatrix(MeshMatrix);
        FbxAMatrix LinkMatrix = BoneNode->EvaluateGlobalTransform();

        CurrentCluster->SetTransformLinkMatrix(LinkMatrix);

        // Add the clusters to the mesh by creating a skin and adding those clusters to that skin.
        Skin->AddCluster(CurrentCluster);
    }

    // Add the skin to the mesh after the clusters have been added
    MeshAttribute->AddDeformer(Skin);
}

void ExportCommonMeshResources(const FStaticMeshVertexBuffers& VertexBuffers, FbxMesh* Mesh) {
    //Initialize vertices first
    const uint32_t NumVertices = VertexBuffers.PositionVertexBuffer.NumVertices;

    Mesh->InitControlPoints(NumVertices);
    FbxVector4* ControlPoints = Mesh->GetControlPoints();

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector& SrcPosition = VertexBuffers.PositionVertexBuffer.VertexPosition(i);
        FbxVector4& DestPosition = ControlPoints[i];
        DestPosition = ConvertToFbxPos(SrcPosition);
    }

    //Initialize vertex colors (if we have any) TODO: Implement FColor so this doesn't have to be skipped
//    if (VertexBuffers.ColorVertexBuffer.GetNumVertices() > 0) {
//        check(VertexBuffers.ColorVertexBuffer.GetNumVertices() == NumVertices);
//
//        FbxGeometryElementVertexColor* VertexColor = Mesh->CreateElementVertexColor();
//        VertexColor->SetMappingMode(FbxLayerElement::eByControlPoint);
//        VertexColor->SetReferenceMode(FbxLayerElement::eDirect);
//
//        FbxLayerElementArrayTemplate<FbxColor>& ColorArray = VertexColor->GetDirectArray();
//        ColorArray.AddMultiple(NumVertices);
//
//        for (uint32 i = 0; i < NumVertices; i++) {
//            const FColor& SrcColor = VertexBuffers.ColorVertexBuffer.VertexColor(i);
//            ColorArray.SetAt(i, FFbxDataConverter::ConvertToFbxColor(SrcColor));
//        }
//    }

    if (VertexBuffers.StaticMeshVertexBuffer.NumVertices != NumVertices) return;

    //Initialize normals
    FbxGeometryElementNormal* Normal = Mesh->CreateElementNormal();
    Normal->SetMappingMode(FbxLayerElement::eByControlPoint);
    Normal->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& NormalArray = Normal->GetDirectArray();
    NormalArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcNormal = VertexBuffers.StaticMeshVertexBuffer.Vertex[i].VertexTangentZ.VertexTangent;
        FbxVector4 DestNormal = ConvertToFbxPos(SrcNormal);
        NormalArray.SetAt(i, DestNormal);
    }

    //Initialize tangents
    FbxGeometryElementTangent* Tangent = Mesh->CreateElementTangent();
    Tangent->SetMappingMode(FbxLayerElement::eByControlPoint);
    Tangent->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& TangentArray = Tangent->GetDirectArray();
    TangentArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcTangent = VertexBuffers.StaticMeshVertexBuffer.Vertex[i].VertexTangentX.VertexTangent;
        FbxVector4 DestTangent = ConvertToFbxPos(SrcTangent);
        TangentArray.SetAt(i, DestTangent);
    }

    //Initialize binormals
    FbxGeometryElementBinormal* Binormal = Mesh->CreateElementBinormal();
    Binormal->SetMappingMode(FbxLayerElement::eByControlPoint);
    Binormal->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& BinormalArray = Binormal->GetDirectArray();
    BinormalArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcBinormal = VertexBuffers.StaticMeshVertexBuffer.Vertex[i].VertexTangentY.VertexTangent;
        FbxVector4 DestBinormal = ConvertToFbxPos(SrcBinormal);
        BinormalArray.SetAt(i, DestBinormal);
    }

    //Initialize UV positions for each channel
    const uint32_t NumTexCoords = VertexBuffers.StaticMeshVertexBuffer.NumTexCoords;
    std::vector<FbxLayerElementArrayTemplate<FbxVector2>*> UVCoordsArray;

    for (uint32_t i = 0; i < NumTexCoords; i++) {
        //TODO proper names, can know if texture is lightmap by checking lightmap tex coord index from static mesh
        const std::string UVChannelName = GetNameForUVChannel(i);
        FbxGeometryElementUV* UVCoords = Mesh->CreateElementUV(UVChannelName.c_str());
        UVCoords->SetMappingMode(FbxLayerElement::eByControlPoint);
        UVCoords->SetReferenceMode(FbxLayerElement::eDirect);
        FbxLayerElementArrayTemplate<FbxVector2>* TexCoordArray = &UVCoords->GetDirectArray();
        TexCoordArray->AddMultiple(NumVertices);

        UVCoordsArray.push_back(TexCoordArray);
    }

    //Populate UV coords for each vertex
    for (uint32_t j = 0; j < NumTexCoords; j++) {
        FbxLayerElementArrayTemplate<FbxVector2>* UVArray = UVCoordsArray[j];
        for (uint32_t i = 0; i < NumVertices; i++) {
            const FVector2D& SrcTextureCoord = VertexBuffers.StaticMeshVertexBuffer.Vertex[i].VertexUV[j].UV;
            FbxVector2 DestUVCoord(SrcTextureCoord.X, -SrcTextureCoord.Y + 1.0f);
            UVArray->SetAt(i, DestUVCoord);
        }
    }
}

void ExportStaticMesh(FStaticMeshLODResources& StaticMeshLOD, std::vector<FStaticMaterial> ReferencedMaterials, FbxMesh* Mesh) {
    //Initialize material element
    FbxGeometryElementMaterial* Material = Mesh->CreateElementMaterial();
    Material->SetMappingMode(FbxLayerElement::eByPolygon);
    Material->SetReferenceMode(FbxLayerElement::eIndexToDirect);

    //Create basic static mesh buffers
    ExportCommonMeshResources(StaticMeshLOD.VertexBuffers, Mesh);

    FRawStaticIndexBuffer& IndexBuffer = StaticMeshLOD.IndexBuffer;
    FbxNode* MeshNode = Mesh->GetNode();

    //Create sections and initialize dummy materials
    for (const FStaticMeshSection& MeshSection : StaticMeshLOD.Sections) {
        const uint32_t NumTriangles = MeshSection.NumTriangles;
        const uint32_t StartIndex = MeshSection.FirstIndex;

        //Create dummy material for this section
        const std::string MaterialSlotName = ReferencedMaterials[MeshSection.MaterialIndex].MaterialSlotName;
        const int32_t MaterialIndex = ExportDummyMaterialIntoFbxScene(MaterialSlotName, MeshNode);

        //Add all triangles associated with this section
        for (uint32_t TriangleIndex = 0; TriangleIndex < NumTriangles; TriangleIndex++) {
            Mesh->BeginPolygon(MaterialIndex, -1, -1, false);
            Mesh->AddPolygon(IndexBuffer.GetIndex(StartIndex + TriangleIndex * 3 + 0));
            Mesh->AddPolygon(IndexBuffer.GetIndex(StartIndex + TriangleIndex * 3 + 1));
            Mesh->AddPolygon(IndexBuffer.GetIndex(StartIndex + TriangleIndex * 3 + 2));
            Mesh->EndPolygon();
        }
    }

}

FbxNode* ExportSkeleton(FbxScene* Scene, const FReferenceSkeleton& Skeleton, std::vector<FbxNode*>& BoneNodes) {
    //Return nullptr if we don't have any bones in the skeleton
    if (Skeleton.RawRefBoneInfo.empty()) {
        return nullptr;
    }

    //Create list of all skeleton nodes, and then link them according to skeleton bone hierarchy
    BoneNodes.resize(Skeleton.RawRefBoneInfo.size());

    for(int BoneIndex = 0; BoneIndex < Skeleton.RawRefBoneInfo.size(); ++BoneIndex) {
        const FMeshBoneInfo& CurrentBone = Skeleton.RawRefBoneInfo[BoneIndex];
        const FTransform& BoneTransform = Skeleton.RawRefBonePose[BoneIndex];

        // Create the node's attributes
        FbxString BoneName = CurrentBone.Name;
        FbxSkeleton* SkeletonAttribute = FbxSkeleton::Create(Scene, BoneName.Buffer());
        if(BoneIndex) {
            SkeletonAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
        } else {
            //First bone in the skeleton bone's list is a root one
            SkeletonAttribute->SetSkeletonType(FbxSkeleton::eRoot);
        }

        // Create the node
        FbxNode* BoneNode = FbxNode::Create(Scene, BoneName.Buffer());
        BoneNode->SetNodeAttribute(SkeletonAttribute);

        // Set the bone node's local orientation
        const FVector UnrealRotation = BoneTransform.Rotation.Euler();
        FbxVector4 LocalPos = ConvertToFbxPos(BoneTransform.Translation);
        FbxVector4 LocalRot = ConvertToFbxRot(UnrealRotation);
        FbxVector4 LocalScale = ConvertToFbxScale(BoneTransform.Scale3D);

        BoneNode->LclTranslation.Set(LocalPos);
        BoneNode->LclRotation.Set(LocalRot);
        BoneNode->LclScaling.Set(LocalScale);

        // If this is not the root bone, attach it to its parent
        if(BoneIndex) {
            BoneNodes[CurrentBone.ParentIndex]->AddChild(BoneNode);
        }
        // Add the node to the list of nodes, in bone order
        BoneNodes.push_back(BoneNode);
    }

    return BoneNodes[0];
}

void ExportSkeletalMesh(const FSkeletalMeshLODRenderData& SkeletalMeshLOD,
                        const std::vector<FSkeletalMaterial>& ReferencedMaterials, FbxMesh* FbxMesh) {
    //Initialize material element
    FbxGeometryElementMaterial* Material = FbxMesh->CreateElementMaterial();
    Material->SetMappingMode(FbxLayerElement::eByPolygon);
    Material->SetReferenceMode(FbxLayerElement::eIndexToDirect);

    //Create basic static mesh buffers
    ExportCommonMeshResources(SkeletalMeshLOD.StaticVertexBuffers, FbxMesh);

    FRawStaticIndexBuffer IndexBuffer = SkeletalMeshLOD.Indices.IndexBuffer;
    FbxNode* MeshNode = FbxMesh->GetNode();

    //Create sections and initialize dummy materials
    for (const FSkelMeshSection& MeshSection : SkeletalMeshLOD.RenderSections) {
        const uint32_t NumTriangles = MeshSection.NumTriangles;
        const uint32_t StartVertexIndex = MeshSection.BaseIndex;

        //Create dummy material for this section
        const std::string MaterialSlotName = ReferencedMaterials[MeshSection.MaterialIndex].MaterialSlotName;
        const int MaterialIndex = ExportDummyMaterialIntoFbxScene(MaterialSlotName, MeshNode);

        //Add all triangles associated with this section
        for (uint32_t TriangleIndex = 0; TriangleIndex < NumTriangles; TriangleIndex++) {
            FbxMesh->BeginPolygon(MaterialIndex, -1, -1, false);
            FbxMesh->AddPolygon(IndexBuffer.GetIndex(StartVertexIndex + TriangleIndex * 3 + 0));
            FbxMesh->AddPolygon(IndexBuffer.GetIndex(StartVertexIndex + TriangleIndex * 3 + 1));
            FbxMesh->AddPolygon(IndexBuffer.GetIndex(StartVertexIndex + TriangleIndex * 3 + 2));
            FbxMesh->EndPolygon();
        }
    }
}

void* ExportStaticMeshIntoFbxFile(FStaticMeshStruct* StaticMeshData, char& OutFileName,
                                  bool bExportAsText, char* OutErrorMessage) {
    FbxManager* FbxManager = AllocateFbxManagerForExport();
    if (!FbxManager) return nullptr;

    //Create root scene which we will use to export mesh
    FbxScene* Scene = CreateFbxSceneForFbxManager(FbxManager);
    if (!Scene) return nullptr;

    //Create mesh object
    const FbxString MeshNodeName = StaticMeshData->Name;
    FbxNode* MeshNode = FbxNode::Create(Scene, MeshNodeName);
    FbxMesh* OutExportedMesh = FbxMesh::Create(Scene, MeshNodeName);
    MeshNode->SetNodeAttribute(OutExportedMesh);

    FStaticMeshLODResources& LODResources = StaticMeshData->RenderData.LODs[0];
    ExportStaticMesh(LODResources, StaticMeshData->StaticMaterials, OutExportedMesh);

    Scene->GetRootNode()->AddChild(MeshNode);

    // TODO: Finish GenerateSmoothingGroups
    /*FbxNode* RootNode = Scene->GetRootNode();
    GenerateSmoothingGroups::GetNormals(RootNode);
    GenerateSmoothingGroups::GetSmoothing(FbxManager, RootNode, true);*/

    //Export scene into the file
    const bool bResult = ExportFbxSceneToFileByPath(OutFileName, Scene, bExportAsText, (std::string*)OutErrorMessage);

    //Destroy FbxManager, which will also destroy all objects allocated by it
    FbxManager->Destroy();
    return (bool*)bResult;
}

void* ExportSkeletonIntoFbxFile(FSkeletonStruct* SkeletonData, char& OutFileName,
                                bool bExportAsText, char* OutErrorMessage) {
    FbxManager* FbxManager = AllocateFbxManagerForExport();
    if (!FbxManager) return nullptr;

    //Create root scene which we will use to export mesh
    FbxScene* Scene = CreateFbxSceneForFbxManager(FbxManager);
    if (!Scene) return nullptr;

    // Add the skeleton to the scene
    std::vector<FbxNode*> BoneNodes;
    FbxNode* SkeletonRootNode = ExportSkeleton(Scene, SkeletonData->Skeleton, BoneNodes);
    Scene->GetRootNode()->AddChild(SkeletonRootNode);

    //Export scene into the file
    const bool bResult = ExportFbxSceneToFileByPath(OutFileName, Scene, bExportAsText, (std::string*)OutErrorMessage);

    //Destroy FbxManager, which will also destroy all objects allocated by it
    FbxManager->Destroy();
    return (bool*)bResult;
}

void* ExportSkeletalMeshIntoFbxFile(FSkeletalMeshStruct* SkeletalMeshData, char& OutFileName,
                                    bool bExportAsText, char* OutErrorMessage) {
    FbxManager* FbxManager = AllocateFbxManagerForExport();
    if (!FbxManager) return nullptr;

    //Create root scene which we will use to export mesh
    FbxScene* Scene = CreateFbxSceneForFbxManager(FbxManager);
    if (!Scene) return nullptr;

    //Create a temporary node attach to the scene root.
    //This will allow us to do the binding without the scene transform (non-uniform scale is not supported when binding the skeleton)
    //We then detach from the temp node and attach to the parent and remove the temp node
    const std::string FbxNodeName = GetRandomGUID();
    if (FbxNodeName.length() != 32) return nullptr;
    FbxNode* TmpNodeNoTransform = FbxNode::Create(Scene, *FbxNodeName);
    Scene->GetRootNode()->AddChild(TmpNodeNoTransform);

    // Add the skeleton to the scene
    std::vector<FbxNode*> BoneNodes;
    FbxNode* SkeletonRootNode = ExportSkeleton(Scene, SkeletalMeshData->RefSkeleton, BoneNodes);
    if(SkeletonRootNode) {
        TmpNodeNoTransform->AddChild(SkeletonRootNode);
    }

    //Create mesh from first LOD of the skeletal mesh
    FSkeletalMeshLODRenderData& LODRenderData = SkeletalMeshData->SkeletalMeshRenderData.LODRenderData[0];

    const FbxString MeshNodeName = SkeletalMeshData->Name;
    FbxNode* MeshRootNode = FbxNode::Create(Scene, MeshNodeName);
    FbxMesh* ExportedMesh = FbxMesh::Create(Scene, MeshNodeName);
    MeshRootNode->SetNodeAttribute(ExportedMesh);

    //Populate basic mesh information
    ExportSkeletalMesh(LODRenderData, SkeletalMeshData->Materials, ExportedMesh);

    TmpNodeNoTransform->AddChild(MeshRootNode);

    if (SkeletonRootNode) {
        // Bind the mesh to the skeleton
        BindSkeletalMeshToSkeleton(LODRenderData, BoneNodes, MeshRootNode);

        // Add the bind pose
        CreateBindPose(MeshRootNode);
    }

    //Re-bind skeleton to scene root if we have one
    if (SkeletonRootNode) {
        //TmpNodeNoTransform->RemoveChild(SkeletonRootNode);
        Scene->GetRootNode()->AddChild(SkeletonRootNode);
    }

    //Re-bind mesh to the scene root
    TmpNodeNoTransform->RemoveChild(MeshRootNode);
    Scene->GetRootNode()->AddChild(MeshRootNode);

    //Remove old temporary node
    Scene->GetRootNode()->RemoveChild(TmpNodeNoTransform);
    Scene->RemoveNode(TmpNodeNoTransform);

    //Export scene into the file
    const bool bResult = ExportFbxSceneToFileByPath(OutFileName, Scene, bExportAsText, (std::string*)OutErrorMessage);

    //Destroy FbxManager, which will also destroy all objects allocated by it
    FbxManager->Destroy();
    return (bool*)bResult;
}

void* ExportAnimSequenceIntoFbxFile(FAnimSequenceStruct* AnimSequenceData, char& OutFileName,
                                    bool bExportAsText, char* OutErrorMessage) {

}