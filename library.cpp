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
    //Scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::cm);
    Scene->GetGlobalSettings().SetTimeMode(FbxTime::eDefaultMode);

    return Scene;
}

bool ExportFbxSceneToFileByPath(std::string OutFileName, FbxScene* Scene, bool bExportAsText, std::string* OutErrorMessage) {
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

    const FbxString FbxFileName = OutFileName.c_str();
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

void ExportSkelMeshResources(const FVertexBufferGPUSkin& VertexBuffer,
                             const FSkelMeshColorVertexBuffer ColorVertexBuffer,
                             int NumTexCoords, FbxMesh* Mesh) {
    //Initialize vertices first
    const uint32_t NumVertices = VertexBuffer.NumVertices;

    Mesh->InitControlPoints(NumVertices);
    FbxVector4* ControlPoints = Mesh->GetControlPoints();

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector& SrcPosition = VertexBuffer.Verts[i].Pos;
        FbxVector4& DestPosition = ControlPoints[i];
        DestPosition = ConvertToFbxPos(SrcPosition);
    }

    std::cout << "initialized vertices" << std::endl;

    //Initialize vertex colors (if we have any)
    if (!ColorVertexBuffer.Colors.empty()) {
        if (ColorVertexBuffer.Colors.size() != NumVertices) return;

        FbxGeometryElementVertexColor* VertexColor = Mesh->CreateElementVertexColor();
        VertexColor->SetMappingMode(FbxLayerElement::eByControlPoint);
        VertexColor->SetReferenceMode(FbxLayerElement::eDirect);

        FbxLayerElementArrayTemplate<FbxColor>& ColorArray = VertexColor->GetDirectArray();
        ColorArray.AddMultiple(NumVertices);

        for (uint32_t i = 0; i < NumVertices; i++) {
            const FColor& SrcColor = ColorVertexBuffer.Colors[i];
            ColorArray.SetAt(i, ConvertToFbxColor(SrcColor));
        }
    }

    if (VertexBuffer.NumVertices != NumVertices) return;

    //Initialize normals
    FbxGeometryElementNormal* Normal = Mesh->CreateElementNormal();
    Normal->SetMappingMode(FbxLayerElement::eByControlPoint);
    Normal->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& NormalArray = Normal->GetDirectArray();
    NormalArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcNormal = VertexBuffer.Verts[i].Normal.VertexTangentZ.VertexTangent;
        FbxVector4 DestNormal = ConvertToFbxPos(SrcNormal);
        NormalArray.SetAt(i, DestNormal);
    }

    std::cout << "initialized normals" << std::endl;

    //Initialize tangents
    FbxGeometryElementTangent* Tangent = Mesh->CreateElementTangent();
    Tangent->SetMappingMode(FbxLayerElement::eByControlPoint);
    Tangent->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& TangentArray = Tangent->GetDirectArray();
    TangentArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcTangent = VertexBuffer.Verts[i].Normal.VertexTangentX.VertexTangent;
        FbxVector4 DestTangent = ConvertToFbxPos(SrcTangent);
        TangentArray.SetAt(i, DestTangent);
    }

    std::cout << "initialized tangents" << std::endl;

    //Initialize binomials
    FbxGeometryElementBinormal* Binomial = Mesh->CreateElementBinormal();
    Binomial->SetMappingMode(FbxLayerElement::eByControlPoint);
    Binomial->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& BinomialArray = Binomial->GetDirectArray();
    BinomialArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcBinomial = VertexBuffer.Verts[i].Normal.VertexTangentY.VertexTangent;
        FbxVector4 DestBinomial = ConvertToFbxPos(SrcBinomial);
        BinomialArray.SetAt(i, DestBinomial);
    }

    std::cout << "initialized binomials" << std::endl;

    //Initialize UV positions for each channel
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

    std::cout << "initialized uv" << std::endl;

    //Populate UV coords for each vertex
    for (uint32_t j = 0; j < NumTexCoords; j++) {
        FbxLayerElementArrayTemplate<FbxVector2>* UVArray = UVCoordsArray[j];
        for (uint32_t i = 0; i < NumVertices; i++) {
            const FVector2D& SrcTextureCoord = VertexBuffer.Verts[i].UV[j].UV;
            FbxVector2 DestUVCoord(SrcTextureCoord.X, -SrcTextureCoord.Y + 1.0f);
            UVArray->SetAt(i, DestUVCoord);
        }
    }

    std::cout << "populated uv" << std::endl;
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

