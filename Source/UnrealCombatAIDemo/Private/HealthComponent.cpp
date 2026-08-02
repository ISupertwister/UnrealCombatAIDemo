// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "Math/UnrealMathUtility.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	if (bIsDead)
	{
		return;
	}

	if (bIsInvulnerable)
	{
		UE_LOG(LogTemp, Log, TEXT("%s ignored %.1f damage because it is invulnerable."),
			*GetOwner()->GetName(),
			DamageAmount);

		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage. Health: %.1f / %.1f"),
		*GetOwner()->GetName(),
		DamageAmount,
		CurrentHealth,
		MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;

		UE_LOG(LogTemp, Warning, TEXT("%s died."), *GetOwner()->GetName());

		OnDeath.Broadcast();
	}
}

void UHealthComponent::Heal(float HealAmount)
{
	if (bIsDead || HealAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("%s healed %.1f. Health: %.1f / %.1f"),
		*GetOwner()->GetName(),
		HealAmount,
		CurrentHealth,
		MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
	bIsDead = false;
	bIsInvulnerable = false;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("%s health reset. Health: %.1f / %.1f"),
		*GetOwner()->GetName(),
		CurrentHealth,
		MaxHealth);
}

float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float UHealthComponent::GetHealthPercent() const
{
	if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}

	return CurrentHealth / MaxHealth;
}

bool UHealthComponent::IsDead() const
{
	return bIsDead;
}

void UHealthComponent::SetInvulnerable(bool bNewInvulnerable)
{
	bIsInvulnerable = bNewInvulnerable;

	UE_LOG(LogTemp, Log, TEXT("%s invulnerability %s."),
		*GetOwner()->GetName(),
		bIsInvulnerable ? TEXT("enabled") : TEXT("disabled"));
}

bool UHealthComponent::IsInvulnerable() const
{
	return bIsInvulnerable;
}