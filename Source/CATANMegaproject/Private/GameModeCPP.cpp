// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeCPP.h"
#include "BoardManager.h"

// Beginplay
void AGameModeCPP::BeginPlay()
{
    Super::BeginPlay();
    // Spawn the BoardManager
    if (BoardManagerClass)
    {
        GetWorld()->SpawnActor<ABoardManager>(BoardManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
    }
}