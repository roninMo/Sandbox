// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveComponent_Character.h"

#include "Engine/PackageMapClient.h"


void USaveComponent_Character::SetNetAndPlatformId()
{
	// Net Id
	const UNetDriver* NetDriver = GetWorld()->GetNetDriver();
	if (NetDriver && NetDriver->GuidCache.Get())
	{
		FNetGUIDCache* NetworkGuids = NetDriver->GuidCache.Get();
		if (NetworkGuids->NetGUIDLookup.Contains(GetOuter())) NetId = NetworkGuids->NetGUIDLookup[GetOuter()].Value;
		// UE_LOGFMT(LogTemp, Log, "{0} network guid: {1}, net id: {2}", *GetName(), *NetworkGuids->NetGUIDLookup[this].ToString(), SaveData.NetId);
	}

	// Platform Id -> TODO: Use the console/steam account Id here
	PlatformId = FGenericPlatformMisc::GetLoginId();
}
