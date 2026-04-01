#include "DebugUserWidget.h"

#include <string>

#include "GameModeCPP.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerColorClickHandler.h"
#include "ComboBoxSelectionHandler.h"

void UDebugUserWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // Optional: Initialize with default values when the widget is created
    if (CurrentPlayerUI)
    {
        CurrentPlayerUI->SetText(FText::FromString(TEXT("Current Player: None")));
    }

    if (CurrentTurnStepUI)
    {
        CurrentTurnStepUI->SetText(FText::FromString(TEXT("Turn Step: Waiting")));
    }

    if (CurrentPhaseUI)
    {
        CurrentPhaseUI->SetText(FText::FromString(TEXT("Phase: None")));
    }

    // Initialize resources to 0
    if (WoodUI)  WoodUI->SetText(FText::FromString(TEXT("Wood: 0")));
    if (BrickUI) BrickUI->SetText(FText::FromString(TEXT("Brick: 0")));
    if (SheepUI) SheepUI->SetText(FText::FromString(TEXT("Sheep: 0")));
    if (WheatUI) WheatUI->SetText(FText::FromString(TEXT("Wheat: 0")));
    if (OreUI)   OreUI->SetText(FText::FromString(TEXT("Ore: 0")));

    if (VictoryPointsUI)
    {
        VictoryPointsUI->SetText(FText::FromString(TEXT("Victory Points: 0")));
    }

    BuildModalUI->SetVisibility(ESlateVisibility::Hidden);
    TradeModalUI->SetVisibility(ESlateVisibility::Hidden);
    TradeOfferModalUI->SetVisibility(ESlateVisibility::Hidden);
    BankTradeModalUI->SetVisibility(ESlateVisibility::Hidden);
    TradeOptionsModalUI->SetVisibility(ESlateVisibility::Hidden);
    

        // Bind button click events
    SettlementBTN->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedSettlement);
    RoadBTN->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedRoad);
    CityBTN->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedCity);
    BuildButtonUI->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedBuildModal);
    CloseBTN->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedCloseButtonBM);
    CloseBTN_1->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedCloseButtonTM);
    CloseBTN_2->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedCloseButtonTOM);
    CloseBTN_3->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedCloseTradeOptions);
    CloseBTN_4->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedCloseBankTradeOptions);
    TradeBankUI->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedBankTrade);
    TradeButtonUI->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedTradeOptionsModalUI);
    TradePlayerUI->OnClicked.AddDynamic(this, &UDebugUserWidget::OnClickedTradeModal);
    FinalizeTrade->OnClicked.AddDynamic(this, &UDebugUserWidget::SendOffer);
    AcceptBTN->OnClicked.AddDynamic(this, &UDebugUserWidget::AcceptTrade);
    DeclineBTN->OnClicked.AddDynamic(this, &UDebugUserWidget::DeclineTrade);



    YouGiveResources.Add(EResourceType::Wood, 0);
    YouGiveResources.Add(EResourceType::Brick, 0);
    YouGiveResources.Add(EResourceType::Sheep, 0);
    YouGiveResources.Add(EResourceType::Wheat, 0);
    YouGiveResources.Add(EResourceType::Ore, 0);

    YouGetResources.Add(EResourceType::Wood, 0);
    YouGetResources.Add(EResourceType::Brick, 0);
    YouGetResources.Add(EResourceType::Sheep, 0);
    YouGetResources.Add(EResourceType::Wheat, 0);
    YouGetResources.Add(EResourceType::Ore, 0);

    InitializeComboboxes();
    InitializeBankTradeComboBoxes();

}

void UDebugUserWidget::OnClickedCloseButtonTOM()
{
    TradeOfferModalUI->SetVisibility(ESlateVisibility::Hidden);
}

void UDebugUserWidget::OnClickedCloseTradeOptions()
{
    TradeOptionsModalUI->SetVisibility(ESlateVisibility::Hidden);
}

void UDebugUserWidget::SetCurrentPlayerUI(EPlayerColor PlayerColor)
{
    if (CurrentPlayerUI)
    {
        FString PlayerColorString = UEnum::GetValueAsString(PlayerColor);
        // Clean up the enum string (removes "EPlayerColor::" prefix)
        PlayerColorString = PlayerColorString.Replace(TEXT("EPlayerColor::"), TEXT(""));
        
        CurrentPlayerUI->SetText(FText::FromString(TEXT("Current Player: ") + PlayerColorString));
    }
}

void UDebugUserWidget::SetVictoryPointsUI(int32 VictoryPoints)
{
    if (VictoryPointsUI)
    {
        VictoryPointsUI->SetText(FText::FromString(FString::Printf(TEXT("Victory Points: %d"), VictoryPoints)));
    }
}

void UDebugUserWidget::SetVisibleBM(bool Enable)
{
    if (BuildButtonUI)
    {
        if (Enable)
            BuildButtonUI->SetIsEnabled(true);
        else
        {
            BuildButtonUI->SetIsEnabled(false);
        }
    }
}

void UDebugUserWidget::SetEnabledINDIVIDUALBM(bool Enable, EBuildable Buildable)
{
    switch (Buildable)
    {
        case EBuildable::Settlement:
            if (SettlementBTN) SettlementBTN->SetIsEnabled(Enable);
            break;
        case EBuildable::Road:
            if (RoadBTN) RoadBTN->SetIsEnabled(Enable);
            break;
        case EBuildable::City:
            if (CityBTN) CityBTN->SetIsEnabled(Enable);
            break;
        case EBuildable::Development:
            if (DevelopmentUpgradeBTN) DevelopmentUpgradeBTN->SetIsEnabled(Enable);
            break;
        default:
            break;
    }
}

