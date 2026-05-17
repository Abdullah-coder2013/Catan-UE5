#include "BoardTerrain.h"

#include "CatanPCGSettings.h"
#include "HexTile.h"
#include "PCGComponent.h"
#include "PCGGraph.h"
#include "Data/PCGPointData.h"
#include "PCGContext.h"
#include "KismetProceduralMeshLibrary.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"

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

float ABoardTerrain::ComputeTerrainHeightAt(float WorldX, float WorldY) const
{
    if (CachedHeightGrid.Num() == 0 || CachedGridCellSize <= 0.f) return 0.f;

    float U = (WorldX - CachedGridWorldMin.X) / CachedGridCellSize;
    float V = (WorldY - CachedGridWorldMin.Y) / CachedGridCellSize;

    int32 Col = FMath::FloorToInt(U);
    int32 Row = FMath::FloorToInt(V);

    Col = FMath::Clamp(Col, 0, CachedGridNumCols - 2);
    Row = FMath::Clamp(Row, 0, CachedGridNumRows - 2);

    float FracCol = FMath::Clamp(U - (float)Col, 0.f, 1.f);
    float FracRow = FMath::Clamp(V - (float)Row, 0.f, 1.f);

    int32 BL = Row * CachedGridNumCols + Col;
    int32 BR = BL + 1;
    int32 TL = BL + CachedGridNumCols;
    int32 TR = TL + 1;

    float H_BL = CachedHeightGrid[BL];
    float H_BR = CachedHeightGrid[BR];
    float H_TL = CachedHeightGrid[TL];
    float H_TR = CachedHeightGrid[TR];

    if (FracCol + FracRow <= 1.f)
    {
        return H_BL * (1.f - FracCol - FracRow) + H_TL * FracRow + H_BR * FracCol;
    }
    else
    {
        float Fu = 1.f - FracCol;
        float Fv = 1.f - FracRow;
        return H_TR * (1.f - Fu - Fv) + H_BR * Fu + H_TL * Fv;
    }
}

float ABoardTerrain::ComputeRoadMaskAt(float WorldX, float WorldY) const
{
    if (CachedHexData.Num() == 0) return 0.f;

    float ClosestDist = FLT_MAX;
    float SecondDist  = FLT_MAX;

    for (const FCachedHexData& Tile : CachedHexData)
    {
        float Dist = FVector2D::Distance(FVector2D(WorldX, WorldY), Tile.Pos);
        if (Dist < ClosestDist)
        {
            SecondDist  = ClosestDist;
            ClosestDist = Dist;
        }
        else if (Dist < SecondDist)
        {
            SecondDist = Dist;
        }
    }

    float BorderDist = SecondDist - ClosestDist;
    float RoadWidth  = CachedHexSize * RoadWidthGlobal;
    float RoadMask   = FMath::Clamp(1.f - BorderDist / RoadWidth, 0.f, 1.f);
    return RoadMask;
}

FVector ABoardTerrain::ComputeTerrainNormalAt(float WorldX, float WorldY) const
{
    if (CachedHeightGrid.Num() == 0) return FVector::UpVector;

    const float Epsilon = CachedGridCellSize * 0.5f;

    float HC = ComputeTerrainHeightAt(WorldX, WorldY);
    float HX = ComputeTerrainHeightAt(WorldX + Epsilon, WorldY);
    float HY = ComputeTerrainHeightAt(WorldX, WorldY + Epsilon);

    float Dx = (HX - HC) / Epsilon;
    float Dy = (HY - HC) / Epsilon;

    return FVector(-Dx, -Dy, 1.f).GetSafeNormal();
}

float ABoardTerrain::SampleRoadMask(float WorldX, float WorldY) const
{
    if (CachedRoadMask.Num() == 0 || CachedRoadMaskWorldSize.X <= 0.f || CachedRoadMaskWorldSize.Y <= 0.f) return 0.f;

    float U = (WorldX - CachedRoadMaskWorldMin.X) / CachedRoadMaskWorldSize.X;
    float V = (WorldY - CachedRoadMaskWorldMin.Y) / CachedRoadMaskWorldSize.Y;

    if (U < 0.f || U > 1.f || V < 0.f || V > 1.f) return 0.f;

    int32 TexSize = BiomeTextureResolution;
    int32 PX = FMath::Clamp(FMath::FloorToInt(U * (TexSize - 1)), 0, TexSize - 1);
    int32 PY = FMath::Clamp(FMath::FloorToInt(V * (TexSize - 1)), 0, TexSize - 1);

    return CachedRoadMask[PY * TexSize + PX] / 255.f;
}

float ABoardTerrain::SampleBeachMask(float WorldX, float WorldY) const
{
    if (CachedBeachMask.Num() == 0 || CachedRoadMaskWorldSize.X <= 0.f || CachedRoadMaskWorldSize.Y <= 0.f) return 0.f;

    float U = (WorldX - CachedRoadMaskWorldMin.X) / CachedRoadMaskWorldSize.X;
    float V = (WorldY - CachedRoadMaskWorldMin.Y) / CachedRoadMaskWorldSize.Y;

    if (U < 0.f || U > 1.f || V < 0.f || V > 1.f) return 0.f;

    int32 TexSize = BiomeTextureResolution;
    int32 PX = FMath::Clamp(FMath::FloorToInt(U * (TexSize - 1)), 0, TexSize - 1);
    int32 PY = FMath::Clamp(FMath::FloorToInt(V * (TexSize - 1)), 0, TexSize - 1);

    return CachedBeachMask[PY * TexSize + PX] / 255.f;
}

