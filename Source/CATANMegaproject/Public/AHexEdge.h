// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AHexVertex;
class AHexTile;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CatanSharedValues.h"
#include "AHexEdge.generated.h"

UCLASS()
class CATANMEGAPROJECT_API AAHexEdge : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge Properties")
	EPlayerColor occupiedBy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge Properties")
	bool isOccupied;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge Relations")
	TArray<AHexVertex*> AdjacentVertices; // Store pointers to adjacent vertices for easy access
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge Relations")
	TArray<AHexTile*> AdjacentHexes; // Store pointers to adjacent hexes for easy access
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge Relations")
	TArray<AAHexEdge*> AdjacentEdges; // Store pointers to adjacent edges for easy access
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edge Components")
	UStaticMeshComponent* EdgeMesh;

	void InitializeEdge(TArray<AHexVertex*> InAdjacentVertices);

	bool TryPlaceRoad(EPlayerColor PlayerColor);

	AAHexEdge();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
