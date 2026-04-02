#include "Dock.h"
#include "HexVertex.h"

ADock::ADock()
{
    PrimaryActorTick.bCanEverTick = false;
    DockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DockMesh"));
    RootComponent = DockMesh;
    DockType = EDockType::Generic;
    AssociatedVertex = nullptr;
}

void ADock::BeginPlay()
{
    Super::BeginPlay();
}

EBankTradeMethods ADock::GetBankTradeMethod() const
{
    switch (DockType)
    {
    case EDockType::Generic: return EBankTradeMethods::Dock;
    case EDockType::Wood:    return EBankTradeMethods::Wood;
    case EDockType::Brick:   return EBankTradeMethods::Brick;
    case EDockType::Sheep:   return EBankTradeMethods::Sheep;
    case EDockType::Wheat:   return EBankTradeMethods::Wheat;
    case EDockType::Ore:     return EBankTradeMethods::Ore;
    default:                 return EBankTradeMethods::None;
    }
}

bool ADock::IsOwnedByPlayer(EPlayerColor PlayerColor) const
{
    if (!AssociatedVertex) return false;
    return AssociatedVertex->isOccupied && AssociatedVertex->occupiedBy == PlayerColor;
}
