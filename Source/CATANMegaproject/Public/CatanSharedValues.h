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
enum class EDockType : uint8
{
    Generic,
    Wood,
    Brick,
    Sheep,
    Wheat,
    Ore
};

UENUM(BlueprintType)
enum class EBankTradeMethods : uint8
{
    None,
    Standard, // 4:1
    Dock, // 3:1
    Wood, // 2:1
    Brick, // 2:1
    Sheep, // 2:1
    Wheat, // 2:1
    Ore // 2:1
    
};

UENUM(BlueprintType)
enum class ESettlementType : uint8
{
    Empty,
    Settlement,
    City
};

UENUM(BlueprintType)
enum class EBuildable : uint8
{
    None,
    Settlement,
    City,
    Road,
    Development
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

USTRUCT(BlueprintType)
struct FResourceCosts
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<EResourceType, int32> Costs;
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
    DynamicEnvironment,
    EndTurn
};

USTRUCT(BlueprintType)
struct FPlayerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Player")
    EPlayerColor PlayerColor = EPlayerColor::None;
    
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    int32 VictoryPoints = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Player")
    TMap<EResourceType, int32> Resources;

    UPROPERTY(BlueprintReadWrite, Category = "General")
    TMap<EBuildable, FResourceCosts> ResourceCosts;

    FPlayerData()
    {
        // Initialize all resource types to 0
        Resources.Add(EResourceType::Wood, 0);
        Resources.Add(EResourceType::Brick, 0);
        Resources.Add(EResourceType::Sheep, 0);
        Resources.Add(EResourceType::Wheat, 0);
        Resources.Add(EResourceType::Ore, 0);

        // Initialize build costs
        FResourceCosts SettlementCost;
        SettlementCost.Costs.Add(EResourceType::Wood, 1);
        SettlementCost.Costs.Add(EResourceType::Brick, 1);
        SettlementCost.Costs.Add(EResourceType::Sheep, 1);
        SettlementCost.Costs.Add(EResourceType::Wheat, 1);
        ResourceCosts.Add(EBuildable::Settlement, SettlementCost);

        FResourceCosts CityCost;
        CityCost.Costs.Add(EResourceType::Wheat, 2);
        CityCost.Costs.Add(EResourceType::Ore, 3);
        ResourceCosts.Add(EBuildable::City, CityCost);

        FResourceCosts RoadCost;
        RoadCost.Costs.Add(EResourceType::Wood, 1);
        RoadCost.Costs.Add(EResourceType::Brick, 1);
        ResourceCosts.Add(EBuildable::Road, RoadCost);

        FResourceCosts DevelopmentCost;
        DevelopmentCost.Costs.Add(EResourceType::Sheep, 1);
        DevelopmentCost.Costs.Add(EResourceType::Wheat, 1);
        DevelopmentCost.Costs.Add(EResourceType::Ore, 1);
        ResourceCosts.Add(EBuildable::Development, DevelopmentCost);
    }

    FPlayerData(EPlayerColor InColor)
        : PlayerColor(InColor)
    {
        // Initialize all resource types to 0
        Resources.Add(EResourceType::Wood, 0);
        Resources.Add(EResourceType::Brick, 0);
        Resources.Add(EResourceType::Sheep, 0);
        Resources.Add(EResourceType::Wheat, 0);
        Resources.Add(EResourceType::Ore, 0);

        // Initialize build costs
        FResourceCosts SettlementCost;
        SettlementCost.Costs.Add(EResourceType::Wood, 1);
        SettlementCost.Costs.Add(EResourceType::Brick, 1);
        SettlementCost.Costs.Add(EResourceType::Sheep, 1);
        SettlementCost.Costs.Add(EResourceType::Wheat, 1);
        ResourceCosts.Add(EBuildable::Settlement, SettlementCost);

        FResourceCosts CityCost;
        CityCost.Costs.Add(EResourceType::Wheat, 2);
        CityCost.Costs.Add(EResourceType::Ore, 3);
        ResourceCosts.Add(EBuildable::City, CityCost);

        FResourceCosts RoadCost;
        RoadCost.Costs.Add(EResourceType::Wood, 1);
        RoadCost.Costs.Add(EResourceType::Brick, 1);
        ResourceCosts.Add(EBuildable::Road, RoadCost);

        FResourceCosts DevelopmentCost;
        DevelopmentCost.Costs.Add(EResourceType::Sheep, 1);
        DevelopmentCost.Costs.Add(EResourceType::Wheat, 1);
        DevelopmentCost.Costs.Add(EResourceType::Ore, 1);
        ResourceCosts.Add(EBuildable::Development, DevelopmentCost);
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
    
    bool CanBuild(EBuildable BuildableType) const
    {
        if (ResourceCosts.Contains(BuildableType))
        {
            return CanAfford(ResourceCosts[BuildableType].Costs);
        }
        return false;
    }
    
    void SpendResourceFor(EBuildable forBuild)
    {
        if (ResourceCosts.Contains(forBuild))
        {
            for (auto& ResourcePair : ResourceCosts[forBuild].Costs)
            {
                SpendResource(ResourcePair.Key, ResourcePair.Value);
            }
        }
    }
    
    void AddVictoryPoint(int32 Amount)
    {
        VictoryPoints += Amount;
    }
    void DeduceVictoryPoint(int32 Amount)
    {
        VictoryPoints -= Amount;
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