// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpringRPG.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "SpringAbilityTypes.generated.h"

class UGameplayEffect;
class USpringTargetType;

/**
 * 定义游戏效果列表的结构，一个标签，以及目标信息
 * 这些容器在蓝图或资产中被静态定义，然后在运行时变成Specs。
 */
USTRUCT(BlueprintType)
struct FSpringGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FSpringGameplayEffectContainer(){}

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=GameplayEffectContainer)
	TSubclassOf<USpringTargetType> TargetType;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=GameplayEffectContainer)
	TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffectClasses;
	
};


/** SpringGameplayEffectContainer的 "处理过的 "版本，可以被传递并最终应用 */
USTRUCT(BlueprintType)
struct FSpringGameplayEffectContainerSpec
{
	GENERATED_BODY()
public:
	FSpringGameplayEffectContainerSpec(){}
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	bool HasVailidEffects() const;

	bool HasValidTargets() const;

	void AddTargets(const TArray<FHitResult>& HitResults,const TArray<AActor*>& TargetActor);
};