ABoardTerrain::ABoardTerrain()
{
    PrimaryActorTick.bCanEverTick = false;
    TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
    RootComponent = TerrainMesh;
    PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCGComponent"));
    TerrainMesh->SetupAttachment(RootComponent);
}


void ABoardTerrain::TriggerPCG(const TArray<AHexTile*>& HexTiles)
{
    if (!PCGComponent) return;

    // Find the HexInput node settings inside the PCG graph
    UPCGGraph* Graph = PCGComponent->GetGraph();
    if (!Graph) return;

    for (UPCGNode* Node : Graph->GetNodes())
    {
        UCatanPCGSettingsSettings* HexSettings =
            Cast<UCatanPCGSettingsSettings>(Node->GetSettings());

        if (HexSettings)
        {
            // Inject your hex tiles directly into the node settings
            HexSettings->HexTiles = HexTiles;
            break;
        }
    }

    // Now safe to generate — node has the data it needs
    PCGComponent->Generate();
}

void ABoardTerrain::GenerateTerrain(const TArray<AHexTile*>& HexTiles, float HexSize)
{
    const float WorldScale = 100.f;
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

    const float ActualResolution = Resolution * (WorldScale / 4);

    float GlobalElevMin = FLT_MAX, GlobalElevMax = -FLT_MAX;
    for (AHexTile* Tile : HexTiles)
    {
        if (!Tile) continue;
        GlobalElevMin = FMath::Min(GlobalElevMin, Tile->SmoothedElevation);
        GlobalElevMax = FMath::Max(GlobalElevMax, Tile->SmoothedElevation);
    }
    if (FMath::IsNearlyEqual(GlobalElevMin, GlobalElevMax))
        GlobalElevMax = GlobalElevMin + 1.f;

    int32 NumCols = FMath::CeilToInt((WorldMax.X - WorldMin.X) / ActualResolution) + 1;
    int32 NumRows = FMath::CeilToInt((WorldMax.Y - WorldMin.Y) / ActualResolution) + 1;

    TArray<FVector>   Vertices;
    TArray<int32>     Triangles;
    TArray<FVector2D> UVs;
    TArray<FVector2D> UV3s;
    TArray<FVector2D> UV2s;
    TArray<FColor>    VertexColors;

    const float TransitionWidth = HexSize * 0.08f;
    const float BlendRadius     = HexSize * 4.0f;
    const float RoadWidth       = HexSize * RoadWidthGlobal;
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
    auto BiomeToFloat = [](EHexType Type) -> float
    {
        switch (Type)
        {
        case EHexType::Water:    return 0.0f;
        case EHexType::Desert:   return 0.166f;
        case EHexType::Fields:   return 0.333f;
        case EHexType::Pasture:  return 0.5f;
        case EHexType::Forest:   return 0.666f;
        case EHexType::Hill:     return 0.833f;
        case EHexType::Mountain: return 1.0f;
        default:                 return 0.0f;
        }
    };
    int32 NumVerts = NumCols * NumRows;
    Vertices.SetNum(NumVerts);
    UVs.SetNum(NumVerts);
    UV3s.SetNum(NumVerts);
    UV3s.Init(FVector2D(0.5f, 0.f), NumVerts);
    UV2s.SetNum(NumVerts);
    VertexColors.SetNum(NumVerts);

    CachedHexSize = HexSize;

    TArray<FCachedHexData> HexData;
    HexData.Reserve(HexTiles.Num());

    for (AHexTile* Tile : HexTiles)
    {
        if (!Tile) continue;

        FCachedHexData Data;
        Data.Pos = FVector2D(Tile->GetActorLocation().X, Tile->GetActorLocation().Y);
        Data.Height = GetBiomeBaseHeight(Tile->HexType);
        Data.Amp = GetTileAmplitude(Tile->HexType);
        Data.Type = Tile->HexType;
        Data.BiomeFloat = BiomeToFloat(Tile->HexType);
        HexData.Add(Data);
    }

    CachedHexData = HexData;

    TArray<FVector2D> HexCorners;
    const float CornerClearance = CornerClearanceRadius * HexSize;
    for (const FCachedHexData& Tile : HexData)
    {
        for (int32 k = 0; k < 6; k++)
        {
            float Angle = k * (PI / 3.f) + PI / 6.f;
            HexCorners.Add(Tile.Pos + HexSize * FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)));
        }
    }

    ParallelFor(NumVerts, [&](int32 Index)
        {
                    int32 Col = Index % NumCols;
                    int32 Row = Index / NumCols;

                    float WorldX = WorldMin.X + Col * ActualResolution;
                    float WorldY = WorldMin.Y + Row * ActualResolution;

                    float MinDist = FLT_MAX;

                    float SmoothBiomeHeight = 0.f;
                    float SmoothBiomeAmp = 0.f;
                    float SmoothTotalWeight = 1e-6f;

                    float Forest = 0.f;
                    float Desert = 0.f;
                    float Pasture = 0.f;
                    float Fields = 0.f;
                    float Hill = 0.f;
                    float Mountain = 0.f;

                    FVector2D P(WorldX, WorldY);

                    float ClosestDist = FLT_MAX;
                    float SecondDist = FLT_MAX;
                    const FCachedHexData* ClosestHex = nullptr;
                    const FCachedHexData* SecondHex = nullptr;

                    for (const FCachedHexData& Tile : HexData)
                    {
                        float Dist = FVector2D::Distance(P, Tile.Pos);
                        MinDist = FMath::Min(MinDist, Dist);

                        if (Dist < ClosestDist)
                        {
                            SecondDist = ClosestDist;
                            SecondHex = ClosestHex;
                            ClosestDist = Dist;
                            ClosestHex = &Tile;
                        }
                        else if (Dist < SecondDist)
                        {
                            SecondDist = Dist;
                            SecondHex = &Tile;
                        }

                        if (Dist > BlendRadius) continue;

                        float Norm = Dist / BlendRadius;
                        float Weight = FMath::Exp(-4.f * Norm * Norm);

                        SmoothBiomeHeight += Tile.Height * Weight;
                        SmoothBiomeAmp    += Tile.Amp * Weight;
                        SmoothTotalWeight += Weight;
                    }

                    float BiomeHeight = SmoothBiomeHeight / SmoothTotalWeight;
                    float BiomeAmp = SmoothBiomeAmp / SmoothTotalWeight;

                    MinDist = ClosestDist;

                    float BorderDist = SecondDist - ClosestDist;
                    float BlendT = FMath::Clamp((BorderDist + TransitionWidth) / (2.f * TransitionWidth), 0.f, 1.f);
                    BlendT = SmoothStep(BlendT);

                    auto AssignBiomeWeight = [&](EHexType Type, float W)
                    {
                        switch (Type)
                        {
                            case EHexType::Forest:   Forest += W; break;
                            case EHexType::Desert:   Desert += W; break;
                            case EHexType::Pasture:  Pasture += W; break;
                            case EHexType::Fields:   Fields += W; break;
                            case EHexType::Hill:     Hill += W; break;
                            case EHexType::Mountain: Mountain += W; break;
                            default: break;
                        }
                    };

                    AssignBiomeWeight(ClosestHex->Type, BlendT);
                    if (SecondHex && BlendT < 1.f)
                        AssignBiomeWeight(SecondHex->Type, 1.f - BlendT);

                    float Sum = Forest + Desert + Pasture + Fields + Hill + Mountain + 1e-6f;
                    Forest   /= Sum;
                    Desert   /= Sum;
                    Pasture  /= Sum;
                    Fields   /= Sum;
                    Hill     /= Sum;
                    Mountain /= Sum;

                    float GrassLike = Pasture + Fields;
        uint8 R = (uint8)(FMath::Clamp(Forest,  0.f, 1.f) * 255);
        uint8 G = (uint8)(FMath::Clamp(Desert,  0.f, 1.f) * 255);
        uint8 B = (uint8)(FMath::Clamp(Hill,    0.f, 1.f) * 255);
        uint8 A = (uint8)(FMath::Clamp(Mountain,0.f, 1.f) * 255);

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
                    float BeachMask = FMath::Clamp(EdgeDist / EdgeFade, 0.f, 1.f);

                    Elevation *= (1.f - BeachMask * 0.85f);

        float RoadMask = ComputeRoadMaskAt(WorldX, WorldY);
        for (const FVector2D& Corner : HexCorners)
        {
            float Dist = FVector2D::Distance(P, Corner);
            if (Dist < CornerClearance)
                RoadMask = FMath::Max(RoadMask, 1.f - Dist / CornerClearance);
        }

        const float Sqrt3Over2 = 0.8660254f;
        FVector2D LocalP = P - ClosestHex->Pos;
        float HexSDF = FMath::Max(
            FMath::Abs(LocalP.X),
            FMath::Max(
                FMath::Abs(0.5f * LocalP.X + Sqrt3Over2 * LocalP.Y),
                FMath::Abs(0.5f * LocalP.X - Sqrt3Over2 * LocalP.Y)
            )
        );
        const float HexInner = HexSize * Sqrt3Over2;
        const float BoardEdgeFade = HexSize * 0.15f;
        float BoardMask = 1.f - FMath::Clamp((HexSDF - HexInner) / BoardEdgeFade, 0.f, 1.f);
        BoardMask = SmoothStep(BoardMask);
        

            Vertices[Index] = FVector(WorldX, WorldY, Elevation);
        UVs[Index] = FVector2D(
        Col / (float)(NumCols - 1),
        Row / (float)(NumRows - 1)
        
    );
        UV3s[Index] = FVector2D(BeachMask, RoadMask);
        UV2s[Index] = FVector2D(Pasture, Fields);
        VertexColors[Index] = FColor(R, G, B, A);
    });


    CachedHeightGrid.SetNum(NumVerts);
    for (int32 i = 0; i < NumVerts; i++)
        CachedHeightGrid[i] = Vertices[i].Z;
    CachedGridWorldMin = WorldMin;
    CachedGridNumCols = NumCols;
    CachedGridNumRows = NumRows;
    CachedGridCellSize = ActualResolution;

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
    0, Vertices, Triangles, Normals,
    UVs,
    TArray<FVector2D>(),
    UV2s,
    UV3s,
    VertexColors, Tangents, true
);

    // ----------------------------------------------------
    // BIOME TEXTURE BAKE (high-res RGBA for sharp borders)
    // ----------------------------------------------------
    {
        int32 TexSize = BiomeTextureResolution;

        BiomeTexture = UTexture2D::CreateTransient(TexSize, TexSize, PF_B8G8R8A8);
        BiomeTexture2 = UTexture2D::CreateTransient(TexSize, TexSize, PF_B8G8R8A8);

        if (BiomeTexture)
        {
            BiomeTexture->MipGenSettings = TMGS_NoMipmaps;
            BiomeTexture->SRGB = false;
        }
        if (BiomeTexture2)
        {
            BiomeTexture2->MipGenSettings = TMGS_NoMipmaps;
            BiomeTexture2->SRGB = false;
        }

        FTexture2DMipMap& Mip1 = BiomeTexture->GetPlatformData()->Mips[0];
        FTexture2DMipMap& Mip2 = BiomeTexture2->GetPlatformData()->Mips[0];
        void* Data1 = Mip1.BulkData.Lock(LOCK_READ_WRITE);
        void* Data2 = Mip2.BulkData.Lock(LOCK_READ_WRITE);
        FColor* Pixels1 = static_cast<FColor*>(Data1);
        FColor* Pixels2 = static_cast<FColor*>(Data2);

        float WorldW = WorldMax.X - WorldMin.X;
        float WorldH = WorldMax.Y - WorldMin.Y;
        float MeshWorldW = (float)(NumCols - 1) * ActualResolution;
        float MeshWorldH = (float)(NumRows - 1) * ActualResolution;

        CachedRoadMask.SetNum(TexSize * TexSize);
        CachedBeachMask.SetNum(TexSize * TexSize);
        CachedRoadMaskWorldMin = WorldMin;
        CachedRoadMaskWorldSize = FVector2D(MeshWorldW, MeshWorldH);

        ParallelFor(TexSize * TexSize, [&](int32 Idx)
        {
            int32 PX = Idx % TexSize;
            int32 PY = Idx / TexSize;

            float U = PX / (float)(TexSize - 1);
            float V = PY / (float)(TexSize - 1);

            float WX = WorldMin.X + U * MeshWorldW;
            float WY = WorldMin.Y + V * MeshWorldH;

            FVector2D P(WX, WY);

            float ClosestDist = FLT_MAX;
            float SecondDist = FLT_MAX;
            const FCachedHexData* ClosestHex = nullptr;
            const FCachedHexData* SecondHex = nullptr;

            for (const FCachedHexData& Tile : HexData)
            {
                float Dist = FVector2D::Distance(P, Tile.Pos);
                if (Dist < ClosestDist)
                {
                    SecondDist = ClosestDist;
                    SecondHex = ClosestHex;
                    ClosestDist = Dist;
                    ClosestHex = &Tile;
                }
                else if (Dist < SecondDist)
                {
                    SecondDist = Dist;
                    SecondHex = &Tile;
                }
            }

            float BorderDist = SecondDist - ClosestDist;
            float BlendT = FMath::Clamp((BorderDist + TransitionWidth) / (2.f * TransitionWidth), 0.f, 1.f);
            BlendT = SmoothStep(BlendT);

            float Forest = 0.f, Desert = 0.f, Hill = 0.f, Mountain = 0.f;
            float Pasture = 0.f, Fields = 0.f;

            auto AssignWeight = [&](EHexType Type, float W)
            {
                switch (Type)
                {
                    case EHexType::Forest:   Forest += W; break;
                    case EHexType::Desert:   Desert += W; break;
                    case EHexType::Hill:     Hill += W; break;
                    case EHexType::Mountain: Mountain += W; break;
                    case EHexType::Pasture:  Pasture += W; break;
                    case EHexType::Fields:   Fields += W; break;
                    default: break;
                }
            };

            AssignWeight(ClosestHex->Type, BlendT);
            if (SecondHex && BlendT < 1.f)
                AssignWeight(SecondHex->Type, 1.f - BlendT);

            float Sum = Forest + Desert + Hill + Mountain + Pasture + Fields + 1e-6f;
            Forest   /= Sum;
            Desert   /= Sum;
            Hill     /= Sum;
            Mountain /= Sum;
            Pasture  /= Sum;
            Fields   /= Sum;

            float BeachMask = FMath::Clamp((ClosestDist - EdgeStart) / EdgeFade, 0.f, 1.f);
            CachedBeachMask[Idx] = (uint8)(FMath::Clamp(BeachMask, 0.f, 1.f) * 255);
            float RoadMask = FMath::Clamp(1.f - BorderDist / RoadWidth, 0.f, 1.f);
            for (const FVector2D& Corner : HexCorners)
            {
                float Dist = FVector2D::Distance(P, Corner);
                if (Dist < CornerClearance)
                    RoadMask = FMath::Max(RoadMask, 1.f - Dist / CornerClearance);
            }
            CachedRoadMask[Idx] = (uint8)(FMath::Clamp(RoadMask, 0.f, 1.f) * 255);

            const float Sqrt3Over2 = 0.8660254f;
            FVector2D LocalP = P - ClosestHex->Pos;
            float HexSDF = FMath::Max(
                FMath::Abs(LocalP.X),
                FMath::Max(
                    FMath::Abs(0.5f * LocalP.X + Sqrt3Over2 * LocalP.Y),
                    FMath::Abs(0.5f * LocalP.X - Sqrt3Over2 * LocalP.Y)
                )
            );
            const float HexInner = HexSize * Sqrt3Over2;
            const float BoardEdgeFade = HexSize * 0.15f;
            float BoardMask = 1.f - FMath::Clamp((HexSDF - HexInner) / BoardEdgeFade, 0.f, 1.f);
            BoardMask = SmoothStep(BoardMask);

            Pixels1[Idx] = FColor(
    (uint8)(FMath::Clamp(Forest,   0.f, 1.f) * BoardMask * 255),
    (uint8)(FMath::Clamp(Desert,   0.f, 1.f) * BoardMask * 255),
    (uint8)(FMath::Clamp(Hill,     0.f, 1.f) * BoardMask * 255),
    (uint8)(FMath::Clamp(Mountain, 0.f, 1.f) * BoardMask * 255)
);

Pixels2[Idx] = FColor(
    (uint8)(FMath::Clamp(Pasture,  0.f, 1.f) * BoardMask * 255),
    (uint8)(FMath::Clamp(Fields,   0.f, 1.f) * BoardMask * 255),
    (uint8)(FMath::Clamp(FMath::Max(BeachMask, 1.f - BoardMask), 0.f, 1.f) * 255),
    (uint8)(FMath::Clamp(RoadMask, 0.f, 1.f) * BoardMask * 255)
);
        });

        Mip1.BulkData.Unlock();
        Mip2.BulkData.Unlock();
        BiomeTexture->UpdateResource();
        BiomeTexture2->UpdateResource();
    }
    
    if (TerrainMaterial)
    {
        UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(TerrainMaterial, this);
        if (DynMat)
        {
            if (BiomeTexture)
                DynMat->SetTextureParameterValue(FName("BiomeTexture"), BiomeTexture);
            if (BiomeTexture2)
                DynMat->SetTextureParameterValue(FName("BiomeTexture2"), BiomeTexture2);
        }
        TerrainMesh->SetMaterial(0, DynMat ? DynMat : TerrainMaterial);
    }
    TerrainMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    TerrainMesh->bUseComplexAsSimpleCollision = true;
    TerrainMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    TerrainMesh->SetMobility(EComponentMobility::Static);
    // TriggerPCG(HexTiles);
    GetWorld()->GetTimerManager().SetTimerForNextTick([this, HexTiles]()
    {
        for (auto Hex : HexTiles)
        {
            GenerateFoliage(Hex);
            GenerateBeachFoliage(Hex);
        }
    });
}

