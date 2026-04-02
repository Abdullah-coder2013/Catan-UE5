#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CatanSharedValues.h"
#include "Dock.generated.h"

class AHexVertex;

UCLASS()
class CATANMEGAPROJECT_API ADock : public AActor
{
    GENERATED_BODY()

public:
    ADock();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dock")
    EDockType DockType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dock")
    AHexVertex* AssociatedVertex;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dock Components")
    UStaticMeshComponent* DockMesh;

    EBankTradeMethods GetBankTradeMethod() const;

    bool IsOwnedByPlayer(EPlayerColor PlayerColor) const;

protected:
    virtual void BeginPlay() override;
};
