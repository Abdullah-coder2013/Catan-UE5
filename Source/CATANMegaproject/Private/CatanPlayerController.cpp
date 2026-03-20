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
void ACatanPlayerController::RollDice()
{
    GetWorld()->GetAuthGameMode<AGameModeCPP>()->RollDice();
}

void ACatanPlayerController::ChangeIdentity()
{
    PlayerColor = static_cast<EPlayerColor>((static_cast<uint8>(PlayerColor) + 1) % 5);
    UE_LOG(LogTemp, Warning, TEXT("Player color changed to: %d"), (int32)PlayerColor);
}

void ACatanPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInput->BindAction(ClickAction, ETriggerEvent::Started, this, &ACatanPlayerController::OnClick);
        EnhancedInput->BindAction(SwitchIdentityAction, ETriggerEvent::Started, this, &ACatanPlayerController::ChangeIdentity);
        EnhancedInput->BindAction(RollDiceAction, ETriggerEvent::Started, this, &ACatanPlayerController::RollDice);
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

