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
    if (!GameMode)
    {
        return;
    }
    // Update current player reference
    ChangePlayer(GameMode->GetCurrentPlayer());
    
    bool bCanPlaceSettlement = GameMode->bShouldPlaceSettlement;
    bool bCanPlaceRoad = GameMode->bShouldPlaceRoad;
    bool bCanPlaceCity = GameMode->bShouldPlaceCity;
    
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
            
            if (GameMode->bShouldPlaceRobber)
            {
                if (AHexTile* Tile = Cast<AHexTile>(HitActor))
                {
                    if (Tile->HexType != EHexType::Desert)
                    {
                        GameMode->MoveRobber(Tile);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Cannot place robber on desert tile!"));
                    }
                }
            }

            if (AHexVertex* Vertex = Cast<AHexVertex>(HitActor))
            {
                if (bCanPlaceSettlement)
                {
                    if (GameMode->GetCurrentPlayer().CanBuild(EBuildable::Settlement) || GameMode->CurrentPhase == EGamePhase::Setup)
                    {
                        if (Vertex->TryPlaceSettlement(PlayerColor, ESettlementType::Settlement, GameMode->CurrentPhase))
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
                else if (bCanPlaceCity)
                {
                    if (GameMode->GetCurrentPlayer().CanBuild(EBuildable::City))
                    {
                        if (Vertex->TryPlaceCity(PlayerColor, GameMode->CurrentPhase))
                        {
                            GameMode->GetCurrentPlayer().SpendResourceFor(EBuildable::City);
                            GameMode->GetCurrentPlayer().DeduceVictoryPoint(1); // Add 1 additional point for upgrading to a city
                            GameMode->GetCurrentPlayer().AddVictoryPoint(2);
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Cannot place city here!"));
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("%d does not have enough resources to build a city!"), (int32)CurrentPlayerPlaying.PlayerColor);
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

