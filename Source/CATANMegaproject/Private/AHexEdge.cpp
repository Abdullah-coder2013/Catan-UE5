// Fill out your copyright notice in the Description page of Project Settings.


#include "AHexEdge.h"

#include "GameModeCPP.h"
#include "HexVertex.h"
#include "HexTile.h"

// Sets default values
AAHexEdge::AAHexEdge()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EdgeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EdgeMesh"));
	RootComponent = EdgeMesh;
}

// Called when the game starts or when spawned
void AAHexEdge::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AAHexEdge::TryPlaceRoad(EPlayerColor PlayerColor)
{
	if (isOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("Edge already occupied!"));
		return false;
	}
	if (PlayerColor == EPlayerColor::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid player color!"));
		return false;
	}
	bool bAdjacentVertexOwnedByPlayer = false;
	for (AHexVertex* AdjVertex : AdjacentVertices)
	{
		if (AdjVertex && AdjVertex->isOccupied && AdjVertex->occupiedBy == (EPlayerColor)PlayerColor)
		{
			bAdjacentVertexOwnedByPlayer = true;
			break;
		}
	}
	bool bAdjacentEdgeOwnedByPlayer = false;
	for (AAHexEdge* AdjEdge : AdjacentEdges)
	{
		if (AdjEdge && AdjEdge->isOccupied && AdjEdge->occupiedBy == PlayerColor)
		{
			// Find the shared vertex between this edge and AdjEdge
			for (AHexVertex* SharedVertex : AdjacentVertices)
			{
				if (AdjEdge->AdjacentVertices.Contains(SharedVertex))
				{
					// Check if opponent is blocking this vertex
					bool bBlockedByOpponent = SharedVertex->isOccupied && 
											SharedVertex->occupiedBy != PlayerColor &&
											SharedVertex->occupiedBy != EPlayerColor::None;
					if (!bBlockedByOpponent)
					{
						bAdjacentEdgeOwnedByPlayer = true;
					}
					break;
				}
			}
		}
	}
	bool bCanPlaceRoad = (bAdjacentVertexOwnedByPlayer || bAdjacentEdgeOwnedByPlayer);
	UE_LOG(LogTemp, Warning, TEXT("Can place road: %s"), bCanPlaceRoad ? TEXT("Yes") : TEXT("No"));
	if (bCanPlaceRoad)
	{
		occupiedBy = PlayerColor;
		isOccupied = true;
		UE_LOG(LogTemp, Warning, TEXT("Placed road for player %d"), (int32)occupiedBy);
	}
	AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
	if (GameMode->GetPlayerByColor(PlayerColor).RoadsPlaced >= 15)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %d has already placed the maximum number of roads!"), (int32)PlayerColor);
		return false;
	}
	if (bCanPlaceRoad)
	{
		
		FPlayerData Player = GameMode->GetPlayerByColor(PlayerColor);
		Player.IncreaseRoadsPlaced(1);
	}
	return bCanPlaceRoad;
}

void AAHexEdge::InitializeEdge(TArray<AHexVertex*> InAdjacentVertices)
{
	AdjacentVertices = InAdjacentVertices;
}

// Called every frame
void AAHexEdge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

