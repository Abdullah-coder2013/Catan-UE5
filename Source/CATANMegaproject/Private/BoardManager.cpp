// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardManager.h"
#include "HexTile.h"
#include "HexVertex.h"
#include "AHexEdge.h"

// Sets default values
ABoardManager::ABoardManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoardManager::BeginPlay()
{
	Super::BeginPlay();
	GenerateBoard();
	
}

// Called every frame

FVector ABoardManager::AxialToWorld(int32 Q, int32 R) const
{
    float X = HexSize * (FMath::Sqrt(3.f) * Q + FMath::Sqrt(3.f) / 2.f * R);
    float Y = HexSize * (3.f / 2.f * R);
    return FVector(X, Y, 0.f);
}

void ABoardManager::GenerateBoard()
{
    TArray<EHexType> TileTypes = {
        EHexType::Forest,   EHexType::Forest,   EHexType::Forest,  EHexType::Forest,
        EHexType::Pasture,  EHexType::Pasture,  EHexType::Pasture, EHexType::Pasture,
        EHexType::Fields,   EHexType::Fields,   EHexType::Fields,  EHexType::Fields,
        EHexType::Hill,    EHexType::Hill,    EHexType::Hill,
        EHexType::Mountain,EHexType::Mountain,EHexType::Mountain,
        EHexType::Desert
    };
    TArray<int32> NumberTokens = { 5, 2, 6, 3, 8, 10, 9, 12, 11, 4, 8, 10, 9, 4, 5, 6, 3, 11, 2 };

    // Shuffle tile types
    for (int32 i = TileTypes.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        TileTypes.Swap(i, j);
    }
    for (int32 i = NumberTokens.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        NumberTokens.Swap(i, j);
    }

    int32 TileIndex = 0;
    int32 TokenIndex = 0;

    // Loop though axial coordinates to find the hexes that should be spawned
    for (int32 Q = -2; Q <= 2; Q++)
    {
        for (int32 R = -2; R <= 2; R++)
        {
            // if tile can be spawned
            if (FMath::Abs(Q) + FMath::Abs(R) + FMath::Abs(Q + R) <= 4)
            {
                // Validate we have data for this tile
                if (TileIndex >= TileTypes.Num())
                {
                    break;
                }

                FVector Location = AxialToWorld(Q, R);
                
                if (HexTileClass)
                {
                    FRotator HexRotation = FRotator::ZeroRotator;
                    AHexTile* NewHexTile = GetWorld()->SpawnActor<AHexTile>(HexTileClass, Location, HexRotation);
                    if (NewHexTile)
                    {
                        int32 Token = 0;
                        if (TileTypes[TileIndex] != EHexType::Desert)
                        {
                            Token = NumberTokens[TokenIndex];
                            TokenIndex++;
                        }
                        NewHexTile->InitializeHex(TileTypes[TileIndex], Token, Q, R, true);
                        HexTiles.Add(NewHexTile);
                        TileIndex++;
                    }
                }
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Total tiles in HexTiles array: %d"), HexTiles.Num());

    // Loop through hex tiles
    for (AHexTile* HexTile : HexTiles) {
        if (HexTile) {
            // Calculate corner positions
            TArray<FVector> Corners;
            for (int32 i = 0; i < 6; i++)
            {
                float Angle = FMath::DegreesToRadians(60.f * i - 30.f); 
                float X = HexTile->GetActorLocation().X + HexSize * FMath::Cos(Angle);
                float Y = HexTile->GetActorLocation().Y + HexSize * FMath::Sin(Angle);
                Corners.Add(FVector(X, Y, 0.f));
            }
            int32 CornerIndex = 0;
            for (FVector& Corner : Corners)
            {
                Corner.X = FMath::RoundToFloat(Corner.X / 10.f) * 10.f;
                Corner.Y = FMath::RoundToFloat(Corner.Y / 10.f) * 10.f;
                FString Key = FString::Printf(TEXT("%.0f_%.0f"), Corner.X, Corner.Y);

                AHexVertex* Vertex = nullptr;

                if (SpawnedVertices.Contains(Key))
                {
                    // Reuse existing vertex - don't spawn anything
                    Vertex = SpawnedVertices[Key];
                }
                else
                {
                    // Only spawn if it doesn't exist yet
                    Vertex = GetWorld()->SpawnActor<AHexVertex>(HexVertexClass, Corner, FRotator::ZeroRotator);
                    if (Vertex)
                    {
                        SpawnedVertices.Add(Key, Vertex);
                    }
                }

                if (Vertex)
                {
                    HexTile->Vertices.Add(Vertex);
                    Vertex->AdjacentHexes.Add(HexTile);
                }
            }
        }
    }
    for (AHexTile* HexTile : HexTiles)
    {
        if (HexTile)
        {
            for (int32 i = 0; i < 6; i++)
            {
                AHexVertex* VertexA = HexTile->Vertices[i];
                AHexVertex* VertexB = HexTile->Vertices[(i + 1) % 6];
                if (VertexA && VertexB)
                {
                    if (!VertexA->AdjacentVertices.Contains(VertexB))
                    {
                        VertexA->AdjacentVertices.Add(VertexB);
                    }
                    if (!VertexB->AdjacentVertices.Contains(VertexA))
                    {
                        VertexB->AdjacentVertices.Add(VertexA);
                    }
                }
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Total unique vertices in SpawnedVertices map: %d"), SpawnedVertices.Num());

    // Edge generation
    for (AHexTile* HexTile : HexTiles)
    {
        if (HexTile)
        {
            for (int32 i = 0; i < 6; i++)
            {
                AHexVertex* VertexA = HexTile->Vertices[i];
                AHexVertex* VertexB = HexTile->Vertices[(i + 1) % 6];

                if (VertexA && VertexB)
                {
                    uint64 PtrA = (uint64)VertexA;
                    uint64 PtrB = (uint64)VertexB;
                    if (PtrA > PtrB)
                    {
                        Swap(PtrA, PtrB);
                    }
                    FString EdgeKey = FString::Printf(TEXT("%llu_%llu"), PtrA, PtrB);

                    if (!SpawnedEdges.Contains(EdgeKey))
                    {
                        FVector EdgeLocation = (VertexA->GetActorLocation() + VertexB->GetActorLocation()) / 2.f;
                        FRotator EdgeRotation = FRotator(0.f, FMath::Atan2(VertexB->GetActorLocation().Y - VertexA->GetActorLocation().Y, VertexB->GetActorLocation().X - VertexA->GetActorLocation().X) * 180.f / PI, 0.f);
                        AAHexEdge* NewEdge = GetWorld()->SpawnActor<AAHexEdge>(HexEdgeClass, EdgeLocation, EdgeRotation);
                        if (NewEdge)
                        {
                            NewEdge->InitializeEdge({ VertexA, VertexB });
                            VertexA->AdjacentEdges.Add(NewEdge);
                            VertexB->AdjacentEdges.Add(NewEdge);
                            SpawnedEdges.Add(EdgeKey, NewEdge);
                            SpawnedEdgesArray.Add(NewEdge);
                            
                        }
                    }
                }
            }
        }
    }
    // Go through edges again to set adjacent edges
    for (AAHexEdge* Edge : SpawnedEdgesArray)
    {
        if (Edge)
        {
            for (AHexVertex* Vertex : Edge->AdjacentVertices)
            {
                if (Vertex)
                {
                    for (AHexVertex* AdjVertex : Vertex->AdjacentVertices)
                    {
                        if (AdjVertex && AdjVertex != Vertex)
                        {
                            uint64 PtrA = (uint64)Vertex;
                            uint64 PtrB = (uint64)AdjVertex;
                            if (PtrA > PtrB)
                            {
                                Swap(PtrA, PtrB);
                            }
                            FString EdgeKey = FString::Printf(TEXT("%llu_%llu"), PtrA, PtrB);
                            if (SpawnedEdges.Contains(EdgeKey))
                            {
                                AAHexEdge* AdjEdge = SpawnedEdges[EdgeKey];
                                if (AdjEdge && AdjEdge != Edge && !Edge->AdjacentEdges.Contains(AdjEdge))
                                {
                                    Edge->AdjacentEdges.Add(AdjEdge);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // --- Dock / Harbor placement ---
    if (!DockClass) return;

    TArray<AHexVertex*> OuterVertices;
    for (auto& Pair : SpawnedVertices)
    {
        AHexVertex* Vertex = Pair.Value;
        if (Vertex && Vertex->AdjacentHexes.Num() < 3)
        {
            OuterVertices.Add(Vertex);
        }
    }

    if (OuterVertices.Num() == 0) return;

    OuterVertices.Sort([](const AHexVertex& A, const AHexVertex& B)
    {
        float AngleA = FMath::Atan2(A.GetActorLocation().Y, A.GetActorLocation().X);
        float AngleB = FMath::Atan2(B.GetActorLocation().Y, B.GetActorLocation().X);
        return AngleA < AngleB;
    });

    static const TArray<EDockType> FixedDockTypes = {
        EDockType::Generic,
        EDockType::Ore,
        EDockType::Generic,
        EDockType::Wheat,
        EDockType::Generic,
        EDockType::Sheep,
        EDockType::Generic,
        EDockType::Wood,
        EDockType::Brick
    };

    int32 NumDocks = FMath::Min(FixedDockTypes.Num(), OuterVertices.Num());
    float Step = (float)OuterVertices.Num() / NumDocks;

    for (int32 i = 0; i < NumDocks; i++)
    {
        int32 VertexIndex = FMath::RoundToInt(i * Step);
        VertexIndex = FMath::Clamp(VertexIndex, 0, OuterVertices.Num() - 1);
        AHexVertex* DockVertex = OuterVertices[VertexIndex];

        FVector VertexLocation = DockVertex->GetActorLocation();
        FVector Direction = VertexLocation.GetSafeNormal();
        if (Direction.IsNearlyZero()) Direction = FVector(1.f, 0.f, 0.f);
        FVector DockLocation = VertexLocation + Direction * (HexSize * 0.6f);
        DockLocation.Z = VertexLocation.Z;

        ADock* NewDock = GetWorld()->SpawnActor<ADock>(DockClass, DockLocation, FRotator::ZeroRotator);
        if (NewDock)
        {
            NewDock->DockType = FixedDockTypes[i];
            NewDock->AssociatedVertex = DockVertex;
            Docks.Add(NewDock);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Spawned %d docks"), Docks.Num());
}

