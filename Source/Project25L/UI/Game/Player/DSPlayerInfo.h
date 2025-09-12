#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSPlayerInfo.generated.h"

class UDSHPBar;
class UTextBlock;
class UImage;

UCLASS()
class PROJECT25L_API UDSPlayerInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UDSPlayerInfo(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
public:
	void SetText(float CurrentHP, float MaxHP);

	UDSHPBar* GetHPBar() { return ProgressBar_HP; }
	//닉네임을 업데이트 하는 함수
	void InitPlayerSetting(ECharacterType CharacterType, FText PlayerName);
protected:
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSHPBar> ProgressBar_HP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_PlayerIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_PlayerName;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_HP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_MaxHP;

};
