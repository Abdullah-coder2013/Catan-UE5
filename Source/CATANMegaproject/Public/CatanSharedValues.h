#pragma once
#include "CoreMinimal.h"
#include "CatanSharedValues.generated.h"

UENUM(BlueprintType)
enum class EPlayerColor : uint8
{
    None,
    Red,
    Blue,
    White,
    Orange
};

UENUM(BlueprintType)
enum class ESettlementType : uint8
{
    Empty,
    Settlement,
    City
};

UENUM(BlueprintType)
enum class EPlacementNode : uint8
{
    None,
    Road,
    Settlement
};