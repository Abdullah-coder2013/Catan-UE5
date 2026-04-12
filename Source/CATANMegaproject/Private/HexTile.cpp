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

void AHexTile::InitializeHex(EHexType Type, int32 Token, int32 InQ, int32 InR, bool bInitialized)
{
	HexType = Type;
	NumberToken = Token;
	Q = InQ;
	R = InR;
	isInitialized = bInitialized;
	BaseElevation = GetBaseElevationForType(Type);
}



void AHexTile::PlaceRobber(AActor* RobberActor)
{
	if (RobberActor)
	{
		RobberActor->SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, 50.f)); // Adjust Z as needed
		bHasRobber = true;
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

// Called every frame
void AHexTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

