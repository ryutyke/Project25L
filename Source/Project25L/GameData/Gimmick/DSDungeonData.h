#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSDungeonData.generated.h"

/**
 * @brief 던전 데이터 구조체
 *
 * 이 구조체는 던전에서 수행되어야할 퀘스트 및 보상 조건을 포함합니다.
 */

class UDataAsset;
USTRUCT(BlueprintType)
struct FDSDungeonData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	/*
	* 퀘스트 문자열
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Quest")
	FString DungeonQuest;

	/*
	* 문자열에 들어갈 파라미터 값
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Quest")
	TArray<int> Params;

	/*
	* 선행 퀘스트를 의미하며, 선행 퀘스트 번호를 깼는지를 확인 해야한다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Quest")
	int32 PreQuest;

	/*
	* 실제 퀘스트 조건문 
	* 예) N초 안에 M마리를 쓰러트려라. (N초와 M마리)에 해당하는 조건문을 가진 UDataAsset을 만들어준다. 그리고 연결
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Quest")
	TObjectPtr<UDataAsset> Quest;
};