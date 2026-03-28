#include "DebugUserWidget.h"
#include "Components/TextBlock.h"

void UDebugUserWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // Optional: Initialize with default values when the widget is created
    if (CurrentPlayerUI)
    {
        CurrentPlayerUI->SetText(FText::FromString(TEXT("Current Player: None")));
    }

    if (CurrentTurnStepUI)
    {
        CurrentTurnStepUI->SetText(FText::FromString(TEXT("Turn Step: Waiting")));
    }

    if (CurrentPhaseUI)
    {
        CurrentPhaseUI->SetText(FText::FromString(TEXT("Phase: None")));
    }

    // Initialize resources to 0
    if (WoodUI)  WoodUI->SetText(FText::FromString(TEXT("Wood: 0")));
    if (BrickUI) BrickUI->SetText(FText::FromString(TEXT("Brick: 0")));
    if (SheepUI) SheepUI->SetText(FText::FromString(TEXT("Sheep: 0")));
    if (WheatUI) WheatUI->SetText(FText::FromString(TEXT("Wheat: 0")));
    if (OreUI)   OreUI->SetText(FText::FromString(TEXT("Ore: 0")));
}

void UDebugUserWidget::SetCurrentPlayerUI(EPlayerColor PlayerColor)
{
    if (CurrentPlayerUI)
    {
        FString PlayerColorString = UEnum::GetValueAsString(PlayerColor);
        // Clean up the enum string (removes "EPlayerColor::" prefix)
        PlayerColorString = PlayerColorString.Replace(TEXT("EPlayerColor::"), TEXT(""));
        
        CurrentPlayerUI->SetText(FText::FromString(TEXT("Current Player: ") + PlayerColorString));
    }
}

void UDebugUserWidget::SetCurrentTurnStepUI(ETurnStep TurnStep, EPlacementNode SetupStep, EGamePhase GamePhase)
{
    if (CurrentTurnStepUI)
    {
        FString TurnStepString;

        if (GamePhase == EGamePhase::Setup)
        {
            FString SetupString = UEnum::GetValueAsString(SetupStep);
            SetupString = SetupString.Replace(TEXT("EPlacementNode::"), TEXT(""));
            TurnStepString = FString::Printf(TEXT("Setup: %s"), *SetupString);
        }
        else
        {
            FString TurnString = UEnum::GetValueAsString(TurnStep);
            TurnString = TurnString.Replace(TEXT("ETurnStep::"), TEXT(""));
            TurnStepString = TurnString;
        }

        CurrentTurnStepUI->SetText(FText::FromString(TurnStepString));
    }
}

void UDebugUserWidget::SetCurrentPhaseUI(EGamePhase GamePhase)
{
    if (CurrentPhaseUI)
    {
        FString GamePhaseString = UEnum::GetValueAsString(GamePhase);
        GamePhaseString = GamePhaseString.Replace(TEXT("EGamePhase::"), TEXT(""));
        CurrentPhaseUI->SetText(FText::FromString(GamePhaseString));
    }
}

void UDebugUserWidget::SetResourceUI(TMap<EResourceType, int32> Resources)
{
    // Update each resource independently — no else if!
    if (WoodUI && Resources.Contains(EResourceType::Wood))
    {
        WoodUI->SetText(FText::FromString(FString::Printf(TEXT("Wood: %d"), Resources[EResourceType::Wood])));
    }

    if (BrickUI && Resources.Contains(EResourceType::Brick))
    {
        BrickUI->SetText(FText::FromString(FString::Printf(TEXT("Brick: %d"), Resources[EResourceType::Brick])));
    }

    if (SheepUI && Resources.Contains(EResourceType::Sheep))
    {
        SheepUI->SetText(FText::FromString(FString::Printf(TEXT("Sheep: %d"), Resources[EResourceType::Sheep])));
    }

    if (WheatUI && Resources.Contains(EResourceType::Wheat))
    {
        WheatUI->SetText(FText::FromString(FString::Printf(TEXT("Wheat: %d"), Resources[EResourceType::Wheat])));
    }

    if (OreUI && Resources.Contains(EResourceType::Ore))
    {
        OreUI->SetText(FText::FromString(FString::Printf(TEXT("Ore: %d"), Resources[EResourceType::Ore])));
    }
}