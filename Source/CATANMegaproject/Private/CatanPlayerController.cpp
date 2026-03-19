// Fill out your copyright notice in the Description page of Project Settings.


#include "CatanPlayerController.h"
#include "EnhancedInputComponent.h"
#include "HexVertex.h"
#include "AHexEdge.h"
#include "EnhancedInputSubsystems.h"



ACatanPlayerController::ACatanPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
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
        EnhancedInput->BindAction(ClickAction, ETriggerEvent::Triggered, this, &ACatanPlayerController::OnClick);
    }
}

void ACatanPlayerController::OnClick(const FInputActionValue& Value)
{
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
                Vertex->TryPlaceSettlement(PlayerColor, ESettlementType::Settlement);
            }
            else if (AAHexEdge* Edge = Cast<AAHexEdge>(HitActor))
            {
                Edge->TryPlaceRoad(PlayerColor);
            }
        }
    }
}

