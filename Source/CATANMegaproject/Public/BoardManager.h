// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTile.h"
#include "HexVertex.h"
#include "AHexEdge.h"
#include "Dock.h"
#include "BoardManager.generated.h"


UCLASS()
class CATANMEGAPROJECT_API ABoardManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoardManager();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Settings")
    float HexSize = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Settings")
    TSubclassOf<AHexTile> HexTileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Settings")
	TSubclassOf<AHexVertex> HexVertexClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Settings")
	TSubclassOf<AAHexEdge> HexEdgeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dock Settings")
	TSubclassOf<ADock> DockClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY()
	TArray<AHexTile*> HexTiles; // Store pointers to hex tiles for easy access
	UPROPERTY()
	TMap<FString, AHexVertex*> SpawnedVertices;
	UPROPERTY()
	TMap<FString, AAHexEdge*> SpawnedEdges;
	UPROPERTY()
	TArray<AAHexEdge*> SpawnedEdgesArray; // Store pointers to edges in an array for easy access
	UPROPERTY()
	TArray<ADock*> Docks;

public:	
	// Called every frame
	void GenerateBoard();
	AHexTile* GetHexTile(EHexType HexType);
	AHexTile* GetRobberTile();
	FVector AxialToWorld(int32 Q, int32 R) const;
	TArray<AHexTile*> GetHexTiles() const { return HexTiles; }
	TArray<ADock*> GetDocks() const { return Docks; }
	int32 GetLongestRoadLengthForPlayer(EPlayerColor PlayerColor) const;
	int32 DFS(AAHexEdge* CurrentEdge, AHexVertex* FromVertex, 
						  TSet<AAHexEdge*>& VisitedEdges, EPlayerColor PlayerColor) const;

};
