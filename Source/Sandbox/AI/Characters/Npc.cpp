// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Characters/Npc.h"

#include "Sandbox/AI/Controllers/AIControllerBase.h"
#include "Components/SphereComponent.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Data/Enums/CollisionChannels.h"


void ANpc::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


ANpc::ANpc(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Periphery Radius
	PeripheryRadius = CreateDefaultSubobject<USphereComponent>(TEXT("Periphery Radius"));
	PeripheryRadius->SetupAttachment(RootComponent);
	PeripheryRadius->InitSphereRadius(1340.0f);
	PeripheryRadius->SetHiddenInGame(true);
	
	PeripheryRadius->SetGenerateOverlapEvents(true);
	PeripheryRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
	PeripheryRadius->SetCollisionObjectType(ECC_PeripheryComponents);
	PeripheryRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PeripheryRadius->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	// Inventory
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetIsReplicated(false);

	AIControllerClass = AAIControllerBase::StaticClass();
}




#pragma region Character Initialization
void ANpc::BeginPlay()
{
	Super::BeginPlay();
}


void ANpc::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}


void ANpc::OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	// Super::OnInitAbilityActorInfo(InOwnerActor, InAvatarActor);
	AIController = AIController ? AIController : GetController<AAIControllerBase>();

	SetCharacterMontages();

	// Add the character information
	SetNpcInformation(GetNPCCharacterInformationFromTable(Id));
	
	// Load the inventory items
	if (Inventory)
	{
		for (const FS_Item ItemInformation : CharacterInformation.InventoryItems)
		{
			AddSavedItemToInventory(ItemInformation);
		}
	
		Inventory->SetPlayerId();
	}
	
	// Init the periphery
	PeripheryRadius->OnComponentBeginOverlap.RemoveAll(this);
	PeripheryRadius->OnComponentEndOverlap.RemoveAll(this);
	PeripheryRadius->OnComponentBeginOverlap.AddDynamic(this, &ANpc::PeripheryEnterRadius);
	PeripheryRadius->OnComponentEndOverlap.AddDynamic(this, &ANpc::PeripheryExitRadius);

	// Blueprint function event
	BP_OnInitAbilityActorInfo();
	
}
#pragma endregion






#pragma region Periphery functions
void ANpc::PeripheryEnterRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (!Character || this == Character) return;
	
	CharactersInPeriphery.AddUnique(Character);
}


void ANpc::PeripheryExitRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (!Character || this == Character) return;

	CharactersInPeriphery.Remove(Character);
}

UAISense_Sight::EVisibilityResult ANpc::CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation,
	int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength,
	int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	return Super::CanBeSeenFrom(Context, OutSeenLocation, OutNumberOfLoSChecksPerformed,
	                            OutNumberOfAsyncLosCheckRequested,
	                            OutSightStrength, UserData, Delegate);
}


TArray<ACharacterBase*>& ANpc::GetCharactersInPeriphery()
{
	return CharactersInPeriphery;
}
#pragma region Periphery functions




#pragma region Utility
F_NpcInformation& ANpc::GetNpcInformation()
{
	return CharacterInformation;
}


void ANpc::SetNpcInformation(const F_NpcInformation& Information)
{
	CharacterInformation = Information;
	Id = CharacterInformation.NpcId;
}


F_AISenseConfigurations& ANpc::GetSenseConfig()
{
	return CharacterInformation.SenseConfiguration;
}


FName ANpc::GetId() const
{
	if (!Id.IsNone()) return CharacterInformation.NpcId;
	return Id;
}


void ANpc::AddSavedItemToInventory(FS_Item Information)
{
	if (!Inventory || !Information.IsValid()) return;
	Inventory->Execute_TryAddItem(Inventory, FGuid::NewGuid(), Information.ItemName, nullptr, EItemType::Inv_None);
}


F_NpcInformation ANpc::GetNPCCharacterInformationFromTable(FName RowName)
{
	if (NPCInformationTable)
	{
		const F_Table_NpcInformation* Information = NPCInformationTable->FindRow<F_Table_NpcInformation>(RowName, TEXT("Npc Character Data Context"));
		if (Information)
		{
			return Information->CharacterInformation;
		}
		
		UE_LOGFMT(LogTemp, Error, "{0}: {1} Did not find the npc character information {2}!", *UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetName(), *RowName.ToString());
	}

	return F_NpcInformation();
}
#pragma endregion
