#include "Animation/TPSAnimInstance.h"
#include "Character/TPSCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UTPSAnimInstance::UTPSAnimInstance()
{
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
