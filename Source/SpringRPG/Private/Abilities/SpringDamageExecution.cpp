// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SpringDamageExecution.h"
#include "Abilities/SpringAttributeSet.h"
#include "AbilitySystemComponent.h"

struct SpringDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	SpringDamageStatics()
	{
		// 捕获目标的防御力属性,不要快照,因为我们想在应用执行的时候使用健康值.
		DEFINE_ATTRIBUTE_CAPTUREDEF(USpringAttributeSet,DefensePower,Target,false);
		
		// 捕获源的攻击力。我们确实希望在创建将执行伤害的GameplayEffectSpec的时候将其快照。
		// (想象一下，我们发射一个弹丸：我们在弹丸发射时创建GE Spec。当它击中目标时，我们要使用当时的攻击力。
		// 而不是在它击中时）。
		DEFINE_ATTRIBUTE_CAPTUREDEF(USpringAttributeSet,AttackPower,Source,true);
		
		// 还可以捕获源的原始Damage，这通常是通过执行直接传入的
		DEFINE_ATTRIBUTE_CAPTUREDEF(USpringAttributeSet,Damage,Source,false);
	}
	
};

static const SpringDamageStatics& DamageStatics()
{
	static SpringDamageStatics DmgStatics;
	return DmgStatics;
}


USpringDamageExecution::USpringDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().DefensePowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void USpringDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent?SourceAbilitySystemComponent->GetAvatarActor_Direct():nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent?TargetAbilitySystemComponent->GetAvatarActor_Direct():nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float DefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefensePowerDef,EvaluationParameters,DefensePower);
	if(DefensePower == 0.f)
	{
		DefensePower = 1.0f;
	}

	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef,EvaluationParameters,AttackPower);

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef,EvaluationParameters,Damage);

	float DamageDone = Damage * AttackPower / DefensePower;
	if(DamageDone>0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty,EGameplayModOp::Additive,DamageDone));
	}
}
