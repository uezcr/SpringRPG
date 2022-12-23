// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpringRPG.h"
#include "Abilities/GameplayAbility.h"
#include "SpringAbilityTypes.h"
#include "SpringGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class SPRINGRPG_API USpringGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	//构造
	USpringGameplayAbility();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GameplayEffects)
	TMap<FGameplayTag,FSpringGameplayEffectContainer> EffectContainerMap;

	UFUNCTION(BlueprintCallable,Category=Ability, meta=(AutoCreateRefTerm="EventData"))
	virtual FSpringGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FSpringGameplayEffectContainer& Container,const FGameplayEventData& EventData,int32 OverrideGameplayLevel = -1);

	UFUNCTION(BlueprintCallable,Category=Ability, meta=(AutoCreateRefTerm="EventData"))
	virtual FSpringGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	UFUNCTION(BlueprintCallable,Category=Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FSpringGameplayEffectContainerSpec& ContainerSpec);

	UFUNCTION(BlueprintCallable,Category=Ability,meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag,const FGameplayEventData& EventData,int32 OverrideGameplayLevel = -1);
};
