import { Actor } from "ue";

declare const UE: any;

/**
 * 攻击系统 C++ 代理类
 * 
 * 重要说明：
 * - 此类不包含任何攻击逻辑
 * - 所有攻击处理完全在 C++ AttackSystemComponent 中实现
 * - TypeScript 仅作为 C++ 系统的访问接口
 * - 不进行任何伤害计算、状态检查或攻击验证
 */
export class AttackSystem {
    private static instance: AttackSystem;

    public static getInstance(): AttackSystem {
        if (!AttackSystem.instance) {
            AttackSystem.instance = new AttackSystem();
        }
        return AttackSystem.instance;
    }

    private constructor() {
        console.log("[TS AttackSystem] C++ Proxy initialized - NO logic in TypeScript");
    }

    /**
     * 获取 C++ AttackSystemComponent 实例
     * @returns C++ AttackSystemComponent 实例或 null
     */
    private getCppAttackSystem(): any {
        try {
            const gameSystemManager = (UE as any).GameSystemManager?.GetInstance?.();
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
        } catch (error) {
            console.error("[TS AttackSystem] Error accessing C++ system:", error);
            return null;
        }
    }

    /**
     * 直接转发到 C++ - 处理攻击输入
     */
    public handleAttackInput(attacker: Actor): void {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            console.log("[TS AttackSystem] Forwarding attack input to C++");
            cppSystem.ProcessAttackInput(attacker);
        }
    }

    /**
     * 直接转发到 C++ - 处理攻击命中
     */
    public processAttackHit(attacker: Actor, target: Actor, hitResult?: any): boolean {
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
    public processAttack(attacker: Actor, target: Actor, attackData?: any): boolean {
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
    public canAttack(attacker: Actor): boolean {
        const cppSystem = this.getCppAttackSystem();
        return cppSystem ? cppSystem.CanAttack(attacker) : false;
    }

    /**
     * 直接转发到 C++ - 开始攻击
     */
    public startAttack(attacker: Actor, attackType: string = "Normal"): void {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            cppSystem.StartAttack(attacker, attackType);
        }
    }

    /**
     * 直接转发到 C++ - 结束攻击
     */
    public endAttack(attacker: Actor): void {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            cppSystem.EndAttack(attacker);
        }
    }

    /**
     * 直接查询 C++ - 检查是否正在攻击
     */
    public isAttacking(attacker: Actor): boolean {
        const cppSystem = this.getCppAttackSystem();
        return cppSystem ? cppSystem.IsAttacking(attacker) : false;
    }

    /**
     * 直接查询 C++ - 获取攻击统计信息
     */
    public getAttackStats(): any {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem) {
            return cppSystem.GetAttackStats ? cppSystem.GetAttackStats() : {};
        }
        return {};
    }

    /**
     * 直接转发到 C++ - 重置攻击系统
     */
    public reset(): void {
        const cppSystem = this.getCppAttackSystem();
        if (cppSystem && cppSystem.Reset) {
            cppSystem.Reset();
        }
    }
}