//Default
#include "Player/DSCheatManager.h"

//UE
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"

//Game
#include "DSLogChannels.h"
#include "Player/DSPlayerController.h"
#include "Components/DSPlayerInputComponent.h"
#include "Character/Characters/DSCharacter.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "Skill/Base/DSSkillBase.h"
#include "Skill/Base/DSTestSkill.h"
#include "Components/DSStatComponent.h"

void UDSCheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	DS_LOG(DSNetLog, Display, TEXT("%s"), *TextToOutput);
#endif
}

void UDSCheatManager::Cheat(const FString& Msg)
{
#if USING_CHEAT_MANAGER
	ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController());

	if (IsValid(PlayerController))
	{
		PlayerController->ServerRPC_Cheat(Msg.Left(128));
	}
#endif
}

void UDSCheatManager::CheatAll(const FString& Msg)
{
#if USING_CHEAT_MANAGER
	ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController());

	if (IsValid(PlayerController))
	{
		PlayerController->ServerRPC_CheatAll(Msg.Left(128));
	}
#endif
}

void UDSCheatManager::CrouchMode(ECrouchMode CrouchMode)
{
#if USING_CHEAT_MANAGER
	ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController());
	if (IsValid(PlayerController))
	{
		//UDSPlayerInputComponent* PlayerInputComponent = PlayerController->GetPlayerInputComponent();
		//if (IsValid(PlayerInputComponent))
		//{
		//	PlayerInputComponent->SetCrounchMode(CrouchMode);
		//}
	}
	
#endif
}


void UDSCheatManager::AddSkill(const int32 InputID)
{
#if USING_CHEAT_MANAGER
	DS_LOG(DSSkillLog, Warning, TEXT("AddSkill"));

	if (ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController()))
	{
		if (ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>())
		{
			if (PlayerController->GetNetMode() == NM_Client)
			{
				PlayerController->ServerRPC_Cheat(FString::Printf(TEXT("AddSkill %d"), InputID));
				return;
			}

			Character->AddSkill(InputID);
		}
	}
#endif
}

void UDSCheatManager::ActivateSKill(const int32 InputID)
{
#if USING_CHEAT_MANAGER
	DS_LOG(DSSkillLog, Warning, TEXT("ActivateSKill"));

	if (ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController()))
	{
		if (ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>())
		{
			if (UDSSkillControlComponent* SkillControlComponent = Character->GetSkillControlComponent())
			{
				if (FDSSkillSpec* SkillSpec = SkillControlComponent->FindSkillSpecFromInputID(InputID))
				{
					if (PlayerController->GetNetMode() == NM_Client)
					{
						// 클라이언트일 경우 자동으로 서버에 치트 명령어를 전송
						PlayerController->ServerRPC_Cheat(FString::Printf(TEXT("ActivateSKill %d"), InputID));
						return;
					}
					SkillControlComponent->TryActivateSkill(SkillSpec->Handle);
				}
			}
		}
	}
	#endif
}

void UDSCheatManager::CancelSKill(const int32 InputID)
{
#if USING_CHEAT_MANAGER
	DS_LOG(DSSkillLog, Warning, TEXT("CancelSKill"));

	if (ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController()))
	{
		if (ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>())
		{
			if (UDSSkillControlComponent* SkillControlComponent = Character->GetSkillControlComponent())
			{
				if (FDSSkillSpec* SkillSpec = SkillControlComponent->FindSkillSpecFromInputID(InputID))
				{
					if (PlayerController->GetNetMode() == NM_Client)
					{
						// 클라이언트일 경우 자동으로 서버에 치트 명령어를 전송
						PlayerController->ServerRPC_Cheat(FString::Printf(TEXT("CancelSKill %d"), InputID));
						return;
					}
					SkillControlComponent->CancelSkillSpec(*SkillSpec, nullptr);
				}
			}
		}
	}
	#endif
}

void UDSCheatManager::RemoveSKill(const int32 InputID)
{
#if USING_CHEAT_MANAGER
	DS_LOG(DSSkillLog, Warning, TEXT("RemoveSKill"));

	if (ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController()))
	{
		if (ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>())
		{
			if (UDSSkillControlComponent* SkillControlComponent = Character->GetSkillControlComponent())
			{
				if (FDSSkillSpec* SkillSpec = SkillControlComponent->FindSkillSpecFromInputID(InputID))
				{
					if (PlayerController->GetNetMode() == NM_Client)
					{
						// 클라이언트일 경우 자동으로 서버에 치트 명령어를 전송
						PlayerController->ServerRPC_Cheat(FString::Printf(TEXT("RemoveSKill %d"), InputID));
						return;
					}
					SkillControlComponent->OnRemoveSkill(*SkillSpec);
				}
			}
		}
	}
	#endif
}

