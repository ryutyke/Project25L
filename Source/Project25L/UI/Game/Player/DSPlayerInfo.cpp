// Default
#include "UI/Game/Player/DSPlayerInfo.h"

// UE
#include "Components/Image.h"
#include "Components/TextBlock.h"

// Game
#include "GameFramework/PlayerState.h"
#include "GameData/DSCharacterDataAsset.h"
#include "System/DSGameDataSubsystem.h"
#include "Player/DSPlayerController.h"

UDSPlayerInfo::UDSPlayerInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDSPlayerInfo::NativeConstruct()
{
	Super::NativeConstruct();


}

void UDSPlayerInfo::SetText(float CurrentHP, float MaxHP)
{
	if (IsValid(Text_HP))
	{
		//정수형으로 출력한다.
		FString Text = FString::Printf(TEXT("%.0f"), CurrentHP);
		Text_HP->SetText(FText::FromString(Text));
	}

	if (IsValid(Text_MaxHP))
	{
		FString Text = FString::Printf(TEXT("%.0f"), MaxHP);
		Text_MaxHP->SetText(FText::FromString(Text));
	}
}

void UDSPlayerInfo::InitPlayerSetting(ECharacterType CharacterType, FText PlayerName)
{
	if (false == IsValid(IMG_PlayerIcon))
	{
		return;
	}

	if (false == IsValid(Text_PlayerName))
	{
		return;
	}

	//플레이어 이미지로 로드한다.
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	const UDSCharacterDataAsset* DataAsset = DataSubsystem->GetDataAssetByType<ECharacterType, UDSCharacterDataAsset>(CharacterType);

	if (IsValid(DataAsset))
	{
		IMG_PlayerIcon->SetBrushFromSoftTexture(DataAsset->PlayerIcon.LoadSynchronous());
		
		Text_PlayerName->SetText(PlayerName);
	}

}
