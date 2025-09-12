import { Actor } from "ue";
import { GameConfig } from "../Config/GameConfig";
import { EventSystem } from "../Systems/EventSystem";

/**
 * 状态管理器
 * 处理角色状态转换逻辑和状态验证
 */
export class StateManager {
    private static instance: StateManager;
    private currentState: string = "Idle";
    private previousState: string = "Idle";
    private stateStartTime: number = 0;
    private stateFlags: Map<string, boolean> = new Map();

    // 状态枚举映射
    static readonly STATES = {
        IDLE: "Idle",
        WALK: "Walk", 
        RUN: "Run",
        JUMP: "Jump",
        FALL: "Fall",
        ATTACK: "Attack",
        RUN_ATTACK: "RunAttack",
        DASH: "Dash",
        HURT: "Hurt",
        DEAD: "Dead"
    };

    public static getInstance(): StateManager {
        if (!StateManager.instance) {
            StateManager.instance = new StateManager();
        }
        return StateManager.instance;
    }

    private constructor() {
        this.initializeStateFlags();
        console.log("StateManager initialized");
    }

    /**
     * 初始化状态标志
     */
    private initializeStateFlags(): void {
        this.stateFlags.set("isDashing", false);
        this.stateFlags.set("isAttacking", false);
        this.stateFlags.set("isJumping", false);
        this.stateFlags.set("isWalking", false);
        this.stateFlags.set("isHurt", false);
        this.stateFlags.set("isDead", false);
    }

    /**
     * 检查是否可以转换到目标状态
     * @param fromState 当前状态
     * @param toState 目标状态
     * @returns 是否可以转换
     */
    public canTransitionTo(fromState: string, toState: string): boolean {
        // 死亡状态不能转换到其他状态（除非复活）
        if (fromState === StateManager.STATES.DEAD && toState !== StateManager.STATES.IDLE) {
            return false;
        }

        // 使用配置中的优先级系统
        return GameConfig.canTransitionState(fromState, toState);
    }

    /**
     * 获取状态转换优先级
     * @param state 状态名称
     * @returns 优先级数值
     */
    public getStateTransitionPriority(state: string): number {
        return GameConfig.getStatePriority(state);
    }

    /**
     * 执行状态转换
     * @param newState 新状态
     * @param force 是否强制转换
     * @returns 是否转换成功
     */
    public changeState(newState: string, force: boolean = false): boolean {
        if (!force && !this.canTransitionTo(this.currentState, newState)) {
            console.log(`State transition denied: ${this.currentState} -> ${newState}`);
            return false;
        }

        const oldState = this.currentState;
        this.previousState = oldState;
        this.currentState = newState;
        this.stateStartTime = Date.now();

        // 处理状态退出逻辑
        this.handleStateExit(oldState);
        
        // 处理状态进入逻辑
        this.handleStateEnter(newState, oldState);

        // 广播状态变化事件
        EventSystem.emit("onStateChanged", {
            oldState: oldState,
            newState: newState,
            timestamp: this.stateStartTime
        });

        console.log(`State changed: ${oldState} -> ${newState}`);
        return true;
    }

    /**
     * 处理状态进入逻辑
     * @param state 进入的状态
     * @param previousState 之前的状态
     */
    private handleStateEnter(state: string, previousState: string): void {
        // 更新状态标志
        this.updateStateFlags(state, true);

        switch (state) {
            case StateManager.STATES.ATTACK:
                this.onEnterAttack(previousState);
                break;
            case StateManager.STATES.DASH:
                this.onEnterDash(previousState);
                break;
            case StateManager.STATES.WALK:
                this.onEnterWalk(previousState);
                break;
            case StateManager.STATES.HURT:
                this.onEnterHurt(previousState);
                break;
            case StateManager.STATES.DEAD:
                this.onEnterDead(previousState);
                break;
        }

        EventSystem.emit(`onEnter${state}`, { previousState });
    }

    /**
     * 处理状态退出逻辑
     * @param state 退出的状态
     */
    private handleStateExit(state: string): void {
        // 更新状态标志
        this.updateStateFlags(state, false);

        switch (state) {
            case StateManager.STATES.ATTACK:
                this.onExitAttack();
                break;
            case StateManager.STATES.DASH:
                this.onExitDash();
                break;
            case StateManager.STATES.WALK:
                this.onExitWalk();
                break;
        }

        EventSystem.emit(`onExit${state}`, {});
    }