TArray<FVector> ABoardTerrain::GenerateProceduralFoliageForBiomeHex(AHexTile* HexTile, float PointsPerMeter)
{
        if (!HexTile) return TArray<FVector>();
    
    FVector HexLocation = HexTile->GetActorLocation();
    float HexRadius = HexTile->GetHexOuterRadius();
    // Convert radius to meters first
    float HexRadiusMeters = HexRadius / 100.f;
    float HexAreaM2 = (3.f * FMath::Sqrt(3.f) / 2.f) * HexRadiusMeters * HexRadiusMeters;
    int32 NumPoints = 
        FMath::RoundToInt(HexAreaM2 * PointsPerMeter);
    
        TArray<FVector> FoliagePositions;
        FoliagePositions.Reserve(NumPoints);
    
        for (int32 i = 0; i < NumPoints; i++)
        {
            int32 Sector = FMath::RandRange(0, 5);
            float Angle1 = Sector * (PI / 3.f) + PI / 6.f;
            float Angle2 = (Sector + 1) * (PI / 3.f) + PI / 6.f;

            FVector2D V1(FMath::Cos(Angle1) * HexRadius, FMath::Sin(Angle1) * HexRadius);
            FVector2D V2(FMath::Cos(Angle2) * HexRadius, FMath::Sin(Angle2) * HexRadius);

            float r1 = FMath::Sqrt(FMath::RandRange(0.f, 1.f));
            float r2 = FMath::RandRange(0.f, 1.f);

            FVector2D HexPoint = V1 * (r1 * (1.f - r2)) + V2 * (r1 * r2);
            FVector SpawnLocation = HexLocation + FVector(HexPoint.X, HexPoint.Y, 0.f);
    
            FoliagePositions.Add(SpawnLocation);
        }
    return FoliagePositions;
}

