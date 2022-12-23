// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SpringAbilitySystemComponent.h"
#include "SpringRPGCharacter.h"
#include "Abilities/SpringGameplayAbility.h"
#include "AbilitySystemGlobals.h"

USpringAbilitySystemComponent::USpringAbilitySystemComponent() {}

void USpringAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer,
	TArray<USpringGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer,AbilitiesToActivate,false);
	for(FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();
		for(UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(Cast<USpringGameplayAbility>(ActiveAbility));
		}
	}
}

int32 USpringAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	ASpringRPGCharacter* OwningCharacter = Cast<ASpringRPGCharacter>(GetOwner());
	if(OwningCharacter)
	{
		return OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

USpringAbilitySystemComponent* USpringAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor,
	bool LookForComponent)
{
	return Cast<USpringAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor,LookForComponent));
}
