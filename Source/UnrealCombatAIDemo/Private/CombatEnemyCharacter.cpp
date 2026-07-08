// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatEnemyCharacter.h"
#include "HealthComponent.h"

#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ACombatEnemyCharacter::ACombatEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
}

void ACombatEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	TargetActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = ChaseSpeed;
	}

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ACombatEnemyCharacter::HandleDeath);
	}

	SetEnemyState(ECombatEnemyState::Idle);
}

void ACombatEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAI(DeltaTime);

	if (bDrawDebug)
	{
		DrawDebugInfo();
	}
}

void ACombatEnemyCharacter::UpdateAI(float DeltaTime)
{
	if (CurrentState == ECombatEnemyState::Dead || !TargetActor)
	{
		return;
	}

	const float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());

	UpdateStateByTargetDistance(DistanceToTarget);

	switch (CurrentState)
	{
	case ECombatEnemyState::Idle:
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
		}
		break;

	case ECombatEnemyState::Chase:
		ChaseTarget();
		break;

	case ECombatEnemyState::Attack:
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
		}

		TryAttack();
		break;

	case ECombatEnemyState::Investigate:
		InvestigateLastKnownLocation();
		break;

	case ECombatEnemyState::Dead:
	default:
		break;
	}
}

void ACombatEnemyCharacter::UpdateStateByTargetDistance(float DistanceToTarget)
{
	const bool bTargetDetected = DistanceToTarget <= DetectionRadius;

	if (bTargetDetected)
	{
		LastKnownTargetLocation = TargetActor->GetActorLocation();
		bHasLastKnownTargetLocation = true;
		bHasReachedInvestigationPoint = false;

		if (DistanceToTarget <= AttackRange)
		{
			SetEnemyState(ECombatEnemyState::Attack);
		}
		else
		{
			SetEnemyState(ECombatEnemyState::Chase);
		}

		return;
	}

	if (CurrentState == ECombatEnemyState::Chase || CurrentState == ECombatEnemyState::Attack)
	{
		if (bHasLastKnownTargetLocation)
		{
			SetEnemyState(ECombatEnemyState::Investigate);
		}
		else
		{
			SetEnemyState(ECombatEnemyState::Idle);
		}
	}
}

void ACombatEnemyCharacter::ChaseTarget()
{
	if (!TargetActor)
	{
		return;
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToActor(TargetActor, AttackRange * 0.8f);
	}
}

void ACombatEnemyCharacter::InvestigateLastKnownLocation()
{
	if (!GetWorld() || !bHasLastKnownTargetLocation)
	{
		SetEnemyState(ECombatEnemyState::Idle);
		return;
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(LastKnownTargetLocation, InvestigationAcceptanceRadius);
	}

	const float DistanceToLastKnownLocation = FVector::Dist(GetActorLocation(), LastKnownTargetLocation);

	if (DistanceToLastKnownLocation <= InvestigationAcceptanceRadius)
	{
		if (!bHasReachedInvestigationPoint)
		{
			bHasReachedInvestigationPoint = true;
			InvestigationStartTime = GetWorld()->GetTimeSeconds();

			if (AAIController* AIController = Cast<AAIController>(GetController()))
			{
				AIController->StopMovement();
			}

			UE_LOG(LogTemp, Log, TEXT("%s reached last known target location."), *GetName());
		}

		const float CurrentTime = GetWorld()->GetTimeSeconds();

		if (CurrentTime - InvestigationStartTime >= InvestigationWaitTime)
		{
			bHasLastKnownTargetLocation = false;
			bHasReachedInvestigationPoint = false;

			SetEnemyState(ECombatEnemyState::Idle);
		}
	}
}

void ACombatEnemyCharacter::TryAttack()
{
	if (!TargetActor || !GetWorld())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return;
	}

	LastAttackTime = CurrentTime;

	UHealthComponent* TargetHealth = TargetActor->FindComponentByClass<UHealthComponent>();

	if (!TargetHealth)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s tried to attack %s, but target has no HealthComponent."),
			*GetName(),
			*TargetActor->GetName());

		return;
	}

	if (TargetHealth->IsDead())
	{
		return;
	}

	TargetHealth->ApplyDamage(AttackDamage);

	UE_LOG(LogTemp, Log, TEXT("%s attacked %s for %.1f damage."),
		*GetName(),
		*TargetActor->GetName(),
		AttackDamage);
}

void ACombatEnemyCharacter::SetEnemyState(ECombatEnemyState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;

	if (NewState == ECombatEnemyState::Investigate)
	{
		bHasReachedInvestigationPoint = false;
	}

	if (bLogStateChanges)
	{
		UE_LOG(LogTemp, Log, TEXT("%s state changed to %s."), *GetName(), *GetStateName());
	}
}

void ACombatEnemyCharacter::HandleDeath()
{
	SetEnemyState(ECombatEnemyState::Dead);

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	SetActorEnableCollision(false);
	SetLifeSpan(3.0f);

	UE_LOG(LogTemp, Warning, TEXT("%s entered Dead state."), *GetName());
}

void ACombatEnemyCharacter::DrawDebugInfo() const
{
	if (!GetWorld())
	{
		return;
	}

	const FVector EnemyLocation = GetActorLocation();

	DrawDebugSphere(
		GetWorld(),
		EnemyLocation,
		DetectionRadius,
		32,
		FColor::Cyan,
		false,
		0.0f,
		0,
		1.5f
	);

	DrawDebugSphere(
		GetWorld(),
		EnemyLocation,
		AttackRange,
		24,
		FColor::Red,
		false,
		0.0f,
		0,
		2.0f
	);

	if (TargetActor)
	{
		DrawDebugLine(
			GetWorld(),
			EnemyLocation + FVector(0.0f, 0.0f, 80.0f),
			TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, 80.0f),
			FColor::Yellow,
			false,
			0.0f,
			0,
			1.5f
		);
	}

	if (bHasLastKnownTargetLocation)
	{
		DrawDebugSphere(
			GetWorld(),
			LastKnownTargetLocation + FVector(0.0f, 0.0f, 35.0f),
			35.0f,
			12,
			FColor::Orange,
			false,
			0.0f,
			0,
			2.0f
		);

		DrawDebugString(
			GetWorld(),
			LastKnownTargetLocation + FVector(0.0f, 0.0f, 80.0f),
			TEXT("Last Known Position"),
			nullptr,
			FColor::Orange,
			0.0f,
			true
		);
	}

	DrawDebugString(
		GetWorld(),
		EnemyLocation + FVector(0.0f, 0.0f, 120.0f),
		GetStateName(),
		nullptr,
		FColor::White,
		0.0f,
		true
	);
}

FString ACombatEnemyCharacter::GetStateName() const
{
	switch (CurrentState)
	{
	case ECombatEnemyState::Idle:
		return TEXT("Idle");

	case ECombatEnemyState::Chase:
		return TEXT("Chase");

	case ECombatEnemyState::Attack:
		return TEXT("Attack");

	case ECombatEnemyState::Investigate:
		return TEXT("Investigate");

	case ECombatEnemyState::Dead:
		return TEXT("Dead");

	default:
		return TEXT("Unknown");
	}
}