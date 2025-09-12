import { Actor } from "ue";
import { GameLogicManager } from "../Managers/GameLogicManager";
import { StateManager } from "../Managers/StateManager";
import { EventSystem } from "../Systems/EventSystem";
import { GameConfig } from "../Config/GameConfig";

/**
 * 攻击系统
 * 处理攻击逻辑、伤害计算和攻击效果
 */
export class AttackSystem {
    private static instance: AttackSystem;
    private gameLogicManager: GameLogicManager;
    private stateManager: StateManager;
    private lastAttackTime: number = 0;
    private attackComboCount: number = 0;
    private attackComboResetTime: number = 2000; // 2秒后重置连击

    public static getInstance(): AttackSystem {
        if (!AttackSystem.instance) {
            AttackSystem.instance = new AttackSystem();
        }
        return AttackSystem.instance;
    }

    private constructor() {
        this.gameLogicManager = GameLogicManager.getInstance();
        this.stateManager = StateManager.getInstance();
        this.setupEventListeners();
        console.log("AttackSystem initialized");
    }

    /**
     * 设置事件监听器
     */
    private setupEventListeners(): void {
        EventSystem.subscribe("onAttackInput", (data: any) => {
            this.handleAttackInput(data.attacker);
        });

        EventSystem.subscribe("onAttackHitboxActivated", (data: any) => {
            this.handleAttackHitboxActivation(data.attacker);
        });

        EventSystem.subscribe("onAttackEnd", (data: any) => {
            this.handleAttackEnd(data.attacker);
        });
    }

    /**
     * 处理攻击输入
     * @param attacker 攻击者
     * @returns 是否成功开始攻击
     */
    public handleAttackInput(attacker: Actor): boolean {
        if (!this.canStartAttack(attacker)) {
            return false;
        }

        // 检查攻击冷却
        const currentTime = Date.now();
        if (currentTime - this.lastAttackTime < GameConfig.ATTACK_COOLDOWN * 1000) {
            console.log("Attack on cooldown");
            return false;
        }

        // 开始攻击
        return this.startAttack(attacker);
    }

    /**
     * 检查是否可以开始攻击
     * @param attacker 攻击者
     * @returns 是否可以攻击
     */
    private canStartAttack(attacker: Actor): boolean {
        if (!attacker || !(attacker as any).IsValidLowLevel()) {
            return false;
        }

        // 检查状态是否允许攻击
        const currentState = this.stateManager.getCurrentState();
        if (this.stateManager.isDead() || this.stateManager.isHurt()) {
            return false;
        }

        // 检查是否可以转换到攻击状态
        const targetState = this.stateManager.isMoving() ? 
            StateManager.STATES.RUN_ATTACK : StateManager.STATES.ATTACK;
        
        return this.stateManager.canTransitionTo(currentState, targetState);
    }

    /**
     * 开始攻击
     * @param attacker 攻击者
     * @returns 是否成功开始
     */
    private startAttack(attacker: Actor): boolean {
        // 确定攻击状态
        const attackState = this.stateManager.isMoving() ? 
            StateManager.STATES.RUN_ATTACK : StateManager.STATES.ATTACK;

        // 转换状态
        if (!this.stateManager.changeState(attackState)) {
            return false;
        }

        // 更新攻击时间和连击数
        this.lastAttackTime = Date.now();
        this.updateComboCount();

        // 广播攻击开始事件
        EventSystem.emit("onAttackStarted", {
            attacker: attacker,
            attackType: attackState,
            comboCount: this.attackComboCount,
            timestamp: this.lastAttackTime
        });

        console.log(`Attack started: ${attacker.GetName()}, Type: ${attackState}, Combo: ${this.attackComboCount}`);
        return true;
    }

    /**
     * 处理攻击碰撞盒激活
     * @param attacker 攻击者
     */
    public handleAttackHitboxActivation(attacker: Actor): void {
        console.log(`Attack hitbox activated for ${attacker.GetName()}`);
        
        // 广播碰撞盒激活事件
        EventSystem.emit("onAttackHitboxActive", {
            attacker: attacker,
            timestamp: Date.now()
        });
    }

    /**
     * 处理攻击命中
     * @param attacker 攻击者
     * @param target 目标
     * @param hitResult 命中结果
     * @returns 处理结果
     */
    public processAttackHit(attacker: Actor, target: Actor, hitResult?: any): boolean {
        if (!this.gameLogicManager.validateAttackConditions(attacker, target)) {
            return false;
        }

        // 计算伤害
        const baseDamage = this.calculateBaseDamage(attacker);
        const isCritical = this.gameLogicManager.calculateCriticalHit();
        const finalDamage = this.gameLogicManager.calculateDamage(
            baseDamage, 
            this.getAttackerLevel(attacker), 
            this.getWeaponMultiplier(attacker),
            isCritical
        );

        // 应用伤害
        this.applyDamage(target, finalDamage, attacker, hitResult);

        // 处理攻击结果
        this.gameLogicManager.processAttackResult(attacker, target, finalDamage, isCritical);

        // 处理攻击反馈
        this.handleAttackFeedback(attacker, target, finalDamage, isCritical);

        return true;
    }