void UDebugUserWidget::OnClickedSettlement()
{
    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    if (GameMode)
    {
        GameMode->ChangeRules(EBuildable::Settlement, true);
        OnClickedCloseButtonBM();
    }
}
void UDebugUserWidget::OnClickedRoad()
{
    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    if (GameMode)
    {
        GameMode->ChangeRules(EBuildable::Road, true);
        OnClickedCloseButtonBM();
    }
}
void UDebugUserWidget::OnClickedCity()
{
    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    if (GameMode)
    {
        GameMode->ChangeRules(EBuildable::City, true);
        OnClickedCloseButtonBM();
    }
}

void UDebugUserWidget::OnClickedBuildModal()
{
    BuildModalUI->SetIsEnabled(true);
    BuildModalUI->SetVisibility(ESlateVisibility::Visible);
}

void UDebugUserWidget::OnClickedCloseButtonBM()
{
    BuildModalUI->SetIsEnabled(false);
    BuildModalUI->SetVisibility(ESlateVisibility::Hidden);
}

void UDebugUserWidget::SetVisibleTM(bool Enable)
{
    UE_LOG(LogTemp, Log, TEXT("SetVisibleTM called with Enable=%d"), Enable);
    
    if (TradeModalUI)
    {
        UE_LOG(LogTemp, Log, TEXT("SetVisibleTM: TradeModalUI is valid"));
        
        if (Enable)
        {
            // Reset trade resources before opening
            ResetTradeResources();
            
            // Refresh player list with current resources when opening trade modal
            UE_LOG(LogTemp, Log, TEXT("SetVisibleTM: Calling RefreshPlayerTradeOptions"));
            RefreshPlayerTradeOptions();
            TradeModalUI->SetVisibility(ESlateVisibility::Visible);
            TradeModalUI->SetIsEnabled(true);
            UE_LOG(LogTemp, Log, TEXT("SetVisibleTM: TradeModalUI set to Visible and Enabled"));
        }
        else
        {
            TradeModalUI->SetVisibility(ESlateVisibility::Hidden);
            UE_LOG(LogTemp, Log, TEXT("SetVisibleTM: TradeModalUI set to Hidden"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SetVisibleTM: TradeModalUI is NULL!"));
    }
}

void UDebugUserWidget::OnAmountSelectedBankTrade(FString SelectedItem, ESelectInfo::Type SelectionType,
    EResourceType ResourceType, bool bIsYouGet)
{
    if (SelectionType == ESelectInfo::Direct) return;

    int32 Amount = FCString::Atoi(*SelectedItem);

    if (bIsYouGet)
    {
        if (Amount > 0)
        {
            BankTradeYouGet.Empty();
            BankTradeYouGet.Add(ResourceType, Amount);
        }
        else
        {
            BankTradeYouGet.Remove(ResourceType);
        }
    }
    else
    {
        if (Amount > 0)
        {
            BankTradeYouGive.Empty();
            BankTradeYouGive.Add(ResourceType, Amount);
        }
        else
        {
            BankTradeYouGive.Remove(ResourceType);
        }
    }

    RefreshBankTradeOptions();
}

void UDebugUserWidget::OnClickedTradeModal()
{
    UE_LOG(LogTemp, Log, TEXT("OnClickedTradeModal: Button clicked, calling SetVisibleTM"));
    SetVisibleTM(true);
}

void UDebugUserWidget::OnClickedCloseButtonTM()
{
    TradeModalUI->SetIsEnabled(false);
    TradeModalUI->SetVisibility(ESlateVisibility::Hidden);
}

void UDebugUserWidget::SendOffer()
{
    UE_LOG(LogTemp, Log, TEXT("=== SendOffer: Finalize Trade Clicked ==="));
    
    // Send GIVE Resources and GET Resources with both the player colors.
    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("SendOffer: GameMode is NULL!"));
        return;
    }
    
    EPlayerColor CurrentPlayerColor = GameMode->GetCurrentPlayer().PlayerColor;
    
    UE_LOG(LogTemp, Log, TEXT("SendOffer: From Player: %d"), (int32)CurrentPlayerColor);
    UE_LOG(LogTemp, Log, TEXT("SendOffer: To Player (SelectedTradeTarget): %d"), (int32)SelectedTradeTarget);
    
    if (SelectedTradeTarget == EPlayerColor::None)
    {
        UE_LOG(LogTemp, Error, TEXT("SendOffer: No trade target selected! Please select a player first."));
        return;
    }
    
    // Log the resources being offered
    UE_LOG(LogTemp, Log, TEXT("SendOffer: You Give - Wood:%d Brick:%d Sheep:%d Wheat:%d Ore:%d"),
        YouGiveResources.Contains(EResourceType::Wood) ? YouGiveResources[EResourceType::Wood] : 0,
        YouGiveResources.Contains(EResourceType::Brick) ? YouGiveResources[EResourceType::Brick] : 0,
        YouGiveResources.Contains(EResourceType::Sheep) ? YouGiveResources[EResourceType::Sheep] : 0,
        YouGiveResources.Contains(EResourceType::Wheat) ? YouGiveResources[EResourceType::Wheat] : 0,
        YouGiveResources.Contains(EResourceType::Ore) ? YouGiveResources[EResourceType::Ore] : 0);
    
    UE_LOG(LogTemp, Log, TEXT("SendOffer: You Get - Wood:%d Brick:%d Sheep:%d Wheat:%d Ore:%d"),
        YouGetResources.Contains(EResourceType::Wood) ? YouGetResources[EResourceType::Wood] : 0,
        YouGetResources.Contains(EResourceType::Brick) ? YouGetResources[EResourceType::Brick] : 0,
        YouGetResources.Contains(EResourceType::Sheep) ? YouGetResources[EResourceType::Sheep] : 0,
        YouGetResources.Contains(EResourceType::Wheat) ? YouGetResources[EResourceType::Wheat] : 0,
        YouGetResources.Contains(EResourceType::Ore) ? YouGetResources[EResourceType::Ore] : 0);
    
    // YouGiveResources and YouGetResources already have the amounts set in the combo box functions
    // Check if the sender/ the getter has X resources
    
    if (GameMode->GetPlayerByColor(CurrentPlayerColor).CanAfford(YouGiveResources) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("SendOffer: You cannot afford to give these resources! Offer not sent."));
        return;
    }
    if (GameMode->GetPlayerByColor(SelectedTradeTarget).CanAfford(YouGetResources) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("SendOffer: The selected player cannot afford to give these resources! Offer not sent."));
        return;
    }
    GameMode->ProcessTradeOffer(CurrentPlayerColor, SelectedTradeTarget, YouGiveResources, YouGetResources);
    
    UE_LOG(LogTemp, Log, TEXT("SendOffer: Trade offer sent, closing modal"));
    OnClickedCloseButtonTM();
    OnClickedCloseTradeOptions();
}

