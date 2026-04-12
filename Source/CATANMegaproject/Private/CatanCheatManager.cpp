// Fill out your copyright notice in the Description page of Project Settings.


#include "CatanCheatManager.h"
#include "CatanSharedValues.h"
#include "GameModeCPP.h"


void UCatanCheatManager::GiveResources(int32 Wood, int32 Brick, int32 Ore, int32 Wheat, int32 Sheep, EPlayerColor TargetPlayer)
{
	FPlayerData* TargetPlayerData = nullptr;
	for (FPlayerData& Player : GetWorld()->GetAuthGameMode<AGameModeCPP>()->Players)
	{		if (Player.PlayerColor == TargetPlayer)
		{			TargetPlayerData = &Player;
			break;
		}
	}

	TargetPlayerData->AddResource(EResourceType::Wood, Wood);
	TargetPlayerData->AddResource(EResourceType::Brick, Brick);
	TargetPlayerData->AddResource(EResourceType::Ore, Ore);
	TargetPlayerData->AddResource(EResourceType::Wheat, Wheat);
	TargetPlayerData->AddResource(EResourceType::Sheep, Sheep);

	UE_LOG(LogTemp, Warning, TEXT("Cheat: Resources added!"));
}

void UCatanCheatManager::GiveAllResources(int32 Amount, EPlayerColor TargetPlayer)
{
	GiveResources(Amount, Amount, Amount, Amount, Amount, TargetPlayer);
}

void UCatanCheatManager::GiveDevelopmentCard(FString CardType, EPlayerColor TargetPlayer)
{
	FPlayerData* TargetPlayerData = nullptr;
	for (FPlayerData& Player : GetWorld()->GetAuthGameMode<AGameModeCPP>()->Players)
	{		if (Player.PlayerColor == TargetPlayer)
	{			TargetPlayerData = &Player;
		break;
	}
	}
	if (CardType == "Knight")
		TargetPlayerData->DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::Knight, false, TargetPlayer));
	else if (CardType == "VictoryPoint")
		TargetPlayerData->DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::VictoryPoint, false, TargetPlayer));
	else if (CardType == "RoadBuilding")
		TargetPlayerData->DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::RoadBuilding, false, TargetPlayer));
	else if (CardType == "YOP")
		TargetPlayerData->DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::YearsOfPlenty, false, TargetPlayer));
	else if (CardType == "Monopoly")
		TargetPlayerData->DevelopmentCards.Add(FDevelopmentCard(EDevelopmentCardType::Monopoly, false, TargetPlayer));
	else
		UE_LOG(LogTemp, Warning, TEXT("Unknown card type: %s"), *CardType);
}

void UCatanCheatManager::GiveAllDevCards(int32 Amount, EPlayerColor TargetPlayer)
{
	for (int i = 0; i < Amount; ++i)
	{
		GiveDevelopmentCard("Knight", TargetPlayer);
		GiveDevelopmentCard("VictoryPoint", TargetPlayer);
		GiveDevelopmentCard("RoadBuilding", TargetPlayer);
		GiveDevelopmentCard("YOP", TargetPlayer);
		GiveDevelopmentCard("Monopoly", TargetPlayer);
	}
}

void UCatanCheatManager::GiveVictoryPoints(int32 Amount, EPlayerColor TargetPlayer)
{
	FPlayerData* TargetPlayerData = nullptr;
	for (FPlayerData& Player : GetWorld()->GetAuthGameMode<AGameModeCPP>()->Players)
	{		if (Player.PlayerColor == TargetPlayer)
	{			TargetPlayerData = &Player;
		break;
	}
	}
	TargetPlayerData->AddVictoryPoint(Amount);
}
