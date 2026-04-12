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
#include "HexTile.h"
#include "DebugUserWidget.generated.h"

/**
 * Holds per-player info for the dynamically created trade partner cards.
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
 * Main HUD widget for the CATAN debug interface.
 *
 * Organized into logical groups:
 *   1. Game State Display     - Current player, phase, turn step, resources, victory points, robber
 *   2. Build Modal            - Settlement / Road / City / Development construction
 *   3. Player Trade Modal     - Send resource trade offers to other players
 *   4. Trade Offer Modal      - View / accept / decline incoming trade offers
 *   5. Bank Trade Modal       - Trade resources with the bank at various rates
 *   6. Trade Options Modal    - Root menu to choose between bank or player trade
 *   7. Rob Modal              - Choose which player to rob from
 *   8. Discard Modal          - Select resources to discard (7-roll penalty)
 */
UCLASS()
class CATANMEGAPROJECT_API UDebugUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* DebugWidget;

	// ============================================================
	// 1. Game State Display
	// ============================================================

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CurrentPlayerUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CurrentTurnStepUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CurrentPhaseUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* RobberText = nullptr;

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

	void SetCurrentPlayerUI(EPlayerColor PlayerColor);
	void SetCurrentTurnStepUI(ETurnStep TurnStep, EPlacementNode SetupStep, EGamePhase GamePhase);
	void SetCurrentPhaseUI(EGamePhase GamePhase);
	void SetResourceUI(TMap<EResourceType, int32> Resources);
	void SetVictoryPointsUI(int32 VictoryPoints);
	void SetRobberText(bool bEnable);

	// ============================================================
	// 2. Build Modal
	// ============================================================

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
	UFUNCTION()
	void OnClickedDevelopmentUpgrade();

	// ============================================================
	// 3. Player Trade Modal
	// ============================================================

	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* TradeModalUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* TradePlayerUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_1 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* FinalizeTrade = nullptr;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* PlayerSelectionList = nullptr;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* PlayerSelectionList_1 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SelectedPlayerText = nullptr;

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

	UPROPERTY()
	TMap<EResourceType, int32> YouGiveResources;

	UPROPERTY()
	TMap<EResourceType, int32> YouGetResources;

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

	void SetVisibleTM(bool Enable);
	void SetupComboBoxYOUGIVE(UComboBoxString* ComboBox);
	void SetupComboBoxYOUGET(UComboBoxString* ComboBox);
	void RefreshPlayerTradeOptions();
	void ResetTradeResources();

	UFUNCTION()
	void OnPlayerCardClicked(EPlayerColor PlayerColor);
	UFUNCTION()
	void OnComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType, EResourceType ResourceType, bool bIsYouGet);
	UFUNCTION()
	void OnClickedTradeModal();
	UFUNCTION()
	void OnClickedCloseButtonTM();
	UFUNCTION()
	void SendOffer();

	// ============================================================
	// 4. Trade Offer Modal (incoming offers)
	// ============================================================

	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* TradeOfferModalUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* fromText = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* toText = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_2 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* AcceptBTN = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* DeclineBTN = nullptr;

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

	UPROPERTY()
	TMap<EResourceType, int32> offerGiveSTG;

	UPROPERTY()
	TMap<EResourceType, int32> offerGetSTG;

	void SetupSerializedCombobox(UComboBoxString* ComboBox, int32 amount);
	void ShowOffer(EPlayerColor from, EPlayerColor to, TMap<EResourceType, int32> offerGive, TMap<EResourceType, int32> offerGet);

	UFUNCTION()
	void OnClickedCloseButtonTOM();
	UFUNCTION()
	void AcceptTrade();
	UFUNCTION()
	void DeclineTrade();

	// ============================================================
	// 5. Bank Trade Modal
	// ============================================================

	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* BankTradeModalUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* TradeBankUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_4 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* FinalizeBTN_1 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UComboBoxString* TradeTypeSelection = nullptr;

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

	UPROPERTY()
	EBankTradeMethods BankTradeMethodSelected;

	UPROPERTY()
	TMap<EResourceType, int32> BankTradeYouGive;

	UPROPERTY()
	TMap<EResourceType, int32> BankTradeYouGet;

	UPROPERTY()
	TArray<UComboBoxSelectionHandler*> BankTradeComboBoxHandlers;

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
	void OnClickedBankTrade();

	// ============================================================
	// 6. Trade Options Modal (root trade menu)
	// ============================================================

	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* TradeOptionsModalUI = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_3 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* TradeButtonUI = nullptr;

	UFUNCTION()
	void OnClickedTradeOptionsModalUI();
	UFUNCTION()
	void OnClickedCloseTradeOptions();
	
	// ============================================================
	// 7. Rob Modal
	// ============================================================
	
	UPROPERTY()
	EPlayerColor ButtonOne;
	UPROPERTY()
	EPlayerColor ButtonTwo;
	UPROPERTY()
	EPlayerColor ButtonThree;
	
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_6 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* RobModalUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* PlayerOneBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* PlayerTwoBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* PlayerThreeBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerOneText = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerTwoText = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerThreeText = nullptr;
	
	UFUNCTION()
	void RenameRobButtons(AHexTile* Tile);
	UFUNCTION()
	void OnClickedRobButtonOne();
	UFUNCTION()
	void OnClickedRobButtonTwo();
	UFUNCTION()
	void OnClickedRobButtonThree();
	UFUNCTION()
	void SetVisibleRobModal(AHexTile* Tile);
	UFUNCTION()
	void SetHiddenRobModal();
	
	// ============================================================
	// 8. Discard Modal
	// ============================================================
	
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* RobberDiscardModalUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* DiscardBTN = nullptr;
	
	UPROPERTY()
	TMap<EResourceType, int32> DiscardResources;
	
	UPROPERTY()
	EPlayerColor DiscardPlayerColor = EPlayerColor::None;
	
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WoodSelection_6 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* BrickSelection_6 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* SheepSelection_6 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WheatSelection_6 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* OreSelection_6 = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemsText = nullptr;
	
	UPROPERTY()
	TArray<UComboBoxSelectionHandler*> DiscardComboBoxHandlers;

	int32 GetCurrentDiscardResources();
	
	UFUNCTION()
	void OnDiscardSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType, EResourceType ResourceType);
	
	UFUNCTION()
	void DetermineDiscardAmount();
	
	UFUNCTION()
	void Discard();
	UFUNCTION()
	void ShowDiscardModal(EPlayerColor PlayerColor);
	UFUNCTION()
	void HideDiscardModal();
	
	UFUNCTION()
	void FinishRobPhase();
	
	// ============================================================
	// 9.0 Development Cards: General
	// ============================================================
	
	UPROPERTY(meta=(BindWidget))
	UButton* DevCardButtonUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_7 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoadBuildingText = nullptr;
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* DevelopmentCardsUI = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* KnightAmount = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* KnightBTN = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* VictoryPointAmount = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* VictoryPointBTN = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* YOPAmount = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* YOPBTN = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MonopolyAmount = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* MonopolyBTN = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoadBuildingAmount = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* RoadBuildingBTN = nullptr;
	
	void SetRoadBuildingText(bool bEnable, int32 roadsBuilt);
	UFUNCTION()
	void SetDevCardUIEnabled();
	UFUNCTION()
	void SetDevCardUIDisabled();
	
	void UpdateDevCards();
	
	UFUNCTION()
	void OnClickedKnight();
	UFUNCTION()
	void OnClickedYOP();
	UFUNCTION()
	void OnClickedMonopoly();
	UFUNCTION()
	void OnClickedRoadBuilding();
	
	// ============================================================
	// 9.1 Development Cards: YOP and Monopoly Modal Menus
	// ============================================================
	
	// YOP
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* YOPModalUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_9 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* YOPFinaliseButton = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* YOPText = nullptr;
	
	UPROPERTY()
	TMap<EResourceType, int32> ResourcesSelected;
	
	UPROPERTY()
	TArray<UComboBoxSelectionHandler*> YOPComboBoxHandlers; 
	
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WoodSelection_7 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* BrickSelection_7 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* SheepSelection_7 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* WheatSelection_7 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* OreSelection_7 = nullptr;
	
	UFUNCTION()
	void ShowYOPModal();
	UFUNCTION()
	void HideYOPModal();
	
	UFUNCTION()
	void DetermineAmountGotten();
	UFUNCTION()
	void OnYOPComboboxChanged(const FString& SelectedItem, ESelectInfo::Type SelectionType, EResourceType ResourceType);
	
	UFUNCTION()
	void InitializeYOPComboboxes();
	UFUNCTION()
	void FinaliseYOP();
	
	// Monopoly
	UPROPERTY(meta=(BindWidget))
	UButton* CloseBTN_8 = nullptr;
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* MonopolyModalUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* MNWoodBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* MNBrickBTN = nullptr;
	UPROPERTY(meta=(BindWidget))
	UButton* MNSheepBTN = nullptr;
	UPROPERTY(meta=(BindWidget))	
	UButton* MNWheatBTN = nullptr;
	UPROPERTY(meta=(BindWidget))	
	UButton* MNOreBTN = nullptr;
	
	UFUNCTION()
	void OnClickedWoodMN();
	UFUNCTION()
	void OnClickedBrickMN();
	UFUNCTION()
	void OnClickedSheepMN();
	UFUNCTION()
	void OnClickedWheatMN();
	UFUNCTION()
	void OnClickedOreMN();
	UFUNCTION()
	void ShowMonopolyModal();
	UFUNCTION()
	void HideMonopolyModal();
	// ============================================================
	// Game Over Screen
	// ============================================================
	
	UPROPERTY(meta=(BindWidget))
	UBackgroundBlur* GameOverUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* GameOverText = nullptr;
	
	void ShowGameOver(EPlayerColor Winner);
	
	// ============================================================
	// Lifecycle
	// ============================================================

protected:
	virtual void NativeOnInitialized() override;

private:
	void InitializeComboboxes();
	void InitializeBankTradeComboBoxes();
	void InitializeDiscardComboBoxes(EPlayerColor PlayerColor);
};
