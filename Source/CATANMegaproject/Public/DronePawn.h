// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "DronePawn.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class CATANMEGAPROJECT_API ADronePawn : public APawn
{
	GENERATED_BODY()

public:
	ADronePawn();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

	UPROPERTY(EditAnywhere, Category="Drone")
	float MaxAltitude = 30000.f;

	UPROPERTY(EditAnywhere, Category="Drone")
	float MinAltitude = 500.f;

	UPROPERTY(EditAnywhere, Category="Drone")
	float MoveSpeed = 20000.f;

	UPROPERTY(EditAnywhere, Category="Drone")
	float FastMoveMultiplier = 3.f;

	UPROPERTY(EditAnywhere, Category="Drone")
	float RotationSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category="Drone")
	float ZoomSpeed = 5000.f;

	UPROPERTY(EditAnywhere, Category="Drone")
	float MinPitch = -89.f;

	UPROPERTY(EditAnywhere, Category="Drone")
	float MaxPitch = -15.f;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* DroneMappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* FastMoveAction;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	float ForwardInput = 0.f;
	float RightInput = 0.f;
	float UpInput = 0.f;
	float YawInput = 0.f;
	float PitchInput = 0.f;
	bool bFastMove = false;

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnFastMoveStarted(const FInputActionValue& Value);
	void OnFastMoveCompleted(const FInputActionValue& Value);
};
