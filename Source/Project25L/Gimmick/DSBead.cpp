// Default
#include "Gimmick/DSBead.h"

// UE
#include "Components/StaticMeshComponent.h"

ADSBead::ADSBead()
	: Super()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void ADSBead::InitializeMaterial(UMaterialInterface* MaterialInterface)
{
	if (IsValid(Mesh))
	{
		Mesh->SetMaterial(0, MaterialInterface);
	}
}
