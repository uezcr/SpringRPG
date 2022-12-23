// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GAS_PlayMontageAndWaitForEvent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Abilities/SpringAbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemComponent.h"

UGAS_PlayMontageAndWaitForEvent::UGAS_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

USpringAbilitySystemComponent* UGAS_PlayMontageAndWaitForEvent::GetTargetASC()
{
	return Cast<USpringAbilitySystemComponent>(AbilitySystemComponent);
}

void UGAS_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if(Ability&&Ability->GetCurrentMontage()==MontageToPlay)
	{
		if(Montage==MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if(Character&&(Character->GetLocalRole()==ROLE_Authority||
				(Character->GetLocalRole()==ROLE_AutonomousProxy&&Ability->GetNetExecutionPolicy()== EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}
		}
	}
	if(bInterrupted)
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(),FGameplayEventData());
		}
	}
	else
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(),FGameplayEventData());
		}
	}
}

void UGAS_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	if(StopPlayingMontage())
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(),FGameplayEventData());
		}
	}
}

void UGAS_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(StopPlayingMontage())
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(),FGameplayEventData());
		}
	}

	EndTask();
}

void UGAS_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag,TempData);
	}
}

//Step01
UGAS_PlayMontageAndWaitForEvent* UGAS_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
	UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay,
	FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds,
	float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);
	UGAS_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UGAS_PlayMontageAndWaitForEvent>(OwningAbility,TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	
	return MyObj;
}

void UGAS_PlayMontageAndWaitForEvent::Activate()
{
	if(Ability == nullptr)
	{
		return;
	}
	bool bPlayedMontage = false;
	USpringAbilitySystemComponent* SpringAbilitySystemComponent = GetTargetASC();
	if(SpringAbilitySystemComponent)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if(AnimInstance!=nullptr)
		{
			EventHandle = SpringAbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags,FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this,&UGAS_PlayMontageAndWaitForEvent::OnGameplayEvent));
			if(SpringAbilitySystemComponent->PlayMontage(Ability,Ability->GetCurrentActivationInfo(),MontageToPlay,Rate,StartSection)>0.f)
			{
				if(ShouldBroadcastAbilityTaskDelegates()==false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this,&UGAS_PlayMontageAndWaitForEvent::OnAbilityCancelled);
				BlendingOutDelegate.BindUObject(this,&UGAS_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate,MontageToPlay);
				MontageEndedDelegate.BindUObject(this,&UGAS_PlayMontageAndWaitForEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate,MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if(Character && (Character->GetLocalRole()==ROLE_Authority||
					(Character->GetLocalRole()==ROLE_AutonomousProxy&&Ability->GetNetExecutionPolicy()==EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}
				bPlayedMontage = true;
			}
		}
		else
		{
			UE_LOG(LogSpringRPG,Warning,TEXT("PlayMontageAndWaitForEvent call to PlayMontage failed!"));
		}
	}
	else
	{
		UE_LOG(LogSpringRPG,Warning,TEXT("PlayMontageAndWaitForEvent call on invalid AbilitySystemComponent"));
	}
	if(!bPlayedMontage)
	{
		UE_LOG(LogSpringRPG,Warning,TEXT("PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(),FGameplayEventData());
		}
	}
	SetWaitingOnAvatar();
}

void UGAS_PlayMontageAndWaitForEvent::ExternalCancel()
{
	if(!AbilitySystemComponent.IsValid())
	{
		return;
	}
	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString UGAS_PlayMontageAndWaitForEvent::GetDebugString() const
{
	return Super::GetDebugString();
}

void UGAS_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	if(Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if(AbilityEnded&&bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}
	USpringAbilitySystemComponent* SpringAbilitySystemComponent = GetTargetASC();
	if(SpringAbilitySystemComponent)
	{
		SpringAbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}
	Super::OnDestroy(AbilityEnded);
}



bool UGAS_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if(!ActorInfo)
	{
		return false;
	}
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if(AnimInstance == nullptr)
	{
		return false;
	}
	if(AbilitySystemComponent.IsValid()&&Ability)
	{
		if(AbilitySystemComponent->GetAnimatingAbility()==Ability&&
			AbilitySystemComponent->GetCurrentMontage()==MontageToPlay)
		{
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if(MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}
			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}
	return false;
}



