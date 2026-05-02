#pragma once

#include "CoreMinimal.h"
#include "HexTile.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PCGComponent.h"
#include "BoardTerrain.generated.h"

UENUM()
enum EFoliageType
{
	Small,
	Medium,
	Large
};

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation")
	float FoliageDensityBig = 0.01f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation")
	float FoliageDensityMedium = 0.03f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation")
	float FoliageDensitySmall = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation")
	float Seed;
	
	// Forest
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> BigForestMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> MediumForestMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> SmallForestMeshes;
	
	// Desert
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> BigDesertMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> MediumDesertMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> SmallDesertMeshes;
	
	// Pastures
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> BigPasturesMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> MediumPasturesMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> SmallPasturesMeshes;
	
	// Hill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> BigHillMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> MediumHillMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> SmallHillMeshes;
	
	// Fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> BigFieldsMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> MediumFieldsMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> SmallFieldsMeshes;
	
	// Mountain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> BigMountainMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> MediumMountainMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Biome Meshes")
	TArray<UStaticMesh*> SmallMountainMeshes;
	
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
	
	TArray<FVector> GenerateProceduralFoliageForBiomeHex(AHexTile* HexTile, float PointsPerMeter);
	
	void GenerateFoliage(AHexTile* HexTile, float PointsPerMeterBIG, float PointsPerMeterMEDIUM, float PointsPerMeterSMALL);
	
	void SpawnMeshesForHexTile(AHexTile* HexTile, TArray<FVector> PointPositions, float Density = 0.3f, float Octaves = 3, EFoliageType FoliageType = EFoliageType::Medium);
	
	UStaticMesh* GetStaticMeshForBiome(EHexType HexType, EFoliageType FoliageType = EFoliageType::Medium);
	
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