// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CatanSharedValues.h"
#include "HexTile.generated.h"

class AHexVertex;

UENUM(BlueprintType)
enum class EHexType : uint8
{
	Desert,
	Forest,
	Mountain,
	Pasture,
	Fields,
	Hill,
	Water
};

UCLASS()
class CATANMEGAPROJECT_API AHexTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexTile();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Properties")
	EHexType HexType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Properties")
	int32 NumberToken;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Properties")
	int32 Q; // Axial coordinate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Properties")
	int32 R; // Axial coordinate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Properties")
	bool bHasRobber;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Properties")
	float BaseElevation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex Properties")
	float SmoothedElevation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex Components")
	UStaticMeshComponent* HexMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Properties")
	bool isInitialized;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Properties")
	TArray<AHexVertex*> Vertices;

	void InitializeHex(EHexType Type, int32 Token, int32 InQ, int32 InR, bool bInitialized);
	
	void PlaceRobber(AActor* RobberActor);

	int32 GetNumberToken() const { return NumberToken; }
	EResourceType HexTypeToResource() const;
	static float GetBaseElevationForType(EHexType Type)
	{
		switch (Type)
		{
		case EHexType::Water:    return 0.0f;
		case EHexType::Desert:   return 0.1f;
		case EHexType::Fields:   return 0.2f;
		case EHexType::Pasture:  return 0.3f;
		case EHexType::Hill:     return 0.6f;
		case EHexType::Forest:   return 0.4f;
		case EHexType::Mountain: return 4.0f;
		default:                 return 0.2f;
		}
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
