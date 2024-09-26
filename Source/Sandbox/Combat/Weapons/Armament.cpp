// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/Weapons/Armament.h"

AArmament::AArmament(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

}


void AArmament::BeginPlay()
{
	Super::BeginPlay();
	
}


void AArmament::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

