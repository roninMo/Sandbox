// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveComponent_Npc.h"

void USaveComponent_Npc::SetNetAndPlatformId()
{
	NetId = -1;
	PlatformId = GetOwner() ? GetOwner()->GetName() : FString("Null");
}
