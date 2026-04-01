// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeCPP.h"
#include "BoardManager.h"
#include "HexTile.h"
#include "CatanPlayerController.h"
#include "HexVertex.h"
#include "DebugUserWidget.h"
#include "Blueprint/UserWidget.h"

void AGameModeCPP::ChangeRules(EBuildable ruleType, bool value)
{
    switch (ruleType)
    {
        case EBuildable::Settlement:
            bShouldPlaceSettlement = value;
            bShouldPlaceCity = false; 
            bShouldPlaceRoad = false;
            break;
        case EBuildable::Road:
            bShouldPlaceRoad = value;
            bShouldPlaceSettlement = false;
            bShouldPlaceCity = false;
            break;
        case EBuildable::City:
            bShouldPlaceCity = value;
            bShouldPlaceSettlement = false;
            bShouldPlaceRoad = false;
            break;
        default: break;
    }
}

void AGameModeCPP::ProcessTradeOffer(EPlayerColor from, EPlayerColor to, TMap<EResourceType, int32> offerGive,
    TMap<EResourceType, int32> offerGet)
{
    DebugWidget->ShowOffer(from, to, offerGive, offerGet);
}

void AGameModeCPP::ProcessBankTrade(TMap<EResourceType, int32> offerGive, TMap<EResourceType, int32> offerGet, EPlayerColor from)
{
    FPlayerData* FromPlayer = nullptr;
    for (FPlayerData& Player : Players)
    {
        if (Player.PlayerColor == from)
        {
            FromPlayer = &Player;
            break;
        }
    }
    if (!FromPlayer) return;

    for (auto Element : offerGive)
    {
        FromPlayer->SpendResource(Element.Key, Element.Value);
    }
    for (auto Element : offerGet)
    {
        FromPlayer->AddResource(Element.Key, Element.Value);
    }
}

