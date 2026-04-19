// Fill out your copyright notice in the Description page of Project Settings.

#include "CatanPCGSettings.h"
#include "PCGContext.h"
#include "Data/PCGPointData.h"

#define LOCTEXT_NAMESPACE "CatanPCGSettingsElement"
TArray<FPCGPinProperties> UCatanPCGSettingsSettings::InputPinProperties() const
{
	TArray<FPCGPinProperties> Props;
	return Props;
}

TArray<FPCGPinProperties> UCatanPCGSettingsSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> Props;
	Props.Emplace(PCGPinConstants::DefaultOutputLabel,
				  EPCGDataType::Point);
	return Props;
}

FPCGElementPtr UCatanPCGSettingsSettings::CreateElement() const
{
	return MakeShared<FCatanPCGSettingsElement>();
}

bool FCatanPCGSettingsElement::ExecuteInternal(FPCGContext* Context) const
{
	const UCatanPCGSettingsSettings* Settings =
		Context->GetInputSettings<UCatanPCGSettingsSettings>();
	if (!Settings) return true;

	// Build one PCGPoint per hex tile
	UPCGPointData* PointData = NewObject<UPCGPointData>();
	TArray<FPCGPoint>& Points = PointData->GetMutablePoints();

	for (AHexTile* Tile : Settings->HexTiles)
	{
		if (!Tile) continue;

		FPCGPoint Point;
		Point.Transform = Tile->GetActorTransform();

		// Encode biome type into Density so PCG graph can filter on it:
		// Desert=0.0, Fields=0.2, Pasture=0.4, Forest=0.6, Hill=0.8, Mountain=1.0
		Point.Density   = (float)Tile->HexType / 5.f;

		// Elevation in Steepness for slope-aware prop spawning
		Point.Steepness = Tile->SmoothedElevation;

		Points.Add(Point);
	}

	// Output the point cloud on the default output pin
	FPCGTaggedData& Output = Context->OutputData.TaggedData.Emplace_GetRef();
	Output.Data  = PointData;
	Output.Pin   = PCGPinConstants::DefaultOutputLabel;

	return true;
}

#undef LOCTEXT_NAMESPACE