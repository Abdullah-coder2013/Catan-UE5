// Fill out your copyright notice in the Description page of Project Settings.


#include "CatanPlayerController.h"
#include "EnhancedInputComponent.h"
#include "HexVertex.h"
#include "AHexEdge.h"
#include "GameModeCPP.h"
#include "EnhancedInputSubsystems.h"



ACatanPlayerController::ACatanPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    CurrentPlayerPlaying = FPlayerData(EPlayerColor::None);
}

void ACatanPlayerController::ChangePlayer(FPlayerData NewPlayer)
{
    CurrentPlayerPlaying = NewPlayer;
    PlayerColor = NewPlayer.PlayerColor;
    Resources = NewPlayer.Resources;
}

void ACatanPlayerController::NextStep()
{
    AGameModeCPP* gameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    if (gameMode != nullptr)
    {
        gameMode->AdvanceStep();
    }
}

void ACatanPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(CatanMappingContext, 0);
    }
}

void ACatanPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInput->BindAction(ClickAction, ETriggerEvent::Started, this, &ACatanPlayerController::OnClick);
        EnhancedInput->BindAction(AdvanceStepAction, ETriggerEvent::Started, this, &ACatanPlayerController::NextStep);
    }
}

void ACatanPlayerController::OnClick(const FInputActionValue& Value)
{
    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    bool bCanPlaceSettlement;
    bool bCanPlaceRoad;
    if (!GameMode)
    {
        return;
    }
    // Update current player reference
    ChangePlayer(GameMode->GetCurrentPlayer());
    
    if (GameMode->CurrentPhase == EGamePhase::Setup)
    {
        if (GameMode->CurrentSetupStep == EPlacementNode::Settlement)
        {
            bCanPlaceSettlement = true;
            bCanPlaceRoad = false;
        }
        else if (GameMode->CurrentSetupStep == EPlacementNode::Road)
        {
            bCanPlaceSettlement = false;
            bCanPlaceRoad = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid setup step!"));
            return;
        }
    }
    else if (GameMode->CurrentTurnStep == ETurnStep::DynamicEnvironment)
    {
        bCanPlaceSettlement = true; // In a full implementation, you'd check if the player is trying to place a road or settlement
        bCanPlaceRoad = true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("You can only build during the Build phase!"));
        return;
    }
    FVector WorldLocation, WorldDirection;

    if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        FVector TraceStart = WorldLocation;
        FVector TraceEnd = WorldLocation + WorldDirection * 10000.f;

        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(GetPawn());

        if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
        {
            AActor* HitActor = HitResult.GetActor();

            if (AHexVertex* Vertex = Cast<AHexVertex>(HitActor))
            {
                if (bCanPlaceSettlement)
                {
                    if (GameMode->GetCurrentPlayer().CanBuild(EBuildable::Settlement) || GameMode->CurrentPhase == EGamePhase::Setup)
                    {
                        if (Vertex->TryPlaceSettlement(PlayerColor, ESettlementType::Settlement))
                        {
                            GameMode->GetCurrentPlayer().SpendResourceFor(EBuildable::Settlement);
                            GameMode->GetCurrentPlayer().AddVictoryPoint(1);
                            if (GameMode->CurrentPhase == EGamePhase::Setup)
                            {
                                GameMode->AdvanceSetup(false);
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Cannot place settlement here!"));
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("%d does not have enough resources to build a settlement!"), (int32)CurrentPlayerPlaying.PlayerColor);
                    }
                }
            }
            else if (AAHexEdge* Edge = Cast<AAHexEdge>(HitActor))
            {
                if (bCanPlaceRoad)
                {
                    if (GameMode->GetCurrentPlayer().CanBuild(EBuildable::Road) || GameMode->CurrentPhase == EGamePhase::Setup)
                    {
                        if (Edge->TryPlaceRoad(PlayerColor))
                        {
                            GameMode->GetCurrentPlayer().SpendResourceFor(EBuildable::Road);
                            if (GameMode->CurrentPhase == EGamePhase::Setup)
                            {
                                GameMode->AdvanceSetup(false);
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Cannot place road here!"));
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("%d does not have enough resources to build a road!"), (int32)CurrentPlayerPlaying.PlayerColor);
                    }
                }
            }
        }
    }
}

