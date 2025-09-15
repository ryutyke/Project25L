#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Components/ListView.h"

// UHT
#include "DSItemListView.generated.h"

/* 코드리뷰 : 클래스명 변경 => 구체화 */
UCLASS()
class PROJECT25L_API UDSItemListView : public UListView
{
	GENERATED_BODY()
	
public:
	DECLARE_DELEGATE_OneParam(FOnListEntryHovered,int)
	FOnListEntryHovered OnListEntryHovered;

	DECLARE_DELEGATE_OneParam(FOnListEntryUnhovered, int)
	FOnListEntryUnhovered OnListEntryUnhovered;

protected:
	virtual void HandleListEntryHovered(UUserWidget& EntryWidget) override;
	virtual void HandleListEntryUnhovered(UUserWidget& EntryWidget) override;
	virtual void OnItemClickedInternal(UObject* Item) override;
};
