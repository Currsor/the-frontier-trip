#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CurrsorPlayerState.generated.h"

class ACurrsorCharacter;
class ACurrsorPlayerController;

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Walk        UMETA(DisplayName = "Walk"),
    Run         UMETA(DisplayName = "Run"),
    Jump        UMETA(DisplayName = "Jump"),
    Fall        UMETA(DisplayName = "Fall"),
    Attack      UMETA(DisplayName = "Attack"),
    RunAttack   UMETA(DisplayName = "RunAttack"),
    Dash        UMETA(DisplayName = "Dash")
};

UCLASS()
class CURRSOR_API ACurrsorPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ACurrsorPlayerState();

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

public:
    // 状态更新
    UFUNCTION(BlueprintCallable, Category = "Player State")
    void UpdateState();

    // 状态检测
    UFUNCTION(BlueprintPure, Category = "Player State")
    bool IsDashing() const;
    
    UFUNCTION(BlueprintPure, Category = "Player State")
    bool IsAttacking() const;
    
    UFUNCTION(BlueprintPure, Category = "Player State")
    bool IsJumping() const;
    
    UFUNCTION(BlueprintPure, Category = "Player State")
    bool IsFalling() const;

    // 状态设置
    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SetDashing(bool bDashing);
    
    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SetAttacking(bool bAttacking);
    
    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SetJumping(bool bJumping);

    // 获取当前状态
    UFUNCTION(BlueprintPure, Category = "Player State")
    EPlayerState GetCurrentState() const { return CurrentState; }

protected:
    // 状态变更核心逻辑
    void ChangeState(EPlayerState NewState);

    // 状态进入/退出函数
    void OnEnterDash(EPlayerState PreviousState);
    void OnExitDash();
    
    void OnEnterAttack(EPlayerState PreviousState);
    void OnExitAttack();
    
    // void OnEnterRunAttack(EPlayerState PreviousState);
    // void OnExitRunAttack();
    // 
    // void OnEnterJump(EPlayerState PreviousState);
    // void OnExitJump();
    // 
    // void OnEnterFall(EPlayerState PreviousState);
    // void OnExitFall();
    // 
    // void OnEnterRun(EPlayerState PreviousState);
    // void OnExitRun();
    // 
    // void OnEnterWalk(EPlayerState PreviousState);
    // void OnExitWalk();
    // 
    // void OnEnterIdle(EPlayerState PreviousState);
    // void OnExitIdle();

private:

    UPROPERTY(VisibleAnywhere, Category = "Player State")
    TWeakObjectPtr<ACurrsorPlayerController> CurrsorController;

    UPROPERTY(VisibleAnywhere, Category = "Player State")
    TWeakObjectPtr<ACurrsorCharacter> CurrsorCharacter;

    UPROPERTY(VisibleAnywhere, Category = "Player State")
    FVector CurrsorVelocity = FVector::ZeroVector;
    
    UPROPERTY(VisibleAnywhere, Category = "Player State")
    EPlayerState CurrentState = EPlayerState::Idle;
    
    UPROPERTY(EditDefaultsOnly, Category = "Player State|Thresholds")
    float WalkThreshold = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Player State|Thresholds")
    float RunThreshold = 600.0f;

    // 状态标志
    UPROPERTY(VisibleInstanceOnly, Category = "Player State")
    bool bIsDashing = false;

    UPROPERTY(VisibleInstanceOnly, Category = "Player State")
    bool bIsAttacking = false;

    UPROPERTY(VisibleInstanceOnly, Category = "Player State")
    bool bIsJumping = false;
};
