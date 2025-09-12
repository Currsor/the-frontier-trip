#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "AreaCollisionBox.generated.h"

UCLASS()
class CURRSOR_API AAreaCollisionBox : public AActor
{
    GENERATED_BODY()

public:
    AAreaCollisionBox(const FObjectInitializer& ObjectInitializer);
    AAreaCollisionBox(const FObjectInitializer& ObjectInitializer, const FVector& InitialLocation);
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* BoxCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currsor|Area")
    FString Name;

    UFUNCTION(BlueprintCallable, Category = "Currsor|Area")
    void SetAreaID(int32 InAreaID) { AreaID = InAreaID; }
    
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                       bool bFromSweep, const FHitResult& SweepResult);
    
    void UpdateSymmetricBillboard(USceneComponent* UpdatedComponent, 
                                 EUpdateTransformFlags UpdateTransformFlags, 
                                 ETeleportType Teleport);

private:
    UPROPERTY(VisibleAnywhere)
    int32 AreaID;

    // ========== Category声明 ==========
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBillboardComponent* MainBattleBillboard;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBillboardComponent* PlayerBillboard;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBillboardComponent* EnemyBillboard;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBillboardComponent* CameraBillboard;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* PlayerArrow;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* EnemyArrow;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* CameraArrow;

    // ========== 加载 ==========
    UPROPERTY(EditAnywhere, Category = "Currsor|Billboard")
    TSoftObjectPtr<UTexture2D> MainTexture;

    UPROPERTY(EditAnywhere, Category = "Currsor|Billboard")
    TSoftObjectPtr<UTexture2D> PlayerTexture;

    UPROPERTY(EditAnywhere, Category = "Currsor|Billboard")
    TSoftObjectPtr<UTexture2D> EnemyTexture;

    UPROPERTY(EditAnywhere, Category = "Currsor|Billboard")
    TSoftObjectPtr<UTexture2D> CameraTexture;

    UFUNCTION()
    void SetupBillboards();
};