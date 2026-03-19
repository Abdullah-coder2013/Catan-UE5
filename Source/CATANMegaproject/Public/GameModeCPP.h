// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoardManager.h"
#include "GameModeCPP.generated.h"


/**
 * 
 */
UCLASS()
class CATANMEGAPROJECT_API AGameModeCPP : public AGameModeBase
{
	GENERATED_BODY()

	public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Settings")
	TSubclassOf<ABoardManager> BoardManagerClass;

	protected:
	virtual void BeginPlay() override;
	
};
