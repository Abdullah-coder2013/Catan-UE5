// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeCPP.h"
#include "BoardManager.h"
#include "HexTile.h"
#include "CatanPlayerController.h"
#include "HexVertex.h"
#include "DebugUserWidget.h"
#include "Blueprint/UserWidget.h"

// Beginplay
void AGameModeCPP::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(DebugWidgetClass))
    {
        DebugWidget = Cast<UDebugUserWidget>(CreateWidget(GetWorld(), DebugWidgetClass));
        if (DebugWidget)
        {
            DebugWidget->AddToViewport();
        }
    }
    
    
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
    // Initialize default player colors if not set
    if (DefaultColors.Num() == 0)
    {
        DefaultColors = { EPlayerColor::Red, EPlayerColor::Blue, EPlayerColor::White, EPlayerColor::Orange };
    }
    StartGame();
}

void AGameModeCPP::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (DebugWidget)
    {
        DebugWidget->SetCurrentPlayerUI(GetCurrentPlayer().PlayerColor);
        DebugWidget->SetCurrentTurnStepUI(CurrentTurnStep, CurrentSetupStep, CurrentPhase);
        DebugWidget->SetCurrentPhaseUI(CurrentPhase);
        DebugWidget->SetResourceUI(GetCurrentPlayer().Resources);
    }
}

AGameModeCPP::AGameModeCPP()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGameModeCPP::InitializePlayers(TArray<EPlayerColor> PlayerColors)
{
    Players.Empty();
    for (EPlayerColor Color : PlayerColors)
    {
        Players.Add(FPlayerData(Color));
    }
}

FPlayerData& AGameModeCPP::GetCurrentPlayer()
{
    return Players[CurrentPlayerIndex];
}

void AGameModeCPP::AdvanceSetup(bool bFirstTimeCalled) {
    // Switch between Settlement and Road placement
    if (CurrentSetupStep == EPlacementNode::Settlement) {
        CurrentSetupStep = EPlacementNode::Road;
    }
    else if (CurrentSetupStep == EPlacementNode::Road) {
        CurrentSetupStep = EPlacementNode::Settlement;
        if (bFirstTimeCalled)
        {
            return;
        }
        // Move to next player after completing both settlement and road
        if (!bSetupReversed && CurrentPlayerIndex < PlayerOrder.Num() - 1) {
            CurrentPlayerIndex++;
        }
        else if (!bSetupReversed && CurrentPlayerIndex == PlayerOrder.Num() - 1) {
            bSetupReversed = true;
        }
        else if (bSetupReversed && CurrentPlayerIndex > 0) {
            CurrentPlayerIndex--;
        }
        else if (bSetupReversed && CurrentPlayerIndex == 0) {
            bSetupReversed = false;
            CurrentPhase = EGamePhase::MainGame;
            ChangePlayer();
            StartMainGame();
            return;
        }
        ChangePlayer();
        return;
    }
}

void AGameModeCPP::ChangePlayer()
{
    ACatanPlayerController* PC = Cast<ACatanPlayerController>(GetWorld()->GetFirstPlayerController());
    if (PC)
    {
        PC->ChangePlayer(GetCurrentPlayer());
        PC->CurrentPlacementNode = CurrentSetupStep;
    }
}

void AGameModeCPP::StartMainGame()
{
    CurrentPhase = EGamePhase::MainGame;
    CurrentTurnStep = ETurnStep::RollDice;
    // AdvanceStep();
}

void AGameModeCPP::EndTurn() {
    if (CurrentPhase == EGamePhase::Setup) {
        AdvanceSetup(false);
    }
    else if (CurrentPhase == EGamePhase::MainGame) {
        AdvanceTurn();
    }
    ChangePlayer();
}

void AGameModeCPP::AdvanceStep() {
    UE_LOG(LogTemp, Warning, TEXT("AdvanceStep called. CurrentTurnStep: %d"), (int32)CurrentTurnStep);
    if (CurrentTurnStep == ETurnStep::RollDice) {
        RollDice();
        CurrentTurnStep = ETurnStep::DynamicEnvironment;
    }
    else if (CurrentTurnStep == ETurnStep::DynamicEnvironment) {
        CurrentTurnStep = ETurnStep::RollDice;
        EndTurn();
    }
}