void UDebugUserWidget::OnClickedBankTrade()
{
    BankTradeModalUI->SetVisibility(ESlateVisibility::Visible);

    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    TArray<EBankTradeMethods> AvailableMethods;
    if (GameMode)
    {
        AvailableMethods = GameMode->GetAvailableBankTradeMethods(GameMode->GetCurrentPlayer().PlayerColor);
    }
    else
    {
        AvailableMethods.Add(EBankTradeMethods::Standard);
    }

    SetupBankTradeSelectionComboBox(TradeTypeSelection, AvailableMethods);
    BankTradeYouGive.Empty();
    BankTradeYouGet.Empty();
    RefreshBankTradeOptions();
}

void UDebugUserWidget::OnClickedTradeOptionsModalUI()
{
    TradeOptionsModalUI->SetVisibility(ESlateVisibility::Visible);
}

void UDebugUserWidget::InitializeComboboxes()
{
    UE_LOG(LogTemp, Log, TEXT("InitializeComboboxes: Starting initialization"));
    UE_LOG(LogTemp, Log, TEXT("InitializeComboboxes: PlayerSelectionList is %s"), PlayerSelectionList ? TEXT("valid") : TEXT("NULL"));
    UE_LOG(LogTemp, Log, TEXT("InitializeComboboxes: SelectedPlayerText is %s"), SelectedPlayerText ? TEXT("valid") : TEXT("NULL"));
    
    // Note: Player selection is now done via RefreshPlayerTradeOptions when the trade modal is opened
    // This ensures PlayerOrder is populated in the GameMode
    
    SetupComboBoxYOUGIVE(WoodSelection);
    SetupComboBoxYOUGIVE(BrickSelection);
    SetupComboBoxYOUGIVE(WheatSelection);
    SetupComboBoxYOUGIVE(SheepSelection);
    SetupComboBoxYOUGIVE(OreSelection);

    SetupComboBoxYOUGET(WoodSelection_1);
    SetupComboBoxYOUGET(BrickSelection_1);
    SetupComboBoxYOUGET(WheatSelection_1);
    SetupComboBoxYOUGET(SheepSelection_1);
    SetupComboBoxYOUGET(OreSelection_1);
    
    UE_LOG(LogTemp, Log, TEXT("InitializeComboboxes: Finished initialization"));
}

void UDebugUserWidget::SetupComboBoxYOUGIVE(UComboBoxString* ComboBox)
{
    if (!ComboBox) return;

    ComboBox->ClearOptions();

    // Populate options
    for (int32 i = 0; i <= 10; i++)
    {
        ComboBox->AddOption(FString::FromInt(i));
    }
    ComboBox->SetSelectedOption(FString::FromInt(0)); // Default to 0 for "You Give"

    // Map this combobox to its resource type
    EResourceType ResourceType = EResourceType::None;
    if (ComboBox == WoodSelection) ResourceType = EResourceType::Wood;
    else if (ComboBox == BrickSelection) ResourceType = EResourceType::Brick;
    else if (ComboBox == SheepSelection) ResourceType = EResourceType::Sheep;
    else if (ComboBox == WheatSelection) ResourceType = EResourceType::Wheat;
    else if (ComboBox == OreSelection) ResourceType = EResourceType::Ore;

    ComboToKey.Add(ComboBox, ResourceType);

    // Create a handler for this combobox
    UComboBoxSelectionHandler* Handler = NewObject<UComboBoxSelectionHandler>(this);
    Handler->ResourceType = ResourceType;
    Handler->bIsYouGet = false; // You Give
    Handler->ParentWidget = this;
    ComboBoxHandlers.Add(Handler);

    // Bind the selection changed event
    ComboBox->OnSelectionChanged.AddDynamic(Handler, &UComboBoxSelectionHandler::HandleSelectionChanged);
}

