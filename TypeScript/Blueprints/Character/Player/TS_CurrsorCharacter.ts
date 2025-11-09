import * as UE from 'ue';
import { $Ref, $ref, $set, $unref, blueprint } from 'puerts';
import { gameSystemManager } from '../../../GameSystemManager';
import { AttackSystem } from '../../../Systems/AttackSystem';
import { EventSystem } from '../../../Systems/EventSystem';
import { GameLogicManager } from '../../../Managers/GameLogicManager';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_CurrsorCharacter extends UE.Game.Blueprints.Character.Player.BP_CurrsorCharacter.BP_CurrsorCharacter_C {}

export class TS_CurrsorCharacter implements TS_CurrsorCharacter {
    private attackSystem!: AttackSystem;
    private gameLogicManager!: GameLogicManager;
    private isSystemsInitialized: boolean = false;

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

            // 获取系统实例
            this.attackSystem = gameSystemManager.getSystem("AttackSystem");
            this.gameLogicManager = gameSystemManager.getSystem("GameLogicManager");
            
            this.isSystemsInitialized = true;
            console.log("TS_CurrsorCharacter: Game systems initialized successfully");

            // 设置事件监听
            this.setupEventListeners();

        } catch (error) {
            console.error("TS_CurrsorCharacter: Failed to initialize game systems:", error);
        }
    }

    private setupEventListeners(): void {
        // 所有攻击逻辑和事件都在 C++ 中处理
        // TypeScript 仅监听必要的状态变化事件
        
        EventSystem.subscribe("onStateChanged", (data: any) => {
            this.onStateChanged(data.newState, data.oldState);
        });
    }

    BndEvt__BP_CurrsorCharacter_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent: UE.PrimitiveComponent, OtherActor: UE.Actor, OtherComp: UE.PrimitiveComponent, OtherBodyIndex: number, bFromSweep: boolean, SweepResult: UE.HitResult): void {
        console.log(`[TS Character] Attack hitbox overlap with ${OtherActor.GetName()} - Forwarding to C++`);
        
        // 直接转发到 C++ 攻击系统处理
        // 不在 TypeScript 中进行任何攻击逻辑处理
        if (this.attackSystem) {
            this.attackSystem.processAttackHit(this, OtherActor, SweepResult);
        } else {
            console.error("[TS Character] AttackSystem not available - cannot process attack");
        }
    }

    // 状态变化处理 - 仅记录和广播，不处理攻击逻辑
    private onStateChanged(newState: string, oldState: string): void {
        console.log(`[TS Character] State changed: ${oldState} -> ${newState} (Logic handled in C++)`);
        
        // 仅广播状态变化事件，不进行任何攻击逻辑处理
        EventSystem.emit("onCharacterStateChanged", {
            character: this,
            newState: newState,
            oldState: oldState,
            timestamp: Date.now()
        });
    }

    // 获取角色基本信息（供调试使用）
    public getCharacterInfo(): any {
        return {
            name: this.GetName(),
            note: "All logic handled in C++",
            location: this.K2_GetActorLocation()
        };
    }

    // 手动触发攻击（用于测试）- 纯粹转发到 C++
    public triggerAttack(): void {
        console.log("[TS Character] Triggering attack - forwarding to C++");
        if (this.attackSystem) {
            this.attackSystem.handleAttackInput(this);
        } else {
            console.error("[TS Character] AttackSystem not available");
        }
    }

    public GetDataFromName(RowName: string): UE.Game.Data.Structs.S_CardInfo.S_CardInfo {
        let cardInfo = {} as UE.Game.Data.Structs.S_CardInfo.S_CardInfo;

        cardInfo = this.BP_GetDataFromName(RowName);

        return cardInfo;
    }
}

blueprint.mixin(jsClass, TS_CurrsorCharacter);