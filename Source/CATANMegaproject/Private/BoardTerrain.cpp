#include "BoardTerrain.h"
#include "HexTile.h"
#include "KismetProceduralMeshLibrary.h"

// ---------------------------------------------------------------------------
// Simple 2-D Perlin noise (value-noise variant with smoothstep interpolation)
// ---------------------------------------------------------------------------
namespace
{
    // Repeatable hash → pseudo-random vector in [-1,1]
    static FVector2D PerlinGrad(int32 X, int32 Y)
    {
        int32 H = (X * 92812373 + Y * 42070493 + 1234567) & 0x7FFFFFFF;
        H = (H ^ (H >> 13)) * 1103515245;
        return FVector2D(
            ((H >> 16) & 0xFFFF) / 32767.5f - 1.f,
            ((H)       & 0xFFFF) / 32767.5f - 1.f
        );
    }

    static float SmoothStep(float T)
    {
        return T * T * T * (T * (T * 6.f - 15.f) + 10.f);
    }

    static float PerlinNoise2D(float X, float Y)
    {
        int32 X0 = FMath::FloorToInt(X);
        int32 Y0 = FMath::FloorToInt(Y);
        int32 X1 = X0 + 1;
        int32 Y1 = Y0 + 1;

        float SX = X - (float)X0;
        float SY = Y - (float)Y0;
        float UX = SmoothStep(SX);
        float UY = SmoothStep(SY);

        FVector2D G00 = PerlinGrad(X0, Y0);
        FVector2D G10 = PerlinGrad(X1, Y0);
        FVector2D G01 = PerlinGrad(X0, Y1);
        FVector2D G11 = PerlinGrad(X1, Y1);

        float N00 = FVector2D::DotProduct(G00, FVector2D(SX, SY));
        float N10 = FVector2D::DotProduct(G10, FVector2D(SX - 1.f, SY));
        float N01 = FVector2D::DotProduct(G01, FVector2D(SX, SY - 1.f));
        float N11 = FVector2D::DotProduct(G11, FVector2D(SX - 1.f, SY - 1.f));

        float IX0 = FMath::Lerp(N00, N10, UX);
        float IX1 = FMath::Lerp(N01, N11, UX);
        return FMath::Lerp(IX0, IX1, UY);
    }

    // Multi-octave fractal-like noise (fractional Brownian motion)
    static float FBMNoise(float X, float Y, int32 Octaves, float Persistence, float Lacunarity)
    {
        float Total      = 0.f;
        float Amplitude  = 1.f;
        float Frequency  = 1.f;
        float MaxValue   = 0.f;

        for (int32 I = 0; I < Octaves; I++)
        {
            Total     += PerlinNoise2D(X * Frequency, Y * Frequency) * Amplitude;
            MaxValue  += Amplitude;
            Amplitude *= Persistence;
            Frequency *= Lacunarity;
        }
        return Total / MaxValue;  // Normalise to ~[-1,1]
    }
}

ABoardTerrain::ABoardTerrain()
{
    PrimaryActorTick.bCanEverTick = false;
    TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
    RootComponent = TerrainMesh;
}