void BindSkeletalMeshToSkeleton(const FStaticLODModel& SkeletalMeshLOD,
                                const std::vector<FbxNode*>& BoneNodes, FbxNode* MeshRootNode) {
    FbxScene* Scene = MeshRootNode->GetScene();
    FbxAMatrix MeshMatrix = MeshRootNode->EvaluateGlobalTransform();

    FbxGeometry* MeshAttribute = (FbxGeometry*) MeshRootNode->GetNodeAttribute();
    FbxSkin* Skin = FbxSkin::Create(Scene, "");

    const int BoneCount = BoneNodes.size();
    for (int BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex) {
        FbxNode* BoneNode = BoneNodes[BoneIndex];

        // Create the deforming cluster
        FbxCluster* CurrentCluster = FbxCluster::Create(Scene, "");
        CurrentCluster->SetLink(BoneNode);
        CurrentCluster->SetLinkMode(FbxCluster::eTotalOne);

        //We need to do it per-section because bone indices of vertex are local to the section they are contained in
        //So we can mesh bone index from section local bone index and then apply transform to this bone
        for (const FSkelMeshSection& RenderSection : SkeletalMeshLOD.Sections) {
            const uint32_t BaseVertexIndex = RenderSection.BaseVertexIndex;
            const uint32_t MaxVertexIndex = BaseVertexIndex + RenderSection.NumVertices;

            for (uint32_t VertexIndex = BaseVertexIndex; VertexIndex < MaxVertexIndex; VertexIndex++) {
                //Retrieve influences for this exact vertex
                FSkinWeightInfo WeightInfo = SkeletalMeshLOD.VertexBufferGPUSkin.Verts[VertexIndex].Influences;

                for (int InfluenceIndex = 0; InfluenceIndex < WeightInfo.BoneIndex.size(); InfluenceIndex++) {
                    const int InfluenceBone = RenderSection.BoneMap[WeightInfo.BoneIndex[InfluenceIndex]];
                    const float InfluenceWeight = WeightInfo.BoneWeight[InfluenceIndex] / 255.f;

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

void ExportStaticMeshResources(const FStaticMeshVertexBuffer& VertexBuffer,
                               const FPositionVertexBuffer& PositionVertexBuffer,
                               const FColorVertexBuffer& ColorVertexBuffer, FbxMesh* Mesh) {
    //Initialize vertices first
    const uint32_t NumVertices = PositionVertexBuffer.NumVertices;

    Mesh->InitControlPoints(NumVertices);
    FbxVector4* ControlPoints = Mesh->GetControlPoints();

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector& SrcPosition = PositionVertexBuffer.Verts[i];
        FbxVector4& DestPosition = ControlPoints[i];
        DestPosition = ConvertToFbxPos(SrcPosition);
    }

    std::cout << "initialized vertices" << std::endl;

    //Initialize vertex colors (if we have any)
    if (ColorVertexBuffer.NumVertices > 0) {
        if (ColorVertexBuffer.NumVertices != NumVertices) return;

        FbxGeometryElementVertexColor* VertexColor = Mesh->CreateElementVertexColor();
        VertexColor->SetMappingMode(FbxLayerElement::eByControlPoint);
        VertexColor->SetReferenceMode(FbxLayerElement::eDirect);

        FbxLayerElementArrayTemplate<FbxColor>& ColorArray = VertexColor->GetDirectArray();
        ColorArray.AddMultiple(NumVertices);

        for (uint32_t i = 0; i < NumVertices; i++) {
            const FColor& SrcColor = ColorVertexBuffer.Colors[i];
            ColorArray.SetAt(i, ConvertToFbxColor(SrcColor));
        }
    }

    if (VertexBuffer.NumVertices != NumVertices) return;

    //Initialize normals
    FbxGeometryElementNormal* Normal = Mesh->CreateElementNormal();
    Normal->SetMappingMode(FbxLayerElement::eByControlPoint);
    Normal->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& NormalArray = Normal->GetDirectArray();
    NormalArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcNormal = VertexBuffer.UV[i].VertexTangentZ.VertexTangent;
        FbxVector4 DestNormal = ConvertToFbxPos(SrcNormal);
        NormalArray.SetAt(i, DestNormal);
    }

    std::cout << "initialized normals" << std::endl;

    //Initialize tangents
    FbxGeometryElementTangent* Tangent = Mesh->CreateElementTangent();
    Tangent->SetMappingMode(FbxLayerElement::eByControlPoint);
    Tangent->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& TangentArray = Tangent->GetDirectArray();
    TangentArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcTangent = VertexBuffer.UV[i].VertexTangentX.VertexTangent;
        FbxVector4 DestTangent = ConvertToFbxPos(SrcTangent);
        TangentArray.SetAt(i, DestTangent);
    }

    std::cout << "initialized tangents" << std::endl;

    //Initialize binomials
    FbxGeometryElementBinormal* Binomial = Mesh->CreateElementBinormal();
    Binomial->SetMappingMode(FbxLayerElement::eByControlPoint);
    Binomial->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& BinomialArray = Binomial->GetDirectArray();
    BinomialArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcBinomial = VertexBuffer.UV[i].VertexTangentY.VertexTangent;
        FbxVector4 DestBinomial = ConvertToFbxPos(SrcBinomial);
        BinomialArray.SetAt(i, DestBinomial);
    }

    std::cout << "initialized binomials" << std::endl;

    //Initialize UV positions for each channel
    const uint32_t NumTexCoords = VertexBuffer.NumTexCoords;
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

    std::cout << "initialized uv" << std::endl;

    //Populate UV coords for each vertex
    for (uint32_t j = 0; j < NumTexCoords; j++) {
        FbxLayerElementArrayTemplate<FbxVector2>* UVArray = UVCoordsArray[j];
        for (uint32_t i = 0; i < NumVertices; i++) {
            const FVector2D& SrcTextureCoord = VertexBuffer.UV[i].UV[j].UV;
            FbxVector2 DestUVCoord(SrcTextureCoord.X, -SrcTextureCoord.Y + 1.0f);
            UVArray->SetAt(i, DestUVCoord);
        }
    }

    std::cout << "populated uv" << std::endl;
}

void ExportStaticMesh(FStaticMeshLODResources& StaticMeshLOD,
                      std::vector<FStaticMaterial> ReferencedMaterials, FbxMesh* Mesh) {
    //Initialize material element
    FbxGeometryElementMaterial* Material = Mesh->CreateElementMaterial();
    Material->SetMappingMode(FbxLayerElement::eByPolygon);
    Material->SetReferenceMode(FbxLayerElement::eIndexToDirect);

    std::cout << "initialised material element" << std::endl;

    //Create basic static mesh buffers
    ExportStaticMeshResources(StaticMeshLOD.VertexBuffer,
                              StaticMeshLOD.PositionVertexBuffer,
                              StaticMeshLOD.ColorVertexBuffer, Mesh);

    FRawStaticIndexBuffer& IndexBuffer = StaticMeshLOD.IndexBuffer;
    FbxNode* MeshNode = Mesh->GetNode();

    std::cout << "created mesh node" << std::endl;

    // print num sections
    std::cout << "num sections: " << StaticMeshLOD.Sections.size() << std::endl;

    //Create sections and initialize dummy materials
    for (const FStaticMeshSection& MeshSection : StaticMeshLOD.Sections) {
        const uint32_t NumTriangles = MeshSection.NumTriangles;
        const uint32_t StartIndex = MeshSection.FirstIndex;

        // print num triangles
        std::cout << "num triangles: " << NumTriangles << std::endl;

        //Create dummy material for this section
        const std::string MaterialSlotName = ReferencedMaterials[MeshSection.MaterialIndex].MaterialSlotName;
        const int32_t MaterialIndex = ExportDummyMaterialIntoFbxScene(MaterialSlotName, MeshNode);

        //Add all triangles associated with this section
        for (uint32_t TriangleIndex = 0; TriangleIndex < NumTriangles; TriangleIndex++) {
            // print for every 10000 triangles
            if (TriangleIndex % 10000 == 0) {
                std::cout << "triangle index: " << TriangleIndex << std::endl;
            }
            Mesh->BeginPolygon(MaterialIndex, -1, -1, false);
            Mesh->AddPolygon(GetIndex(IndexBuffer, StartIndex + TriangleIndex * 3 + 0));
            Mesh->AddPolygon(GetIndex(IndexBuffer, StartIndex + TriangleIndex * 3 + 1));
            Mesh->AddPolygon(GetIndex(IndexBuffer, StartIndex + TriangleIndex * 3 + 2));
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
        FbxString BoneName = CurrentBone.Name.c_str();
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
        const FVector UnrealRotation = Euler(BoneTransform.Rotation);
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
        BoneNodes[BoneIndex] = BoneNode;
    }

    return BoneNodes[0];
}

void ExportSkeletalMesh(const FStaticLODModel& SkeletalMeshLOD,
                        const std::vector<FSkeletalMaterial>& ReferencedMaterials, FbxMesh* FbxMesh) {
    //Initialize material element
    FbxGeometryElementMaterial* Material = FbxMesh->CreateElementMaterial();
    Material->SetMappingMode(FbxLayerElement::eByPolygon);
    Material->SetReferenceMode(FbxLayerElement::eIndexToDirect);

    //Create basic static mesh buffers
    ExportSkelMeshResources(SkeletalMeshLOD.VertexBufferGPUSkin,
                            SkeletalMeshLOD.ColorVertexBuffer,
                            SkeletalMeshLOD.NumTexCoords, FbxMesh);

    FRawStaticIndexBuffer IndexBuffer = SkeletalMeshLOD.Indices;
    FbxNode* MeshNode = FbxMesh->GetNode();

    //Create sections and initialize dummy materials
    for (const FSkelMeshSection& MeshSection : SkeletalMeshLOD.Sections) {
        const uint32_t NumTriangles = MeshSection.NumTriangles;
        const uint32_t StartVertexIndex = MeshSection.BaseIndex;

        //Create dummy material for this section
        const std::string MaterialSlotName = ReferencedMaterials[MeshSection.MaterialIndex].MaterialSlotName;
        const int MaterialIndex = ExportDummyMaterialIntoFbxScene(MaterialSlotName, MeshNode);

        //Add all triangles associated with this section
        for (uint32_t TriangleIndex = 0; TriangleIndex < NumTriangles; TriangleIndex++) {
            FbxMesh->BeginPolygon(MaterialIndex, -1, -1, false);
            FbxMesh->AddPolygon(GetIndex(IndexBuffer, StartVertexIndex + TriangleIndex * 3 + 0));
            FbxMesh->AddPolygon(GetIndex(IndexBuffer, StartVertexIndex + TriangleIndex * 3 + 1));
            FbxMesh->AddPolygon(GetIndex(IndexBuffer, StartVertexIndex + TriangleIndex * 3 + 2));
            FbxMesh->EndPolygon();
        }
    }
}

void* ExportStaticMeshIntoFbxFile(char* StaticMeshJson, char* OutFileName,
                                  bool bExportAsText, char* OutErrorMessage) {
    FStaticMeshStruct StaticMeshData = JsonDeserializer::DeserializeSM(StaticMeshJson);

    FbxManager* FbxManager = AllocateFbxManagerForExport();
    if (!FbxManager) return nullptr;

    //Create root scene which we will use to export mesh
    FbxScene* Scene = CreateFbxSceneForFbxManager(FbxManager);
    if (!Scene) return nullptr;

    //Create mesh object
    const FbxString MeshNodeName = StaticMeshData.Name.c_str();
    FbxNode* MeshNode = FbxNode::Create(Scene, MeshNodeName);
    FbxMesh* OutExportedMesh = FbxMesh::Create(Scene, MeshNodeName);
    int out = OutExportedMesh->RemoveBadPolygons();
    MeshNode->SetNodeAttribute(OutExportedMesh);

    std::cout << "created mesh object" << std::endl;

    ExportStaticMesh(StaticMeshData.RenderData.LODs[0], StaticMeshData.StaticMaterials, OutExportedMesh);

    std::cout << "exported static mesh" << std::endl;

    Scene->GetRootNode()->AddChild(MeshNode);

    // TODO: Finish GenerateSmoothingGroups
    /*FbxNode* RootNode = Scene->GetRootNode();
    GenerateSmoothingGroups::GetNormals(RootNode);
    GenerateSmoothingGroups::GetSmoothing(FbxManager, RootNode, true);*/

    //Export scene into the file
    const bool bResult = ExportFbxSceneToFileByPath(OutFileName, Scene, bExportAsText, (std::string*)OutErrorMessage);

    std::cout << "export fbx to file" << std::endl;

    //Destroy FbxManager, which will also destroy all objects allocated by it
    FbxManager->Destroy();
    return (bool*)bResult;
}

void* ExportSkeletonIntoFbxFile(char* SkeletonJson, char* OutFileName,
                                bool bExportAsText, char* OutErrorMessage) {
    FSkeletonStruct SkeletonData;
    SkeletonData.Skeleton = JsonDeserializer::DeserializeSK(SkeletonJson, "Skeleton");

    FbxManager* FbxManager = AllocateFbxManagerForExport();
    if (!FbxManager) return nullptr;

    //Create root scene which we will use to export mesh
    FbxScene* Scene = CreateFbxSceneForFbxManager(FbxManager);
    if (!Scene) return nullptr;

    // Add the skeleton to the scene
    std::vector<FbxNode*> BoneNodes;
    FbxNode* SkeletonRootNode = ExportSkeleton(Scene, SkeletonData.Skeleton, BoneNodes);
    Scene->GetRootNode()->AddChild(SkeletonRootNode);

    //Export scene into the file
    const bool bResult = ExportFbxSceneToFileByPath(OutFileName, Scene, bExportAsText, (std::string*)OutErrorMessage);

    //Destroy FbxManager, which will also destroy all objects allocated by it
    FbxManager->Destroy();
    return (bool*)bResult;
}

void* ExportSkeletalMeshIntoFbxFile(char* SkeletalMeshJson, char* OutFileName,
                                    bool bExportAsText, char* OutErrorMessage) {
    FSkeletalMeshStruct SkeletalMeshData = JsonDeserializer::DeserializeSKM(SkeletalMeshJson);
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
    FbxNode* TmpNodeNoTransform = FbxNode::Create(Scene, FbxNodeName.c_str());
    Scene->GetRootNode()->AddChild(TmpNodeNoTransform);

    // Add the skeleton to the scene
    std::vector<FbxNode*> BoneNodes;
    FbxNode* SkeletonRootNode = ExportSkeleton(Scene, SkeletalMeshData.RefSkeleton, BoneNodes);
    if(SkeletonRootNode) {
        TmpNodeNoTransform->AddChild(SkeletonRootNode);
    }

    //Create mesh from first LOD of the skeletal mesh
    FStaticLODModel& LODRenderData = SkeletalMeshData.LODModels[0];

    const FbxString MeshNodeName = SkeletalMeshData.Name.c_str();
    FbxNode* MeshRootNode = FbxNode::Create(Scene, MeshNodeName);
    FbxMesh* ExportedMesh = FbxMesh::Create(Scene, MeshNodeName);
    MeshRootNode->SetNodeAttribute(ExportedMesh);

    //Populate basic mesh information
    ExportSkeletalMesh(LODRenderData, SkeletalMeshData.Materials, ExportedMesh);

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
    return (bool*) false;
}