void AGameModeCPP::TradeAccepted(EPlayerColor from, EPlayerColor to, TMap<EResourceType, int32> offerGive,
                                 TMap<EResourceType, int32> offerGet)
{
    UE_LOG(LogTemp, Log, TEXT("TradeAccepted: from=%d, to=%d"), (int32)from, (int32)to);
    
    // Get REFERENCES to the actual player data (not copies!)
    FPlayerData* FromPlayer = nullptr;
    FPlayerData* ToPlayer = nullptr;

    for (FPlayerData& Player : Players)
    {
        if (Player.PlayerColor == from)
        {
            FromPlayer = &Player;
        }
        else if (Player.PlayerColor == to)
        {
            ToPlayer = &Player;
        }
    }
    
    if (!FromPlayer || !ToPlayer)
    {
        UE_LOG(LogTemp, Error, TEXT("TradeAccepted: Could not find players! FromPlayer=%s, ToPlayer=%s"), 
            FromPlayer ? TEXT("found") : TEXT("NULL"), 
            ToPlayer ? TEXT("found") : TEXT("NULL"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("TradeAccepted: Before trade - FromPlayer Wood: %d, ToPlayer Wood: %d"), 
        FromPlayer->Resources.Contains(EResourceType::Wood) ? FromPlayer->Resources[EResourceType::Wood] : 0,
        ToPlayer->Resources.Contains(EResourceType::Wood) ? ToPlayer->Resources[EResourceType::Wood] : 0);

    // From player gives resources to To player
    FromPlayer->SpendResource(EResourceType::Wood, offerGive.Contains(EResourceType::Wood) ? offerGive[EResourceType::Wood] : 0);
    FromPlayer->SpendResource(EResourceType::Brick, offerGive.Contains(EResourceType::Brick) ? offerGive[EResourceType::Brick] : 0);
    FromPlayer->SpendResource(EResourceType::Sheep, offerGive.Contains(EResourceType::Sheep) ? offerGive[EResourceType::Sheep] : 0);
    FromPlayer->SpendResource(EResourceType::Wheat, offerGive.Contains(EResourceType::Wheat) ? offerGive[EResourceType::Wheat] : 0);
    FromPlayer->SpendResource(EResourceType::Ore, offerGive.Contains(EResourceType::Ore) ? offerGive[EResourceType::Ore] : 0);
    
    ToPlayer->AddResource(EResourceType::Wood, offerGive.Contains(EResourceType::Wood) ? offerGive[EResourceType::Wood] : 0);
    ToPlayer->AddResource(EResourceType::Brick, offerGive.Contains(EResourceType::Brick) ? offerGive[EResourceType::Brick] : 0);
    ToPlayer->AddResource(EResourceType::Sheep, offerGive.Contains(EResourceType::Sheep) ? offerGive[EResourceType::Sheep] : 0);
    ToPlayer->AddResource(EResourceType::Wheat, offerGive.Contains(EResourceType::Wheat) ? offerGive[EResourceType::Wheat] : 0);
    ToPlayer->AddResource(EResourceType::Ore, offerGive.Contains(EResourceType::Ore) ? offerGive[EResourceType::Ore] : 0);
    
    // To player gives resources to From player
    FromPlayer->AddResource(EResourceType::Wood, offerGet.Contains(EResourceType::Wood) ? offerGet[EResourceType::Wood] : 0);
    FromPlayer->AddResource(EResourceType::Brick, offerGet.Contains(EResourceType::Brick) ? offerGet[EResourceType::Brick] : 0);
    FromPlayer->AddResource(EResourceType::Sheep, offerGet.Contains(EResourceType::Sheep) ? offerGet[EResourceType::Sheep] : 0);
    FromPlayer->AddResource(EResourceType::Wheat, offerGet.Contains(EResourceType::Wheat) ? offerGet[EResourceType::Wheat] : 0);
    FromPlayer->AddResource(EResourceType::Ore, offerGet.Contains(EResourceType::Ore) ? offerGet[EResourceType::Ore] : 0);
    
    ToPlayer->SpendResource(EResourceType::Wood, offerGet.Contains(EResourceType::Wood) ? offerGet[EResourceType::Wood] : 0);
    ToPlayer->SpendResource(EResourceType::Brick, offerGet.Contains(EResourceType::Brick) ? offerGet[EResourceType::Brick] : 0);
    ToPlayer->SpendResource(EResourceType::Sheep, offerGet.Contains(EResourceType::Sheep) ? offerGet[EResourceType::Sheep] : 0);
    ToPlayer->SpendResource(EResourceType::Wheat, offerGet.Contains(EResourceType::Wheat) ? offerGet[EResourceType::Wheat] : 0);
    ToPlayer->SpendResource(EResourceType::Ore, offerGet.Contains(EResourceType::Ore) ? offerGet[EResourceType::Ore] : 0);
    
    UE_LOG(LogTemp, Log, TEXT("TradeAccepted: After trade - FromPlayer Wood: %d, ToPlayer Wood: %d"), 
        FromPlayer->Resources.Contains(EResourceType::Wood) ? FromPlayer->Resources[EResourceType::Wood] : 0,
        ToPlayer->Resources.Contains(EResourceType::Wood) ? ToPlayer->Resources[EResourceType::Wood] : 0);
}

FPlayerData AGameModeCPP::GetPlayerByColor(EPlayerColor Color)
{
    for (const FPlayerData& Player : Players)
    {
        if (Player.PlayerColor == Color)
        {
            return Player;
        }
    }
    return FPlayerData(EPlayerColor::None); // Return a default player if not found
}

TMap<EResourceType, int32> AGameModeCPP::GetPlayerResources(EPlayerColor PlayerColor) const
{
    TMap<EResourceType, int32> EmptyResources;
    EmptyResources.Add(EResourceType::Wood, 0);
    EmptyResources.Add(EResourceType::Brick, 0);
    EmptyResources.Add(EResourceType::Sheep, 0);
    EmptyResources.Add(EResourceType::Wheat, 0);
    EmptyResources.Add(EResourceType::Ore, 0);

    for (const FPlayerData& Player : Players)
    {
        if (Player.PlayerColor == PlayerColor)
        {
            return Player.Resources;
        }
    }

    return EmptyResources;
}

TArray<EBankTradeMethods> AGameModeCPP::GetAvailableBankTradeMethods(EPlayerColor PlayerColor) const
{
    TArray<EBankTradeMethods> Available;
    Available.Add(EBankTradeMethods::Standard);

    if (!BoardManager) return Available;

    for (ADock* Dock : BoardManager->GetDocks())
    {
        if (Dock && Dock->IsOwnedByPlayer(PlayerColor))
        {
            EBankTradeMethods Method = Dock->GetBankTradeMethod();
            if (!Available.Contains(Method))
            {
                Available.Add(Method);
            }
        }
    }

    return Available;
}

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
        DebugWidget->SetVictoryPointsUI(GetCurrentPlayer().VictoryPoints);
        
        if (CurrentTurnStep == ETurnStep::DynamicEnvironment)
        {
            DebugWidget->SetVisibleBM(true);
            DebugWidget->TradeButtonUI->SetIsEnabled(true); // Disable trade button during dynamic environment step
        }
        else
        {
            DebugWidget->SetVisibleBM(false);
            DebugWidget->TradeButtonUI->SetIsEnabled(false);
        }
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
        ChangeRules(EBuildable::Road, true);
    }
    else if (CurrentSetupStep == EPlacementNode::Road) {
        CurrentSetupStep = EPlacementNode::Settlement;
        ChangeRules(EBuildable::Settlement, true);
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

