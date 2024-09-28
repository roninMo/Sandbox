// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/Weapons/Armament.h"

AArmament::AArmament(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

}


void AArmament::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


void AArmament::BeginPlay()
{
	Super::BeginPlay();
	
}
