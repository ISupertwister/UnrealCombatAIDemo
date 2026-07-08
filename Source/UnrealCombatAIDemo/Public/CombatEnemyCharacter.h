// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatEnemyCharacter.generated.h"

class UHealthComponent;

UENUM(BlueprintType)
enum class ECombatEnemyState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Chase	UMETA(DisplayName = "Chase"),
	Attack	UMETA(DisplayName = "Attack"),
	Dead	UMETA(DisplayName = "Dead")
};

UCLASS()
class UNREALCOMBATAIDEMO_API ACombatEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACombatEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	ECombatEnemyState CurrentState = ECombatEnemyState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Detection", meta = (ClampMin = "0.0"))
	float DetectionRadius = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat", meta = (ClampMin = "0.0"))
	float AttackRange = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat", meta = (ClampMin = "0.0"))
	float AttackDamage = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat", meta = (ClampMin = "0.0"))
	float AttackCooldown = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement", meta = (ClampMin = "0.0"))
	float ChaseSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Debug")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Debug")
	bool bLogStateChanges = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	AActor* TargetActor = nullptr;

	float LastAttackTime = -999.0f;

	void UpdateAI(float DeltaTime);
	void UpdateStateByDistance(float DistanceToTarget);
	void ChaseTarget();
	void TryAttack();
	void SetEnemyState(ECombatEnemyState NewState);
	void DrawDebugInfo() const;

	UFUNCTION()
	void HandleDeath();

	FString GetStateName() const;
};