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
        case EBuildable::None:
             bShouldPlaceSettlement = false;
             bShouldPlaceCity = false;
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

void AGameModeCPP::MoveRobber(AHexTile* Tile)
{
    Tile->PlaceRobber(Robber);
    bShouldPlaceRobber = false;
    DebugWidget->SetRobberText(false);
    StartDiscardPhase();
}

void AGameModeCPP::StartDiscardPhase()
{
    PlayersNeedingDiscard.Empty();
    DiscardQueueIndex = 0;

    for (EPlayerColor PlayerColor : PlayerOrder)
    {
        if (GetTotalCardsInHand(PlayerColor) > 7)
        {
            PlayersNeedingDiscard.Add(PlayerColor);
        }
    }

    CurrentTurnStep = ETurnStep::RobberDiscard;

    if (PlayersNeedingDiscard.Num() == 0)
    {
        StartRobPhase();
        return;
    }

    CurrentDiscardPlayer = PlayersNeedingDiscard[0];
    DebugWidget->ShowDiscardModal(CurrentDiscardPlayer);
}

void AGameModeCPP::AdvanceDiscardPhase()
{
    DiscardQueueIndex++;

    if (DiscardQueueIndex < PlayersNeedingDiscard.Num())
    {
        CurrentDiscardPlayer = PlayersNeedingDiscard[DiscardQueueIndex];
        DebugWidget->ShowDiscardModal(CurrentDiscardPlayer);
    }
    else
    {
        StartRobPhase();
    }
}

void AGameModeCPP::StartRobPhase()
{
    CurrentTurnStep = ETurnStep::RobberSteal;
    DebugWidget->SetVisibleRobModal(BoardManager->GetRobberTile());
}

void AGameModeCPP::FinishRobPhase()
{
    CurrentTurnStep = ETurnStep::DynamicEnvironment;
}

void AGameModeCPP::RobFromPlayer(EPlayerColor FromPlayerColor, EPlayerColor ToPlayerColor)
{
    // Choose a random resource from FromPlayerColor
    TMap<EResourceType, int32> FromPlayerResources = GetPlayerResources(FromPlayerColor);
    TArray<EResourceType> AvailableResources;
    EResourceType* SelectedResource;
    for (auto& ResourcePair : FromPlayerResources)
    {        if (ResourcePair.Value > 0)
    {            AvailableResources.Add(ResourcePair.Key);
    }    }
    if (AvailableResources.Num() == 0)    {
        UE_LOG(LogTemp, Log, TEXT("RobFromPlayer: Player %d has no resources to steal!"), (int32)FromPlayerColor);
        return;
    }
    SelectedResource = &AvailableResources[FMath::RandRange(0, AvailableResources.Num() - 1)];
    GetPlayerByColor(FromPlayerColor).SpendResource(*SelectedResource, 1);
    GetPlayerByColor(ToPlayerColor).AddResource(*SelectedResource, 1);
     UE_LOG(LogTemp, Log, TEXT("RobFromPlayer: Player %d robbed Player %d for 1 %s"), (int32)ToPlayerColor, (int32)FromPlayerColor, *UEnum::GetValueAsString(*SelectedResource));
}

int32 AGameModeCPP::GetTotalCardsInHand(EPlayerColor PlayerColor)
{
    TMap<EResourceType, int32> PlayerResources = GetPlayerResources(PlayerColor);
    int32 Amount = 0;
    for (auto& ResourcePair : PlayerResources)
    {
        Amount += ResourcePair.Value;
    }
    return Amount;
}

void AGameModeCPP::InitializeDevCards()
{
    // Add all the dev cards to cardslist
    DevelopmentCards.Empty();
    for (int32 i = 0; i < 14; i++)
    {
        DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::Knight, false, EPlayerColor::None));
    }
    for (int32 i = 0; i < 5; i++)
    {
        DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::VictoryPoint, false, EPlayerColor::None));
    }
    for (int32 i = 0; i < 2; i++)
    {
        DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::RoadBuilding, false, EPlayerColor::None));
        DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::YearsOfPlenty, false, EPlayerColor::None));
        DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::Monopoly, false, EPlayerColor::None));
    }
    
    // Mix the cards thoroughly
    for (int32 i = DevelopmentCards.Num() - 1; i >= 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        DevelopmentCards.Swap(i, j);
    }
}

