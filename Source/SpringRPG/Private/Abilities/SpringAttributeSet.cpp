// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SpringAttributeSet.h"
#include "Abilities/SpringAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "SpringRPGCharacter.h"

USpringAttributeSet::USpringAttributeSet()
: Health(1.f)
, MaxHealth(1.f)
, Mana(0.f)
, MaxMana(0.f)
, AttackPower(1.0f)
, DefensePower(1.0f)
, MoveSpeed(1.0f)
, Damage(0.0f)
{
}

void USpringAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USpringAttributeSet,Health);
	DOREPLIFETIME(USpringAttributeSet,MaxHealth);
	DOREPLIFETIME(USpringAttributeSet, Mana);
	DOREPLIFETIME(USpringAttributeSet, MaxMana);
	DOREPLIFETIME(USpringAttributeSet, AttackPower);
	DOREPLIFETIME(USpringAttributeSet, DefensePower);
	DOREPLIFETIME(USpringAttributeSet, MoveSpeed);
}

void USpringAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if(!FMath::IsNearlyEqual(CurrentMaxValue,NewMaxValue)&&AbilityComp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue>0.f)?(CurrentValue* NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty,EGameplayModOp::Additive,NewDelta);
	}
}

void USpringAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpringAttributeSet,Health,OldValue);
}

void USpringAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpringAttributeSet,MaxHealth,OldValue);
}

void USpringAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpringAttributeSet,Mana,OldValue);
}

void USpringAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpringAttributeSet,MaxMana,OldValue);
}

void USpringAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpringAttributeSet,AttackPower,OldValue);
}

void USpringAttributeSet::OnRep_DefensePower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpringAttributeSet,DefensePower,OldValue);
}

void USpringAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpringAttributeSet,MoveSpeed,OldValue);
}

void USpringAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpringAttributeSet,Damage,OldValue);
}

void USpringAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if(Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health,MaxHealth,NewValue,GetHealthAttribute());
	}
	else if(Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana,MaxMana,NewValue,GetManaAttribute());
	}
}

void USpringAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	/** Context 这告诉我们如何来到这里（谁/什么应用了我们） */
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetEffectContext();

	/** Returns the ability system component of the instigator that started the whole chain
	 * 返回启动整个链的启动器的能力系统组件
	 */
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	/** Compute the delta between old and new, if it is available
	 * 计算新旧之间的增量，如果它可用
	 */
	float DeltaValue = 0;
	if(Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		/* If this was additive, store the raw delta value to be passed along later
		*如果这是附加的，存储原始增量值，以便稍后传递
		*/
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	//获取Target角色，它应该是我们的所有者
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ASpringRPGCharacter* TargetCharacter = nullptr;
	if(Data.Target.AbilityActorInfo.IsValid()&&Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ASpringRPGCharacter>(TargetActor);
	}

	if(Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		ASpringRPGCharacter* SourceCharacter = nullptr;
		if(Source&&Source->AbilityActorInfo.IsValid()&&Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if(SourceController == nullptr&& SourceActor != nullptr)
			{
				if(APawn* Pawn = Cast<APawn>(Source))
				{
					SourceController = Pawn->GetController();
				}
			}
			if(SourceController)
			{
				SourceCharacter=Cast<ASpringRPGCharacter>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter=Cast<ASpringRPGCharacter>(SourceActor);
			}
		
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}
		FHitResult HitResult;
		if(Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if(LocalDamageDone>0)
		{
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone,0.0f,GetMaxHealth()));

			if(TargetCharacter)
			{
				TargetCharacter->HandleDamage(LocalDamageDone,HitResult,SourceTags,SourceCharacter,SourceActor);
				TargetCharacter->HandleHealthChanged(-LocalDamageDone,SourceTags);
			}
		}
	}
	else if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(),0.0f,GetMaxHealth()));
		if(TargetCharacter)
		{
			TargetCharacter->HandleHealthChanged(DeltaValue,SourceTags);
		}
	}

	else if(Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.0f,GetMaxMana()));

		if(TargetCharacter)
		{
			TargetCharacter->HandleManaChanged(DeltaValue,SourceTags);
		}
	}

	else if(Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		if(TargetCharacter)
		{
			TargetCharacter->HandleMoveSpeedChanged(DeltaValue,SourceTags);
		}
	}



	

	
}


