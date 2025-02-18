// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveComponent_Character.h"


FString USaveComponent_Character::GetSaveSlotIdReference(const ESaveType Saving) const
{
	// return Super::GetSaveIdReference(Saving);
	return GetPlayerNetId().ToString()
		.Append(FString("_"))
		.Append(GetSaveTypeName(Saving)
	);
}


FName USaveComponent_Character::GetPlayerNetId() const
{
	// Super::GetPlayerNetId(); // TODO: Retrieve steam / console account references for the player's Network id
	FString PlatformId = FGenericPlatformMisc::GetLoginId();

	// int32 NetId = 0; 
	// const UNetDriver* NetDriver = GetWorld()->GetNetDriver();
	// if (NetDriver && NetDriver->GuidCache.Get())
	// {
	// 	FNetGUIDCache* NetworkGuids = NetDriver->GuidCache.Get();
	// 	if (NetworkGuids->NetGUIDLookup.Contains(GetOuter())) NetId = NetworkGuids->NetGUIDLookup[GetOuter()].Value;
	// 	// UE_LOGFMT(LogTemp, Log, "{0} network guid: {1}, net id: {2}", *GetName(), *NetworkGuids->NetGUIDLookup[this].ToString(), SaveData.NetId);
	// }

	return FName(PlatformId);
}
