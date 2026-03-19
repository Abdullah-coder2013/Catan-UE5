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
	
}

void AHexTile::InitializeHex(EHexType Type, int32 Token, int32 InQ, int32 InR, bool bInitialized)
{
	HexType = Type;
	NumberToken = Token;
	Q = InQ;
	R = InR;
	isInitialized = bInitialized;
}

// Called every frame
void AHexTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

