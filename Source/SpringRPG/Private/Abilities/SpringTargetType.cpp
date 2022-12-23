// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SpringTargetType.h"
#include "Abilities/SpringGameplayAbility.h"
#include "SpringRPGCharacter.h"

void USpringTargetType::GetTargets_Implementation(ASpringRPGCharacter* TargetingCharacter, AActor* TargetingActor,
	FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void USpringTargetType_UseOwner::GetTargets_Implementation(ASpringRPGCharacter* TargetingCharacter,
	AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults,
	TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void USpringTargetType_UseEventData::GetTargets_Implementation(ASpringRPGCharacter* TargetingCharacter,
	AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults,
	TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if(FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if(EventData.Target)
	{
		OutActors.Add(const_cast<AActor*>(EventData.Target.Get()));
	}
}
