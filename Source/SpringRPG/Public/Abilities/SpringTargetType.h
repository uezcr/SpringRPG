// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpringRPG.h"
#include "UObject/Object.h"
#include "Abilities/SpringAbilityTypes.h"
#include "SpringTargetType.generated.h"

class ASpringRPGCharacter;
class AActor;
struct FGameplayEventData;

/**
 * 用来确定能力的目标的类。
 * 它是用来运行目标逻辑的蓝图。
 * 这不是GameplayAbilityTargetActor的子类，因为这个类从未被实例化到世界中。
 * 这可以作为特定游戏的目标定位蓝图的基础。
 * 如果你的目标设定比较复杂，你可能需要将其实例化到世界中一次，或者作为一个集合的角色。
 */

UCLASS(Blueprintable,meta=(ShowWorldContextPin))
class SPRINGRPG_API USpringTargetType : public UObject
{
	GENERATED_BODY()
public:
	USpringTargetType(){};

	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(ASpringRPGCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

UCLASS(NotBlueprintable)
class SPRINGRPG_API USpringTargetType_UseOwner : public USpringTargetType
{
	GENERATED_BODY()
public:
	USpringTargetType_UseOwner(){};

	virtual void GetTargets_Implementation(ASpringRPGCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

UCLASS(NotBlueprintable)
class SPRINGRPG_API USpringTargetType_UseEventData : public USpringTargetType
{
	GENERATED_BODY()
public:
	USpringTargetType_UseEventData(){};

	virtual void GetTargets_Implementation(ASpringRPGCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