void ABoardTerrain::GenerateTerrain(const TArray<AHexTile*>& HexTiles, float HexSize)
{
    const float WorldScale = 10.f;
    TMap<TPair<int32,int32>, AHexTile*> CoordMap;
    for (AHexTile* Tile : HexTiles)
        if (Tile) CoordMap.Add({Tile->Q, Tile->R}, Tile);

    FVector2D WorldMin(FLT_MAX, FLT_MAX);
    FVector2D WorldMax(-FLT_MAX, -FLT_MAX);
    for (AHexTile* Tile : HexTiles)
    {
        if (!Tile) continue;
        FVector Loc = Tile->GetActorLocation();
        WorldMin.X = FMath::Min(WorldMin.X, Loc.X - HexSize);
        WorldMin.Y = FMath::Min(WorldMin.Y, Loc.Y - HexSize);
        WorldMax.X = FMath::Max(WorldMax.X, Loc.X + HexSize);
        WorldMax.Y = FMath::Max(WorldMax.Y, Loc.Y + HexSize);
    }
    WorldMin.X -= BorderPadding; WorldMin.Y -= BorderPadding;
    WorldMax.X += BorderPadding; WorldMax.Y += BorderPadding;

    Resolution = Resolution * (WorldScale/4);

    float GlobalElevMin = FLT_MAX, GlobalElevMax = -FLT_MAX;
    for (AHexTile* Tile : HexTiles)
    {
        if (!Tile) continue;
        GlobalElevMin = FMath::Min(GlobalElevMin, Tile->SmoothedElevation);
        GlobalElevMax = FMath::Max(GlobalElevMax, Tile->SmoothedElevation);
    }
    if (FMath::IsNearlyEqual(GlobalElevMin, GlobalElevMax))
        GlobalElevMax = GlobalElevMin + 1.f;

    int32 NumCols = FMath::CeilToInt((WorldMax.X - WorldMin.X) / Resolution) + 1;
    int32 NumRows = FMath::CeilToInt((WorldMax.Y - WorldMin.Y) / Resolution) + 1;

    TArray<FVector>   Vertices;
    TArray<int32>     Triangles;
    TArray<FVector2D> UVs;
    TArray<FColor>    VertexColors;

    const float BlendRadius    = HexSize * 4.0f;
    const float EdgeStart      = HexSize * 0.95f;
    const float EdgeFade       = HexSize * 0.8f;
    const float HexInnerRadius = HexSize * FMath::Sqrt(3.f) / 2.f;
    const float WaterDepth     = HexSize * 0.12f;
    const float BumpSize       = HexSize * 0.6f;
    const float NoiseScale     = 1.f / BumpSize;

    auto GetTileAmplitude = [&](EHexType Type) -> float
    {
        switch (Type)
        {
            case EHexType::Mountain: return MountainDetailAmplitude;
            case EHexType::Hill:     return HillDetailAmplitude;
            case EHexType::Forest:   return ForestDetailAmplitude;
            case EHexType::Pasture:  return PastureDetailAmplitude;
            case EHexType::Fields:   return FieldsDetailAmplitude;
            case EHexType::Desert:   return DesertDetailAmplitude;
            default:                 return 0.05f;
        }
    };

    auto GetBiomeBaseHeight = [&](EHexType Type) -> float
    {
        switch (Type)
        {
            case EHexType::Mountain: return MountainHeight;
            case EHexType::Hill:     return HillHeight;
            case EHexType::Forest:   return ForestHeight;
            case EHexType::Pasture:  return PastureHeight;
            case EHexType::Fields:   return FieldHeight; // slightly above sea level
            case EHexType::Desert:   return DesertHeight;
            default:                 return 0.3f;
        }
    };
    int32 NumVerts = NumCols * NumRows;
    Vertices.SetNum(NumVerts);
    UVs.SetNum(NumVerts);

    TArray<FHexData> HexData;
    HexData.Reserve(HexTiles.Num());

    for (AHexTile* Tile : HexTiles)
    {
        if (!Tile) continue;

        FHexData Data;
        Data.Pos = FVector2D(Tile->GetActorLocation().X, Tile->GetActorLocation().Y);
        Data.Height = GetBiomeBaseHeight(Tile->HexType);
        Data.Amp = GetTileAmplitude(Tile->HexType);

        HexData.Add(Data);
    }
    
    ParallelFor(NumVerts, [&](int32 Index)
        {
                    int32 Col = Index % NumCols;
                    int32 Row = Index / NumCols;

                    float WorldX = WorldMin.X + Col * Resolution;
                    float WorldY = WorldMin.Y + Row * Resolution;

                    float MinDist = FLT_MAX;
                    float TotalWeight = 1e-6f;

                    float BiomeHeight = 0.f;
                    float BiomeAmp = 0.f;

                    FVector2D P(WorldX, WorldY);

                    for (const FHexData& Tile : HexData)
                    {
                        float Dist = FVector2D::Distance(P, Tile.Pos);

                        MinDist = FMath::Min(MinDist, Dist);
                        if (Dist > BlendRadius) continue;

                        float Norm = Dist / BlendRadius;
                        float Weight = FMath::Exp(-4.f * Norm * Norm);

                        BiomeHeight += Tile.Height * Weight;
                        BiomeAmp    += Tile.Amp * Weight;

                        TotalWeight += Weight;
                    }

                    BiomeHeight /= TotalWeight;
                    BiomeAmp /= TotalWeight;

                    // ====================================================
                    // 1. DOMAIN WARPING (world shape)
                    // ====================================================
                    const float WarpScale = 0.0025f;
                    const float WarpStrength = 120.f;

                    float WarpX = FBMNoise(WorldX * WarpScale, WorldY * WarpScale, 2, 0.5f, 2.0f);
                    float WarpY = FBMNoise(WorldX * WarpScale + 100.f, WorldY * WarpScale + 100.f, 2, 0.5f, 2.0f);

                    float WX = WorldX + WarpX * WarpStrength;
                    float WY = WorldY + WarpY * WarpStrength;

                    // ====================================================
                    // 2. CONTINENTAL SHAPE (land masses)
                    // ====================================================
                    float Continental = FBMNoise(WX * 0.0015f, WY * 0.0015f, 1.5f, 0.5f, 2.0f);
                    Continental = FMath::Pow(FMath::Clamp(Continental * 0.5f + 0.5f, 0.f, 1.f), 1.3f);

                    // ====================================================
                    // 3. REAL MOUNTAIN RANGES (ridge system)
                    // ====================================================
                    float RidgeNoise = FBMNoise(WX * 0.0008f, WY * 0.0008f, 3, 0.5f, 2.0f);
                    float Angle = RidgeNoise * 6.28318f;

                    FVector2D FlowDir(FMath::Cos(Angle), FMath::Sin(Angle));

                    float RidgeField = 0.f;
                    for (int i = 0; i < 4; i++)
                    {
                        float PX = WX + FlowDir.X * i * 140.f;
                        float PY = WY + FlowDir.Y * i * 140.f;

                        RidgeField += FBMNoise(PX * 0.002f, PY * 0.002f, 2, 0.5f, 2.0f);
                    }
                    float MountainRanges = 1.0f - FMath::Abs(RidgeField / 4.f);
                    MountainRanges = FMath::Pow(MountainRanges, 4.5f);
                    MountainRanges *= 0.6f;

                    MountainRanges *= Continental; // only on land

                    // ====================================================
                    // 4. BIOME BASE HEIGHT
                    // ====================================================
                    float BaseHeight =
                        BiomeHeight * 0.6f +
                        Continental * 0.2f +
                        MountainRanges * 0.4f;

                    // ====================================================
                    // 5. SCALE (clean separation)
                    // ====================================================
                    float HeightScale = ElevationScale;
                    float Elevation = BaseHeight * HeightScale;

                    // ====================================================
                    // 6. GLACIER EDGE FALLOFF (no cliffs)
                    // ====================================================
                    float EdgeDist = MinDist - EdgeStart;
                    float EdgeMask = FMath::Clamp(EdgeDist / EdgeFade, 0.f, 1.f);
                    EdgeMask = FMath::Pow(EdgeMask, 2.2f);

                    Elevation *= (1.f - EdgeMask * 0.85f);

                    // ====================================================
                    // 7. DETAIL NOISE (safe, non-breaking)
                    // ====================================================
                    float Detail = FBMNoise(
                        WX * NoiseScale,
                        WY * NoiseScale,
                        4,
                        0.5f,
                        2.0f
                    );

                    float DetailAmp = BiomeAmp * HeightScale * 0.12f;

                    float DistFromBorder = HexInnerRadius - MinDist;
                    float NoiseMask = FMath::SmoothStep(0.f, 1.f,
                        FMath::Clamp(DistFromBorder / (HexInnerRadius * 0.4f), 0.f, 1.f));

                    // Elevation += Detail * DetailAmp * NoiseMask;

            Vertices[Index] = FVector(WorldX, WorldY, Elevation);
        UVs.Add(FVector2D(
    Col / (float)(NumCols - 1),
    Row / (float)(NumRows - 1)
));
        });


    // ----------------------------------------------------
    // MESH BUILD
    // ----------------------------------------------------
    for (int32 Row = 0; Row < NumRows - 1; Row++)
    {
        for (int32 Col = 0; Col < NumCols - 1; Col++)
        {
            int32 BL = Row * NumCols + Col;
            int32 BR = BL + 1;
            int32 TL = BL + NumCols;
            int32 TR = TL + 1;

            Triangles.Add(BL); Triangles.Add(TL); Triangles.Add(BR);
            Triangles.Add(BR); Triangles.Add(TL); Triangles.Add(TR);
        }
    }

    TArray<FProcMeshTangent> Tangents;
    TArray<FVector> Normals;

    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
        Vertices, Triangles, UVs, Normals, Tangents);

    TerrainMesh->CreateMeshSection(
        0, Vertices, Triangles, Normals, UVs,
        VertexColors, Tangents, true);
}