void UDebugUserWidget::SetupComboBoxYOUGET(UComboBoxString* ComboBox)
{
    if (!ComboBox) return;

    ComboBox->ClearOptions();

    // Populate options
    for (int32 i = 0; i <= 10; i++)
    {
        ComboBox->AddOption(FString::FromInt(i));
    }
    ComboBox->SetSelectedOption(FString::FromInt(0)); // Default to 0 for "You Get"

    // Map this combobox to its resource type
    EResourceType ResourceType = EResourceType::None;
    if (ComboBox == WoodSelection_1) ResourceType = EResourceType::Wood;
    else if (ComboBox == BrickSelection_1) ResourceType = EResourceType::Brick;
    else if (ComboBox == SheepSelection_1) ResourceType = EResourceType::Sheep;
    else if (ComboBox == WheatSelection_1) ResourceType = EResourceType::Wheat;
    else if (ComboBox == OreSelection_1) ResourceType = EResourceType::Ore;

    ComboToKey.Add(ComboBox, ResourceType);

    // Create a handler for this combobox
    UComboBoxSelectionHandler* Handler = NewObject<UComboBoxSelectionHandler>(this);
    Handler->ResourceType = ResourceType;
    Handler->bIsYouGet = true; // You Get
    Handler->ParentWidget = this;
    ComboBoxHandlers.Add(Handler);

    // Bind the selection changed event
    ComboBox->OnSelectionChanged.AddDynamic(Handler, &UComboBoxSelectionHandler::HandleSelectionChanged);
}