void AGameModeCPP::PlayDevelopmentCard(FDevelopmentCard* Card)
{
        UE_LOG(LogTemp, Log, TEXT("PlayDevelopmentCard: Player %d plays a %s card"), (int32)GetCurrentPlayer().PlayerColor, *UEnum::GetValueAsString(Card->DevelopmentCardType));
        
        switch (Card->DevelopmentCardType)
        {
            case EDevelopmentCardType::Knight:
                // Robber
                ChangeRules(EBuildable::None, false); // Disable all building during robber step
                bShouldPlaceRobber = true;
                DebugWidget->SetRobberText(true);
                GiveAward(Card->OwnedBy, EAwardType::LargestArmy);
                break;
            case EDevelopmentCardType::VictoryPoint:
                GetPlayerByColor(Card->OwnedBy).VictoryPoints += 1;
                break;
            case EDevelopmentCardType::RoadBuilding:
                // Handle road building effect
                bCanPlaceTwoRoads = true;
                RoadsPlaced = 0;
                ChangeRules(EBuildable::Road, true); // Allow placing roads
                break;
            case EDevelopmentCardType::YearsOfPlenty:
                DebugWidget->ShowYOPModal();
                break;
            case EDevelopmentCardType::Monopoly:
                DebugWidget->ShowMonopolyModal();
                break;
            default: break;
        }
        Card->bIsPlayed = true;
}

void AGameModeCPP::GiveDevCard(EPlayerColor PlayerColor)
{
    FPlayerData* PlayerData = &GetPlayerByColor(PlayerColor);
    if (DevelopmentCards.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("GiveDevCard: No more development cards available to give!"));
        return;
    }
    // check if player can afford
    bool bPlayerCanAfford = PlayerData->CanBuild(EBuildable::Development);
    if (!bPlayerCanAfford)
    {
        UE_LOG(LogTemp, Warning, TEXT("GiveDevCard: Player %d cannot afford to buy a development card!"), (int32)PlayerColor);
        return;
    }
    
    DevelopmentCards[0].OwnedBy = PlayerColor;
    PlayerData->DevelopmentCards.Add(DevelopmentCards[0]);
    PlayerData->SpendResourceFor(EBuildable::Development);
     UE_LOG(LogTemp, Log, TEXT("GiveDevCard: Gave %d development card to Player %d. Cards left in deck: %d"), DevelopmentCards[0].DevelopmentCardType, (int32)PlayerColor, DevelopmentCards.Num()-1);
    if (DevelopmentCards[0].DevelopmentCardType == EDevelopmentCardType::VictoryPoint)
    {
        PlayDevelopmentCard(&DevelopmentCards[0]);
    }
    DevelopmentCards.RemoveAt(0);
    DebugWidget->UpdateDevCards();
    UE_LOG(LogTemp, Warning, TEXT("Player %d: %d Knight cards, %d YOP cards, %d RoadBuilding cards, %d Monopoly cards, %d Victory Points cards"), GetCurrentPlayer().PlayerColor, GetCurrentPlayer().GetDevelopmentCardAmount(EDevelopmentCardType::Knight), GetCurrentPlayer().GetDevelopmentCardAmount(EDevelopmentCardType::YearsOfPlenty), GetCurrentPlayer().GetDevelopmentCardAmount(EDevelopmentCardType::RoadBuilding), GetCurrentPlayer().GetDevelopmentCardAmount(EDevelopmentCardType::Monopoly), GetCurrentPlayer().GetDevelopmentCardAmount(EDevelopmentCardType::VictoryPoint));
}

void AGameModeCPP::MonopolyResource(EPlayerColor PlayerColor, EResourceType ResourceType)
{
    int32 TotalStolen = 0;
    for (FPlayerData& Player : Players)
    {
        if (Player.PlayerColor != PlayerColor)
        {
            int32 AmountToSteal = Player.Resources.Contains(ResourceType) ? Player.Resources[ResourceType] : 0;
            if (AmountToSteal > 0)
            {
                Player.SpendResource(ResourceType, AmountToSteal);
                GetPlayerByColor(PlayerColor).AddResource(ResourceType, AmountToSteal);
                TotalStolen += AmountToSteal;
            }
        }
    }
    UE_LOG(LogTemp, Log, TEXT("MonopolyResource: Player %d monopolizes %d of %s"), (int32)PlayerColor, TotalStolen, *UEnum::GetValueAsString(ResourceType));
}

