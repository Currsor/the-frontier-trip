"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorCharacter extends jsClass {
    Construct() {
        // 构造函数中可以添加其他初始化逻辑
        console.log("TS_CurrsorCharacter constructed");
    }
    BndEvt__BP_CurrsorCharacter_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult) {
        // 获取当前实例的PlayerState，添加空值检查
        const currentPlayerState = this.PlayerState;
        if (!currentPlayerState) {
            console.log("PlayerState is not available");
            return;
        }
        if (currentPlayerState.GetAttackNum() <= 0)
            return;
        const damageableInterface = UE.Damageable.StaticClass();
        const hasApplyDamageMethod = typeof OtherActor.ApplyDamage === 'function';
        if (hasApplyDamageMethod && damageableInterface && UE.KismetSystemLibrary.DoesImplementInterface(OtherActor, damageableInterface)) {
            // 设置伤害参数
            const damageAmount = 10.0;
            const damageInstigator = this;
            try {
                OtherActor.ApplyDamage(damageAmount, damageInstigator, SweepResult);
            }
            catch (error) {
                console.log(`Failed to apply damage: ${error}`);
            }
            // 减少攻击次数
            currentPlayerState.SetAttackNum(currentPlayerState.GetAttackNum() - 1);
        }
        else {
            console.log(`${OtherActor.GetName()} does not implement IDamageable interface`);
        }
    }
}
puerts_1.blueprint.mixin(jsClass, TS_CurrsorCharacter);
//# sourceMappingURL=TS_CurrsorCharacter.js.map