void UDebugUserWidget::RefreshPlayerTradeOptions()
{
    UE_LOG(LogTemp, Log, TEXT("=== RefreshPlayerTradeOptions START ==="));
    
    if (!PlayerSelectionList)
    {
        UE_LOG(LogTemp, Error, TEXT("RefreshPlayerTradeOptions: PlayerSelectionList is NULL! Check UMG binding!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: PlayerSelectionList is valid"));

    // Clear existing player entries
    PlayerSelectionList->ClearChildren();
    AvailablePlayers.Empty();
    ClickHandlers.Empty();
    UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: Cleared existing children"));

    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("RefreshPlayerTradeOptions: GameMode is NULL!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: GameMode is valid"));

    if (GameMode->PlayerOrder.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("RefreshPlayerTradeOptions: PlayerOrder is EMPTY! Game hasn't started or no players?"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: PlayerOrder has %d players"), GameMode->PlayerOrder.Num());

    EPlayerColor CurrentPlayerColor = GameMode->GetCurrentPlayer().PlayerColor;
    UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: Current player is %d"), (int32)CurrentPlayerColor);

    int32 CardsCreated = 0;
    
    // Create a player card for each player in the game
    for (EPlayerColor PlayerColor : GameMode->PlayerOrder)
    {
        FString PlayerColorName = UEnum::GetValueAsString(PlayerColor);
        PlayerColorName = PlayerColorName.Replace(TEXT("EPlayerColor::"), TEXT(""));
        
        // Skip current player - can't trade with yourself
        if (PlayerColor == CurrentPlayerColor)
        {
            UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: Skipping current player %s"), *PlayerColorName);
            continue;
        }

        // Get player resources from GameMode
        TMap<EResourceType, int32> PlayerResources = GameMode->GetPlayerResources(PlayerColor);
        UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: Player %s has %d resources"), *PlayerColorName, PlayerResources.Num());

        // Create the player card widget dynamically
        UHorizontalBox* PlayerCard;
        if (CardsCreated % 2 == 0)
        {
            PlayerCard = NewObject<UHorizontalBox>(PlayerSelectionList);
        }
        else
        {
            PlayerCard = NewObject<UHorizontalBox>(PlayerSelectionList_1);
        }
        
        PlayerCard->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: Created PlayerCard HorizontalBox"));

        // Create border for selection highlighting
        UBorder* PlayerBorder = NewObject<UBorder>(PlayerCard);
        PlayerBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        
        // Create button for selection
        UButton* PlayerButton = NewObject<UButton>(PlayerBorder);
        PlayerButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        // Create text block for player name
        UTextBlock* PlayerNameText = NewObject<UTextBlock>(PlayerButton);
        FString PlayerName = UEnum::GetValueAsString(PlayerColor);
        PlayerName = PlayerName.Replace(TEXT("EPlayerColor::"), TEXT(""));
        PlayerNameText->SetText(FText::FromString(PlayerName));
        PlayerNameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

        // Create vertical box for resources
        UVerticalBox* ResourcesBox = NewObject<UVerticalBox>(PlayerButton);
        ResourcesBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        // Add resource text blocks
        TArray<EResourceType> ResourceTypes = { EResourceType::Wood, EResourceType::Brick, EResourceType::Sheep, EResourceType::Wheat, EResourceType::Ore };
        
        for (EResourceType ResType : ResourceTypes)
        {
            UTextBlock* ResText = NewObject<UTextBlock>(ResourcesBox);
            int32 Amount = PlayerResources.Contains(ResType) ? PlayerResources[ResType] : 0;
            FString ResName = UEnum::GetValueAsString(ResType);
            ResName = ResName.Replace(TEXT("EResourceType::"), TEXT(""));
            ResText->SetText(FText::FromString(FString::Printf(TEXT("%s: %d"), *ResName, Amount)));
            ResText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f)));
            ResourcesBox->AddChild(ResText);
        }

        // Create a vertical box to hold both name and resources (Button can only have 1 child)
        UVerticalBox* MainContentBox = NewObject<UVerticalBox>(PlayerButton);
        MainContentBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        MainContentBox->AddChild(PlayerNameText);
        MainContentBox->AddChild(ResourcesBox);

        // Store player info for later updates
        FPlayerTradeInfo PlayerInfo;
        PlayerInfo.PlayerColor = PlayerColor;
        PlayerInfo.Resources = PlayerResources;
        PlayerInfo.PlayerButton = PlayerButton;
        PlayerInfo.PlayerBorder = PlayerBorder;
        AvailablePlayers.Add(PlayerInfo);

        // Add to hierarchy - THIS IS CRITICAL
        PlayerBorder->SetContent(PlayerButton);
        PlayerButton->SetContent(MainContentBox);
        PlayerCard->AddChild(PlayerBorder);
        
        // Add to the correct list based on odd/even
        if (CardsCreated % 2 == 0)
        {
            PlayerSelectionList->AddChild(PlayerCard);
        }
        else
        {
            PlayerSelectionList_1->AddChild(PlayerCard);
        }
        
        UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: Added PlayerCard to hierarchy for %s"), *PlayerName);

        // Create a click handler for this specific player color
        UPlayerColorClickHandler* ClickHandler = NewObject<UPlayerColorClickHandler>(this);
        ClickHandler->PlayerColor = PlayerColor;
        ClickHandler->ParentWidget = this;
        ClickHandlers.Add(ClickHandler);

        // Bind click event using the handler
        PlayerButton->OnClicked.AddDynamic(ClickHandler, &UPlayerColorClickHandler::HandleClick);
        
        CardsCreated++;
        UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: Created card #%d for player %s"), CardsCreated, *PlayerName);
    }

    // Update selected player text
    if (SelectedPlayerText)
    {
        SelectedPlayerText->SetText(FText::FromString(TEXT("Select a player to trade with")));
        UE_LOG(LogTemp, Log, TEXT("RefreshPlayerTradeOptions: Updated SelectedPlayerText"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RefreshPlayerTradeOptions: SelectedPlayerText is NULL!"));
    }

    UE_LOG(LogTemp, Log, TEXT("=== RefreshPlayerTradeOptions END: Created %d player cards ==="), CardsCreated);
}

void UDebugUserWidget::OnPlayerCardClicked(EPlayerColor PlayerColor)
{
    SelectedTradeTarget = PlayerColor;
    
    // Update visual feedback - highlight selected player
    for (FPlayerTradeInfo& PlayerInfo : AvailablePlayers)
    {
        if (PlayerInfo.PlayerBorder && PlayerInfo.PlayerButton)
        {
            if (PlayerInfo.PlayerColor == PlayerColor)
            {
                // Highlight selected player
                PlayerInfo.PlayerBorder->SetBrushColor(FLinearColor(0.0f, 0.5f, 1.0f, 0.3f)); // Blue highlight
            }
            else
            {
                // Reset other players
                PlayerInfo.PlayerBorder->SetBrushColor(FLinearColor::Transparent);
            }
        }
    }

    // Update selected player text
    if (SelectedPlayerText)
    {
        FString PlayerName = UEnum::GetValueAsString(PlayerColor);
        PlayerName = PlayerName.Replace(TEXT("EPlayerColor::"), TEXT(""));
        SelectedPlayerText->SetText(FText::FromString(FString::Printf(TEXT("Trading with: %s"), *PlayerName)));
    }

    UE_LOG(LogTemp, Log, TEXT("OnPlayerCardClicked: Selected player %s"), *UEnum::GetValueAsString(PlayerColor));
}

void UDebugUserWidget::SetupSerializedCombobox(UComboBoxString* ComboBox, int32 amount)
{
    if (!ComboBox) return;
    ComboBox->ClearOptions();
    for (int32 i = 0; i <= 10; i++)
    {
        ComboBox->AddOption(FString::FromInt(i));
    }
    ComboBox->SetSelectedOption(FString::FromInt(amount));
    
}

void UDebugUserWidget::ShowOffer(EPlayerColor from, EPlayerColor to, TMap<EResourceType, int32> offerGive, TMap<EResourceType, int32> offerGet)
{
        // This function would be responsible for populating the trade offer UI with the details of the offer.
        // You would set the text of various UI elements to show what is being offered and requested, and who the offer is from/to.
    TradeOfferModalUI->SetVisibility(ESlateVisibility::Visible);
    fromText->SetText(FText::FromString("from: " + UEnum::GetValueAsString(from).Replace(TEXT("EPlayerColor::"), TEXT(""))));
    toText->SetText(FText::FromString("to: " + UEnum::GetValueAsString(to).Replace(TEXT("EPlayerColor::"), TEXT(""))));
    
    SetupSerializedCombobox(WoodSelection_2, offerGive.Contains(EResourceType::Wood) ? offerGive[EResourceType::Wood] : 0);
    SetupSerializedCombobox(BrickSelection_2, offerGive.Contains(EResourceType::Brick) ? offerGive[EResourceType::Brick] : 0);
    SetupSerializedCombobox(SheepSelection_2, offerGive.Contains(EResourceType::Sheep) ? offerGive[EResourceType::Sheep] : 0);
    SetupSerializedCombobox(WheatSelection_2, offerGive.Contains(EResourceType::Wheat) ? offerGive[EResourceType::Wheat] : 0);
    SetupSerializedCombobox(OreSelection_2, offerGive.Contains(EResourceType::Ore) ? offerGive[EResourceType::Ore] : 0);
    
    SetupSerializedCombobox(WoodSelection_3, offerGet.Contains(EResourceType::Wood) ? offerGet[EResourceType::Wood] : 0);
    SetupSerializedCombobox(BrickSelection_3, offerGet.Contains(EResourceType::Brick) ? offerGet[EResourceType::Brick] : 0);
    SetupSerializedCombobox(SheepSelection_3, offerGet.Contains(EResourceType::Sheep) ? offerGet[EResourceType::Sheep] : 0);
    SetupSerializedCombobox(WheatSelection_3, offerGet.Contains(EResourceType::Wheat) ? offerGet[EResourceType::Wheat] : 0);
    SetupSerializedCombobox(OreSelection_3, offerGet.Contains(EResourceType::Ore) ? offerGet[EResourceType::Ore] : 0);
    
    offerGiveSTG = offerGive;
    offerGetSTG = offerGet;
}

void UDebugUserWidget::SetupBankTradeSelectionComboBox(UComboBoxString* ComboBox, const TArray<EBankTradeMethods>& AvailableMethods)
{
    if (!ComboBox) return;
    ComboBox->ClearOptions();

    for (EBankTradeMethods BankTradeMethod : AvailableMethods)
    {
        FString bankTradeMethodName;
        if (BankTradeMethod == EBankTradeMethods::Standard)
        {
            bankTradeMethodName = TEXT("4:1");
        }
        else if (BankTradeMethod == EBankTradeMethods::Dock)
        {
            bankTradeMethodName = TEXT("3:1");
        }
        else if (BankTradeMethod == EBankTradeMethods::Wheat)
        {
            bankTradeMethodName = TEXT("2:1 - Wheat");
        }
        else if (BankTradeMethod == EBankTradeMethods::Ore)
        {
            bankTradeMethodName = TEXT("2:1 - Ore");
        }
        else if (BankTradeMethod == EBankTradeMethods::Wood)
        {
            bankTradeMethodName = TEXT("2:1 - Wood");
        }
        else if (BankTradeMethod == EBankTradeMethods::Brick)
        {
            bankTradeMethodName = TEXT("2:1 - Brick");
        }
        else if (BankTradeMethod == EBankTradeMethods::Sheep)
        {
            bankTradeMethodName = TEXT("2:1 - Sheep");
        }
        ComboBox->AddOption(bankTradeMethodName);
    }
    ComboBox->SetSelectedOption(TEXT("4:1"));
}

void UDebugUserWidget::SetupBankTradeComboBoxYOUGIVE(UComboBoxString* ComboBox, EResourceType ResourceType)
{
    if (!ComboBox) return;
    if (BankTradeMethodSelected == EBankTradeMethods::Standard)
    {
        // For standard 4:1 trade, allow selection from 0 to 10
        ComboBox->ClearOptions();
        ComboBox->AddOption(TEXT("0"));
        ComboBox->AddOption(TEXT("4"));
    }
    else if (BankTradeMethodSelected == EBankTradeMethods::Dock)
    {
        ComboBox->ClearOptions();
        ComboBox->AddOption(TEXT("0"));
        ComboBox->AddOption(TEXT("3"));
    }
    else
    {
        // For specific resource trades (2:1), only allow selection of 0 or 2
        ComboBox->ClearOptions();
        ComboBox->AddOption(TEXT("0"));
        ComboBox->AddOption(TEXT("2"));
    }
    if (BankTradeYouGive.Contains(ResourceType))
    {
        ComboBox->SetSelectedOption(FString::FromInt(BankTradeYouGive[ResourceType]));
    }
    else
    {
        ComboBox->SetSelectedOption(TEXT("0"));
    };
}

void UDebugUserWidget::SetupBankTradeComboBoxBANKGIVES(UComboBoxString* ComboBox, EResourceType ResourceType)
{
    if (!ComboBox) return;
    ComboBox->ClearOptions();
    ComboBox->AddOption(TEXT("0"));
    ComboBox->AddOption(TEXT("1"));
    if (BankTradeYouGet.Contains(ResourceType))
    {
        ComboBox->SetSelectedOption(FString::FromInt(BankTradeYouGet[ResourceType]));
    }
    else
    {
        ComboBox->SetSelectedOption(TEXT("0"));
    };
}

void UDebugUserWidget::RefreshBankTradeOptions()
{
    SetupBankTradeComboBoxYOUGIVE(WoodSelection_4, EResourceType::Wood);
    SetupBankTradeComboBoxYOUGIVE(BrickSelection_4, EResourceType::Brick);
    SetupBankTradeComboBoxYOUGIVE(SheepSelection_4, EResourceType::Sheep);
    SetupBankTradeComboBoxYOUGIVE(WheatSelection_4, EResourceType::Wheat);
    SetupBankTradeComboBoxYOUGIVE(OreSelection_4, EResourceType::Ore);
    SetupBankTradeComboBoxBANKGIVES(WoodSelection_5, EResourceType::Wood);
    SetupBankTradeComboBoxBANKGIVES(BrickSelection_5, EResourceType::Brick);
    SetupBankTradeComboBoxBANKGIVES(SheepSelection_5, EResourceType::Sheep);
    SetupBankTradeComboBoxBANKGIVES(WheatSelection_5, EResourceType::Wheat);
    SetupBankTradeComboBoxBANKGIVES(OreSelection_5, EResourceType::Ore);
}

void UDebugUserWidget::InitializeBankTradeComboBoxes()
{
    if (TradeTypeSelection)
    {
        TradeTypeSelection->OnSelectionChanged.AddDynamic(this, &UDebugUserWidget::OnTradeSelected);
    }

    auto SetupHandler = [&](UComboBoxString* ComboBox, EResourceType ResourceType, bool bIsYouGet)
    {
        if (!ComboBox) return;
        UComboBoxSelectionHandler* Handler = NewObject<UComboBoxSelectionHandler>(this);
        Handler->ResourceType = ResourceType;
        Handler->bIsYouGet = bIsYouGet;
        Handler->bIsBankTrade = true;
        Handler->ParentWidget = this;
        BankTradeComboBoxHandlers.Add(Handler);
        ComboBox->OnSelectionChanged.AddDynamic(Handler, &UComboBoxSelectionHandler::HandleSelectionChanged);
    };

    SetupHandler(WoodSelection_4, EResourceType::Wood, false);
    SetupHandler(BrickSelection_4, EResourceType::Brick, false);
    SetupHandler(SheepSelection_4, EResourceType::Sheep, false);
    SetupHandler(WheatSelection_4, EResourceType::Wheat, false);
    SetupHandler(OreSelection_4, EResourceType::Ore, false);

    SetupHandler(WoodSelection_5, EResourceType::Wood, true);
    SetupHandler(BrickSelection_5, EResourceType::Brick, true);
    SetupHandler(SheepSelection_5, EResourceType::Sheep, true);
    SetupHandler(WheatSelection_5, EResourceType::Wheat, true);
    SetupHandler(OreSelection_5, EResourceType::Ore, true);
}

void UDebugUserWidget::OnTradeSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (SelectedItem == TEXT("4:1"))
    {
        BankTradeMethodSelected = EBankTradeMethods::Standard;
    }
    else if (SelectedItem == TEXT("3:1"))
    {
        BankTradeMethodSelected = EBankTradeMethods::Dock;
    }
    else if (SelectedItem == TEXT("2:1 - Wood"))
    {
        BankTradeMethodSelected = EBankTradeMethods::Wood;
    }
    else if (SelectedItem == TEXT("2:1 - Brick"))
    {
        BankTradeMethodSelected = EBankTradeMethods::Brick;
    }
    else if (SelectedItem == TEXT("2:1 - Sheep"))
    {
        BankTradeMethodSelected = EBankTradeMethods::Sheep;
    }
    else if (SelectedItem == TEXT("2:1 - Wheat"))
    {
        BankTradeMethodSelected = EBankTradeMethods::Wheat;
    }
    else if (SelectedItem == TEXT("2:1 - Ore"))
    {
        BankTradeMethodSelected = EBankTradeMethods::Ore;
    }
    RefreshBankTradeOptions();
}

