"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.AttackSystem = void 0;
/**
 * 攻击系统 C++ 代理类
 *
 * 重要说明：
 * - 此类不包含任何攻击逻辑
 * - 所有攻击处理完全在 C++ AttackSystemComponent 中实现
 * - TypeScript 仅作为 C++ 系统的访问接口
 * - 不进行任何伤害计算、状态检查或攻击验证
 */
class AttackSystem {
    static instance;
    static getInstance() {
        if (!AttackSystem.instance) {
            AttackSystem.instance = new AttackSystem();
        }
        return AttackSystem.instance;
    }
    constructor() {
        console.log("[TS AttackSystem] C++ Proxy initialized - NO logic in TypeScript");
    }
    /**
     * 获取 C++ AttackSystemComponent 实例
     * @returns C++ AttackSystemComponent 实例或 null
     */
    getCppAttackSystem() {
        try {
            const gameSystemManager = UE.GameSystemManager?.GetInstance?.();
            if (!gameSystemManager) {
                console.error("[TS AttackSystem] GameSystemManager not found");
                return null;
            }
            const cppSystem = gameSystemManager.GetAttackSystem();
            if (!cppSystem) {
                console.error("[TS AttackSystem] C++ AttackSystemComponent not found");
                return null;
            }
            return cppSystem;
        }
        catch (error) {
            console.error("[TS AttackSystem] Error accessing C++ system:", error);
            return null;
        }
    }
    /**
     * 直接转发到 C++ - 处理攻击输入
     */
    handleAttackInput(attacker) {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            console.log("[TS AttackSystem] Forwarding attack input to C++");
            cppSystem.ProcessAttackInput(attacker);
        }
    }
    /**
     * 直接转发到 C++ - 处理攻击命中
     */
    processAttackHit(attacker, target, hitResult) {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            console.log("[TS AttackSystem] Forwarding attack hit to C++");
            return cppSystem.ProcessAttackHit(attacker, target, hitResult || {});
        }
        return false;
    }
    /**
     * 直接转发到 C++ - 处理完整攻击
     */
    processAttack(attacker, target, attackData) {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            console.log("[TS AttackSystem] Forwarding full attack to C++");
            return cppSystem.ProcessAttack(attacker, target, attackData || {});
        }
        return false;
    }
    /**
     * 直接查询 C++ - 检查是否可以攻击
     */
    canAttack(attacker) {
        const cppSystem = this.getCppAttackSystem();
        return cppSystem ? cppSystem.CanAttack(attacker) : false;
    }
    /**
     * 直接转发到 C++ - 开始攻击
     */
    startAttack(attacker, attackType = "Normal") {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            cppSystem.StartAttack(attacker, attackType);
        }
    }
    /**
     * 直接转发到 C++ - 结束攻击
     */
    endAttack(attacker) {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            cppSystem.EndAttack(attacker);
        }
    }
    /**
     * 直接查询 C++ - 检查是否正在攻击
     */
    isAttacking(attacker) {
        const cppSystem = this.getCppAttackSystem();
        return cppSystem ? cppSystem.IsAttacking(attacker) : false;
    }
    /**
     * 直接查询 C++ - 获取攻击统计信息
     */
    getAttackStats() {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            return cppSystem.GetAttackStats ? cppSystem.GetAttackStats() : {};
        }
        return {};
    }
    /**
     * 直接转发到 C++ - 重置攻击系统
     */
    reset() {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem && cppSystem.Reset) {
            cppSystem.Reset();
        }
    }
}
exports.AttackSystem = AttackSystem;
//# sourceMappingURL=AttackSystem.js.map