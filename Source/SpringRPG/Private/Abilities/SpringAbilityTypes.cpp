﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SpringAbilityTypes.h"

bool FSpringGameplayEffectContainerSpec::HasVailidEffects() const
{
	return TargetGameplayEffectSpecs.Num()>0;
}

bool FSpringGameplayEffectContainerSpec::HasValidTargets() const
{
	return TargetData.Num()>0;
}

void FSpringGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults,
	const TArray<AActor*>& TargetActors)
{
	for(const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if(TargetActors.Num()>0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}
}