// 쿨타임 찾기
void UDSCheatManager::DisplaySkillCooltime(const int32 InputID)
{
#if USING_CHEAT_MANAGER
	DS_LOG(DSSkillLog, Warning, TEXT("DisplaySkillCooltime"));

	if (ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController()))
	{
		if (ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>())
		{
			if (UDSSkillControlComponent* SkillControlComponent = Character->GetSkillControlComponent())
			{
				if (FDSSkillSpec* SkillSpec = SkillControlComponent->FindSkillSpecFromInputID(InputID))
				{
					if (PlayerController->GetNetMode() == NM_Client)
					{
						// 클라이언트일 경우 자동으로 서버에 치트 명령어를 전송
						PlayerController->ServerRPC_Cheat(FString::Printf(TEXT("DisplaySkillCooltime %d"), InputID));
						return;
					}
					for (auto Instance : SkillSpec->GetSkillInstances())
					{
						if (IsValid(Instance))
						{
							if (Instance->IsActive())
							{
								float TimeRemaining, Duration;
								Instance->GetCooldownTimeRemainingAndDuration(SkillSpec->Handle, SkillControlComponent->SkillActorInfo.Get(), TimeRemaining, Duration);
								DS_LOG(DSSkillLog, Warning, TEXT("DisplaySkillCooltime - TimeRemaining[%f],  Duration[%f]"), TimeRemaining, Duration);
							}
						}
					}
				}
			}
		}
	}
	#endif
}

void UDSCheatManager::DisplaySkillCooltimeFromTag(const FGameplayTag SkillTag)
{
	#if USING_CHEAT_MANAGER
	DS_LOG(DSSkillLog, Warning, TEXT("DisplaySkillCooltime"));

	if (ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController()))
	{
		if (ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>())
		{
			if (UDSSkillControlComponent* SkillControlComponent = Character->GetSkillControlComponent())
			{
				if (FDSSkillSpec* SkillSpec = SkillControlComponent->FindSkillSpecFromTag(SkillTag))
				{
					if (PlayerController->GetNetMode() == NM_Client)
					{
						// 클라이언트일 경우 자동으로 서버에 치트 명령어를 전송
						PlayerController->ServerRPC_Cheat(FString::Printf(TEXT("DisplaySkillCooltime %s"), *SkillTag.ToString()));
						return;
					}
					for (auto Instance : SkillSpec->GetSkillInstances())
					{
						if (IsValid(Instance))
						{
							if (Instance->IsActive())
							{
								float TimeRemaining, Duration;
								Instance->GetCooldownTimeRemainingAndDuration(SkillSpec->Handle, SkillControlComponent->SkillActorInfo.Get(), TimeRemaining, Duration);
								DS_LOG(DSSkillLog, Warning, TEXT("DisplaySkillCooltime - TimeRemaining[%f],  Duration[%f]"), TimeRemaining, Duration);
							}
						}
					}
				}
			}
		}
	}
	#endif
}

void UDSCheatManager::StoreItems(int32 PlayerID, int32 ItemID, int32 ItemCount)
{
#if USING_CHEAT_MANAGER

	UWorld* World = GetWorld();

	check(World);

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (PlayerID == 0)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (IsValid(PlayerController))
			{
				ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>();

				if (IsValid(Character))
				{
					//아이템 저장할 수 있도록 함.
					for (int i = 0; i < ItemCount; ++i)
					{
						bool bIsItemStored = Character->StoreItem(ItemID);
						if (bIsItemStored)
						{
							DS_LOG(DSItemLog, Warning, TEXT("Store Item"));
						}
					}
				}
			}
			break;
		}

		PlayerID--;

	}

#endif
}

void UDSCheatManager::UseItems(int32 PlayerID, int32 ItemIdx, int32 ItemCount)
{
#if USING_CHEAT_MANAGER

	UWorld* World = GetWorld();

	check(World);

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (PlayerID == 0)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (IsValid(PlayerController))
			{
				ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>();
				
				if (IsValid(Character))
				{
					//인벤토리에 넣지 않고 적용할 수 있는 치트 로직 구현
					Character->ServerRPC_UseItem(ItemIdx,ItemCount);
					DS_LOG(DSItemLog, Warning, TEXT("UseItem"));
				}
			}
			break;
		}

		PlayerID--;

	}

#endif
}

void UDSCheatManager::PrintPlayerItems(int32 PlayerID)
{
#if USING_CHEAT_MANAGER

	UWorld* World = GetWorld();

	check(World);

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (PlayerID == 0)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (IsValid(PlayerController))
			{
				ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>();

				if (IsValid(Character))
				{
					//인벤토리에 넣지 않고 적용할 수 있는 치트 로직 구현
					Character->ServerRPC_PrintItem();
				}
			}
			break;
		}

		PlayerID--;

	}

#endif
}

void UDSCheatManager::ApplyBuff(EDSStatType InStatType, EOperationType InOperationType, float InBuffValue, float InDuration)
{
#if USING_CHEAT_MANAGER
	if (ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController()))
	{
		if (ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>())
		{
			if (UDSStatComponent* StatComponent = Character->GetStatComponent())
			{
				StatComponent->ApplyBuff(InStatType, InOperationType, InBuffValue, InDuration);
			}
		}
	}
#endif
}

void UDSCheatManager::ModifyHP(float Delta)
{
#if USING_CHEAT_MANAGER
	if (ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOuterAPlayerController()))
	{
		if (ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>())
		{
			if (UDSStatComponent* StatComponent = Character->GetStatComponent())
			{
				StatComponent->ModifyHP(Delta);
			}
		}
	}
#endif
}