void ABoardTerrain::GenerateFoliage(AHexTile* HexTile, float DensityScaleBig, float DensityScaleMedium, float DensityScaleSmall)
{
    if (!HexTile) return;

    const FBiomeFoliageSettings& Settings = GetBiomeFoliageSettings(HexTile->HexType);

    TArray<FVector> PointsForBigMesh = GenerateProceduralFoliageForBiomeHex(HexTile, Settings.DensityBig * DensityScaleBig);
    SpawnMeshesForHexTile(HexTile, PointsForBigMesh, 0.3f, 3, EFoliageType::Large, Settings);
    
    TArray<FVector> PointsForMediumMesh = GenerateProceduralFoliageForBiomeHex(HexTile, Settings.DensityMedium * DensityScaleMedium);
    SpawnMeshesForHexTile(HexTile, PointsForMediumMesh, 0.5f, 3, EFoliageType::Medium, Settings);
    
    TArray<FVector> PointsForSmallMesh = GenerateProceduralFoliageForBiomeHex(HexTile, Settings.DensitySmall * DensityScaleSmall);
    SpawnMeshesForHexTile(HexTile, PointsForSmallMesh, 0.7f, 3, EFoliageType::Small, Settings);
}

void ABoardTerrain::GenerateBeachFoliage(AHexTile* HexTile)
{
    if (!HexTile || HexTile->HexType == EHexType::Water) return;

    FVector HexLocation = HexTile->GetActorLocation();
    float HexRadius = HexTile->GetHexOuterRadius();

    float EdgeStartVal = HexRadius * 0.95f;
    float EdgeFadeVal = HexRadius * 0.8f;
    float InnerR = EdgeStartVal;
    float OuterR = EdgeStartVal + EdgeFadeVal * 0.6f;

    FRandomStream RNG(Seed + HexTile->GetUniqueID() + 999);

    auto PickMesh = [&](const TArray<UStaticMesh*>& Meshes) -> UStaticMesh*
    {
        if (Meshes.Num() == 0) return nullptr;
        return Meshes[RNG.RandRange(0, Meshes.Num() - 1)];
    };

    auto SpawnBeachSize = [&](const TArray<UStaticMesh*>& Meshes, float Density,
        EFoliageSize Size, float MinScale, float MaxScale, bool bCastShadow)
    {
        if (Meshes.Num() == 0) return;
        UStaticMesh* Mesh = PickMesh(Meshes);
        if (!Mesh) return;

        UHierarchicalInstancedStaticMeshComponent* HISM = HexTile->GetOrCreateHISM(Mesh, Size);
        if (!HISM) return;

        float RingAreaM2 = PI * (OuterR * OuterR - InnerR * InnerR) / 10000.f;
        int32 NumPoints = FMath::Max(1, FMath::RoundToInt(RingAreaM2 * Density));

        HISM->bAutoRebuildTreeOnInstanceChanges = false;

        float InnerRSq = InnerR * InnerR;
        float OuterRSq = OuterR * OuterR;

        for (int32 i = 0; i < NumPoints; i++)
        {
            float Angle = RNG.FRandRange(0.f, 2.f * PI);
            float R = FMath::Sqrt(RNG.FRandRange(InnerRSq, OuterRSq));
            float PX = HexLocation.X + R * FMath::Cos(Angle);
            float PY = HexLocation.Y + R * FMath::Sin(Angle);

            if (ComputeRoadMaskAt(PX, PY) > FoliageClearanceThreshold) continue;
            if (SampleBeachMask(PX, PY) < 0.05f) continue;

            FHitResult Hit;
            FVector Start(PX, PY, 500000.f);
            FVector End(PX, PY, -500000.f);
            FCollisionQueryParams Params(TEXT("BeachFoliage"), false);
            Params.AddIgnoredActor(HexTile);

            if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params)) continue;

            float Yaw = RNG.FRandRange(0.f, 360.f);
            float Scale = RNG.FRandRange(MinScale, MaxScale);
            FQuat SlopeAlign = FQuat::FindBetween(FVector::UpVector, Hit.ImpactNormal);
            FQuat Rotation = SlopeAlign * FQuat(FVector::UpVector, FMath::DegreesToRadians(Yaw));

            HISM->AddInstance(FTransform(Rotation, Hit.ImpactPoint, FVector(Scale)), true);
        }

        HISM->BuildTreeIfOutdated(false, true);
        HISM->SetCastShadow(bCastShadow);
        if (bCastShadow)
            HISM->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Static;
        HISM->SetCullDistances(0, 10000000);
    };

    SpawnBeachSize(BigBeachMeshes, BeachFoliage.DensityBig, EFoliageSize::LargeF,
        BeachFoliage.MinScaleBig, BeachFoliage.MaxScaleBig, true);
    SpawnBeachSize(MediumBeachMeshes, BeachFoliage.DensityMedium, EFoliageSize::MediumF,
        BeachFoliage.MinScaleMedium, BeachFoliage.MaxScaleMedium, false);
    SpawnBeachSize(SmallBeachMeshes, BeachFoliage.DensitySmall, EFoliageSize::SmallF,
        BeachFoliage.MinScaleSmall, BeachFoliage.MaxScaleSmall, false);
}

