// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CatanSharedValues.h"
#include "InputActionValue.h"
#include "CatanPlayerController.generated.h"



class UInputMappingContext;
class UInputAction;
/**
 *
 */
UCLASS()
class CATANMEGAPROJECT_API ACatanPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	EPlayerColor PlayerColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	EPlacementNode CurrentPlacementNode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	TMap<EResourceType, int32> Resources;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* CatanMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ClickAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* AdvanceStepAction;
	ACatanPlayerController();

	void ChangePlayer(FPlayerData NewPlayer);
	void NextStep();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	FPlayerData CurrentPlayerPlaying;

	protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	private:
	void OnClick(const FInputActionValue& Value);

};
