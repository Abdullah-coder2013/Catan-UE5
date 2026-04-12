#include "ComboBoxSelectionHandler.h"
#include "DebugUserWidget.h"

void UComboBoxSelectionHandler::HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (ParentWidget)
    {
        if (bIsBankTrade)
        {
            ParentWidget->OnAmountSelectedBankTrade(SelectedItem, SelectionType, ResourceType, bIsYouGet);
        }
        else if (bIsDiscard)
        {
            ParentWidget->OnDiscardSelectionChanged(SelectedItem, SelectionType, ResourceType);
        }
        else if (bIsYOP)
        {
            ParentWidget->OnYOPComboboxChanged(SelectedItem, SelectionType, ResourceType);
        }
        else
        {
            ParentWidget->OnComboBoxSelectionChanged(SelectedItem, SelectionType, ResourceType, bIsYouGet);
        }
    }
}
