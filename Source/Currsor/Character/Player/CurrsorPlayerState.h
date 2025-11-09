#pragma once

#include "CoreMinimal.h"
#include "Currsor/Character/Component/BaseState.h"
#include "Engine/UserDefinedStruct.h"
#include "CurrsorPlayerState.generated.h"

class ACurrsorCharacter;
class ACurrsorPlayerController;

UCLASS()
class CURRSOR_API ACurrsorPlayerState : public ABaseState
{
    GENERATED_BODY()

public:
    ACurrsorPlayerState();

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

public:

    // 牌库属性（存储卡牌行名称）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    TArray<FName> DeckCardNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    UDataTable* CardDataTable;
    
    template <class T>
    UFUNCTION(BlueprintCallable)
    T* FindRow(FName RowName, const TCHAR* ContextString, bool bWarnIfRowMissing = true);

    // 状态检测
    
    UFUNCTION(BlueprintPure, Category = "Player State")
    bool IsGrounding() const;

    UFUNCTION(BlueprintPure, Category = "Player State")
    bool IsWalking() const { return GetCurrentState() == ECharacterState::Walk; }

    // 状态设置
    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SetDashing(bool bDashing);
    
    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SetAttacking(bool bAttacking);
    
    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SetJumping(bool bJumping);

    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SetWalking(bool bWalking);

    UFUNCTION(BlueprintCallable, Category = "Key")
    void SetAttackKey(bool bAttackKey) { bIsAttackKeyReleased = bAttackKey; }

    UFUNCTION(BlueprintPure, Category = "Key")
    bool GetAttackKey() const { return bIsAttackKeyReleased; }

    UFUNCTION(BlueprintCallable, Category = "Player State")
    bool ShouldMove();

    UFUNCTION(BlueprintCallable, Category = "Player State")
    bool CanStartAttack() const;

    // Attack
    UFUNCTION(BlueprintCallable, Category = "Player State|Attack")
    int GetAttackNum() const { return AttackNum; }
    
    UFUNCTION(BlueprintCallable, Category = "Player State|Attack")
    void SetAttackNum(int Num) { AttackNum = Num; }

private:

    UPROPERTY(VisibleAnywhere, Category = "Player State")
    TWeakObjectPtr<ACurrsorPlayerController> CurrsorController;

    UPROPERTY(VisibleAnywhere, Category = "Player State")
    TWeakObjectPtr<ACurrsorCharacter> CurrsorCharacter;

    UPROPERTY(VisibleAnywhere, Category = "Player State")
    FVector CurrsorVelocity = FVector::ZeroVector;

    // 按键标志
    UPROPERTY(VisibleInstanceOnly, Category = "Player State")
    bool bIsAttackKeyReleased = false;

    UPROPERTY(VisibleInstanceOnly, Category = "Player State|Attack")
    int AttackNum = 0;
};