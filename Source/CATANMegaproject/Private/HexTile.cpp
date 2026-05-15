// Fill out your copyright notice in the Description page of Project Settings.


#include "HexTile.h"
#include "HexVertex.h"

// Sets default values
AHexTile::AHexTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HexMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HexMesh"));
	RootComponent = HexMesh;
}

// Called when the game starts or when spawned
void AHexTile::BeginPlay()
{
	Super::BeginPlay();
	HexMesh->SetVisibility(false);
}

void AHexTile::InitializeHex(EHexType Type, int32 Token, int32 InQ, int32 InR, float HexSizeIn, bool bInitialized)
{
	HexType = Type;
	NumberToken = Token;
	Q = InQ;
	R = InR;
	isInitialized = bInitialized;
	BaseElevation = GetBaseElevationForType(Type);
	HexSize = HexSizeIn;
}



void AHexTile::PlaceRobber(AActor* RobberActor)
{
	if (RobberActor)
	{
		RobberActor->SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, 50.f)); // Adjust Z as needed
		bHasRobber = true;
	}
}

void AHexTile::SetFoliageVisible(bool bVisible)
{
	if (bFoliageVisible == bVisible) return; // no change, skip
	bFoliageVisible = bVisible;

	for (UHierarchicalInstancedStaticMeshComponent* HISM : TrackedHISMs)
	{
		if (HISM) HISM->SetVisibility(bVisible);
	}
}

void AHexTile::ClearFoliage()
{
	for (UHierarchicalInstancedStaticMeshComponent* HISM : TrackedHISMs)
	{
		if (HISM) HISM->ClearInstances();
	}
}

EResourceType AHexTile::HexTypeToResource() const
{
	switch (static_cast<uint8>(HexType))
	{
	case static_cast<uint8>(EHexType::Desert):
		return EResourceType::None;
	case static_cast<uint8>(EHexType::Forest):
		return EResourceType::Wood;
	case static_cast<uint8>(EHexType::Mountain):
		return EResourceType::Ore;
	case static_cast<uint8>(EHexType::Pasture):
		return EResourceType::Sheep;
	case static_cast<uint8>(EHexType::Fields):
		return EResourceType::Wheat;
	case static_cast<uint8>(EHexType::Hill):
		return EResourceType::Brick;
	default:
		return EResourceType::None;
	}
}

UHierarchicalInstancedStaticMeshComponent* AHexTile::GetOrCreateHISM(UStaticMesh* Mesh, EFoliageSize DetailLevel)
{
	if (!Mesh) return nullptr;
    
	if (MeshHISMs.Contains(Mesh))
		return MeshHISMs[Mesh];

	UHierarchicalInstancedStaticMeshComponent* HISM = 
		NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
    
	HISM->SetStaticMesh(Mesh);
	HISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HISM->bAutoRebuildTreeOnInstanceChanges = false;
	HISM->RegisterComponent();
	for (int32 i = 0; i < Mesh->GetStaticMaterials().Num(); i++)
	{
		HISM->SetMaterial(i, Mesh->GetStaticMaterials()[i].MaterialInterface);
	}
	switch (DetailLevel)
	{
		case EFoliageSize::SmallF:
			HISM->SetCullDistances(0.f, 5000.f);
			break;
		case EFoliageSize::MediumF:
			HISM->SetCullDistances(0.f, 15000.f);
			break;
		case EFoliageSize::LargeF:	
			HISM->SetCullDistances(0.f, 50000.f);
			break;
		 default:
			HISM->SetCullDistances(2000.f, 4000.f);
			 break;
	}
	
	HISM->AttachToComponent(
		RootComponent,
		FAttachmentTransformRules::KeepRelativeTransform);
	HISM->SetMobility(EComponentMobility::Static);
	MeshHISMs.Add(Mesh, HISM);
	TrackedHISMs.Add(HISM);
	return HISM;
}

void AHexTile::SpawnHISM(UStaticMesh* Mesh, FTransform* SpawnTransform, EFoliageSize DetailLevel)
{
	if (UHierarchicalInstancedStaticMeshComponent* HISM = GetOrCreateHISM(Mesh, DetailLevel))
	{
		HISM->AddInstance(SpawnTransform ? *SpawnTransform : FTransform::Identity);
	}
}

float AHexTile::GetHexOuterRadius()
{
	// HexSize = HexOuterRadius^2
	return HexSize;
}

// Called every frame
void AHexTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

