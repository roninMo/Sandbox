// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Characters/Npc.h"

#include "Sandbox/AI/Controllers/AIControllerBase.h"
#include "Components/SphereComponent.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"


ANpc::ANpc(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Periphery Radius
	PeripheryRadius = CreateDefaultSubobject<USphereComponent>(TEXT("Periphery Radius"));
	PeripheryRadius->SetupAttachment(RootComponent);
	PeripheryRadius->InitSphereRadius(1340.0f);
	PeripheryRadius->SetHiddenInGame(true);
	
	// Inventory
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetIsReplicated(false);

	AIControllerClass = AAIControllerBase::StaticClass();
}




#pragma region Character Initialization
void ANpc::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// InitCharacterGlobals(CharacterGlobals);
	InitCharacterComponents(true);	
	InitAbilitySystemAndAttributes(true);
	InitCharacterInformation();
	// BP_InitCharacterInformation();
}

void ANpc::InitCharacterGlobals(UDataAsset* Data)
{
	Super::InitCharacterGlobals(Data);
}

void ANpc::InitCharacterComponents(const bool bCalledFromPossessedBy)
{
	Super::InitCharacterComponents(bCalledFromPossessedBy);
	AIController = AIController ? AIController : GetController<AAIControllerBase>();
}

void ANpc::InitAbilitySystemAndAttributes(const bool bCalledFromPossessedBy)
{
	Super::InitAbilitySystemAndAttributes(bCalledFromPossessedBy);
}

void ANpc::InitCharacterInformation()
{
	Super::InitCharacterInformation();
	
	// SetCharacterMontages();
	// if (BaseAnimInstance) BaseAnimInstance->GetCharacterInformation();
	
	// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// GetCharacterMovement()->GravityScale = PreservedGravity;

	// Add the character information
	SetNpcInformation(GetNPCCharacterInformationFromTable(Id));
	
	// Load the inventory items
	for (const FS_Item ItemInformation : CharacterInformation.InventoryItems)
	{
		AddSavedItemToInventory(ItemInformation);
	}
	
	// Update the npc stats
	// BaseAbilitySystem->SetCharacterAbilitiesAndPassiveEffects(CharacterAbilities, CharacterInformation.Statuses);
	// for (const TSubclassOf<UGameplayEffect> Effect : InitialEffectsState) ApplyEffectToSelf(Effect);
	// if (CharacterInformation.PrimaryAttributes) ApplyEffectToSelf(CharacterInformation.PrimaryAttributes);
	// if (SecondaryAttributes) ApplyEffectToSelf(SecondaryAttributes);
	//
	// if (BaseAttributeSet) MoveSpeed = BaseAttributeSet->GetMoveSpeed();

	// Init the periphery
	if (PeripheryRadius)
	{
		PeripheryRadius->OnComponentBeginOverlap.AddDynamic(this, &ANpc::PeripheryEnterRadius);
		PeripheryRadius->OnComponentEndOverlap.AddDynamic(this, &ANpc::PeripheryExitRadius);
		PeripheryRadius->SetGenerateOverlapEvents(true);
		PeripheryRadius->SetCollisionObjectType(ECC_GameTraceChannel1);
		PeripheryRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PeripheryRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
		PeripheryRadius->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}
#pragma endregion




void ANpc::BeginPlay()
{
	Super::BeginPlay();
}




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
	Inventory->TryAddItem(Information.ItemName, nullptr, EItemType::Inv_None);
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
