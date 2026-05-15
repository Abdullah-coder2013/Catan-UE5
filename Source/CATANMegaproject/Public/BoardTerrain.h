#pragma once

#include "CoreMinimal.h"
#include "HexTile.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PCGComponent.h"
#include "BoardTerrain.generated.h"

UENUM()
enum class EFoliageType : uint8
{
	Small,
	Medium,
	Large
};

USTRUCT(BlueprintType)
struct FBiomeFoliageSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DensityBig = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DensityMedium = 0.03f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DensitySmall = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinScaleBig = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxScaleBig = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinScaleMedium = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxScaleMedium = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinScaleSmall = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxScaleSmall = 3.2f;
};

USTRUCT()
struct FCachedHexData
{
	GENERATED_BODY()
	
	FVector2D Pos;
	float Height = 0.f;
	float Amp = 0.f;
	EHexType Type = EHexType::Water;
	float BiomeFloat = 0.f;
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
	float Seed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Forest")
	FBiomeFoliageSettings ForestFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Desert")
	FBiomeFoliageSettings DesertFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Pasture")
	FBiomeFoliageSettings PastureFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Hill")
	FBiomeFoliageSettings HillFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Fields")
	FBiomeFoliageSettings FieldsFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Generation|Mountain")
	FBiomeFoliageSettings MountainFoliage;
	
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
	
	UPROPERTY()
	TArray<AHexTile*> CachedHexTiles;

	UPROPERTY(EditAnywhere, Category="Streaming")
	float FullDetailRadius = 150000.f;

	UPROPERTY(EditAnywhere, Category="Streaming")
	float MediumDetailRadius = 300000.f;

	UPROPERTY(EditAnywhere, Category="Streaming")
	float ImpostorRadius = 500000.f;

	UPROPERTY(EditAnywhere, Category="Streaming")
	float StreamingUpdateInterval = 0.5f;

	FTimerHandle StreamingTimer;

	void UpdateHexStreaming(FVector CameraLocation);
	void SetHexDetailLevel(AHexTile* Tile, EHexDetailLevel Level);
	AHexTile* GetFocusedHex(FVector CameraLocation);

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
	
	void GenerateFoliage(AHexTile* HexTile, float DensityScaleBig = 1.f, float DensityScaleMedium = 1.f, float DensityScaleSmall = 1.f);
	
	void SpawnMeshesForHexTile(AHexTile* HexTile, TArray<FVector> PointPositions, float Density, float Octaves, EFoliageType FoliageType, const FBiomeFoliageSettings& Settings);
	
	UStaticMesh* GetStaticMeshForBiome(EHexType HexType, EFoliageType FoliageType = EFoliageType::Medium);

	const FBiomeFoliageSettings& GetBiomeFoliageSettings(EHexType HexType) const;
	
	void TriggerPCG(const TArray<AHexTile*>& HexTiles);
	
	float ComputeTerrainHeightAt(float WorldX, float WorldY) const;
	FVector ComputeTerrainNormalAt(float WorldX, float WorldY) const;
	
	UPROPERTY()
	TArray<FCachedHexData> CachedHexData;
	
	UPROPERTY()
	float CachedHexSize = 0.f;
	
	TArray<float> CachedHeightGrid;
	FVector2D CachedGridWorldMin = FVector2D::ZeroVector;
	int32 CachedGridNumCols = 0;
	int32 CachedGridNumRows = 0;
	float CachedGridCellSize = 0.f;
	
protected:
	virtual void BeginPlay() override;
};