#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSArmedCharacter.generated.h"


UENUM(BlueprintType)
enum class EWeaponSocketType : uint8
{
	Stow UMETA(DisplayName = "Store weapons"),
	Equipped UMETA(DisplayName = "Equip Weapons")
};

class ADSWeapon;

UCLASS()
class PROJECT25L_API ADSArmedCharacter : public ADSCharacter
{
	GENERATED_BODY()
	
public:
	ADSArmedCharacter(const FObjectInitializer& ObjectInitializer);

public:
/*Getter 함수*/
	const ADSWeapon* GetWeapon() const { return Weapon; }
	ADSWeapon* GetWeapon() { return Weapon; }
	bool GetIsEquipped() { return bIsEquipped; }
	const FTransform& GetWeaponRelativeTransform() {return WeaponRelativeTransform; }
	const FName& GetWeaponSocketName(EWeaponSocketType SocketType) {return SocketName[SocketType]; }

public:
/*무기를 장착하고, 무기와 관련된 애니메이션 몽타주를 실행하는 함수*/
	void PlayWeaponActionMontage(EWeaponState WeaponState);

	void MoveEquip();

	void PlayAnimation(EWeaponState WeaponState);
	
/*RPC*/
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_PlayAnimation(EWeaponState WeaponState);
	UFUNCTION(Server, Reliable)
	void ServerRPC_PlayWeaponAction(EWeaponState EquipState);

	UFUNCTION(Client, Reliable)
	void ClientRPC_StopAnimation(EWeaponState WeaponState);

	UFUNCTION(Client, Reliable)
	void ClientRPC_SyncComboEndMontage(EWeaponState WeaponState, float AnimPos, double InServerWorldTime);
protected:

	UFUNCTION(Client, Reliable)
	void ClientRPC_PlayWeaponAction(ADSArmedCharacter* Character, EWeaponState EquipState);
	
protected:
	void LoadWeaponMontage();
	void LoadWeapon();

public:
	virtual void PreAttackHitCheck(UAnimMontage* Anim) override;
	virtual void AttackHitCheck(const ESkillType& SkillTag) override;
	virtual void PostAttackHitCheck() override;

protected:
/*Unreal Engine 기본 함수*/
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Transient)
	TMap<EWeaponState,TObjectPtr<UAnimMontage>> WeaponMontages;

	UPROPERTY(Transient)
	uint8 bIsEquipped : 1;

	UPROPERTY(Transient, Replicated)
	TObjectPtr<ADSWeapon> Weapon;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Weapon")
	TMap<EWeaponSocketType, FName> SocketName;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Weapon")
	FTransform WeaponRelativeTransform;

};
