// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SpringDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class SPRINGRPG_API USpringDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	USpringDamageExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
