// Fill out your copyright notice in the Description page of Project Settings.


#include "HexVertex.h"
#include "HexTile.h"

// Sets default values
AHexVertex::AHexVertex()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	VertexMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VertexMesh"));
	RootComponent = VertexMesh;

}

// Called when the game starts or when spawned
void AHexVertex::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AHexVertex::TryPlaceSettlement(EPlayerColor PlayerColor, ESettlementType Type, EGamePhase CurrentPhase)
{
	if (isOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("Vertex already occupied!"));
		return false;
	}
	if (Type == ESettlementType::Empty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid settlement type!"));
		return false;
	}
	for (AHexVertex* AdjVertex : AdjacentVertices)
	{
		if (AdjVertex && AdjVertex->isOccupied)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot place settlement next to an occupied vertex!"));
			return false;
		}
	}
	for (const AAHexEdge* AdjEdge : AdjacentEdges)
	{
		if (CurrentPhase == EGamePhase::Setup)
		{
			continue; // Ignore edge occupation during setup phase
		}
		if ((AdjEdge && !AdjEdge->isOccupied && AdjEdge->occupiedBy != PlayerColor) || !AdjEdge)
		{
			UE_LOG(LogTemp, Warning, TEXT("Edge not occupied!"));
			return false;
		}
	}

	occupiedBy = PlayerColor;
	settlementType = Type;
	isOccupied = true;

	UE_LOG(LogTemp, Warning, TEXT("Placed %s for player %d"), *UEnum::GetValueAsString(settlementType), (int32)occupiedBy);
	return true;
}

bool AHexVertex::TryPlaceCity(EPlayerColor PlayerColor, EGamePhase CurrentPhase)
{
	if (!isOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("Vertex not occupied!"));
		return false;
	}
	if (CurrentPhase == EGamePhase::Setup)
	{
		return false;
	}
	if (isOccupied && PlayerColor != occupiedBy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot upgrade to city - vertex occupied by another player!"));
		return false;
	}
	
	occupiedBy = PlayerColor;
	settlementType = ESettlementType::City;
	isOccupied = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Upgraded to city for player %d"), (int32)occupiedBy);
	return true;
}


void AHexVertex::InitializeVertex(TArray<AHexTile*> InAdjacentHexes)
{
	AdjacentHexes = InAdjacentHexes;
	occupiedBy = EPlayerColor::None;
	settlementType = ESettlementType::Empty;
	isOccupied = false;
}

// Called every frame
void AHexVertex::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