void UDebugUserWidget::OnClickedCloseBankTradeOptions()
{
    BankTradeMethodSelected = EBankTradeMethods::None;
    BankTradeModalUI->SetVisibility(ESlateVisibility::Hidden);
}

void UDebugUserWidget::FinalizeBankTrade()
{
    UE_LOG(LogTemp, Log, TEXT("FinalizeBankTrade: Not yet implemented"));
    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    GameMode->ProcessBankTrade(BankTradeYouGive, BankTradeYouGet, GameMode->GetCurrentPlayer().PlayerColor);
}

void UDebugUserWidget::AcceptTrade()
{
    UE_LOG(LogTemp, Log, TEXT("AcceptTrade: Accepting trade offer"));

    TradeOfferModalUI->SetVisibility(ESlateVisibility::Hidden);
    
    // The current player is the one who made the offer (from)
    // SelectedTradeTarget is the one who will receive (to)
    AGameModeCPP* GameMode = GetWorld()->GetAuthGameMode<AGameModeCPP>();
    EPlayerColor CurrentPlayer = GameMode->GetCurrentPlayer().PlayerColor;
    
    UE_LOG(LogTemp, Log, TEXT("AcceptTrade: CurrentPlayer=%d, SelectedTradeTarget=%d"), (int32)CurrentPlayer, (int32)SelectedTradeTarget);
    UE_LOG(LogTemp, Log, TEXT("AcceptTrade: offerGiveSTG - Wood:%d Brick:%d Sheep:%d Wheat:%d Ore:%d"),
        offerGiveSTG.Contains(EResourceType::Wood) ? offerGiveSTG[EResourceType::Wood] : 0,
        offerGiveSTG.Contains(EResourceType::Brick) ? offerGiveSTG[EResourceType::Brick] : 0,
        offerGiveSTG.Contains(EResourceType::Sheep) ? offerGiveSTG[EResourceType::Sheep] : 0,
        offerGiveSTG.Contains(EResourceType::Wheat) ? offerGiveSTG[EResourceType::Wheat] : 0,
        offerGiveSTG.Contains(EResourceType::Ore) ? offerGiveSTG[EResourceType::Ore] : 0);
    
    GameMode->TradeAccepted(CurrentPlayer, SelectedTradeTarget, offerGiveSTG, offerGetSTG);

    // Reset resources properly using direct assignment
    ResetTradeResources();
    
    offerGetSTG.Empty();
    offerGiveSTG.Empty();
}

