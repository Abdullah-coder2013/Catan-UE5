// Fill out your copyright notice in the Description page of Project Settings.


#include "Robber.h"

// Sets default values
ARobber::ARobber()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobberMesh"));
	RootComponent = StaticMesh;
}

// Called when the game starts or when spawned
void ARobber::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARobber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