    /**
     * 更新状态标志
     * @param state 状态
     * @param isEntering 是否进入状态
     */
    private updateStateFlags(state: string, isEntering: boolean): void {
        switch (state) {
            case StateManager.STATES.ATTACK:
            case StateManager.STATES.RUN_ATTACK:
                this.stateFlags.set("isAttacking", isEntering);
                break;
            case StateManager.STATES.DASH:
                this.stateFlags.set("isDashing", isEntering);
                break;
            case StateManager.STATES.WALK:
            case StateManager.STATES.RUN:
                this.stateFlags.set("isWalking", isEntering);
                break;
            case StateManager.STATES.JUMP:
                this.stateFlags.set("isJumping", isEntering);
                break;
            case StateManager.STATES.HURT:
                this.stateFlags.set("isHurt", isEntering);
                break;
            case StateManager.STATES.DEAD:
                this.stateFlags.set("isDead", isEntering);
                break;
        }
    }

    // 状态进入处理函数
    private onEnterAttack(previousState: string): void {
        console.log(`Entering Attack state from ${previousState}`);
        // 可以在这里添加攻击开始的特殊逻辑
    }

    private onEnterDash(previousState: string): void {
        console.log(`Entering Dash state from ${previousState}`);
        // 冲刺开始逻辑
    }

    private onEnterWalk(previousState: string): void {
        console.log(`Entering Walk state from ${previousState}`);
        // 行走开始逻辑
    }

    private onEnterHurt(previousState: string): void {
        console.log(`Entering Hurt state from ${previousState}`);
        // 受伤状态逻辑
    }

    private onEnterDead(previousState: string): void {
        console.log(`Entering Dead state from ${previousState}`);
        // 死亡状态逻辑
        EventSystem.emit("onCharacterDeath", { previousState });
    }

    // 状态退出处理函数
    private onExitAttack(): void {
        console.log("Exiting Attack state");
        // 攻击结束逻辑
    }

    private onExitDash(): void {
        console.log("Exiting Dash state");
        // 冲刺结束逻辑
    }

    private onExitWalk(): void {
        console.log("Exiting Walk state");
        // 行走结束逻辑
    }

    // Getter 方法
    public getCurrentState(): string {
        return this.currentState;
    }

    public getPreviousState(): string {
        return this.previousState;
    }

    public getStateStartTime(): number {
        return this.stateStartTime;
    }

    public getStateDuration(): number {
        return Date.now() - this.stateStartTime;
    }

    public getStateFlag(flag: string): boolean {
        return this.stateFlags.get(flag) || false;
    }

    // 状态检查方法
    public isDead(): boolean {
        return this.currentState === StateManager.STATES.DEAD;
    }

    public isHurt(): boolean {
        return this.currentState === StateManager.STATES.HURT;
    }

    public isAttacking(): boolean {
        return this.currentState === StateManager.STATES.ATTACK || 
               this.currentState === StateManager.STATES.RUN_ATTACK;
    }

    public isDashing(): boolean {
        return this.currentState === StateManager.STATES.DASH;
    }

    public isMoving(): boolean {
        return this.currentState === StateManager.STATES.WALK || 
               this.currentState === StateManager.STATES.RUN;
    }

    public isFalling(): boolean {
        return this.currentState === StateManager.STATES.FALL;
    }

    /**
     * 重置状态管理器
     */
    public reset(): void {
        this.currentState = StateManager.STATES.IDLE;
        this.previousState = StateManager.STATES.IDLE;
        this.stateStartTime = Date.now();
        this.initializeStateFlags();
        
        EventSystem.emit("onStateManagerReset", {});
        console.log("StateManager reset to initial state");
    }

    /**
     * 获取状态统计信息
     */
    public getStateStats(): any {
        return {
            currentState: this.currentState,
            previousState: this.previousState,
            stateDuration: this.getStateDuration(),
            stateFlags: Object.fromEntries(this.stateFlags)
        };
    }
}