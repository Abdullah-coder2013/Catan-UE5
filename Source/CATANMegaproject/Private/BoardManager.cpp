// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardManager.h"
#include "HexTile.h"
#include "HexVertex.h"
#include "AHexEdge.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"

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
	ComputeSmoothedElevations();
    ApplyMaterials();
    
    if (BoardTerrainClass)
    {
        ABoardTerrain* Terrain = GetWorld()->SpawnActor<ABoardTerrain>(BoardTerrainClass,FVector(0, 0, 0),  // explicit location
    FRotator::ZeroRotator);
        if (Terrain)
        {
            Terrain->GenerateTerrain(HexTiles, HexSize);
        }
    }
    SnapActorsToTerrain();
}
void ABoardManager::ApplyMaterials()
{
    for (AHexTile* Tile : HexTiles)
    {
        if (!Tile || !Tile->HexMesh) continue;

        UMaterialInstanceDynamic* MatInst = 
            UMaterialInstanceDynamic::Create(HexMaterial, this);
        
        // Pass smoothed elevation as a scalar
        MatInst->SetScalarParameterValue(
            FName("BiomeElevation"), Tile->SmoothedElevation);
        
        Tile->HexMesh->SetMaterial(0, MatInst);
    }
}

FRotator ABoardManager::GetRotationFromSurfaceNormal(const FVector& Normal, const FRotator& OriginalRotation)
{
    // Keep the original yaw (facing direction) but pitch/roll to match slope
    FRotator SlopeRotation = Normal.Rotation() - FRotator(90.f, 0.f, 0.f);
    return FRotator(SlopeRotation.Pitch, OriginalRotation.Yaw, SlopeRotation.Roll);
}

// Called every frame

FVector ABoardManager::AxialToWorld(int32 Q, int32 R) const
{
    float X = HexSize * (FMath::Sqrt(3.f) * Q + FMath::Sqrt(3.f) / 2.f * R);
    float Y = HexSize * (3.f / 2.f * R);
    return FVector(X, Y, 0.f);
}

int32 ABoardManager::DFS(AAHexEdge* CurrentEdge, AHexVertex* FromVertex, 
                          TSet<AAHexEdge*>& VisitedEdges, EPlayerColor PlayerColor) const
{
    int32 MaxLength = 0;

    for (AHexVertex* Vertex : CurrentEdge->AdjacentVertices)
    {
        if (Vertex && Vertex != FromVertex)
        {
            // Check if opponent blocks this vertex
            if (Vertex->isOccupied && Vertex->occupiedBy != PlayerColor)
                continue;

            for (AAHexEdge* AdjacentEdge : Vertex->AdjacentEdges)
            {
                if (AdjacentEdge && AdjacentEdge->isOccupied 
                    && AdjacentEdge->occupiedBy == PlayerColor 
                    && !VisitedEdges.Contains(AdjacentEdge))
                {
                    VisitedEdges.Add(AdjacentEdge);
                    int32 Length = 1 + DFS(AdjacentEdge, Vertex, VisitedEdges, PlayerColor);
                    MaxLength = FMath::Max(MaxLength, Length);
                    VisitedEdges.Remove(AdjacentEdge); // backtrack
                }
            }
        }
    }
    return MaxLength;
}

