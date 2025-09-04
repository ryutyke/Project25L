// Default
#include "GameData/Animation/BoneToComponentAnimMetaData.h"

// UE
#include "Math/Transform.h"

FTransform UBoneToComponentAnimMetaData::GetBoneToComponentTransform(float Time) const
{
	if (Times.Num() == 0) return FTransform::Identity;
	if (Time <= Times[0]) return BoneToComponent[0];
	if (Time >= Times.Last()) return BoneToComponent.Last();

	int32 Low = 0, High = Times.Num() - 1;
	while (Low + 1 < High)
	{
		const int32 Mid = (Low + High) / 2;
		(Times[Mid] <= Time) ? Low = Mid : High = Mid;
	}
	const float A = Times[Low], B = Times[High];
	const float Alpha = (B > A) ? (Time - A) / (B - A) : 0.f;

	const FTransform& TA = BoneToComponent[Low];
	const FTransform& TB = BoneToComponent[High];

	FTransform Out;
	Out.SetRotation(FQuat::Slerp(TA.GetRotation(), TB.GetRotation(), Alpha).GetNormalized());
	Out.SetTranslation(FMath::Lerp(TA.GetTranslation(), TB.GetTranslation(), Alpha));
	Out.SetScale3D(FMath::Lerp(TA.GetScale3D(), TB.GetScale3D(), Alpha));
	return Out;
}
