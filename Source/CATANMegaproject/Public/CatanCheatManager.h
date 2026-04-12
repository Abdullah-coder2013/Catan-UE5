// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CatanSharedValues.h"
#include "GameFramework/CheatManager.h"
#include "CatanCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class CATANMEGAPROJECT_API UCatanCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
	UFUNCTION(Exec)
	void GiveResources(int32 Wood, int32 Brick, int32 Ore, int32 Wheat, int32 Sheep, EPlayerColor PlayerColor);
	
	UFUNCTION(Exec)
	void GiveAllResources(int32 Amount = 5, EPlayerColor PlayerColor = EPlayerColor::None);

	// Give development cards
	UFUNCTION(Exec)
	void GiveDevelopmentCard(FString CardType, EPlayerColor TargetPlayer);

	UFUNCTION(Exec)
	void GiveAllDevCards(int32 Amount, EPlayerColor TargetPlayer);
	
	UFUNCTION(Exec)
	void GiveVictoryPoints(int32 Amount, EPlayerColor TargetPlayer);
	
};
