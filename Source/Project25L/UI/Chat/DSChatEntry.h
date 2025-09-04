#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/IUserObjectListEntry.h"

// Game
#include "UI/Base/DSUserWidget.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSChatEntry.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PROJECT25L_API UDSChatEntry : public UDSUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatContentText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CharacterIcon;

};