void AGameModeCPP::AdvanceTurn() {
    CurrentPlayerIndex = (CurrentPlayerIndex + 1) % PlayerOrder.Num();
    CurrentTurnStep = ETurnStep::RollDice;
}

void AGameModeCPP::StartGame() {
    CurrentPhase = EGamePhase::Setup;
    CurrentTurnStep = ETurnStep::None;
    CurrentPlayerIndex = 0;
    PlayerOrder = DefaultColors;
    bSetupReversed = false;
    CurrentSetupStep = EPlacementNode::Road;
    InitializePlayers(DefaultColors);
    AdvanceSetup(true);
}

void AGameModeCPP::DistributeResources(int32 DiceRoll)
{
    UE_LOG(LogTemp, Warning, TEXT("DistributeResources called with DiceRoll: %d"), DiceRoll);
    UE_LOG(LogTemp, Warning, TEXT("HexTiles count: %d, Players count: %d"), HexTiles.Num(), Players.Num());

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
        UE_LOG(LogTemp, Warning, TEXT("Processing tile: %s, Token: %d"), *Tile->GetName(), Tile->GetNumberToken());
        // loop through vertices
        for (AHexVertex* Vertex : Tile->Vertices)
        {
            UE_LOG(LogTemp, Warning, TEXT("Vertex %s - isOccupied: %d, occupiedBy: %d, settlementType: %d"),
                *Vertex->GetName(), Vertex->isOccupied, (int32)Vertex->occupiedBy, (int32)Vertex->settlementType);
            if (Vertex && Vertex->isOccupied)
            {
                if (Vertex->settlementType == ESettlementType::Settlement)
                {
                    // Add 1 resource to the player
                    for (FPlayerData& Player : Players)
                    {
                        if (Player.PlayerColor == Vertex->occupiedBy)
                        {
                            Player.AddResource(Tile->HexTypeToResource(), 1);
                        }
                        UE_LOG(LogTemp, Warning, TEXT("Player %d received %d of resource %d"),
                        (int32)Player.PlayerColor, 1, (int32)Tile->HexTypeToResource());
                    }
                }
                else if (Vertex->settlementType == ESettlementType::City)
                {
                    // Add 2 resources to the player
                    for (FPlayerData& Player : Players)
                    {
                        if (Player.PlayerColor == Vertex->occupiedBy)
                        {
                            Player.AddResource(Tile->HexTypeToResource(), 2);
                        }
                        UE_LOG(LogTemp, Warning, TEXT("Player %d received %d of resource %d"),
                        (int32)Player.PlayerColor, 2, (int32)Tile->HexTypeToResource());
                    }
                }
            }
        }
    }
    for (FPlayerData& Player : Players)
    {
        UE_LOG(LogTemp, Log, TEXT("Player %d now has: Lumber=%d, Brick=%d, Wool=%d, Grain=%d, Ore=%d"),
        (int32)Player.PlayerColor,
        Player.GetResourceAmount(EResourceType::Wood),
        Player.GetResourceAmount(EResourceType::Brick),
        Player.GetResourceAmount(EResourceType::Sheep),
        Player.GetResourceAmount(EResourceType::Wheat),
        Player.GetResourceAmount(EResourceType::Ore));
    }
    UE_LOG(LogTemp, Log, TEXT("Resource distribution complete for dice roll: %d"), DiceRoll);

}

void AGameModeCPP::RollDice()
{
    int32 Dice1 = FMath::RandRange(1, 6);
    int32 Dice2 = FMath::RandRange(1, 6);
    int32 Total = Dice1 + Dice2;
    UE_LOG(LogTemp, Log, TEXT("Rolled: %d + %d = %d"), Dice1, Dice2, Total);
    DistributeResources(Total);

}