void UDebugUserWidget::DeclineTrade()
{
    UE_LOG(LogTemp, Log, TEXT("DeclineTrade: Declining trade offer"));
    
    TradeOfferModalUI->SetVisibility(ESlateVisibility::Hidden);
    
    // Reset resources properly using direct assignment
    ResetTradeResources();
    
    offerGetSTG.Empty();
    offerGiveSTG.Empty();
}

void UDebugUserWidget::ResetTradeResources()
{
    // Reset YouGiveResources to 0
    YouGiveResources.FindOrAdd(EResourceType::Wood) = 0;
    YouGiveResources.FindOrAdd(EResourceType::Brick) = 0;
    YouGiveResources.FindOrAdd(EResourceType::Sheep) = 0;
    YouGiveResources.FindOrAdd(EResourceType::Wheat) = 0;
    YouGiveResources.FindOrAdd(EResourceType::Ore) = 0;

    // Reset YouGetResources to 0
    YouGetResources.FindOrAdd(EResourceType::Wood) = 0;
    YouGetResources.FindOrAdd(EResourceType::Brick) = 0;
    YouGetResources.FindOrAdd(EResourceType::Sheep) = 0;
    YouGetResources.FindOrAdd(EResourceType::Wheat) = 0;
    YouGetResources.FindOrAdd(EResourceType::Ore) = 0;
    
    // Reset all comboboxes to 0
    if (WoodSelection) WoodSelection->SetSelectedOption(TEXT("0"));
    if (BrickSelection) BrickSelection->SetSelectedOption(TEXT("0"));
    if (SheepSelection) SheepSelection->SetSelectedOption(TEXT("0"));
    if (WheatSelection) WheatSelection->SetSelectedOption(TEXT("0"));
    if (OreSelection) OreSelection->SetSelectedOption(TEXT("0"));
    
    if (WoodSelection_1) WoodSelection_1->SetSelectedOption(TEXT("0"));
    if (BrickSelection_1) BrickSelection_1->SetSelectedOption(TEXT("0"));
    if (SheepSelection_1) SheepSelection_1->SetSelectedOption(TEXT("0"));
    if (WheatSelection_1) WheatSelection_1->SetSelectedOption(TEXT("0"));
    if (OreSelection_1) OreSelection_1->SetSelectedOption(TEXT("0"));
    
    UE_LOG(LogTemp, Log, TEXT("ResetTradeResources: All trade resources reset to 0"));
}

