#include "Animation/TPSAnimInstance.h"
#include "Character/TPSCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UTPSAnimInstance::UTPSAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireMontageObject
	(TEXT("/Script/Engine.AnimMontage'/Game/Animation/AM_Fire.AM_Fire'"));
	if (FireMontageObject.Succeeded())
	{
		FireMontage = FireMontageObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ReloadMontageObject
	(TEXT("/Script/Engine.AnimMontage'/Game/Animation/AM_Reload.AM_Reload'"));
	if (ReloadMontageObject.Succeeded())
	{
		ReloadMontage = FireMontageObject.Object;
	}
}

void UTPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ATPSCharacter>(TryGetPawnOwner());

	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}
}

void UTPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character == nullptr)
		return;

	if (MovementComponent == nullptr)
		return;

	Velocity = MovementComponent->Velocity;

	FVector ForwardVector = Character->GetActorForwardVector();
	ForwardSpeed = FVector::DotProduct(Velocity, ForwardVector);
	FVector RightVector = Character->GetActorRightVector();
	RightSpeed = ForwardSpeed * FVector::DotProduct(Velocity, RightVector);

	bShouldMove = (Velocity.Size2D() > 3.0f && MovementComponent->GetCurrentAcceleration
	() != FVector::ZeroVector);

	bIsFalling = MovementComponent->IsFalling();
}

void UTPSAnimInstance::PlayReloadMontage()
{
	Montage_Play(ReloadMontage);
}

void UTPSAnimInstance::PlayFireMontage()
{
	Montage_Play(FireMontage);
}
