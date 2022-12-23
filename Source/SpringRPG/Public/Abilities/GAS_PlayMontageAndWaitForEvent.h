// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpringRPG.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GAS_PlayMontageAndWaitForEvent.generated.h"

class USpringAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGASPlayMontagAndWaitForEventDelegate,FGameplayTag,EventTag,FGameplayEventData,EventData);
/**
 * 
 */
UCLASS()
class SPRINGRPG_API UGAS_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	UGAS_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY(BlueprintAssignable)
	FGASPlayMontagAndWaitForEventDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FGASPlayMontagAndWaitForEventDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FGASPlayMontagAndWaitForEventDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FGASPlayMontagAndWaitForEventDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FGASPlayMontagAndWaitForEventDelegate EventReceived;

	//Step1:调用该函数
	UFUNCTION(BlueprintCallable,Category="Ability|Tasks",meta=(HidePin="OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly = "TRUE"))
	static UGAS_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		UAnimMontage* MontageToPlay,
		FGameplayTagContainer EventTags,
		float Rate = 1.f,
		FName StartSection = NAME_None,
		bool bStopWhenAbilityEnds = true,
		float AnimRootMotionTranslationScale = 1.f);

private:
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	FGameplayTagContainer EventTags;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	bool StopPlayingMontage();

	USpringAbilitySystemComponent* GetTargetASC();

	void OnMontageBlendingOut(UAnimMontage* Montage,bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
};
