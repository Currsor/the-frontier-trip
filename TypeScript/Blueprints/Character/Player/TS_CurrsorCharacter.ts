import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = blueprint.tojs<typeof UE.Game.Blueprints.Character.Player.BP_CurrsorCharacter.BP_CurrsorCharacter_C>(uclass);

class TS_CurrsorCharacter extends jsClass {

    Construct() {
        // 构造函数中可以添加其他初始化逻辑
        console.log("TS_CurrsorCharacter constructed");
    }

    BndEvt__BP_CurrsorCharacter_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent: UE.PrimitiveComponent, OtherActor: UE.Actor, OtherComp: UE.PrimitiveComponent, OtherBodyIndex: number, bFromSweep: boolean, SweepResult: UE.HitResult): void {
        // 获取当前实例的PlayerState，添加空值检查
        const currentPlayerState = this.PlayerState as UE.CurrsorPlayerState;
        if (!currentPlayerState) {
            console.log("PlayerState is not available");
            return;
        }

        if (currentPlayerState.GetAttackNum() <= 0) return;
        
        const damageableInterface = UE.Damageable.StaticClass();

        const hasApplyDamageMethod = typeof (OtherActor as any).ApplyDamage === 'function';

        if (hasApplyDamageMethod && damageableInterface && UE.KismetSystemLibrary.DoesImplementInterface(OtherActor, damageableInterface)) {
            // 设置伤害参数
            const damageAmount = 10.0;
            const damageInstigator = this;

            try {
                (OtherActor as any).ApplyDamage(damageAmount, damageInstigator, SweepResult);
            } catch (error) {
                console.log(`Failed to apply damage: ${error}`);
            }
            
            // 减少攻击次数
            currentPlayerState.SetAttackNum(currentPlayerState.GetAttackNum() - 1);
        } else {
            console.log(`${OtherActor.GetName()} does not implement IDamageable interface`);
        }
    }
}

blueprint.mixin(jsClass, TS_CurrsorCharacter);