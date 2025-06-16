#include "Weapon/Weapon.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(MeshComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Art/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SK_AR4.SK_AR4'"));
	if (MeshRef.Succeeded())
	{
		MeshComponent->SetSkeletalMesh(MeshRef.Object);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetAmmoRemainCount(AmmoMaxCount);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

