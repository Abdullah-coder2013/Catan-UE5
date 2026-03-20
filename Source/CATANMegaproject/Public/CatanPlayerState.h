// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CatanSharedValues.h"
#include "CatanPlayerState.generated.h"

UCLASS()
class CATANMEGAPROJECT_API ACatanPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Details")
	EPlayerColor PlayerColor;
	UPROPERTY()
	TMap<EResourceType, int> Resources;

	void AddResource(EResourceType ResourceType, int Amount);
	void SpendResource(EResourceType ResourceType, int Amount);
	bool CanAfford(TMap<EResourceType, int> Cost) const;
	int32 GetResourceAmount(EResourceType ResourceType) const;
	ACatanPlayerState();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
