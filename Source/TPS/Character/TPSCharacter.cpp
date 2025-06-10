// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/TPSAnimInstance.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;

	JumpMaxCount = 2; //더블 점프

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -99.0f, -00.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/MilitaryCharDark/MW_Style2_Male.MW_Style2_Male'"));
	if (MeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshRef.Object);
	}

	//SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SocketOffset = FVector(0.0f, 50.0f, 50.0f);

	//Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

#pragma region InputSystem
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCDefaultRef(TEXT(
		"/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Default.IMC_Default'"));
	if (IMCDefaultRef.Succeeded())
	{
		IMCDefault = IMCDefaultRef.Object;
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT(
		"/Script/Engine.AnimBlueprint'/Game/Animation/ABP_Character.ABP_Character_C'"));
	if (AnimInstanceRef.Succeeded())
	{
		GetMesh()->SetAnimClass(AnimInstanceRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Jump.IA_Jump'"));
	if (JumpActionRef.Succeeded())
	{
		JumpAction = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Move.IA_Move'"));
	if (MoveActionRef.Succeeded())
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> TurnActionRef(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Turn.IA_Turn'"));
	if (TurnActionRef.Succeeded())
	{
		TurnAction = TurnActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> SprintActionRef(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Sprint.IA_Sprint'"));	
	if (SprintActionRef.Succeeded())
	{
		SprintAction = SprintActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionRef(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Fire.IA_Fire'"));
	if (FireActionRef.Succeeded())
	{
		FireAction = FireActionRef.Object;
	}
#pragma endregion

}



// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (SubSystem)
		{
			SubSystem->AddMappingContext(IMCDefault, 0);
		}
	}
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhanedInputComponent =
		CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhanedInputComponent)
	{
		EnhanedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, 
			&ACharacter::Jump);
		EnhanedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, 
			&ACharacter::StopJumping);
		EnhanedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, 
			&ATPSCharacter::Input_Move);
		EnhanedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, 
			&ATPSCharacter::Input_Turn);
		EnhanedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this,
			&ATPSCharacter::Input_Sprint);
		EnhanedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
			&ATPSCharacter::Input_Sprint);
		EnhanedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this,
			&ATPSCharacter::Input_Fire);
	}

}

void ATPSCharacter::Input_Move(const FInputActionValue& InputValue)
{
	const FVector2D MovementVector = InputValue.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementVector.X);
	AddMovementInput(GetActorRightVector(), MovementVector.Y);
}

void ATPSCharacter::Input_Turn(const FInputActionValue& InputValue)
{
	const FVector2D LookAxisVector = InputValue.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ATPSCharacter::Input_Sprint(const FInputActionValue& InputValue)
{
	if (InputValue.Get<bool>())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}

void ATPSCharacter::Input_Fire(const FInputActionValue& InputValue)
{
	UTPSAnimInstance* AnimInstance = Cast<UTPSAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayFireMontage();
	}
}

