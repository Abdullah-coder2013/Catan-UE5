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
    bool bShouldPlaceSettlement;
    bool bShouldPlaceRoad;
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
            bShouldPlaceSettlement = true;
            bShouldPlaceRoad = false;
        }
        else if (GameMode->CurrentSetupStep == EPlacementNode::Road)
        {
            bShouldPlaceSettlement = false;
            bShouldPlaceRoad = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid setup step!"));
            return;
        }
    }
    else if (GameMode->CurrentTurnStep == ETurnStep::Build)
    {
        bShouldPlaceSettlement = true; // In a full implementation, you'd check if the player is trying to place a road or settlement
        bShouldPlaceRoad = true;
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
                if (bShouldPlaceSettlement)
                {
                    Vertex->TryPlaceSettlement(PlayerColor, ESettlementType::Settlement);
                    if (GameMode->CurrentPhase == EGamePhase::Setup)
                    {
                        GameMode->AdvanceSetup();
                    }
                }
            }
            else if (AAHexEdge* Edge = Cast<AAHexEdge>(HitActor))
            {
                if (bShouldPlaceRoad)
                {
                    Edge->TryPlaceRoad(PlayerColor);
                    if (GameMode->CurrentPhase == EGamePhase::Setup)
                    {
                        GameMode->AdvanceSetup();
                    }
                }
            }
        }
    }
}