void AGameModeCPP::CalculateLongestRoad()
{
    for (FPlayerData& Player : Players)
    {
        int32 LongestRoad = BoardManager->GetLongestRoadLengthForPlayer(Player.PlayerColor);
        longestRoadLengths.Add(Player.PlayerColor, LongestRoad);
    }
    // Discard from the longest road
    int32 longest = 0;
    EPlayerColor longestPlayer = EPlayerColor::None;
    for (auto Road : longestRoadLengths)
    {
        if (Road.Value > longest)
        {
            longest = Road.Value;
            longestPlayer = Road.Key;
        }
    }
    longestRoadLengths.Empty();
    longestRoadLengths.Add(longestPlayer, longest);
     UE_LOG(LogTemp, Log, TEXT("CalculateLongestRoad: Player %d has the longest road with length %d"), (int32)longestPlayer, longest);
    GiveAward(longestPlayer, EAwardType::LongestRoad);
}

void AGameModeCPP::DiscardResources(EPlayerColor PlayerColor, TMap<EResourceType, int32> Resources)
{
    TMap<EResourceType, int32> PlayerResources = GetPlayerResources(PlayerColor);
    for (auto& ResourcePair : Resources)
    {
        if (PlayerResources.Contains(ResourcePair.Key))
        {
            int32 DiscardAmount = FMath::Min(ResourcePair.Value, PlayerResources[ResourcePair.Key]);
            GetPlayerByColor(PlayerColor).SpendResource(ResourcePair.Key, DiscardAmount);
            UE_LOG(LogTemp, Log, TEXT("DiscardResources: Player %d discards %d %s"), (int32)PlayerColor, DiscardAmount, *UEnum::GetValueAsString(ResourcePair.Key));
        }
    }
}

