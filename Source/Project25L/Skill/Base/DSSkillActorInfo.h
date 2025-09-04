// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Default
#include "CoreMinimal.h"

//UHT
#include "DSSkillActorInfo.generated.h"

class UDSSkillControlComponent;
class UMovementComponent;

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECT25L_API FDSSkillActorInfo
{
	GENERATED_BODY()

	virtual ~FDSSkillActorInfo() {}

	/** 스킬을 소유한 Actor **/
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr< AActor>	SkillOwner;
	
	 /** 실제 스킬을 실행하는 Actor */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr< AActor>	SkillAvatar;
	
	  /** 스킬을 관리하는 컨트롤 컴포넌트 */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr< UDSSkillControlComponent>	SkillControlComponent;
	
	   /** 스킬을 사용하는 플레이어의 컨트롤러 */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr< APlayerController>	PlayerController;
	
	/** SkillAvatar의 Skeletal Mesh Component */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr< USkeletalMeshComponent>	SkeletalMeshComponent;
	  /** SkillAvatar의 AnimInstance */
	
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr< UAnimInstance>	AnimInstance;

	/** Movement component of the avatar actor. Often null */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr< UMovementComponent>	MovementComponent;

	/** The linked Anim Instance that this component will play montages in. Use NAME_None for the main anim instance. */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	FName AffectedAnimInstanceTag; 
	
	/** Accessor to get the affected anim instance from the SkeletalMeshComponent */
	UAnimInstance* GetAnimInstance() const;
	
	/** Returns true if this actor is locally controlled. Only true for players on the client that owns them (differs from APawn::IsLocallyControlled which requires a Controller) */
	bool IsLocallyControlled() const;
	
	/** Returns true if this actor has a PlayerController that is locally controlled. */
	bool IsLocallyControlledPlayer() const;
	
	/** Returns true if the owning actor has net authority */
	bool IsNetAuthority() const;
	
	/** Initializes the info from an owning actor. Will set both owner and avatar */
	virtual void InitFromActor( AActor* InOwnerActor,  AActor* InAvatarActor,  UDSSkillControlComponent* InSkillControlComponent);
	
	/** Sets a new avatar actor, keeps same owner and ability system component */
	virtual void SetCharacterActor( AActor *NewCharacterActor);
	
	/** Clears out any actor info, both owner and avatar */
	virtual void ClearActorInfo();
};