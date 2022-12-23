// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SpringGameplayAbility.h"
#include "Abilities/SpringTargetType.h"
#include "SpringRPGCharacter.h"
#include "Abilities/SpringAbilitySystemComponent.h"

USpringGameplayAbility::USpringGameplayAbility() {}

FSpringGameplayEffectContainerSpec USpringGameplayAbility::MakeEffectContainerSpecFromContainer(
	const FSpringGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FSpringGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	ASpringRPGCharacter* OwningCharacter = CastChecked<ASpringRPGCharacter>(OwningActor);
	USpringAbilitySystemComponent* OwningASC = USpringAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);
	if(OwningASC)
	{
		if(Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const USpringTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			TargetTypeCDO->GetTargets(OwningCharacter,AvatarActor,EventData,HitResults,TargetActors);
			ReturnSpec.AddTargets(HitResults,TargetActors);
		}
		if(OverrideGameplayLevel == INDEX_NONE)
		{
			OverrideGameplayLevel = OverrideGameplayLevel = this->GetAbilityLevel();
		}
		for(const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass,OverrideGameplayLevel));
		}
	}
	return ReturnSpec;
}

FSpringGameplayEffectContainerSpec USpringGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag,
	const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FSpringGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);
	
	if(FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer,EventData,OverrideGameplayLevel);
	}
	return FSpringGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> USpringGameplayAbility::ApplyEffectContainerSpec(
	const FSpringGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;
	for(const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle,ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> USpringGameplayAbility::ApplyEffectContainer(FGameplayTag ContainerTag,
	const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FSpringGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag,EventData,OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}
