#include "DronePawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ADronePawn::ADronePawn()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(Root);
}

void ADronePawn::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && PC->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            Subsystem->AddMappingContext(DroneMappingContext, 0);
        }
    }
}

void ADronePawn::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(Input))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADronePawn::OnMove);
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &ADronePawn::OnMove);
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADronePawn::OnLook);
        EnhancedInput->BindAction(FastMoveAction, ETriggerEvent::Started, this, &ADronePawn::OnFastMoveStarted);
        EnhancedInput->BindAction(FastMoveAction, ETriggerEvent::Completed, this, &ADronePawn::OnFastMoveCompleted);
    }
}

void ADronePawn::OnMove(const FInputActionValue& Value)
{
    FVector2D MoveVector = Value.Get<FVector2D>();
    ForwardInput = MoveVector.X;
    RightInput = MoveVector.Y;
}

void ADronePawn::OnLook(const FInputActionValue& Value)
{
    FVector2D LookVector = Value.Get<FVector2D>();
    YawInput = LookVector.X;
    PitchInput = LookVector.Y;
}

void ADronePawn::OnFastMoveStarted(const FInputActionValue& Value)
{
    bFastMove = true;
}

void ADronePawn::OnFastMoveCompleted(const FInputActionValue& Value)
{
    bFastMove = false;
}

void ADronePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float Speed = MoveSpeed * (bFastMove ? FastMoveMultiplier : 1.f);

    FVector Forward = Camera->GetForwardVector();
    FVector Right   = Camera->GetRightVector();

    Forward.Z = 0.f;
    Forward.Normalize();

    FVector Movement = FVector::ZeroVector;
    Movement += Forward * ForwardInput * Speed * DeltaTime;
    Movement += Right   * RightInput   * Speed * DeltaTime;
    Movement += FVector::UpVector * UpInput * Speed * DeltaTime;

    FVector NewLocation = GetActorLocation() + Movement;

    FHitResult TerrainHit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(TerrainCheck), false);
    FVector TraceStart = FVector(NewLocation.X, NewLocation.Y, NewLocation.Z + 50000.f);
    FVector TraceEnd = FVector(NewLocation.X, NewLocation.Y, NewLocation.Z - 500000.f);
    float TerrainZ = 0.f;
    if (GetWorld()->LineTraceSingleByChannel(TerrainHit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
    {
        TerrainZ = TerrainHit.ImpactPoint.Z;
    }

    NewLocation.Z = FMath::Clamp(NewLocation.Z, TerrainZ + MinAltitude, TerrainZ + MaxAltitude);
    SetActorLocation(NewLocation);

    FRotator NewRotation = Camera->GetRelativeRotation();
    NewRotation.Yaw   += YawInput   * RotationSpeed * DeltaTime;
    NewRotation.Pitch += PitchInput * RotationSpeed * DeltaTime;

    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, MinPitch, MaxPitch);
    Camera->SetRelativeRotation(NewRotation);
}