void ABoardTerrain::SpawnMeshesForHexTile(AHexTile* HexTile, TArray<FVector> PointPositions, float Density, float Octaves, EFoliageType FoliageType, const FBiomeFoliageSettings& Settings)
{
    if (!HexTile) { UE_LOG(LogTemp, Error, TEXT("HexTile is null")); return; }
    if (PointPositions.Num() == 0) { UE_LOG(LogTemp, Error, TEXT("No points for hex")); return; }

    UStaticMesh* Mesh = GetStaticMeshForBiome(HexTile->HexType, FoliageType);
    if (!Mesh) 
    { 
        UE_LOG(LogTemp, Error, TEXT("No mesh for biome type %d — array is probably empty in editor"), 
            static_cast<int32>(HexTile->HexType)); 
        return; 
    }

    UE_LOG(LogTemp, Warning, TEXT("Spawning %d points for biome %d"), 
        PointPositions.Num(), static_cast<int32>(HexTile->HexType));
    
    EFoliageSize FoliageSize;
    switch (FoliageType)
    {
    case EFoliageType::Large:  FoliageSize = EFoliageSize::LargeF; break;
    case EFoliageType::Medium: FoliageSize = EFoliageSize::MediumF; break;
    case EFoliageType::Small:  FoliageSize = EFoliageSize::SmallF; break;
    default: FoliageSize = EFoliageSize::MediumF; break;
    }
    UHierarchicalInstancedStaticMeshComponent* HISM = HexTile->GetOrCreateHISM(Mesh, FoliageSize);
    if (!HISM) { UE_LOG(LogTemp, Error, TEXT("HISM creation failed")); return; }

    UE_LOG(LogTemp, Warning, TEXT("HISM created successfully"));

    FRandomStream RNG(Seed + GetUniqueID());

    int32 SpawnedCount = 0;
    HISM->bAutoRebuildTreeOnInstanceChanges = false;
    for (const FVector& Position : PointPositions)
    {
        float Yaw   = RNG.FRandRange(0.f, 360.f);
        float Scale = 1.0f;
        switch (FoliageSize)
        {
            case EFoliageSize::LargeF:
                Scale = RNG.FRandRange(Settings.MinScaleBig, Settings.MaxScaleBig);
                break;
            case EFoliageSize::MediumF:
                Scale = RNG.FRandRange(Settings.MinScaleMedium, Settings.MaxScaleMedium);
                break;
            case EFoliageSize::SmallF:
                Scale = RNG.FRandRange(Settings.MinScaleSmall, Settings.MaxScaleSmall);
                break;
            default:
                break;
        }

        float Noise1 = FBMNoise(Position.X * 0.1f, Position.Y * 0.1f, Octaves, 0.5f, 2.0f);
        float Noise2 = FBMNoise(Position.X * 0.5f + 1000.f, Position.Y * 0.5f + 1000.f, Octaves, 0.5f, 2.0f);
        float NoiseValue = (Noise1 + Noise2) * 0.5f;
        float Threshold = FMath::Lerp(0.3f, 0.7f, Density);
        if (NoiseValue > Threshold) { continue; }

        if (ComputeRoadMaskAt(Position.X, Position.Y) > FoliageClearanceThreshold) continue;

        if (SampleBeachMask(Position.X, Position.Y) > BeachClearanceThreshold) continue;

        FHitResult Hit;
        FVector Start = FVector(Position.X, Position.Y, 500000.f);
        FVector End   = FVector(Position.X, Position.Y, -500000.f);

        FCollisionQueryParams Params(TEXT("FoliageSnap"), false);
        Params.AddIgnoredActor(HexTile);

        float SnappedZ = 0.f;
        FVector HitNormal = FVector::UpVector;
        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
        {
            SnappedZ = Hit.ImpactPoint.Z;
            HitNormal = Hit.ImpactNormal;
        }
        else
        {
            continue;
        }

        FQuat SlopeAlignment = FQuat::FindBetween(FVector::UpVector, HitNormal);
        FQuat FinalRotation = SlopeAlignment * FQuat(FVector::UpVector, FMath::DegreesToRadians(Yaw));

        FTransform T(
            FinalRotation,
            FVector(Position.X, Position.Y, SnappedZ),
            FVector(Scale));

        HISM->AddInstance(T, true);
        SpawnedCount++;
    }
    UE_LOG(LogTemp, Warning, TEXT("Actually spawned %d instances after noise filter"), SpawnedCount);
    HISM->BuildTreeIfOutdated(false, true);
    switch (FoliageType)
    {
    case EFoliageType::Large:
        HISM->SetCastShadow(true); // only for large trees
        HISM->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Static;        // trees cast shadows
        HISM->SetCullDistances(0, 10000000);
        break;
    case EFoliageType::Medium:
        HISM->SetCastShadow(true); // only for large trees
        HISM->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Static;        // no shadows
        HISM->SetCullDistances(0, 10000000);
        break;
    case EFoliageType::Small:
        HISM->SetCastShadow(false);         // no shadows
        HISM->SetCullDistances(0, 10000000);
        break;
    }
}