void UDebugUserWidget::OnComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType, EResourceType ResourceType, bool bIsYouGet)
{
    // Only process mouse clicks and key presses
    if (SelectionType != ESelectInfo::Type::OnMouseClick &&
        SelectionType != ESelectInfo::Type::OnKeyPress &&
        SelectionType != ESelectInfo::Type::OnNavigation)
        return;
        
    int32 Amount = FCString::Atoi(*SelectedItem);
    
    if (bIsYouGet)
    {
        YouGetResources.FindOrAdd(ResourceType) = Amount;
        UE_LOG(LogTemp, Log, TEXT("OnComboBoxSelectionChanged: YouGet %d = %d"), (int32)ResourceType, Amount);
    }
    else
    {
        YouGiveResources.FindOrAdd(ResourceType) = Amount;
        UE_LOG(LogTemp, Log, TEXT("OnComboBoxSelectionChanged: YouGive %d = %d"), (int32)ResourceType, Amount);
    }
}

void UDebugUserWidget::SetCurrentTurnStepUI(ETurnStep TurnStep, EPlacementNode SetupStep, EGamePhase GamePhase)
{
    if (CurrentTurnStepUI)
    {
        FString TurnStepString;

        if (GamePhase == EGamePhase::Setup)
        {
            FString SetupString = UEnum::GetValueAsString(SetupStep);
            SetupString = SetupString.Replace(TEXT("EPlacementNode::"), TEXT(""));
            TurnStepString = FString::Printf(TEXT("Setup: %s"), *SetupString);
        }
        else
        {
            FString TurnString = UEnum::GetValueAsString(TurnStep);
            TurnString = TurnString.Replace(TEXT("ETurnStep::"), TEXT(""));
            TurnStepString = TurnString;
        }

        CurrentTurnStepUI->SetText(FText::FromString(TurnStepString));
    }
}

void UDebugUserWidget::SetCurrentPhaseUI(EGamePhase GamePhase)
{
    if (CurrentPhaseUI)
    {
        FString GamePhaseString = UEnum::GetValueAsString(GamePhase);
        GamePhaseString = GamePhaseString.Replace(TEXT("EGamePhase::"), TEXT(""));
        CurrentPhaseUI->SetText(FText::FromString(GamePhaseString));
    }
}

void UDebugUserWidget::SetResourceUI(TMap<EResourceType, int32> Resources)
{
    // Update each resource independently — no else if!
    if (WoodUI && Resources.Contains(EResourceType::Wood))
    {
        WoodUI->SetText(FText::FromString(FString::Printf(TEXT("Wood: %d"), Resources[EResourceType::Wood])));
    }

    if (BrickUI && Resources.Contains(EResourceType::Brick))
    {
        BrickUI->SetText(FText::FromString(FString::Printf(TEXT("Brick: %d"), Resources[EResourceType::Brick])));
    }

    if (SheepUI && Resources.Contains(EResourceType::Sheep))
    {
        SheepUI->SetText(FText::FromString(FString::Printf(TEXT("Sheep: %d"), Resources[EResourceType::Sheep])));
    }

    if (WheatUI && Resources.Contains(EResourceType::Wheat))
    {
        WheatUI->SetText(FText::FromString(FString::Printf(TEXT("Wheat: %d"), Resources[EResourceType::Wheat])));
    }

    if (OreUI && Resources.Contains(EResourceType::Ore))
    {
        OreUI->SetText(FText::FromString(FString::Printf(TEXT("Ore: %d"), Resources[EResourceType::Ore])));
    }
}