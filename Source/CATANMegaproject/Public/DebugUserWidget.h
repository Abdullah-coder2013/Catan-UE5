// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CatanSharedValues.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DebugUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class CATANMEGAPROJECT_API UDebugUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* DebugWidget;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CurrentPlayerUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CurrentTurnStepUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CurrentPhaseUI = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WoodUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* BrickUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SheepUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WheatUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* OreUI = nullptr;
	
	void SetCurrentPlayerUI(EPlayerColor PlayerColor);
	void SetCurrentTurnStepUI(ETurnStep TurnStep, EPlacementNode SetupStep, EGamePhase GamePhase);
	void SetCurrentPhaseUI(EGamePhase GamePhase);
	void SetResourceUI(TMap<EResourceType, int32> Resources);
	
	
protected:
	virtual void NativeOnInitialized() override;
	
};