void ABoardTerrain::UpdateHexStreaming(FVector CameraLocation)
{
    AHexTile* FocusedHex = GetFocusedHex(CameraLocation);
    for (AHexTile* Tile : CachedHexTiles)
    {
        if (!Tile) continue;

        float Dist = FVector::Dist2D(CameraLocation, Tile->GetActorLocation());
        EHexDetailLevel NewLevel;

        if (Tile == FocusedHex)
            NewLevel = EHexDetailLevel::Full;
        else if (Dist < FullDetailRadius)
            NewLevel = EHexDetailLevel::Full;
        else if (Dist < MediumDetailRadius)
            NewLevel = EHexDetailLevel::Medium;
        else if (Dist < ImpostorRadius)
            NewLevel = EHexDetailLevel::Impostor;
        else
            NewLevel = EHexDetailLevel::Canopy;

        if (Tile->CurrentDetailLevel != NewLevel)
        {
            // Defer clearing with timer to avoid pop
            if (NewLevel < Tile->CurrentDetailLevel)
            {
                GetWorld()->GetTimerManager().SetTimer(
                    Tile->ClearTimer,
                    [this, Tile, NewLevel]()
                    {
                        SetHexDetailLevel(Tile, NewLevel);
                    },
                    2.0f, false);
            }
            else
            {
                // Upgrading detail — do immediately
                GetWorld()->GetTimerManager().ClearTimer(Tile->ClearTimer);
                SetHexDetailLevel(Tile, NewLevel);
            }
        }
    }
}

