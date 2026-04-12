#pragma once

#include "CoreMinimal.h"
#include "HexTile.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "BoardTerrain.generated.h"

UCLASS()
class CATANMEGAPROJECT_API ABoardTerrain : public AActor
{
	GENERATED_BODY()

public:
	ABoardTerrain();

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* TerrainMesh;

	UPROPERTY(EditAnywhere, Category = "Terrain")
	float Resolution = 50.f; // vertex spacing in cm

	UPROPERTY(EditAnywhere, Category = "Terrain")
	float ElevationScale = 80.f;
	
	UPROPERTY(EditAnywhere, Category = "Terrain")
	float BorderPadding = 200.f; // extra cm beyond the board edge

	void GenerateTerrain(const TArray<AHexTile*>& HexTiles, float HexSize);
};