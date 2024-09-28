// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/World/Props/CharacterAttachment.h"


ACharacterAttachment::ACharacterAttachment()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	AActor::SetReplicateMovement(true);

	Object = CreateDefaultSubobject<UStaticMeshComponent>("Attachment");
	SetRootComponent(Object);
	Object->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Holster->SetHidden(true);
}


void ACharacterAttachment::BeginPlay()
{
	Super::BeginPlay();
	
}