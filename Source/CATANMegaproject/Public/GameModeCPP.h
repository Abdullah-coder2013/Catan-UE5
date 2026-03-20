// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoardManager.h"
#include "CatanPlayerState.h"
#include "GameModeCPP.generated.h"


/**
 */
UCLASS()
class CATANMEGAPROJECT_API AGameModeCPP : public AGameModeBase
{
	GENERATED_BODY()

	public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Settings")
	TSubclassOf<ABoardManager> BoardManagerClass;

	UPROPERTY()
	TArray<ACatanPlayerState*> Players;

	void RollDice();
	void DistributeResources(int32 DiceRoll);

	protected:
	virtual void BeginPlay() override;

	private:
	UPROPERTY()
	TArray<AHexTile*> HexTiles;
	UPROPERTY()
	ABoardManager* BoardManager;
	
};
