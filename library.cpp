#include "library.h"

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

    //Initialize vertex colors (if we have any) TODO: Implement FColor so this doesn't have to be skipped lol
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

// check(VertexBuffers.StaticMeshVertexBuffer.GetNumVertices() == NumVertices);

    //Initialize normals
    FbxGeometryElementNormal* Normal = Mesh->CreateElementNormal();
    Normal->SetMappingMode(FbxLayerElement::eByControlPoint);
    Normal->SetReferenceMode(FbxLayerElement::eDirect);

    FbxLayerElementArrayTemplate<FbxVector4>& NormalArray = Normal->GetDirectArray();
    NormalArray.AddMultiple(NumVertices);

    for (uint32_t i = 0; i < NumVertices; i++) {
        const FVector4 SrcNormal = VertexBuffers.StaticMeshVertexBuffer.VertexTangent.Normal[i].Z;
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
        const FVector4 SrcTangent = VertexBuffers.StaticMeshVertexBuffer.VertexTangentX(i);
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
        const FVector4 SrcBinormal = VertexBuffers.StaticMeshVertexBuffer.VertexTangentY(i);
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
            const FVector2D& SrcTextureCoord = VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, j);
            FbxVector2 DestUVCoord(SrcTextureCoord.X, -SrcTextureCoord.Y + 1.0f);
            UVArray->SetAt(i, DestUVCoord);
        }
    }
}

void ExportStaticMesh(const FStaticMeshLODResources& StaticMeshLOD, FStaticMaterial ReferencedMaterials[], FbxMesh* Mesh) {
    //Initialize material element
    FbxGeometryElementMaterial* Material = Mesh->CreateElementMaterial();
    Material->SetMappingMode(FbxLayerElement::eByPolygon);
    Material->SetReferenceMode(FbxLayerElement::eIndexToDirect);

    //Create basic static mesh buffers
    ExportCommonMeshResources(StaticMeshLOD.VertexBuffers, Mesh);

    const FRawStaticIndexBuffer& IndexBuffer = StaticMeshLOD.IndexBuffer;
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

void* ExportStaticMeshIntoFbxFile(FStaticMeshStruct *StaticMeshData, const char *OutFileName,
                                  bool bExportAsText, const char *OutErrorMessage) {
    FbxManager* FbxManager = AllocateFbxManagerForExport();
//    check(FbxManager);

    //Create root scene which we will use to export mesh
    FbxScene* Scene = CreateFbxSceneForFbxManager(FbxManager);

    //Create mesh object
    const FbxString MeshNodeName = StaticMeshData->name;
    FbxNode* MeshNode = FbxNode::Create(Scene, MeshNodeName);
    FbxMesh* OutExportedMesh = FbxMesh::Create(Scene, MeshNodeName);
    MeshNode->SetNodeAttribute(OutExportedMesh);

    FStaticMeshLODResources& LODResources = StaticMesh->RenderData->LODResources[0];
    ExportStaticMesh(LODResources, StaticMesh->StaticMaterials, OutExportedMesh);

    Scene->GetRootNode()->AddChild(MeshNode);

    //Export scene into the file
    const bool bResult = ExportFbxSceneToFileByPath(OutFileName, Scene, bExportAsText, OutErrorMessage);

    //Destroy FbxManager, which will also destroy all objects allocated by it
    FbxManager->Destroy();
    return bResult;
}