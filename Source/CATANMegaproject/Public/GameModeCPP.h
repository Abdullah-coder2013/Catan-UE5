// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoardManager.h"
#include "CatanSharedValues.h"
#include "DebugUserWidget.h"
#include "Dock.h"
#include "Robber.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Settings")
	TSubclassOf<ARobber> RobberClass;

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
	
	UPROPERTY()
	bool bShouldPlaceSettlement;
	UPROPERTY()
	bool bShouldPlaceRoad;
	UPROPERTY()
	bool bShouldPlaceCity;
	UPROPERTY()
	bool bShouldPlaceRobber;
	
	void ChangeRules(EBuildable ruleType, bool value);

	void ProcessTradeOffer(EPlayerColor from, EPlayerColor to, TMap<EResourceType, int32> offerGive, TMap<EResourceType, int32> offerGet);
	
	void ProcessBankTrade(TMap<EResourceType, int32> offerGive, TMap<EResourceType, int32> offerGet, EPlayerColor from);

	void TradeAccepted(EPlayerColor from, EPlayerColor to, TMap<EResourceType, int32> offerGive, TMap<EResourceType, int32> offerGet);
	
	void MoveRobber(AHexTile* Tile);
	
	void RobFromPlayer(EPlayerColor FromPlayerColor, EPlayerColor ToPlayerColor);
	
	int32 GetTotalCardsInHand(EPlayerColor PlayerColor);
	
	void DiscardResources(EPlayerColor PlayerColor, TMap<EResourceType, int32> Resources);
	
	FPlayerData& GetPlayerByColor(EPlayerColor Color);

	TMap<EResourceType, int32> GetPlayerResources(EPlayerColor PlayerColor) const;

	TArray<EBankTradeMethods> GetAvailableBankTradeMethods(EPlayerColor PlayerColor) const;

	void StartDiscardPhase();
	void AdvanceDiscardPhase();
	void StartRobPhase();
	void FinishRobPhase();

	UPROPERTY()
	EPlayerColor CurrentDiscardPlayer;

	UPROPERTY()
	TArray<EPlayerColor> PlayersNeedingDiscard;

	UPROPERTY()
	int32 DiscardQueueIndex = 0;

	protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	AGameModeCPP();

	private:
	UPROPERTY()
	TArray<AHexTile*> HexTiles;
	UPROPERTY()
	ABoardManager* BoardManager;
	UPROPERTY()
	ARobber* Robber;

};
