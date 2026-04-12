#include "BoardTerrain.h"
#include "HexTile.h"
#include "KismetProceduralMeshLibrary.h"

ABoardTerrain::ABoardTerrain()
{
    PrimaryActorTick.bCanEverTick = false;
    TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
    RootComponent = TerrainMesh;
}

void ABoardTerrain::GenerateTerrain(const TArray<AHexTile*>& HexTiles, float HexSize)
{
    // Build a lookup map for fast hex sampling
    TMap<TPair<int32,int32>, AHexTile*> CoordMap;
    for (AHexTile* Tile : HexTiles)
    {
        if (Tile) CoordMap.Add({Tile->Q, Tile->R}, Tile);
    }

    // Find world bounds of the board
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
    
    WorldMin.X -= BorderPadding;
    WorldMin.Y -= BorderPadding;
    WorldMax.X += BorderPadding;
    WorldMax.Y += BorderPadding;

    // Calculate grid dimensions
    int32 NumCols = FMath::CeilToInt((WorldMax.X - WorldMin.X) / Resolution) + 1;
    int32 NumRows = FMath::CeilToInt((WorldMax.Y - WorldMin.Y) / Resolution) + 1;

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FColor> VertexColors;

    // Generate vertices
    for (int32 Row = 0; Row < NumRows; Row++)
    {
        for (int32 Col = 0; Col < NumCols; Col++)
        {
            float WorldX = WorldMin.X + Col * Resolution;
            float WorldY = WorldMin.Y + Row * Resolution;
            
            // Check if point is within board bounds with some padding
            float MinDistToHex = FLT_MAX;
            for (AHexTile* Tile : HexTiles)
            {
                if (!Tile) continue;
                FVector TileLoc = Tile->GetActorLocation();
                float Dist = FVector2D::Distance(
                    FVector2D(WorldX, WorldY),
                    FVector2D(TileLoc.X, TileLoc.Y)
                );
                MinDistToHex = FMath::Min(MinDistToHex, Dist);
            }

            // Outside the board - fade to zero elevation gradually
            float EdgeFadeRadius = HexSize * 0.5f;
            if (MinDistToHex > HexSize + EdgeFadeRadius)
            {
                Vertices.Add(FVector(WorldX, WorldY, 0.f));
                UVs.Add(FVector2D(0, 0));
                VertexColors.Add(FColor::Black);
                continue;
            }
            // Blend elevation from nearby hexes using inverse distance weighting
            float TotalWeight = 0.f;
            float BlendedElevation = 0.f;

            // Use a wider radius for smoother transitions
            float BlendRadius = HexSize * 3.0f;

            for (AHexTile* Tile : HexTiles)
            {
                if (!Tile) continue;
                FVector TileLoc = Tile->GetActorLocation();
                float Dist = FVector2D::Distance(
                    FVector2D(WorldX, WorldY),
                    FVector2D(TileLoc.X, TileLoc.Y)
                );

                if (Dist > BlendRadius) continue;

                // Very smooth Gaussian-like falloff
                float NormalizedDist = Dist / BlendRadius;
                float Weight = FMath::Exp(-4.f * NormalizedDist * NormalizedDist);

                BlendedElevation += Tile->BaseElevation * Weight;
                TotalWeight += Weight;
            }

            float Raw = TotalWeight > 0.f ? (BlendedElevation / TotalWeight) : 0.f;

            // Apply edge fade
            if (MinDistToHex > HexSize * 0.95f)
            {
                float BlendFactor = 1.f - FMath::Clamp(
                    (MinDistToHex - HexSize * 0.95f) / EdgeFadeRadius, 0.f, 1.f);
                Raw *= BlendFactor;
            }

            // Remap actual data range to full 0-1 range
            float Remapped = FMath::GetMappedRangeValueClamped(
                FVector2D(0.23f, 0.74f),  // actual min/max from your log data
                FVector2D(0.0f, 1.0f),
                Raw
            );

            float Elevation = Remapped * ElevationScale;
            if (MinDistToHex > HexSize + EdgeFadeRadius)
            {
                Vertices.Add(FVector(WorldX, WorldY, -30.f)); // water depth
                UVs.Add(FVector2D(0, 0));
                VertexColors.Add(FColor::Blue);
                continue;
            }

            Vertices.Add(FVector(WorldX, WorldY, Elevation));
            UVs.Add(FVector2D(
                (WorldX - WorldMin.X) / (WorldMax.X - WorldMin.X),
                (WorldY - WorldMin.Y) / (WorldMax.Y - WorldMin.Y)
            ));
            VertexColors.Add(FColor::White);
        }
    }
    

    // Generate triangles
    for (int32 Row = 0; Row < NumRows - 1; Row++)
    {
        for (int32 Col = 0; Col < NumCols - 1; Col++)
        {
            int32 BL = Row * NumCols + Col;
            int32 BR = BL + 1;
            int32 TL = BL + NumCols;
            int32 TR = TL + 1;

            // Triangle 1
            Triangles.Add(BL);
            Triangles.Add(TL);
            Triangles.Add(BR);

            // Triangle 2
            Triangles.Add(BR);
            Triangles.Add(TL);
            Triangles.Add(TR);
        }
    }

    TArray<FProcMeshTangent> Tangents;
    TArray<FVector> NormalsOut;

    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
        Vertices, Triangles, UVs, NormalsOut, Tangents
    );

    TerrainMesh->CreateMeshSection(
        0, Vertices, Triangles, NormalsOut, UVs,
        VertexColors, Tangents, true
    );
    TerrainMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    TerrainMesh->SetCollisionResponseToAllChannels(ECR_Block);
}