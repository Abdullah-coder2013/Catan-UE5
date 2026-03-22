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
    Settlement,
    Road
};

UENUM(BlueprintType)
enum class EResourceType : uint8
{
    None,
    Wood,
    Brick,
    Sheep,
    Wheat,
    Ore
};

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    Setup,
    MainGame,
    GameOver
};
UENUM(BlueprintType)
enum class ETurnStep : uint8
{
    None,
    RollDice,
    Trade,
    Build,
    EndTurn
};

USTRUCT(BlueprintType)
struct FPlayerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Player")
    EPlayerColor PlayerColor = EPlayerColor::None;

    UPROPERTY(BlueprintReadWrite, Category = "Player")
    TMap<EResourceType, int32> Resources;

    FPlayerData() {}

    FPlayerData(EPlayerColor InColor)
        : PlayerColor(InColor)
    {
        // Initialize all resource types to 0
        Resources.Add(EResourceType::Wood, 0);
        Resources.Add(EResourceType::Brick, 0);
        Resources.Add(EResourceType::Sheep, 0);
        Resources.Add(EResourceType::Wheat, 0);
        Resources.Add(EResourceType::Ore, 0);
    }

    void AddResource(EResourceType ResourceType, int32 Amount)
    {
        if (Resources.Contains(ResourceType))
        {
            Resources[ResourceType] += Amount;
        }
    }

    void SpendResource(EResourceType ResourceType, int32 Amount)
    {
        if (Resources.Contains(ResourceType) && Resources[ResourceType] >= Amount)
        {
            Resources[ResourceType] -= Amount;
        }
    }

    bool CanAfford(TMap<EResourceType, int32> Cost) const
    {
        for (auto& ResourcePair : Cost)
        {
            if (!Resources.Contains(ResourcePair.Key) || Resources[ResourcePair.Key] < ResourcePair.Value)
            {
                return false;
            }
        }
        return true;
    }

    int32 GetResourceAmount(EResourceType ResourceType) const
    {
        if (Resources.Contains(ResourceType))
        {
            return Resources[ResourceType];
        }
        return 0;
    }
};