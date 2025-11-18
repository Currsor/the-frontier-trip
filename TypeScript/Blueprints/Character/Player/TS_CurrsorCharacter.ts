import * as UE from 'ue';
import { $Ref, $ref, $set, $unref, blueprint } from 'puerts';
import { gameSystemManager } from '../../../GameSystemManager';
import { AttackSystem } from '../../../Systems/AttackSystem';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_CurrsorCharacter extends UE.Game.Blueprints.Character.Player.BP_CurrsorCharacter.BP_CurrsorCharacter_C {}

export class TS_CurrsorCharacter implements TS_CurrsorCharacter {
    private attackSystem!: AttackSystem;

    Construct() {
        // 构造函数中可以添加其他初始化逻辑
        console.log("TS_CurrsorCharacter constructed");
        
        // 初始化游戏系统
        this.initializeGameSystems();
    }

    private initializeGameSystems(): void {
        try {
            // 确保游戏系统管理器已初始化
            if (!gameSystemManager.isSystemInitialized("AttackSystem")) {
                gameSystemManager.initialize();
            }

            this.attackSystem = gameSystemManager.getSystem("AttackSystem");

        } catch (error) {
            console.error("[TS Character] 游戏系统初始化失败：", error);
        }
    }

    BndEvt__BP_CurrsorCharacter_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent: UE.PrimitiveComponent, OtherActor: UE.Actor, OtherComp: UE.PrimitiveComponent, OtherBodyIndex: number, bFromSweep: boolean, SweepResult: UE.HitResult): void {
        console.log(`[TS Character] 攻击判定框重叠 ${OtherActor.GetName()}`);
        
        // 直接转发到 C++ 攻击系统处理
        if (this.attackSystem) {
            this.attackSystem.processAttackHit(this, OtherActor, SweepResult);
        } else {
            console.error("[TS Character] 攻击系统不可用");
        }
    }

    // 获取卡牌数据
    public GetDataFromName(RowName: string): UE.Game.Data.Structs.S_CardInfo.S_CardInfo {
        let cardInfo = {} as UE.Game.Data.Structs.S_CardInfo.S_CardInfo;
        cardInfo = this.BP_GetDataFromName(RowName);
        return cardInfo;
    }
}

blueprint.mixin(jsClass, TS_CurrsorCharacter);