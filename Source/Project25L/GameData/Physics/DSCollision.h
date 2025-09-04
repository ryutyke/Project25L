
#pragma once

// Default
#include "CoreMinimal.h"

#define CPROFILE_DSCAPSULE TEXT("DSCapsule")
#define CPROFILE_DSTRIGGER TEXT("DSTrigger")
#define CPROFILE_DSPORTAL_INSTALLFLOORPLATFORM TEXT("DSInstallWallPlatorm")
#define CPROFILE_DSPORTAL_INSTALLWALLPLATFORM TEXT("DSInstallFloorPlatorm")
#define CCHANNEL_DSACTION ECollisionChannel::ECC_GameTraceChannel1
#define CCHANNEL_DSINSTALLPORTAL ECollisionChannel::ECC_GameTraceChannel2
#define CCHANNEL_DSINSTALLGRAVITY ECollisionChannel::ECC_GameTraceChannel3
#define CCHANNEL_DSINCLINE ECollisionChannel::ECC_GameTraceChannel3
#define CCHANNEL_DSAIACTION ECollisionChannel::ECC_GameTraceChannel4 //AI 트레이스 용도
#define CCHANNEL_DSPLAYER ECollisionChannel::ECC_GameTraceChannel5
