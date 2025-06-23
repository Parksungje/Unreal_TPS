// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Character/TPSCharacter.h"
#include "Weapon/Bullet.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(MeshComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/_Art/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SK_AR4.SK_AR4'"));
	if (MeshRef.Succeeded())
	{
		MeshComponent->SetSkeletalMesh(MeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> HitEffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/_Art/Effect/PT_Effect.PT_Effect'"));
	if (HitEffectRef.Succeeded())
	{
		HitEffect = HitEffectRef.Object;
	}

	Firetype = EFireType::EF_LineTrace;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetAmmoRemainCount(AmmoMaxCount);

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::StartFire(TWeakObjectPtr<class ATPSCharacter> OWnerCharacter)
{
	ATPSCharacter* Character = OWnerCharacter.Get();
	if (Character)
	{
		switch (Firetype)
		{
		case EFireType::EF_Projectile:
			GetWorldTimerManager().SetTimer(
				FireTimerHandle,
				[Character, this]() { FireWithProjectile(Character);  },
				FireInterval,
				true);
			break;
		case EFireType::EF_LineTrace:
			GetWorldTimerManager().SetTimer(
				FireTimerHandle,
				[Character, this]() { FireWithLineTrace(Character);  },
				FireInterval,
				true);
			break;
		default:
			break;
		}
	}
}

void AWeapon::StopFire()
{
	if (FireTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
	}
}

void AWeapon::Reloading()
{
}

void AWeapon::FinishReloading()
{
}

void AWeapon::FireWithProjectile(TWeakObjectPtr<class ATPSCharacter> OwnerCharacter)
{
	ATPSCharacter* Character = OwnerCharacter.Get();
	if (Character == nullptr)
		return;

	if (AmmoRamainCount <= 0)
	{
		Character->StopAnimMontage(Character->GetCurrentMontage());
		StopFire();
		return;
	}

	FTransform FireTransform = MeshComponent->GetSocketTransform("FireSocket");
	FVector Direction = FireTransform.GetRotation().GetForwardVector() * TraceDistance;

	FActorSpawnParameters SpawnParameter;
	SpawnParameter.Owner = Character;
	ABullet* SpawnBullet = GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(),
		SpawnParameter);

	if (SpawnBullet)
	{
		SpawnBullet->SetActorLocation(FireTransform.GetLocation());
		SpawnBullet->SetActorRotation(FireTransform.GetRotation());

		if (Direction.Normalize())
		{
			SpawnBullet->Fire(Direction);
		}

		SetAmmoRemainCount(--AmmoRamainCount);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, FString::Printf(TEXT("AMMO Remain Count : %d"), GetAmmoRamainCount()));
	}
}

void AWeapon::PlayHitEffect(FTransform HitTransform)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitTransform);
}

void AWeapon::FireWithLineTrace(TWeakObjectPtr<class ATPSCharacter> OwnerCharacter)
{
	ATPSCharacter* Character = OwnerCharacter.Get();
	if (Character == nullptr) return;

	if (AmmoRamainCount <= 0)
	{
		Character->StopAnimMontage(Character->GetCurrentMontage());
		StopFire();
		return;
	}

	const FVector Start = MeshComponent->GetSocketLocation("FireSocket");
	const FVector End = Start + Character->GetActorForwardVector() * TraceDistance;

	FHitResult HitResult;
	FCollisionQueryParams CollistionParams;
	CollistionParams.AddIgnoredActor(Character);

	bool HitDetected = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollistionParams);
	if (HitDetected)
	{
		if (HitResult.GetActor())
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, (TEXT("¶óÀÎ¤©ÀÎ")));
		}

		FTransform BulletTransform;
		BulletTransform.SetLocation(HitResult.ImpactPoint);
		PlayHitEffect(BulletTransform);
		SetAmmoRemainCount(--AmmoRamainCount);

		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, FString::Printf(TEXT("asdfasdf")));
	}
#if ENABLE_DRAW_DEBUG
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), Start, End, DrawColor, false, 1.0f);
#endif
}