    /**
     * 计算基础伤害
     * @param attacker 攻击者
     * @returns 基础伤害值
     */
    private calculateBaseDamage(attacker: Actor): number {
        // 基础伤害可以根据武器、等级等因素计算
        let baseDamage = 20; // 默认基础伤害

        // 连击加成
        if (this.attackComboCount > 1) {
            baseDamage *= (1 + (this.attackComboCount - 1) * 0.1);
        }

        // 攻击类型加成
        if (this.stateManager.getCurrentState() === StateManager.STATES.RUN_ATTACK) {
            baseDamage *= 1.2; // 跑攻增加20%伤害
        }

        return baseDamage;
    }

    /**
     * 获取攻击者等级
     * @param attacker 攻击者
     * @returns 等级
     */
    private getAttackerLevel(attacker: Actor): number {
        // 这里可以从角色组件获取等级
        return 1; // 默认等级
    }

    /**
     * 获取武器倍率
     * @param attacker 攻击者
     * @returns 武器倍率
     */
    private getWeaponMultiplier(attacker: Actor): number {
        // 这里可以从武器组件获取倍率
        return 1.0; // 默认倍率
    }

    /**
     * 应用伤害
     * @param target 目标
     * @param damage 伤害值
     * @param instigator 伤害来源
     * @param hitResult 命中结果
     */
    private applyDamage(target: Actor, damage: number, instigator: Actor, hitResult?: any): void {
        // 检查目标是否实现了IDamageable接口
        // 这里需要调用C++的接口方法
        console.log(`Applying ${damage} damage to ${target.GetName()}`);
        
        // 广播伤害应用事件
        EventSystem.emit("onDamageApplied", {
            target: target,
            damage: damage,
            instigator: instigator,
            hitResult: hitResult,
            timestamp: Date.now()
        });
    }

    /**
     * 处理攻击反馈
     * @param attacker 攻击者
     * @param target 目标
     * @param damage 伤害值
     * @param isCritical 是否暴击
     */
    private handleAttackFeedback(attacker: Actor, target: Actor, damage: number, isCritical: boolean): void {
        // 屏幕震动
        if (GameConfig.EFFECT_CONFIG.SCREEN_SHAKE_INTENSITY > 0) {
            EventSystem.emit("onScreenShake", {
                intensity: GameConfig.EFFECT_CONFIG.SCREEN_SHAKE_INTENSITY,
                duration: 0.2
            });
        }

        // 伤害数字显示
        if (GameConfig.DEBUG_CONFIG.SHOW_DAMAGE_NUMBERS) {
            EventSystem.emit("onShowDamageNumber", {
                target: target,
                damage: damage,
                isCritical: isCritical,
                position: target.K2_GetActorLocation()
            });
        }

        // 音效播放
        EventSystem.emit("onPlayAttackSound", {
            attacker: attacker,
            target: target,
            isCritical: isCritical
        });

        // 粒子效果
        EventSystem.emit("onPlayAttackEffect", {
            attacker: attacker,
            target: target,
            hitPosition: target.K2_GetActorLocation(),
            isCritical: isCritical
        });
    }

    /**
     * 处理攻击结束
     * @param attacker 攻击者
     */
    public handleAttackEnd(attacker: Actor): void {
        console.log(`Attack ended for ${attacker.GetName()}`);

        // 检查是否需要继续攻击（连击）
        if (this.shouldContinueAttack()) {
            // 继续攻击逻辑
            return;
        }

        // 返回到默认状态
        this.stateManager.changeState(StateManager.STATES.IDLE);

        // 广播攻击结束事件
        EventSystem.emit("onAttackCompleted", {
            attacker: attacker,
            comboCount: this.attackComboCount,
            timestamp: Date.now()
        });
    }

    /**
     * 检查是否应该继续攻击
     * @returns 是否继续攻击
     */
    private shouldContinueAttack(): boolean {
        // 这里可以检查输入缓冲、连击条件等
        return false;
    }

    /**
     * 更新连击数
     */
    private updateComboCount(): void {
        const currentTime = Date.now();
        
        // 如果距离上次攻击时间过长，重置连击
        if (currentTime - this.lastAttackTime > this.attackComboResetTime) {
            this.attackComboCount = 1;
        } else {
            this.attackComboCount++;
        }

        // 限制最大连击数
        this.attackComboCount = Math.min(this.attackComboCount, 5);
    }

    /**
     * 重置攻击系统
     */
    public reset(): void {
        this.lastAttackTime = 0;
        this.attackComboCount = 0;
        console.log("AttackSystem reset");
    }

    /**
     * 获取攻击统计信息
     */
    public getAttackStats(): any {
        return {
            lastAttackTime: this.lastAttackTime,
            attackComboCount: this.attackComboCount,
            timeSinceLastAttack: Date.now() - this.lastAttackTime
        };
    }

    /**
     * 设置攻击参数
     * @param params 参数对象
     */
    public setAttackParams(params: {
        comboResetTime?: number;
    }): void {
        if (params.comboResetTime !== undefined) {
            this.attackComboResetTime = params.comboResetTime;
        }
    }
}