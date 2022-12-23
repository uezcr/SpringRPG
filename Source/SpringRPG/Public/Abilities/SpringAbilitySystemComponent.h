// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpringRPG.h"
#include "AbilitySystemComponent.h"
#include "Abilities/SpringAbilityTypes.h"
#include "SpringAbilitySystemComponent.generated.h"

class USpringGameplayAbility;
/**
 * 
 */
UCLASS()
class SPRINGRPG_API USpringAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	USpringAbilitySystemComponent();

	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer,TArray<USpringGameplayAbility*>& ActiveAbilities);

	int32 GetDefaultAbilityLevel() const;

	static USpringAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor,bool LookForComponent = false);
	
};
