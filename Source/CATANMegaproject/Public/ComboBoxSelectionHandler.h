#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CatanSharedValues.h"
#include "ComboBoxSelectionHandler.generated.h"

/**
 * Helper class to handle combobox selection changes with resource type context
 */
UCLASS()
class CATANMEGAPROJECT_API UComboBoxSelectionHandler : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EResourceType ResourceType;

	UPROPERTY()
	bool bIsYouGet; // true for "You Get", false for "You Give"

	UPROPERTY()
	bool bIsBankTrade = false;

	UPROPERTY()
	bool bIsDiscard = false;
	
	UPROPERTY()
	bool bIsYOP = false;

	UPROPERTY()
	class UDebugUserWidget* ParentWidget;

	UFUNCTION()
	void HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
};
