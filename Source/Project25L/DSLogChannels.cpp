//Default
#include "DSLogChannels.h"

//UE
#include "GameFramework/Actor.h"

//Game

DEFINE_LOG_CATEGORY(DSLog);
DEFINE_LOG_CATEGORY(DSCameraLog);
DEFINE_LOG_CATEGORY(DSNetLog);
DEFINE_LOG_CATEGORY(DSSkillLog);
DEFINE_LOG_CATEGORY(DSItemLog);
DEFINE_LOG_CATEGORY(DSStatLog);
DEFINE_LOG_CATEGORY(DSDataLog);
DEFINE_LOG_CATEGORY(DSUILog);
DEFINE_LOG_CATEGORY(DSInputLog);
DEFINE_LOG_CATEGORY(DSCelShaderLog);

PROJECT25L_API FString GetNetModeContextString(UObject* ContextObject)
{
    FString ContextInfo = TEXT("[]");
    if (nullptr ==ContextObject)
    {
        return ContextInfo;
    }

    AActor* Actor = Cast<AActor>(ContextObject);
    if (nullptr == Actor)
    {
        if (UActorComponent* Component = Cast<UActorComponent>(ContextObject))
        {
            Actor = Component->GetOwner();
        }
    }

    if (nullptr == Actor)
    {
        return ContextInfo;
    }

    ENetMode NetModeInfo = Actor->GetNetMode();
    ENetRole LocalRoleInfo = Actor->GetLocalRole();
    ENetRole RemoteRoleInfo = Actor->GetRemoteRole();

    FString NetModeString = "";
    if (NetModeInfo == ENetMode::NM_Client)
    {
#if WITH_EDITOR
		NetModeString = FString::Printf(TEXT("CLIENT%d"), static_cast<int32>(GPlayInEditorID));
#else
		NetModeString = TEXT("CLIENT");
#endif
    }
    else if (NetModeInfo == ENetMode::NM_Standalone)
    {
        NetModeString = TEXT("STANDALONE");
    }
    else
    {
        NetModeString = TEXT("SERVER");
    }

    ContextInfo = FString::Printf(TEXT("[%s][%s/%s]"),
        *StaticEnum<ENetRole>()->GetNameStringByValue(static_cast<int64>(LocalRoleInfo)),
        *StaticEnum<ENetRole>()->GetNameStringByValue(static_cast<int64>(RemoteRoleInfo)),
        *NetModeString);

    return ContextInfo;
}

