
//Defualt
#include "Skill/Base/DSSkillSpec.h"

//UE
#include "UObject/NoExportTypes.h"
#include "Engine/NetSerialization.h"
#include "Net/Serialization/FastArraySerializer.h"

//Game
#include "GameData/DSEnums.h"
#include "DSSkillBase.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"

// ---------------------------------------------------- FDSSkillActivationInfo ----------------------------------------------------
FDSSkillActivationInfo::FDSSkillActivationInfo(AActor* InActor)
	: bCanBeEndedByOtherInstance(false)
{
	ActivationMode = (InActor->GetLocalRole() == ROLE_Authority ? ESkillActivationMode::Authority : ESkillActivationMode::NonAuthority);
}

void FDSSkillActivationInfo::SetActivationConfirmed()
{
	ActivationMode = ESkillActivationMode::Confirmed;
	bCanBeEndedByOtherInstance = true;
}

void FDSSkillActivationInfo::SetActivationRejected()
{
	ActivationMode = ESkillActivationMode::Rejected;
}

// ---------------------------------------------------- FDSSkillSpecHandle ----------------------------------------------------
void FDSSkillSpecHandle::GenerateNewHandle()
{
	static int32 GHandle = 1;
	Handle = GHandle++;
}


// ---------------------------------------------------- FDSSkillSpec ----------------------------------------------------
FDSSkillSpec::FDSSkillSpec(TSubclassOf<UDSSkillBase> InSkillClass,ESkillType InType,  FGameplayTag InSkillTag, int32 InInputID, UObject* InSourceObject)
	: Skill(InSkillClass ? InSkillClass.GetDefaultObject() : nullptr)
	, SkillTag(InSkillTag)
	, SkillType(InType)
	, InputID(InInputID)
	, SourceObject(InSourceObject)
	, ActiveCount(0)
	, InputPressed(false)
	, RemoveAfterActivation(false)
	, PendingRemove(false)
	, bActivateOnce(false)
{
	Handle.GenerateNewHandle();
}

FDSSkillSpec::FDSSkillSpec(UDSSkillBase* InSkill, ESkillType InType, FGameplayTag InSkillTag, int32 InInputID, UObject* InSourceObject)
	: Skill(InSkill)
	, SkillTag(InSkillTag)
	, SkillType(InType)
	, InputID(InInputID)
	, SourceObject(InSourceObject)
	, ActiveCount(0)
	, InputPressed(false)
	, RemoveAfterActivation(false)
	, PendingRemove(false)
	, bActivateOnce(false)
{
	Handle.GenerateNewHandle();
}

UDSSkillBase* FDSSkillSpec::GetPrimaryInstance() const
{
	if(false == IsValid(Skill))
	{
		return nullptr;
	}

	if (ESkillInstancingPolicy::InstancedPerActor == Skill->GetInstancingPolicy())
	{
		if (NonReplicatedInstances.Num() > 0)
		{
			return NonReplicatedInstances[0];
		}

		if (ReplicatedInstances.Num() > 0)
		{
			return ReplicatedInstances[0];
		}
	}
	return nullptr;
}

bool FDSSkillSpec::ShouldReplicateSkillSpec() const
{
	if (false == IsValid(Skill))
	{
		return false;
	}

	if (Skill->ShouldReplicateSkillSpec(*this))
	{
		return true;
	}

	return false;
}

bool FDSSkillSpec::IsActive() const
{
	return Skill != nullptr && ActiveCount > 0;
}

void FDSSkillSpec::PreReplicatedRemove(const FDSSkillSpecContainer& InArraySerializer)
{
	if (true == IsValid(InArraySerializer.Owner))
	{
		// UE_LOG(LogTemp, Warning, TEXT("PreReplicatedRemove called for Skill: %s, Owner : %s"), *GetDebugString(), *InArraySerializer.Owner->GetName());
		InArraySerializer.Owner->OnRemoveSkill(*this);
	}
}

void FDSSkillSpec::PostReplicatedAdd(const FDSSkillSpecContainer& InArraySerializer)
{
	if (true == IsValid(InArraySerializer.Owner))
	{
		// DS_LOG(DSSkillLog, Log , TEXT("PostReplicatedAdd called for Skill: %s, Owner : %s"), *GetDebugString(), *InArraySerializer.Owner->GetName());
		InArraySerializer.Owner->OnAddSkill(*this);
	}
}

FString FDSSkillSpec::GetDebugString()
{
	return FString::Printf(TEXT("(%s)"), *GetNameSafe(Skill));
}


// ---------------------------------------------------- FDSSkillSpecContainer ----------------------------------------------------

void FDSSkillSpecContainer::RegisterWithOwner(UDSSkillControlComponent* InOwner)
{
	Owner = InOwner;
}
