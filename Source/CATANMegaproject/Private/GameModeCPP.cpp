// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeCPP.h"
#include "CatanPlayerState.h"
#include "BoardManager.h"
#include "HexTile.h"
#include "HexVertex.h"

// Beginplay
void AGameModeCPP::BeginPlay()
{
    Super::BeginPlay();
    // Spawn the BoardManager
    if (BoardManagerClass)
    {
        BoardManager = GetWorld()->SpawnActor<ABoardManager>(BoardManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
    }
    // Initialize HexTiles from BoardManager
    if (BoardManager)
    {
        HexTiles = BoardManager->GetHexTiles();
    }
    // Get all players
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ACatanPlayerState* PS = It->Get()->GetPlayerState<ACatanPlayerState>())
        {
            Players.Add(PS);
        }
    }
}

void AGameModeCPP::DistributeResources(int32 DiceRoll)
{
    TArray<AHexTile*> RelevantTiles;
    for (AHexTile* Tile : BoardManager->GetHexTiles())
    {
        if (Tile && Tile->GetNumberToken() == DiceRoll)
        {
            RelevantTiles.Add(Tile);
        }
    }
    for (AHexTile* Tile : RelevantTiles)
    {
        // loop through vertices
        for (AHexVertex* Vertex : Tile->Vertices)
        {
            if (Vertex && Vertex->isOccupied)
            {
                if (Vertex->settlementType == ESettlementType::Settlement)
                {
                    // Add 1 resource to the player
                    for (ACatanPlayerState* Player : Players)
                    {
                        if (Player && Player->PlayerColor == Vertex->occupiedBy)
                        {
                            Player->AddResource(Tile->HexTypeToResource(), 1);
                        }
                        UE_LOG(LogTemp, Warning, TEXT("Player %d received %d of resource %d"), 
                        (int32)Player->PlayerColor, 1, (int32)Tile->HexTypeToResource());
                    }
                }
                else if (Vertex->settlementType == ESettlementType::City)
                {
                    // Add 2 resources to the player
                    for (ACatanPlayerState* Player : Players)
                    {
                        if (Player && Player->PlayerColor == Vertex->occupiedBy)
                        {
                            Player->AddResource(Tile->HexTypeToResource(), 2);
                        }
                        UE_LOG(LogTemp, Warning, TEXT("Player %d received %d of resource %d"), 
                        (int32)Player->PlayerColor, 2, (int32)Tile->HexTypeToResource());
                    }
                }
            }
        }
    }
    
}

void AGameModeCPP::RollDice()
{
    int32 Dice1 = FMath::RandRange(1, 6);
    int32 Dice2 = FMath::RandRange(1, 6);
    int32 Total = Dice1 + Dice2;
    UE_LOG(LogTemp, Log, TEXT("Rolled: %d + %d = %d"), Dice1, Dice2, Total);
    DistributeResources(Total);

}

