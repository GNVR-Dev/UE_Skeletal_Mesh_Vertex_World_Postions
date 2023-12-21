// Copyright Epic Games, Inc. All Rights Reserved.

#include "BPLibraryFuncs.h"
#include "SkeletalRenderPublic.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "GameFramework/PawnMovementComponent.h"

UBPLibraryFuncs::UBPLibraryFuncs(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

// float UBPLibraryFuncs::BP_LibrarySampleFunction(float Param)
// {
// 	return -1;
// }

bool UBPLibraryFuncs::BP_GetSkeletalMeshVertexLocations(USkeletalMeshComponent* Mesh, TArray<FVector3f>& Locations, int32 LODIndex)
{
	if (!Mesh || !Mesh->SkeletalMesh) return false;

	//CPU or GPU skinning
	if (Mesh->GetCPUSkinningEnabled()) {
		TArray<FFinalSkinVertex> SkinnedVertices;
		Mesh->GetCPUSkinnedVertices(SkinnedVertices, LODIndex);
		Locations.Empty();
		Locations.AddUninitialized(SkinnedVertices.Num());
		for (int32 i = 0; i < SkinnedVertices.Num(); ++i) {
			Locations[i] = SkinnedVertices[i].Position;
		}
	} else {
		const FSkeletalMeshRenderData* RenderData = Mesh->GetSkeletalMeshRenderData();
		if (!RenderData) return false;
		const FSkeletalMeshLODRenderData& LOD = RenderData->LODRenderData[LODIndex];
		const FSkinWeightVertexBuffer& Buffer = LOD.SkinWeightVertexBuffer;
		TArray<FMatrix44f> CacheToLocals;
		Mesh->GetCurrentRefToLocalMatrices(CacheToLocals, LODIndex);
		Mesh->ComputeSkinnedPositions(Mesh, Locations, CacheToLocals, LOD, Buffer);
	}

	//LocalToWorld
	const FTransform ToWorld = Mesh->GetComponentTransform();
	for (FVector3f& EachVertex : Locations) {
		EachVertex = static_cast<FVector3f>(ToWorld.TransformPosition(FVector(EachVertex.X, EachVertex.Y, EachVertex.Z)));
	}

	return true;
}

void UBPLibraryFuncs::BP_SetCPUSkinningEnabled(USkinnedMeshComponent* Mesh, bool bEnable, bool bRecreateRenderStateImmediately)
{
	Mesh->SetCPUSkinningEnabled(bEnable, bRecreateRenderStateImmediately);
}
