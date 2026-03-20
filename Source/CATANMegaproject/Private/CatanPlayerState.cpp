// Fill out your copyright notice in the Description page of Project Settings.


#include "CatanPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "CatanSharedValues.h"

// Sets default values
ACatanPlayerState::ACatanPlayerState()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Resources = {
		{EResourceType::Wood, 0},
		{EResourceType::Brick, 0},
		{EResourceType::Sheep, 0},
		{EResourceType::Wheat, 0},
		{EResourceType::Ore, 0}
	};
}

void ACatanPlayerState::AddResource(EResourceType ResourceType, int Amount)
{
	Resources.FindOrAdd(ResourceType) += Amount;
}

bool ACatanPlayerState::CanAfford(TMap<EResourceType, int> Cost) const
{
	for (const auto& Pair : Cost)
	{
		if (GetResourceAmount(Pair.Key) < Pair.Value)
		{
			return false;
		}
	}
	return true;
}

void ACatanPlayerState::SpendResource(EResourceType ResourceType, int Amount)
{
	if (Resources.Contains(ResourceType))
	{
		Resources[ResourceType] -= Amount;
	}
}

int32 ACatanPlayerState::GetResourceAmount(EResourceType ResourceType) const
{
	const int* Amount = Resources.Find(ResourceType);
	return Amount ? *Amount : 0;
}

// Called when the game starts or when spawned
void ACatanPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACatanPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

