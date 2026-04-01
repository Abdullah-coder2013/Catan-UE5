// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CatanSharedValues.h"
#include "Blueprint/UserWidget.h"
#include "Components/BackgroundBlur.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"
#include "PlayerColorClickHandler.h"
#include "ComboBoxSelectionHandler.h"
#include "DebugUserWidget.generated.h"

/**
 * Struct to hold player trade information
 */
USTRUCT(BlueprintType)
struct FPlayerTradeInfo
{
	GENERATED_BODY()

	UPROPERTY()
	EPlayerColor PlayerColor;

	UPROPERTY()
	TMap<EResourceType, int32> Resources;

	UPROPERTY()
	UButton* PlayerButton;

	UPROPERTY()
	UBorder* PlayerBorder;

	FPlayerTradeInfo() : PlayerColor(EPlayerColor::None), PlayerButton(nullptr), PlayerBorder(nullptr) {}
};

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
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* VictoryPointsUI = nullptr;
	
	// BuildModal
	UPROPERTY(meta=(BindWidget))
	UButton* BuildButtonUI = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* BuildModalUI = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UButton* SettlementBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* RoadBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* CityBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* DevelopmentUpgradeBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN = nullptr;
	
	// TradeModal

	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* TradeModalUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* TradeOptionsModalUI = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UButton* TradeBankUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* TradePlayerUI = nullptr;

	// Player Selection Panel (replaces ToPlayer combobox)
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* PlayerSelectionList = nullptr;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* PlayerSelectionList_1 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SelectedPlayerText = nullptr;
	
	// You give
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WoodSelection = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* BrickSelection = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* SheepSelection = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WheatSelection = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* OreSelection = nullptr;
	
	// You get
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WoodSelection_1 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* BrickSelection_1 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* SheepSelection_1 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WheatSelection_1 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* OreSelection_1 = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UButton* FinalizeTrade = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_1 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* TradeButtonUI = nullptr;
	
	UPROPERTY()
	TMap<EResourceType, int32> YouGetResources;
	UPROPERTY()
	TMap<EResourceType, int32> YouGiveResources;
	UPROPERTY()
	TMap<UComboBoxString*, EResourceType> ComboToKey;

	UPROPERTY()
	EPlayerColor SelectedTradeTarget;

	UPROPERTY()
	TArray<FPlayerTradeInfo> AvailablePlayers;

	UPROPERTY()
	TMap<UButton*, EPlayerColor> ButtonToPlayerColor;

	UPROPERTY()
	TArray<UPlayerColorClickHandler*> ClickHandlers;

	UPROPERTY()
	TArray<UComboBoxSelectionHandler*> ComboBoxHandlers;

	UPROPERTY()
	TArray<UComboBoxSelectionHandler*> BankTradeComboBoxHandlers;

	// TradeOfferModal
	
	UPROPERTY()
	TMap<EResourceType, int32> offerGiveSTG;
	TMap<EResourceType, int32> offerGetSTG;
	
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* TradeOfferModalUI = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* fromText = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* toText = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_2 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_3 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* AcceptBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* DeclineBTN = nullptr;
	
	// They give OFFER
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WoodSelection_2 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* BrickSelection_2 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* SheepSelection_2 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WheatSelection_2 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* OreSelection_2 = nullptr;
	
	// You give OFFER
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WoodSelection_3 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* BrickSelection_3 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* SheepSelection_3 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WheatSelection_3 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* OreSelection_3 = nullptr;
	
	// Bank Trade
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* TradeTypeSelection = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_4 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* BankTradeModalUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* FinalizeBTN_1 = nullptr;
	
	// You give OFFER
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WoodSelection_4 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* BrickSelection_4 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* SheepSelection_4 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WheatSelection_4 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* OreSelection_4 = nullptr;
	
	// Bank gives OFFER
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WoodSelection_5 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* BrickSelection_5 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* SheepSelection_5 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WheatSelection_5 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* OreSelection_5 = nullptr;
	
	
	
	void SetCurrentPlayerUI(EPlayerColor PlayerColor);
	void SetCurrentTurnStepUI(ETurnStep TurnStep, EPlacementNode SetupStep, EGamePhase GamePhase);
	void SetCurrentPhaseUI(EGamePhase GamePhase);
	void SetResourceUI(TMap<EResourceType, int32> Resources);
	void SetVictoryPointsUI(int32 VictoryPoints);
	
	// Build Modal
	void SetVisibleBM(bool Enable);
	void SetEnabledINDIVIDUALBM(bool Enable, EBuildable Buildable);
	
	UFUNCTION()
	void OnClickedSettlement();
	UFUNCTION()
	void OnClickedRoad();
	UFUNCTION()
	void OnClickedCity();
	UFUNCTION()
	void OnClickedBuildModal();
	UFUNCTION()
	void OnClickedCloseButtonBM();
	
	// Trade Modal
	void SetVisibleTM(bool Enable);

	void SetupComboBoxYOUGIVE(UComboBoxString* ComboBox);

	void SetupComboBoxYOUGET(UComboBoxString* ComboBox);

	// Player Selection System
	void RefreshPlayerTradeOptions();

	UFUNCTION()
	void OnPlayerCardClicked(EPlayerColor PlayerColor);

	// Trade resource management
	void ResetTradeResources();

	UFUNCTION()
	void OnComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType, EResourceType ResourceType, bool bIsYouGet);

	// TradeModal Offer
	void SetupSerializedCombobox(UComboBoxString* ComboBox, int32 amount);
	void ShowOffer(EPlayerColor from, EPlayerColor to, TMap<EResourceType, int32> offerGive, TMap<EResourceType, int32> offerGet);
	
	// BankTrade
	UPROPERTY()
	EBankTradeMethods BankTradeMethodSelected;
	UPROPERTY()
	TMap<EResourceType, int32> BankTradeYouGive;
	UPROPERTY()
	TMap<EResourceType, int32> BankTradeYouGet;
	
	void SetupBankTradeSelectionComboBox(UComboBoxString* ComboBox, const TArray<EBankTradeMethods>& AvailableMethods);
	void SetupBankTradeComboBoxYOUGIVE(UComboBoxString* ComboBox, EResourceType ResourceType = EResourceType::None);
	void SetupBankTradeComboBoxBANKGIVES(UComboBoxString* ComboBox, EResourceType ResourceType = EResourceType::None);

	void RefreshBankTradeOptions();
	
	UFUNCTION()
	void OnTradeSelected(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnClickedCloseBankTradeOptions();
	UFUNCTION()
	void FinalizeBankTrade();
	UFUNCTION()
	void OnAmountSelectedBankTrade(FString SelectedItem, ESelectInfo::Type SelectionType, EResourceType ResourceType, bool bIsYouGet);
	
	UFUNCTION()
	void OnClickedTradeModal();
	UFUNCTION()
	void OnClickedBankTrade();
	UFUNCTION()
	void OnClickedCloseButtonTM();
	UFUNCTION()
	void OnClickedCloseButtonTOM();
	UFUNCTION()
	void OnClickedCloseTradeOptions();
	UFUNCTION()
	void SendOffer();
	UFUNCTION()
	void OnClickedTradeOptionsModalUI();
	UFUNCTION()
	void AcceptTrade();
	UFUNCTION()
	void DeclineTrade();
	
	
	
protected:
	virtual void NativeOnInitialized() override;
	
private:
	void InitializeComboboxes();
	void InitializeBankTradeComboBoxes();
	
};
