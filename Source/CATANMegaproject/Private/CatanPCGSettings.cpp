// Fill out your copyright notice in the Description page of Project Settings.

#include "CatanPCGSettings.h"

#include "PCGComponent.h"
#include "PCGContext.h"
#include "Data/PCGPointData.h"
#include "Math/RandomStream.h"

#define LOCTEXT_NAMESPACE "CatanPCGSettingsElement"

TArray<FPCGPinProperties> UCatanPCGSettingsSettings::InputPinProperties() const
{
	TArray<FPCGPinProperties> Props;
	return Props;
}

TArray<FPCGPinProperties> UCatanPCGSettingsSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> Props;
	Props.Emplace(PCGPinConstants::DefaultOutputLabel, EPCGDataType::Point);
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

    UWorld* World = nullptr;
    {
        PRAGMA_DISABLE_DEPRECATION_WARNINGS
        if (UPCGComponent* Comp = Context->SourceComponent.Get())
        {
            World = Comp->GetWorld();
        }
        PRAGMA_ENABLE_DEPRECATION_WARNINGS
    }
    if (!World)
    {
        for (AHexTile* Tile : Settings->HexTiles)
        {
            if (Tile)
            {
                World = Tile->GetWorld();
                break;
            }
        }
    }

    // ---- biome color helper ----
    auto HexTypeToColor = [](EHexType Type) -> FLinearColor
    {
        switch (Type)
        {
        case EHexType::Desert:   return FLinearColor(1.f, 0.9f, 0.5f);
        case EHexType::Forest:   return FLinearColor(0.1f, 0.5f, 0.1f);
        case EHexType::Mountain: return FLinearColor(0.5f, 0.5f, 0.5f);
        case EHexType::Pasture:  return FLinearColor(0.4f, 0.8f, 0.3f);
        case EHexType::Fields:   return FLinearColor(0.9f, 0.8f, 0.2f);
        case EHexType::Hill:     return FLinearColor(0.6f, 0.4f, 0.2f);
        case EHexType::Water:    return FLinearColor(0.2f, 0.4f, 0.8f);
        default:                 return FLinearColor::White;
        }
    };

    const float R           = Settings->HexOuterRadius;
    const float HexAreaCm2  = (3.f * FMath::Sqrt(3.f) / 2.f) * R * R;
    const float PtsPerCm2   = Settings->PointsPerSquareMeter / 10000.f;

    // Capped point count — calculated once since R and density are constant
    const int32 NumPointsPerHex = FMath::Min(
        FMath::Max(1, FMath::RoundToInt(HexAreaCm2 * PtsPerCm2))
    );

    // Flat-top hex corners
    FVector2D Corners[6];
    for (int32 i = 0; i < 6; i++)
    {
        float Angle = FMath::DegreesToRadians(60.f * i - 30.f);
        Corners[i]  = FVector2D(R * FMath::Cos(Angle), R * FMath::Sin(Angle));
    }

    FRandomStream RNG(Settings->Seed);

    FCollisionQueryParams TraceParams(TEXT("HexSurfaceSnap"), true);

    for (AHexTile* Tile : Settings->HexTiles)
    {
        if (!Tile) continue;

        // Skip water tiles — no foliage on water
        if (Tile->HexType == EHexType::Water) continue;

        TraceParams.AddIgnoredActor(Tile);

        const FVector      HexCenter  = Tile->GetActorLocation();
        const float        Density    = static_cast<float>(Tile->HexType) / 5.f;
        const float        Steepness  = Tile->SmoothedElevation;
        const FLinearColor BiomeColor = HexTypeToColor(Tile->HexType);

        // ---- one PointData per tile so PCG processes each hex independently ----
        UPCGPointData*    PointData = nullptr;
        if (IsInGameThread())
        {
            PointData = NewObject<UPCGPointData>();
        }
        else
        {
            // Marshal to game thread
            FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&]()
            {
                PointData = NewObject<UPCGPointData>();
            }, TStatId(), nullptr, ENamedThreads::GameThread);
            FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);
        }
        TArray<FPCGPoint>& Points   = PointData->GetMutablePoints();
        Points.Reserve(NumPointsPerHex);

        // Trace once per hex center for base Z — reused as fallback
        float BaseZ = HexCenter.Z;
        if (World)
        {
            FHitResult CenterHit;
            if (World->LineTraceSingleByChannel(
                    CenterHit,
                    FVector(HexCenter.X, HexCenter.Y, HexCenter.Z + 50000.f),
                    FVector(HexCenter.X, HexCenter.Y, HexCenter.Z - 50000.f),
                    ECC_WorldStatic,
                    TraceParams))
            {
                BaseZ = CenterHit.ImpactPoint.Z;
            }
        }

        for (int32 p = 0; p < NumPointsPerHex; p++)
        {
            // ---- uniform triangle sampling over hex ----
            int32 TriIdx       = RNG.RandRange(0, 5);
            const FVector2D& B = Corners[TriIdx];
            const FVector2D& C = Corners[(TriIdx + 1) % 6];

            float u = RNG.FRand();
            float v = RNG.FRand();
            if (u + v > 1.f) { u = 1.f - u; v = 1.f - v; }

            FVector2D LocalPos = u * B + v * C;

            if (Settings->SnapGridSizeY > 0.f)
                LocalPos.Y = FMath::GridSnap(LocalPos.Y, Settings->SnapGridSizeY);

            const FVector WorldXY(
                HexCenter.X + LocalPos.X,
                HexCenter.Y + LocalPos.Y,
                0.f);

            // ---- per-point Z trace, falls back to BaseZ ----
            float SnappedZ = BaseZ;
            if (World)
            {
                FHitResult Hit;
                if (World->LineTraceSingleByChannel(
                        Hit,
                        FVector(WorldXY.X, WorldXY.Y, HexCenter.Z + 50000.f),
                        FVector(WorldXY.X, WorldXY.Y, HexCenter.Z - 50000.f),
                        ECC_WorldStatic,
                        TraceParams))
                {
                    SnappedZ = Hit.ImpactPoint.Z;
                }
            }

            // ---- random yaw + scale variation ----
            const float Yaw         = RNG.FRandRange(0.f, 360.f);
            const float Scale       = RNG.FRandRange(Settings->MinScale, Settings->MaxScale);
            const FRotator Rotation = FRotator(0.f, Yaw, 0.f);
            const FVector  ScaleVec = FVector(Scale);
            const FVector  Position = FVector(WorldXY.X, WorldXY.Y, SnappedZ);

            FPCGPoint Point;
            Point.Transform = FTransform(Rotation, Position, ScaleVec);
            Point.Density   = Density;
            Point.Color     = BiomeColor;
            Point.Steepness = Steepness;
            Points.Add(Point);
        }

        // ---- output one tagged data entry per tile ----
        FPCGTaggedData& Output = Context->OutputData.TaggedData.Emplace_GetRef();
        Output.Data = PointData;
        Output.Pin  = PCGPinConstants::DefaultOutputLabel;
    }

    UE_LOG(LogTemp, Warning, TEXT("PCG Executing - %d tiles"), Settings->HexTiles.Num());

    return true;
}

#undef LOCTEXT_NAMESPACE