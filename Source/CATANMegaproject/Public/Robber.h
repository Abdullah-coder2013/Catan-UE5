// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Robber.generated.h"

UCLASS()
class CATANMEGAPROJECT_API ARobber : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robber Properties")
	UStaticMeshComponent* StaticMesh;
	
	ARobber();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
