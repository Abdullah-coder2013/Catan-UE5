// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CatanSharedValues.h"
#include "HexVertex.generated.h"

class AHexTile;

UCLASS()
class CATANMEGAPROJECT_API AHexVertex : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertex Properties")
	EPlayerColor occupiedBy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertex Properties")
	ESettlementType settlementType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertex Properties")
	bool isOccupied;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertex Relations")
	TArray<AHexTile*> AdjacentHexes; // Store indices of adjacent hexes for easy access
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertex Relations")
	TArray<AHexVertex*> AdjacentVertices; // Store pointers to adjacent vertices for easy access

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vertex Components")
	UStaticMeshComponent* VertexMesh;

	void InitializeVertex(TArray<AHexTile*> AdjacentHexes);

	bool TryPlaceSettlement(EPlayerColor PlayerColor, ESettlementType Type);

	AHexVertex();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
