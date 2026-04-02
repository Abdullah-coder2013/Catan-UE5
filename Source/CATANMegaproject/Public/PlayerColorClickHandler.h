#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CatanSharedValues.h"
#include "PlayerColorClickHandler.generated.h"

/**
 * Helper class to handle button clicks with player color context
 */
UCLASS()
class CATANMEGAPROJECT_API UPlayerColorClickHandler : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EPlayerColor PlayerColor;

	UPROPERTY()
	class UDebugUserWidget* ParentWidget;

	UFUNCTION()
	void HandleClick();
};
