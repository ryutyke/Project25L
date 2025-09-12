#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/NoExportTypes.h"

// UHT
#include "DSGameUtils.generated.h"

class ADSCharacter;
class APlayerController;
class AActor;

UCLASS()
class PROJECT25L_API UDSGameUtils : public UObject
{
	GENERATED_BODY()

public:
	UDSGameUtils();

	static UDSGameUtils* Get(UObject* Object);
	
public:
	/**
	* @details 액터가 캐릭터의 시야범위에 드는지를 확인해주는 함수
	* @param[in] Character 실제 캐릭터
	* @param[in] Target 목표 액터
	* @param[in] Angle 캐릭터 각도 (파밍 각도, 스킬 각도 등 각도는 달라질 수 있기 때문에 파라미터로 넘겨준다. 
	* @return bool 시야 범위 내 있는지 결정하는 함수
	*/
	static bool IsWithinCharacterFOV(const ACharacter* Character,const AActor* Target, float AnchorAngle);
	static float GetSpreadOffset();
	
	static bool IsLocallyExecutable(const ACharacter* Character);
	static void LoadSpreadOffset(FString Path, float SpreadCoef);
	
	/**
	* @details 두 벡터간의 사이각을 얻을 수 있는 함수
	* @param[in] A FromVector
	* @param[in] B ToVector
	* @return float 두 벡터간의 각도
	*/
	static float GetSeparationAngle(const FVector& A,const FVector& B);
public:
	/**
	* @details PlayerController을 넘겼을 때 캐릭터를 리턴하는 함수
	* @param[in] PlayerController
	*/
	static ACharacter* GetCharacter(const APlayerController* PlayerController);

	/**
	* @details Character을 넘겼을 때 PlayerController를 리턴하는 함수
	* @param[in] Character
	*/
	static APlayerController* GetPlayerController(const ACharacter* Character);


public:
	static uint32 GenerateUniqueSkillID();

private:
	/**
	*
	*
	*/
	struct FSkillIDGenerator
	{
		TSet<uint32> UsedIDs;

		uint32 GenerateUniqueID()
		{
			uint32 Hash = 0;
			do {
				Hash = GenerateRawHash();
			} while (UsedIDs.Contains(Hash));

			UsedIDs.Add(Hash);
			return Hash;
		}

		void ReleaseID(uint32 ID)
		{
			UsedIDs.Remove(ID);
		}

	private:
		uint32 GenerateRawHash()
		{
			FString Seed = FString::Printf(TEXT("Skill_%lld"), FDateTime::Now().GetTicks());
			return GetTypeHash(Seed);
		}
	};

	static FSkillIDGenerator& GetSkillIDGenerator();

protected:

	/*Offset 관련 함수*/
	static void GenerateUniqueSpreadOffset(float SpreadCoef);
protected:

	static FString ReadFromFile(FString FilePath, bool& bOutSuccess);
	static void WriteToFile(FString FilePath, FString String, bool& bOutSuccess);

protected:

	static TArray<float> SpreadOffsets;

};
