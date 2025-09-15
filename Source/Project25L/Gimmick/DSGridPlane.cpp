// Default
#include "Gimmick/DSGridPlane.h"

// UE
#include "Components/StaticMeshComponent.h"

// Game
#include "DSLogChannels.h"

ADSGridPlane::ADSGridPlane()
{
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	SetRootComponent(Plane);
}

FIntPoint ADSGridPlane::GetGridCell(const FVector& TargetLocation)
{
	int32 CellX = 0;
	int32 CellY = 0;
	if (IsValid(Plane))
	{
		//Mesh의 크기를 가지고옴 -> 중심지 오른쪽 상단..
		float PlaneX = GetActorLocation().X - (Plane->Bounds.BoxExtent.X * 2.f); //X축의 절반 크기 => 열
		float PlaneY = GetActorLocation().Y - (Plane->Bounds.BoxExtent.Y * 2.f); //Y축의 절반 크기 => 행

		//GetActorLocation은 중점이기 때문에 왼쪽으로 보간해주어야함
		FVector CellLocation = TargetLocation - FVector(PlaneX, PlaneY, 0); //진짜 위치 - 시작 위치 

		CellX = FMath::FloorToInt(CellLocation.X / ColSize);
		CellY = FMath::FloorToInt(CellLocation.Y / RowSize);
	}

	return FIntPoint(CellX, CellY);
}

FVector ADSGridPlane::GetGridCellCenter(const FIntPoint& Point)
{
	FVector GridCell;
	if (IsValid(Plane))
	{
		//Mesh의 크기를 가지고옴 -> 중심지 오른쪽 상단..
		float PlaneX = GetActorLocation().X - (Plane->Bounds.BoxExtent.X * 2.f); //X축의 절반 크기 => 열
		float PlaneY = GetActorLocation().Y - (Plane->Bounds.BoxExtent.Y * 2.f); //Y축의 절반 크기 => 행

		// Point.X 중심지로 옮겨둔다. -> int형이기 때문에
		GridCell.X = PlaneX + (Point.X + 0.5) * ColSize;
		GridCell.Y = PlaneY + (Point.Y + 0.5) * RowSize;

		GridCell.Z = GetActorLocation().Z;
	}
	return GridCell;
}
