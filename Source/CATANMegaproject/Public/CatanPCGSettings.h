// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HexTile.h"
#include "PCGSettings.h"

#include "CatanPCGSettings.generated.h"

/**
* Add your tooltip here
*/
UCLASS(MinimalAPI, BlueprintType)
class UCatanPCGSettingsSettings : public UPCGSettings
{
	GENERATED_BODY()

public:
	//~Begin UPCGSettings interface
#if WITH_EDITOR
	virtual FName GetDefaultNodeName() const override
	{ return TEXT("HexTileInput"); }
	virtual FText GetDefaultNodeTitle() const override
	{ return FText::FromString("Hex Tile Input"); }
	virtual EPCGSettingsType GetType() const override
	{ return EPCGSettingsType::Spatial; }
#endif //WITH_EDITOR

protected:
	virtual TArray<FPCGPinProperties> InputPinProperties() const override;
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;
	virtual FPCGElementPtr CreateElement() const override;
	//~End UPCGSettings interface
public:
	// Populated by BoardTerrain before PCG generates
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<AHexTile*> HexTiles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable, ClampMin = "0.0001"))
	float PointsPerSquareMeter = 0.01f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable, ClampMin = "1.0"))
	float HexOuterRadius = 100000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable, ClampMin = "0.0"))
	float SnapGridSizeY = 0.f;
	UPROPERTY(EditAnywhere, Category="PCG")
	int32 MaxTreesPerHex = 150;

	UPROPERTY(EditAnywhere, Category="PCG")
	float MinScale = 0.8f;

	UPROPERTY(EditAnywhere, Category="PCG")
	float MaxScale = 1.2f;
};
/**
* Define various user facing properties here. 
* Tooltip can be added above UPROPERTY line.
* PCG_Overridable meta tag allows the properties to be overriden with other attributes in the graph editor.
*
* public:
*	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta = (PCG_Overridable))
*	bool CheckBox = true;
*/


class FCatanPCGSettingsElement : public IPCGElement
{
protected:
	virtual bool ExecuteInternal(FPCGContext* Context) const override;
};