FPlayerData& AGameModeCPP::GetPlayerByColor(EPlayerColor Color)
{
    for (FPlayerData& Player : Players)
    {
        if (Player.PlayerColor == Color)
        {
            return Player;
        }
    }
    static FPlayerData DefaultPlayer(EPlayerColor::None);
    DefaultPlayer = FPlayerData(EPlayerColor::None);
    UE_LOG(LogTemp, Error, TEXT("GetPlayerByColor: Player not found for color %d"), (int32)Color);
    return DefaultPlayer;
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

void AGameModeCPP::GiveAward(EPlayerColor PlayerColor, EAwardType AwardType)
{
    if (AwardType == EAwardType::LongestRoad && longestRoadLengths.Contains(PlayerColor) && longestRoadLengths[PlayerColor] >= 5)
    {
        if (PlayerAwards.Contains(PlayerColor) && PlayerAwards[PlayerColor] == EAwardType::LongestRoad)
        {
            UE_LOG(LogTemp, Warning, TEXT("GiveAward: Player %d already has the Longest Road award!"), (int32)PlayerColor);
            return;
        }

        // Remove award from previous holder
        EPlayerColor PreviousRoadHolder = EPlayerColor::None;
        for (auto& Award : PlayerAwards)
        {
            if (Award.Value == EAwardType::LongestRoad)
            {
                PreviousRoadHolder = Award.Key;
                break;
            }
        }
        if (PreviousRoadHolder != EPlayerColor::None)
        {
            PlayerAwards.Remove(PreviousRoadHolder);
            GetPlayerByColor(PreviousRoadHolder).VictoryPoints -= 2;
            UE_LOG(LogTemp, Log, TEXT("GiveAward: Player %d loses Longest Road award!"), (int32)PreviousRoadHolder);
        }

        PlayerAwards.Add(PlayerColor, EAwardType::LongestRoad);
        GetPlayerByColor(PlayerColor).VictoryPoints += 2;
        UE_LOG(LogTemp, Log, TEXT("GiveAward: Player %d awarded Longest Road!"), (int32)PlayerColor);
    }
    else if (AwardType == EAwardType::LargestArmy)
    {
        // Count PLAYED Knight cards (not unplayed ones)
        int32 KnightCount = 0;
        for (auto& Card : GetPlayerByColor(PlayerColor).DevelopmentCards)
        {
            if (Card.DevelopmentCardType == EDevelopmentCardType::Knight && Card.bIsPlayed)
            {
                KnightCount++;
            }
        }

        int32 LargestArmySize = 0;
        EPlayerColor LargestArmyHolder = EPlayerColor::None;
        for (auto& Award : PlayerAwards)
        {
            if (Award.Value == EAwardType::LargestArmy)
            {
                // Count PLAYED Knight cards for current holder too
                int32 CurrentHolderKnightCount = 0;
                for (auto& Card : GetPlayerByColor(Award.Key).DevelopmentCards)
                {
                    if (Card.DevelopmentCardType == EDevelopmentCardType::Knight && Card.bIsPlayed)
                    {
                        CurrentHolderKnightCount++;
                    }
                }
                if (CurrentHolderKnightCount > LargestArmySize)
                {
                    LargestArmySize = CurrentHolderKnightCount;
                    LargestArmyHolder = Award.Key;
                }
            }
        }
        if (KnightCount >= 3 && KnightCount > LargestArmySize)
        {
            if (LargestArmyHolder != EPlayerColor::None)
            {
                PlayerAwards.Remove(LargestArmyHolder);
                GetPlayerByColor(LargestArmyHolder).VictoryPoints -= 2;
                 UE_LOG(LogTemp, Log, TEXT("GiveAward: Player %d loses Largest Army award!"), (int32)LargestArmyHolder);
            }
            PlayerAwards.Add(PlayerColor, EAwardType::LargestArmy);
            GetPlayerByColor(PlayerColor).VictoryPoints += 2;
             UE_LOG(LogTemp, Log, TEXT("GiveAward: Player %d awarded Largest Army!"), (int32)PlayerColor);
        }
    }
}

void AGameModeCPP::PauseGame(bool bPause)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetPause(bPause);
    }
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
    if (RobberClass)
    {
        Robber = GetWorld()->SpawnActor<ARobber>(RobberClass, FVector::ZeroVector, FRotator::ZeroRotator);
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
    if (DebugWidget)
    {
        DebugWidget->SetRobberText(false);
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
            DebugWidget->DevCardButtonUI->SetIsEnabled(true);
        }
        else
        {
            DebugWidget->SetVisibleBM(false);
            DebugWidget->TradeButtonUI->SetIsEnabled(false);
            DebugWidget->DevCardButtonUI->SetIsEnabled(false);
        }
        if (bCanPlaceTwoRoads && RoadsPlaced < 2)
        {
            DebugWidget->SetRoadBuildingText(true, RoadsPlaced);
        }
        else
        {
            DebugWidget->SetRoadBuildingText(false, RoadsPlaced);
        }
        DebugWidget->UpdateDevCards();
    }
    
    if (RoadsPlaced == 2 && bCanPlaceTwoRoads == true)
    {
        RoadsPlaced = 0;
        bCanPlaceTwoRoads = false;
         ChangeRules(EBuildable::Road, false); // Disable road placement after using road
        CurrentTurnStep = ETurnStep::DynamicEnvironment;
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
        if (bShouldPlaceRobber)
        {
            // Wait for player to click a hex to place the robber
        }
        else
        {
            CurrentTurnStep = ETurnStep::DynamicEnvironment;
        }
    }
    else if (CurrentTurnStep == ETurnStep::DynamicEnvironment) {
        if (GetCurrentPlayer().VictoryPoints >= 10)
        {
            CurrentTurnStep = ETurnStep::None;
            CurrentPhase = EGamePhase::GameOver;
            DebugWidget->ShowGameOver(GetCurrentPlayer().PlayerColor);
            return;
        }
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
    if (BoardManager)
    {
        AHexTile* DesertTile = BoardManager->GetHexTile(EHexType::Desert);
        if (DesertTile && Robber)
        {
            Robber->SetActorLocation(DesertTile->GetActorLocation());
        }
    }
    InitializePlayers(DefaultColors);
    InitializeDevCards();
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
        if (Tile->bHasRobber)
        {
            UE_LOG(LogTemp, Warning, TEXT("Skipping tile %s because it has the robber"), *Tile->GetName());
            continue; // Skip resource distribution for tiles with the robber
        }
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

void AGameModeCPP::DistributeResourcesFor(AHexVertex* HexVertex)
{
    FPlayerData* Player = &GetPlayerByColor(HexVertex->occupiedBy);
    if (!Player) return;
    for (auto HexTile : HexVertex->AdjacentHexes)
    {
        Player->AddResource(HexTile->HexTypeToResource(), 1);
    }
}

void AGameModeCPP::RollDice()
{
    int32 Dice1 = FMath::RandRange(1, 6);
    int32 Dice2 = FMath::RandRange(1, 6);
    int32 Total = Dice1 + Dice2;
    if (Total == 7)
    {
        // Robber
        ChangeRules(EBuildable::None, false); // Disable all building during robber step
        bShouldPlaceRobber = true;
        DebugWidget->SetRobberText(true);
    }
    UE_LOG(LogTemp, Log, TEXT("Rolled: %d + %d = %d"), Dice1, Dice2, Total);
    DistributeResources(Total);

}

