"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_CurrsorCharacter = void 0;
const UE = require("ue");
const puerts_1 = require("puerts");
const GameSystemManager_1 = require("../../../GameSystemManager");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorCharacter {
    attackSystem;
    Construct() {
        // 构造函数中可以添加其他初始化逻辑
        console.log("TS_CurrsorCharacter constructed");
        // 初始化游戏系统
        this.initializeGameSystems();
    }
    initializeGameSystems() {
        try {
            // 确保游戏系统管理器已初始化
            if (!GameSystemManager_1.gameSystemManager.isSystemInitialized("AttackSystem")) {
                GameSystemManager_1.gameSystemManager.initialize();
            }
            this.attackSystem = GameSystemManager_1.gameSystemManager.getSystem("AttackSystem");
        }
        catch (error) {
            console.error("[TS Character] 游戏系统初始化失败：", error);
        }
    }
    BndEvt__BP_CurrsorCharacter_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult) {
        console.log(`[TS Character] 攻击判定框重叠 ${OtherActor.GetName()}`);
        // 直接转发到 C++ 攻击系统处理
        if (this.attackSystem) {
            this.attackSystem.processAttackHit(this, OtherActor, SweepResult);
        }
        else {
            console.error("[TS Character] 攻击系统不可用");
        }
    }
    // 获取卡牌数据
    GetDataFromName(RowName) {
        let cardInfo = {};
        cardInfo = this.BP_GetDataFromName(RowName);
        return cardInfo;
    }
}
exports.TS_CurrsorCharacter = TS_CurrsorCharacter;
puerts_1.blueprint.mixin(jsClass, TS_CurrsorCharacter);
//# sourceMappingURL=TS_CurrsorCharacter.js.map