void ABoardTerrain::SetHexDetailLevel(AHexTile* Tile, EHexDetailLevel Level)
{
    if (Tile->CurrentDetailLevel == Level) return;
    Tile->CurrentDetailLevel = Level;

    Tile->ClearFoliage();

    switch (Level)
    {
    case EHexDetailLevel::Full:
        GenerateFoliage(Tile);
        break;

    case EHexDetailLevel::Medium:
        GenerateFoliage(Tile, 0.2f, 0.f, 0.f);
        break;

    case EHexDetailLevel::Impostor:
        // SpawnImpostorForHex(Tile);
        break;

    case EHexDetailLevel::Canopy:
        // Terrain material handles visual
        break;
    }
}



AHexTile* ABoardTerrain::GetFocusedHex(FVector CameraLocation)
{
    AHexTile* Closest = nullptr;
    float ClosestDist = FLT_MAX;

    for (AHexTile* Tile : CachedHexTiles)
    {
        if (!Tile) continue;
        float Dist = FVector::Dist2D(CameraLocation, Tile->GetActorLocation());
        if (Dist < ClosestDist)
        {
            ClosestDist = Dist;
            Closest = Tile;
        }
    }
    return Closest;
}

void ABoardTerrain::BeginPlay()
{
    Super::BeginPlay();

    // Update streaming every 0.5 seconds not every tick
    GetWorld()->GetTimerManager().SetTimer(
        StreamingTimer,
        [this]()
        {
            if (APlayerController* PC = 
                GetWorld()->GetFirstPlayerController())
            {
                if (APawn* Pawn = PC->GetPawn())
                    UpdateHexStreaming(Pawn->GetActorLocation());
            }
        },
        StreamingUpdateInterval, true);
}



