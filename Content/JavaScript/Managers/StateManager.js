"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.StateManager = void 0;
const GameConfig_1 = require("../Config/GameConfig");
const EventSystem_1 = require("../Systems/EventSystem");
/**
 * 状态管理器
 * 处理角色状态转换逻辑和状态验证
 */
class StateManager {
    static instance;
    currentState = "Idle";
    previousState = "Idle";
    stateStartTime = 0;
    stateFlags = new Map();
    // 状态枚举映射
    static STATES = {
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
    static getInstance() {
        if (!StateManager.instance) {
            StateManager.instance = new StateManager();
        }
        return StateManager.instance;
    }
    constructor() {
        this.initializeStateFlags();
        console.log("StateManager initialized");
    }
    /**
     * 初始化状态标志
     */
    initializeStateFlags() {
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
    canTransitionTo(fromState, toState) {
        // 死亡状态不能转换到其他状态（除非复活）
        if (fromState === StateManager.STATES.DEAD && toState !== StateManager.STATES.IDLE) {
            return false;
        }
        // 使用配置中的优先级系统
        return GameConfig_1.GameConfig.canTransitionState(fromState, toState);
    }
    /**
     * 获取状态转换优先级
     * @param state 状态名称
     * @returns 优先级数值
     */
    getStateTransitionPriority(state) {
        return GameConfig_1.GameConfig.getStatePriority(state);
    }
    /**
     * 执行状态转换
     * @param newState 新状态
     * @param force 是否强制转换
     * @returns 是否转换成功
     */
    changeState(newState, force = false) {
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
        EventSystem_1.EventSystem.emit("onStateChanged", {
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
    handleStateEnter(state, previousState) {
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
        EventSystem_1.EventSystem.emit(`onEnter${state}`, { previousState });
    }
    /**
     * 处理状态退出逻辑
     * @param state 退出的状态
     */
    handleStateExit(state) {
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
        EventSystem_1.EventSystem.emit(`onExit${state}`, {});
    }
    /**
     * 更新状态标志
     * @param state 状态
     * @param isEntering 是否进入状态
     */
    updateStateFlags(state, isEntering) {
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
    onEnterAttack(previousState) {
        console.log(`Entering Attack state from ${previousState}`);
        // 可以在这里添加攻击开始的特殊逻辑
    }
    onEnterDash(previousState) {
        console.log(`Entering Dash state from ${previousState}`);
        // 冲刺开始逻辑
    }
    onEnterWalk(previousState) {
        console.log(`Entering Walk state from ${previousState}`);
        // 行走开始逻辑
    }
    onEnterHurt(previousState) {
        console.log(`Entering Hurt state from ${previousState}`);
        // 受伤状态逻辑
    }
    onEnterDead(previousState) {
        console.log(`Entering Dead state from ${previousState}`);
        // 死亡状态逻辑
        EventSystem_1.EventSystem.emit("onCharacterDeath", { previousState });
    }
    // 状态退出处理函数
    onExitAttack() {
        console.log("Exiting Attack state");
        // 攻击结束逻辑
    }
    onExitDash() {
        console.log("Exiting Dash state");
        // 冲刺结束逻辑
    }
    onExitWalk() {
        console.log("Exiting Walk state");
        // 行走结束逻辑
    }
    // Getter 方法
    getCurrentState() {
        return this.currentState;
    }
    getPreviousState() {
        return this.previousState;
    }
    getStateStartTime() {
        return this.stateStartTime;
    }
    getStateDuration() {
        return Date.now() - this.stateStartTime;
    }
    getStateFlag(flag) {
        return this.stateFlags.get(flag) || false;
    }
    // 状态检查方法
    isDead() {
        return this.currentState === StateManager.STATES.DEAD;
    }
    isHurt() {
        return this.currentState === StateManager.STATES.HURT;
    }
    isAttacking() {
        return this.currentState === StateManager.STATES.ATTACK ||
            this.currentState === StateManager.STATES.RUN_ATTACK;
    }
    isDashing() {
        return this.currentState === StateManager.STATES.DASH;
    }
    isMoving() {
        return this.currentState === StateManager.STATES.WALK ||
            this.currentState === StateManager.STATES.RUN;
    }
    isFalling() {
        return this.currentState === StateManager.STATES.FALL;
    }
    /**
     * 重置状态管理器
     */
    reset() {
        this.currentState = StateManager.STATES.IDLE;
        this.previousState = StateManager.STATES.IDLE;
        this.stateStartTime = Date.now();
        this.initializeStateFlags();
        EventSystem_1.EventSystem.emit("onStateManagerReset", {});
        console.log("StateManager reset to initial state");
    }
    /**
     * 获取状态统计信息
     */
    getStateStats() {
        return {
            currentState: this.currentState,
            previousState: this.previousState,
            stateDuration: this.getStateDuration(),
            stateFlags: Object.fromEntries(this.stateFlags)
        };
    }
}
exports.StateManager = StateManager;
//# sourceMappingURL=StateManager.js.map