// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoardManager.h"
#include "CatanSharedValues.h"
#include "DebugUserWidget.h"
#include "GameModeCPP.generated.h"


/**
 */
UCLASS()
class CATANMEGAPROJECT_API AGameModeCPP : public AGameModeBase
{
	GENERATED_BODY()

	public:
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DebugWidgetClass;

	UPROPERTY()
	UDebugUserWidget* DebugWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Settings")
	TSubclassOf<ABoardManager> BoardManagerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Players")
	TArray<FPlayerData> Players;

	void RollDice();
	void DistributeResources(int32 DiceRoll);
	void InitializePlayers(TArray<EPlayerColor> PlayerColors);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn System Settings")
	EGamePhase CurrentPhase;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn System Settings")
	ETurnStep CurrentTurnStep;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Players")
	int32 CurrentPlayerIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn System Settings")
	TArray<EPlayerColor> PlayerOrder;
	UPROPERTY()
	bool bSetupReversed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn System Settings")
	EPlacementNode CurrentSetupStep;

	void StartGame();
	void EndTurn();
	FPlayerData& GetCurrentPlayer();
	void AdvanceSetup(bool bFirstTimeCalled);
	void AdvanceTurn();
	void AdvanceStep();
	void ChangePlayer();
	void StartMainGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Players")
	TArray<EPlayerColor> DefaultColors;

	protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	AGameModeCPP();

	private:
	UPROPERTY()
	TArray<AHexTile*> HexTiles;
	UPROPERTY()
	ABoardManager* BoardManager;

};