const FBiomeFoliageSettings& ABoardTerrain::GetBiomeFoliageSettings(EHexType HexType) const
{
    switch (HexType)
    {
    case EHexType::Forest:   return ForestFoliage;
    case EHexType::Desert:   return DesertFoliage;
    case EHexType::Pasture:  return PastureFoliage;
    case EHexType::Hill:     return HillFoliage;
    case EHexType::Fields:   return FieldsFoliage;
    case EHexType::Mountain: return MountainFoliage;
    default:                 return ForestFoliage;
    }
}

UStaticMesh* ABoardTerrain::GetStaticMeshForBiome(EHexType HexType, EFoliageType FoliageType)
{
    FRandomStream RNG(Seed + GetUniqueID() + static_cast<int32>(HexType) + static_cast<int32>(FoliageType));

    // Safe random pick from any array
    auto SafePick = [&](const TArray<UStaticMesh*>& Meshes) -> UStaticMesh*
    {
        if (Meshes.Num() == 0) return nullptr;
        return Meshes[RNG.RandRange(0, Meshes.Num() - 1)];
    };

    // Map biome + foliage type to the right array
    switch (FoliageType)
    {
    case EFoliageType::Large:
        switch (HexType)
        {
        case EHexType::Forest:   return SafePick(BigForestMeshes);
        case EHexType::Hill:     return SafePick(BigHillMeshes);
        case EHexType::Desert:   return SafePick(BigDesertMeshes);
        case EHexType::Fields:   return SafePick(BigFieldsMeshes);
        case EHexType::Pasture:  return SafePick(BigPasturesMeshes);
        case EHexType::Mountain: return SafePick(BigMountainMeshes);
        default:                 return nullptr;
        }

    case EFoliageType::Medium:
        switch (HexType)
        {
        case EHexType::Forest:   return SafePick(MediumForestMeshes);
        case EHexType::Hill:     return SafePick(MediumHillMeshes);
        case EHexType::Desert:   return SafePick(MediumDesertMeshes);
        case EHexType::Fields:   return SafePick(MediumFieldsMeshes);
        case EHexType::Pasture:  return SafePick(MediumPasturesMeshes);
        case EHexType::Mountain: return SafePick(MediumMountainMeshes);
        default:                 return nullptr;
        }

    case EFoliageType::Small:
        switch (HexType)
        {
        case EHexType::Forest:   return SafePick(SmallForestMeshes);
        case EHexType::Hill:     return SafePick(SmallHillMeshes);
        case EHexType::Desert:   return SafePick(SmallDesertMeshes);
        case EHexType::Fields:   return SafePick(SmallFieldsMeshes);
        case EHexType::Pasture:  return SafePick(SmallPasturesMeshes);
        case EHexType::Mountain: return SafePick(SmallMountainMeshes);
        default:                 return nullptr;
        }

    default:
        return nullptr;
    }
}
