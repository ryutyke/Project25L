#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "GameData/DSActorDataAsset.h"
#include "GameData/DSEnums.h"

// Game

// UHT
#include "DSCharacterDataAsset.generated.h"

class UTexture2D;

UCLASS()
class PROJECT25L_API UDSCharacterDataAsset : public UDSActorDataAsset
{
	GENERATED_BODY()
	
public:
	virtual uint32 GetKey() override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Type")
	ECharacterType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Name")
	FText PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Icon")
	TSoftObjectPtr<UTexture2D> PlayerIcon;
};
