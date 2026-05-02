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

UHierarchicalInstancedStaticMeshComponent* AHexTile::GetOrCreateHISM(UStaticMesh* Mesh)
{
	if (!Mesh) return nullptr;
    
	if (MeshHISMs.Contains(Mesh))
		return MeshHISMs[Mesh];

	UHierarchicalInstancedStaticMeshComponent* HISM = 
		NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
    
	HISM->SetStaticMesh(Mesh);
	HISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HISM->RegisterComponent();
	HISM->AttachToComponent(
		RootComponent,
		FAttachmentTransformRules::KeepRelativeTransform);

	MeshHISMs.Add(Mesh, HISM);
	TrackedHISMs.Add(HISM);
	return HISM;
}

void AHexTile::SpawnHISM(UStaticMesh* Mesh, FTransform* SpawnTransform)
{
	if (UHierarchicalInstancedStaticMeshComponent* HISM = GetOrCreateHISM(Mesh))
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

