// Fill out your copyright notice in the Description page of Project Settings.

#include "DestructibleItem.h"
#include "Currsor/Component/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

ADestructibleItem::ADestructibleItem()
{
    PrimaryActorTick.bCanEverTick = false;

    // 创建生命值组件
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    
    // 设置默认值
    if (HealthComponent)
    {
        HealthComponent->SetMaxHealth(50.0f);
    }

    // 设置碰撞配置以支持攻击检测
    if (UStaticMeshComponent* MeshComp = GetStaticMeshComponent())
    {
        // 设置碰撞启用状态 - 支持查询和物理
        MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        
        // 设置为WorldStatic对象类型
        MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
        
        // 重要：确保能被重叠检测到
        MeshComp->SetGenerateOverlapEvents(true);
        
        // 设置碰撞响应 - 对所有通道都允许重叠
        MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
        
        // 确保对Pawn通道（攻击碰撞盒可能使用的通道）响应重叠
        MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
        MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
        
        UE_LOG(LogTemp, Warning, TEXT("DestructibleItem %s collision configured for attack detection"), *GetName());
    }
}

void ADestructibleItem::BeginPlay()
{
    Super::BeginPlay();
    
    // 绑定生命值组件的死亡事件
    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &ADestructibleItem::OnHealthDepleted);
    }

    // 验证接口实现
    if (this->Implements<UDamageable>())
    {
        UE_LOG(LogTemp, Warning, TEXT("DestructibleItem %s correctly implements IDamageable interface"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DestructibleItem %s does NOT implement IDamageable interface!"), *GetName());
    }

    // 输出碰撞配置信息
    if (UStaticMeshComponent* MeshComp = GetStaticMeshComponent())
    {
        UE_LOG(LogTemp, Warning, TEXT("DestructibleItem %s collision settings: Enabled=%d, GenerateOverlapEvents=%d"), 
               *GetName(), 
               (int32)MeshComp->GetCollisionEnabled(),
               MeshComp->GetGenerateOverlapEvents());
    }
}

void ADestructibleItem::ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult)
{
    // 调用接口的默认实现
    IDamageable::ApplyDamage_Implementation(DamageAmount, DamageInstigator, HitResult);

    UE_LOG(LogTemp, Warning, TEXT("DestructibleItem %s ApplyDamage called! Damage: %f, Instigator: %s"), 
           *GetName(), 
           DamageAmount, 
           DamageInstigator ? *DamageInstigator->GetName() : TEXT("Unknown"));

    // 如果已经被破坏，不再处理伤害
    if (bIsDestroyed)
    {
        UE_LOG(LogTemp, Warning, TEXT("DestructibleItem %s is already destroyed, ignoring damage"), *GetName());
        return;
    }

    if (!HealthComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("DestructibleItem %s has no HealthComponent!"), *GetName());
        return;
    }

    // 应用伤害到生命值组件
    HealthComponent->TakeDamage(DamageAmount, DamageInstigator);

    UE_LOG(LogTemp, Warning, TEXT("DestructibleItem %s took %f damage from %s. Health: %f/%f"), 
           *GetName(), 
           DamageAmount, 
           DamageInstigator ? *DamageInstigator->GetName() : TEXT("Unknown"),
           HealthComponent->GetCurrentHealth(),
           HealthComponent->GetMaxHealth());
}

void ADestructibleItem::DestroyItem()
{
    if (bIsDestroyed)
    {
        return;
    }

    // 直接设置生命值为0来触发破坏
    if (HealthComponent)
    {
        HealthComponent->SetCurrentHealth(0.0f);
    }
    else
    {
        // 如果没有生命值组件，直接处理破坏
        HandleDestruction();
    }
}

void ADestructibleItem::OnHealthDepleted(AActor* DeadActor)
{
    if (bIsDestroyed)
    {
        return;
    }

    HandleDestruction();
}

void ADestructibleItem::HandleDestruction_Implementation()
{
    if (bIsDestroyed)
    {
        return;
    }

    bIsDestroyed = true;

    // 播放破坏效果
    PlayDestructionEffects();

    // 生成掉落物
    SpawnLoot();

    // 广播破坏事件
    OnItemDestroyed.Broadcast(this);
    OnItemDestroyedBP();

    // 禁用碰撞
    if (GetStaticMeshComponent())
    {
        GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // 延迟销毁Actor
    if (bDestroyActorOnDeath)
    {
        if (DestroyDelay > 0.0f)
        {
            FTimerHandle DestroyTimer;
            GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this]()
            {
                Destroy();
            }, DestroyDelay, false);
        }
        else
        {
            Destroy();
        }
    }

    UE_LOG(LogTemp, Log, TEXT("%s has been destroyed"), *GetName());
}

void ADestructibleItem::SpawnLoot_Implementation()
{
    if (DropItemClasses.Num() == 0)
    {
        return;
    }

    // 计算掉落数量
    int32 DropCount = FMath::RandRange(MinDropCount, MaxDropCount);
    
    for (int32 i = 0; i < DropCount; i++)
    {
        // 随机选择掉落物品类型
        int32 RandomIndex = FMath::RandRange(0, DropItemClasses.Num() - 1);
        TSubclassOf<AActor> DropClass = DropItemClasses[RandomIndex];
        
        if (!DropClass)
        {
            continue;
        }

        // 计算随机掉落位置
        FVector DropLocation = GetActorLocation();
        FVector RandomOffset = FVector(
            FMath::RandRange(-DropRadius, DropRadius),
            FMath::RandRange(-DropRadius, DropRadius),
            0.0f
        );
        DropLocation += RandomOffset;

        // 生成掉落物
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
        AActor* DroppedItem = GetWorld()->SpawnActor<AActor>(DropClass, DropLocation, GetActorRotation(), SpawnParams);
        
        if (DroppedItem)
        {
            UE_LOG(LogTemp, Log, TEXT("Spawned loot: %s at location %s"), 
                   *DroppedItem->GetName(), 
                   *DropLocation.ToString());
        }
    }
}

void ADestructibleItem::PlayDestructionEffects_Implementation()
{
    FVector EffectLocation = GetActorLocation();

    // 播放粒子效果
    if (DestructionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            DestructionEffect,
            EffectLocation,
            GetActorRotation(),
            EffectScale
        );
    }

    // 播放音效
    if (DestructionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            DestructionSound,
            EffectLocation
        );
    }
}