int32 ABoardManager::GetLongestRoadLengthForPlayer(EPlayerColor PlayerColor) const
{
    int32 LongestRoad = 0;

    for (AAHexEdge* Edge : SpawnedEdgesArray)
    {
        if (Edge && Edge->isOccupied && Edge->occupiedBy == PlayerColor)
        {
            for (AHexVertex* StartVertex : Edge->AdjacentVertices)
            {
                TSet<AAHexEdge*> VisitedEdges;
                VisitedEdges.Add(Edge);
                int32 Length = 1 + DFS(Edge, StartVertex, VisitedEdges, PlayerColor);
                LongestRoad = FMath::Max(LongestRoad, Length);
            }
        }
    }
    return LongestRoad;
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
                    FRotator HexRotation = FRotator(0.f, 30.0f, 0.f);
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

void ABoardManager::SnapActorsToTerrain()
{
    // Snap vertices
    for (auto& Pair : SpawnedVertices)
    {
        AHexVertex* Vertex = Pair.Value;
        if (!Vertex) continue;

        FVector Start = Vertex->GetActorLocation() + FVector(0, 0, 5000.f);
        FVector End = Vertex->GetActorLocation() - FVector(0, 0, 5000.f);

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Vertex);

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            FVector NewLoc = Vertex->GetActorLocation();
            NewLoc.Z = Hit.ImpactPoint.Z + 2.f; // slight offset so it sits on top
            Vertex->SetActorLocation(NewLoc);
        }
        // For vertices
        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            FVector NewLoc = Vertex->GetActorLocation();
            NewLoc.Z = Hit.ImpactPoint.Z + 2.f;
            Vertex->SetActorLocation(NewLoc);
    
            // Align to slope
            Vertex->SetActorRotation(
                GetRotationFromSurfaceNormal(Hit.ImpactNormal, Vertex->GetActorRotation())
            );
        }
    }
    
    // Snap edges - AFTER vertices are already snapped
    for (AAHexEdge* Edge : SpawnedEdgesArray)
    {
        if (!Edge) continue;

        // Get the two snapped vertex positions
        AHexVertex* VertA = Edge->AdjacentVertices[0];
        AHexVertex* VertB = Edge->AdjacentVertices[1];

        if (!VertA || !VertB) continue;

        FVector PosA = VertA->GetActorLocation();
        FVector PosB = VertB->GetActorLocation();

        // Place edge at midpoint between the two snapped vertices
        FVector Midpoint = (PosA + PosB) / 2.f;
        Edge->SetActorLocation(Midpoint);

        // Calculate rotation from A to B in 3D — this handles slope naturally
        FVector Direction = (PosB - PosA).GetSafeNormal();
        FRotator NewRotation = Direction.Rotation();
        Edge->SetActorRotation(NewRotation);
    }
    
    // Snap Hex Tiles

    for (auto Element : HexTiles)
    {
        if (!Element) continue;
        FVector Start = Element->GetActorLocation() + FVector(0, 0, 5000.f);
        FVector End = Element->GetActorLocation() - FVector(0, 0, 5000.f);

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Element);

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            FVector NewLoc = Element->GetActorLocation();
            NewLoc.Z = Hit.ImpactPoint.Z + 2.f; // slight offset so it sits on top
            Element->SetActorLocation(NewLoc);
        }
        // For vertices
        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            FVector NewLoc = Element->GetActorLocation();
            NewLoc.Z = Hit.ImpactPoint.Z + 2.f;
            Element->SetActorLocation(NewLoc);
    
            // Align to slope
            Element->SetActorRotation(
                GetRotationFromSurfaceNormal(Hit.ImpactNormal, Element->GetActorRotation())
            );
        }
    }
}

void ABoardManager::ComputeSmoothedElevations()
{
    static const TArray<TPair<int32,int32>> AxialNeighbours = {
        {1,0},{-1,0},{0,1},{0,-1},{1,-1},{-1,1}
    };

    TMap<TPair<int32,int32>, AHexTile*> CoordMap;
    for (AHexTile* Tile : HexTiles)
        if (Tile) CoordMap.Add({Tile->Q, Tile->R}, Tile);

    // Seed SmoothedElevation from BaseElevation before any pass
    for (AHexTile* Tile : HexTiles)
        if (Tile) Tile->SmoothedElevation = Tile->BaseElevation;

    for (int32 Pass = 0; Pass < 2; Pass++)
    {
        TMap<AHexTile*, float> NewElevations;

        for (AHexTile* Tile : HexTiles)
        {
            if (!Tile) continue;

            // Strong center weight preserves original elevation dominance
            float Sum       = Tile->SmoothedElevation * 8.0f;
            float WeightSum = 8.0f;

            for (auto& Offset : AxialNeighbours)
            {
                TPair<int32,int32> Coord = {Tile->Q + Offset.Key, Tile->R + Offset.Value};
                if (AHexTile** Neighbour = CoordMap.Find(Coord))
                {
                    Sum       += (*Neighbour)->SmoothedElevation;
                    WeightSum += 1.f;
                }
            }
            // Gentle blend — preserves most of the original elevation
            float Smoothed = Sum / WeightSum;
            NewElevations.Add(Tile, FMath::Lerp(Tile->SmoothedElevation, Smoothed, 0.2f));
        }

        // Write back so the next pass picks up this pass's results
        for (AHexTile* Tile : HexTiles)
            if (Tile) Tile->SmoothedElevation = NewElevations[Tile];
    }
}

AHexTile* ABoardManager::GetHexTile(EHexType HexType)
{
    for (auto Element : HexTiles)
    {
        if (Element && Element->HexType == HexType)
        {
            return Element;
        }
    }
    return nullptr;
}

AHexTile* ABoardManager::GetRobberTile()
{
    for (auto Element : HexTiles)
    {
        if (Element && Element->bHasRobber)
        {
            return Element;
        }
    }
    return nullptr;
}

