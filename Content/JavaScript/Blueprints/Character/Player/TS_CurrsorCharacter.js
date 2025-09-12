"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const GameSystemManager_1 = require("../../../GameSystemManager");
const StateManager_1 = require("../../../Managers/StateManager");
const EventSystem_1 = require("../../../Systems/EventSystem");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorCharacter extends jsClass {
    attackSystem;
    stateManager;
    gameLogicManager;
    isSystemsInitialized = false;
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
            // 获取系统实例
            this.attackSystem = GameSystemManager_1.gameSystemManager.getSystem("AttackSystem");
            this.stateManager = GameSystemManager_1.gameSystemManager.getSystem("StateManager");
            this.gameLogicManager = GameSystemManager_1.gameSystemManager.getSystem("GameLogicManager");
            this.isSystemsInitialized = true;
            console.log("TS_CurrsorCharacter: Game systems initialized successfully");
            // 设置事件监听
            this.setupEventListeners();
        }
        catch (error) {
            console.error("TS_CurrsorCharacter: Failed to initialize game systems:", error);
        }
    }
    setupEventListeners() {
        // 监听攻击命中事件
        EventSystem_1.EventSystem.subscribe("onAttackHit", (data) => {
            if (data.attacker === this) {
                console.log("TS: Character attack hit processed");
            }
        });
        // 监听状态变化事件
        EventSystem_1.EventSystem.subscribe("onStateChanged", (data) => {
            this.onStateChanged(data.newState, data.oldState);
        });
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
        console.log(`TS: Attack hitbox overlap with ${OtherActor.GetName()}`);
        // 使用新的架构系统处理攻击
        if (this.isSystemsInitialized) {
            this.processAttackHitWithNewSystem(OtherActor, SweepResult, currentPlayerState);
        }
        else {
            // 回退到原有逻辑
            this.processAttackHitLegacy(OtherActor, SweepResult, currentPlayerState);
        }
    }
    processAttackHitWithNewSystem(OtherActor, SweepResult, currentPlayerState) {
        // 使用攻击系统处理攻击命中
        const hitProcessed = this.attackSystem.processAttackHit(this, OtherActor, SweepResult);
        if (hitProcessed) {
            console.log("TS: Attack hit processed by new system");
            // 减少攻击次数
            currentPlayerState.SetAttackNum(currentPlayerState.GetAttackNum() - 1);
            // 广播攻击命中事件
            EventSystem_1.EventSystem.emit("onCharacterAttackHit", {
                attacker: this,
                target: OtherActor,
                hitResult: SweepResult,
                timestamp: Date.now()
            });
        }
        else {
            console.log("TS: Attack hit failed validation");
        }
    }
    processAttackHitLegacy(OtherActor, SweepResult, currentPlayerState) {
        const damageableInterface = UE.Damageable.StaticClass();
        const hasApplyDamageMethod = typeof OtherActor.ApplyDamage === 'function';
        if (hasApplyDamageMethod && damageableInterface && UE.KismetSystemLibrary.DoesImplementInterface(OtherActor, damageableInterface)) {
            // 使用游戏逻辑管理器计算伤害（如果可用）
            let damageAmount = 10.0;
            let isCritical = false;
            if (this.gameLogicManager) {
                isCritical = this.gameLogicManager.calculateCriticalHit();
                damageAmount = this.gameLogicManager.calculateDamage(10.0, 1, 1.0, isCritical);
            }
            const damageInstigator = this;
            try {
                OtherActor.ApplyDamage(damageAmount, damageInstigator, SweepResult);
                console.log(`TS: Applied ${damageAmount} damage to ${OtherActor.GetName()}${isCritical ? ' (CRITICAL!)' : ''}`);
                // 广播伤害应用事件
                EventSystem_1.EventSystem.emit("onDamageApplied", {
                    target: OtherActor,
                    damage: damageAmount,
                    instigator: damageInstigator,
                    isCritical: isCritical,
                    hitResult: SweepResult,
                    timestamp: Date.now()
                });
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
    // 状态变化处理
    onStateChanged(newState, oldState) {
        console.log(`TS: Character state changed from ${oldState} to ${newState}`);
        // 根据状态变化执行相应的逻辑
        switch (newState) {
            case StateManager_1.StateManager.STATES.ATTACK:
            case StateManager_1.StateManager.STATES.RUN_ATTACK:
                this.onEnterAttackState(oldState);
                break;
            case StateManager_1.StateManager.STATES.HURT:
                this.onEnterHurtState(oldState);
                break;
            case StateManager_1.StateManager.STATES.DEAD:
                this.onEnterDeadState(oldState);
                break;
            case StateManager_1.StateManager.STATES.DASH:
                this.onEnterDashState(oldState);
                break;
        }
        // 广播角色状态变化事件
        EventSystem_1.EventSystem.emit("onCharacterStateChanged", {
            character: this,
            newState: newState,
            oldState: oldState,
            timestamp: Date.now()
        });
    }
    // 进入攻击状态
    onEnterAttackState(previousState) {
        console.log("TS: Character entered attack state");
        EventSystem_1.EventSystem.emit("onCharacterStartAttack", {
            character: this,
            previousState: previousState
        });
    }
    // 进入受伤状态
    onEnterHurtState(previousState) {
        console.log("TS: Character entered hurt state");
        EventSystem_1.EventSystem.emit("onCharacterHurt", {
            character: this,
            previousState: previousState
        });
    }
    // 进入死亡状态
    onEnterDeadState(previousState) {
        console.log("TS: Character entered dead state");
        EventSystem_1.EventSystem.emit("onCharacterDeath", {
            character: this,
            previousState: previousState
        });
    }
    // 进入冲刺状态
    onEnterDashState(previousState) {
        console.log("TS: Character entered dash state");
        EventSystem_1.EventSystem.emit("onCharacterDash", {
            character: this,
            previousState: previousState
        });
    }
    // 获取角色状态信息（供调试使用）
    getCharacterInfo() {
        return {
            name: this.GetName(),
            state: this.stateManager ? this.stateManager.getCurrentState() : "Unknown",
            isSystemsInitialized: this.isSystemsInitialized,
            location: this.K2_GetActorLocation()
        };
    }
    // 手动触发攻击（用于测试）
    triggerAttack() {
        if (this.attackSystem) {
            EventSystem_1.EventSystem.emit("onAttackInput", {
                attacker: this,
                inputType: "manual",
                timestamp: Date.now()
            });
        }
    }
    // 重置角色状态
    resetCharacter() {
        if (this.stateManager) {
            this.stateManager.reset();
        }
        console.log("TS_CurrsorCharacter reset");
    }
}
puerts_1.blueprint.mixin(jsClass, TS_CurrsorCharacter);
//# sourceMappingURL=TS_CurrsorCharacter.js.map