// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Data/Interfaces/PeripheryObject/PeripheryObjectInterface.h"

// Add default functionality here for any IPeripheryObjectInterface functions that are not pure virtual.
void IPeripheryObjectInterface::WithinPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
}

void IPeripheryObjectInterface::OutsideOfPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
}

void IPeripheryObjectInterface::WithinPlayerConePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
}

void IPeripheryObjectInterface::OutsideOfConePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
}

void IPeripheryObjectInterface::WithinPlayerTracePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
}

void IPeripheryObjectInterface::OutsideOfPlayerTracePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
}
