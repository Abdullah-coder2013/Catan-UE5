#pragma once

#include "CoreMinimal.h"
#include "HexTile.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "BoardTerrain.generated.h"

class UPCGComponent;

UCLASS()
class CATANMEGAPROJECT_API ABoardTerrain : public AActor
{
	GENERATED_BODY()

public:
	ABoardTerrain();

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* TerrainMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	UMaterialInterface* TerrainMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain|Texture")
	UTexture2D* BiomeTexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain|Texture")
	UTexture2D* BiomeTexture2;

	UPROPERTY(EditAnywhere, Category = "Terrain|Texture", meta = (ClampMin = "256", ClampMax = "8192"))
	int32 BiomeTextureResolution = 4096;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG")
	UPCGComponent* PCGComponent;

	UPROPERTY(EditAnywhere, Category = "Terrain")
	float Resolution = 50.f;

	UPROPERTY(EditAnywhere, Category = "Terrain")
	float ElevationScale = 80.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Material")
	float RoadWidthGlobal = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Detail")
	float DetailAmplitudeMultiplier = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Detail")
	float MountainDetailAmplitude = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Detail")
	float HillDetailAmplitude = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Detail")
	float ForestDetailAmplitude = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Detail")
	float PastureDetailAmplitude = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Detail")
	float FieldsDetailAmplitude = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Detail")
	float DesertDetailAmplitude = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Terrain")
	float BorderPadding = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Terrain|Biome")
	float MountainHeight = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Terrain|Biome")
	float FieldHeight = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Terrain|Biome")
	float ForestHeight = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Terrain|Biome")
	float PastureHeight = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Terrain|Biome")
	float HillHeight = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Terrain|Biome")
	float DesertHeight = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Noise")
	float ContinentalWeight = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Noise")
	float TerrainShapeWeight = 0.1f;

	void GenerateTerrain(const TArray<AHexTile*>& HexTiles, float HexSize);
	
	void TriggerPCG(const TArray<AHexTile*>& HexTiles);

	struct FHexData
	{
		FVector2D Pos;
		float Height;
		float Amp;
		EHexType Type;
		float BiomeFloat;
	};
};