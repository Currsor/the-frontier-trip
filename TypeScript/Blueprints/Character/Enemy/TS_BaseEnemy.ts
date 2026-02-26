import * as UE from 'ue';
import { $ref, $Ref, $set, $Nullable, blueprint } from 'puerts';
import { EventSystem } from '../../../Systems/EventSystem';
import { HealthBarComponent } from '../../../Components/HealthBarComponent';
import { TS_EnemyTurnManager } from '../../../Systems/TS_EnemyTurnManager';
import { TS_CardMainUI } from '../../../UMG/Card/TS_CardMainUI';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Enemy/Base/BP_BaseEnemy.BP_BaseEnemy_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_BaseEnemy extends UE.Game.Blueprints.Character.Enemy.Base.BP_BaseEnemy.BP_BaseEnemy_C {}

export class TS_BaseEnemy extends jsClass {
    private healthBar: HealthBarComponent = new HealthBarComponent();

    ReceiveBeginPlay() {
        // 确保血条组件已实例化（防止字段初始化器未执行）
        if (!this.healthBar) {
            this.healthBar = new HealthBarComponent();
        }

        // 初始化血条组件
        this.initializeHealthBar();

        // 订阅攻击广播
        this.subscribeToAttackEvent();

        // 注册到敌人回合管理器
        TS_EnemyTurnManager.getInstance().registerEnemy(this);
        console.log(`[TS Enemy] ${this.GetName()} 已注册到 EnemyTurnManager`);
    }

    /**
     * 敌人攻击判定框重叠（敌人主动攻击玩家 → 敌人先手 Encounter）
     */
    BndEvt__BP_BaseEnemy_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent: UE.PrimitiveComponent, OtherActor: UE.Actor, OtherComp: UE.PrimitiveComponent, OtherBodyIndex: number, bFromSweep: boolean, SweepResult: UE.HitResult): void {
        const otherName = OtherActor.GetName ? OtherActor.GetName() : '';
        // 判断是否碰到玩家
        if (otherName.toLowerCase().includes('currsor') || (OtherActor as any).IsA?.('CurrsorCharacter')) {
            console.log(`[TS Enemy] ${this.GetName()} 主动攻击玩家，敌人先手（Encounter）`);
            // 敌人主动攻击玩家 → 敌人先手
            TS_CardMainUI.combatInitiator = 'enemy';
        }
    }

    /**
     * 初始化血条组件
     */
    private initializeHealthBar(): void {
        if (!this.bp_HPBar) {
            console.error('[TS Enemy] bp_HPBar 组件不存在！');
            return;
        }

        const healthComp = this.HealthComponent;
        if (!healthComp) {
            console.error('[TS Enemy] HealthComponent 不存在！');
            return;
        }

        const currentHP = healthComp.GetCurrentHealth();
        const maxHP = healthComp.GetMaxHealth();
        const defense = healthComp.GetDefense();

        // 初始化血条组件（默认隐藏，战斗开始时自动显示）
        this.healthBar.initialize(this.bp_HPBar, this.GetName(), currentHP, maxHP, defense);

        // 订阅血量变化
        healthComp.OnHealthChanged.Add((currentHealth: number, maxHealth: number, damageAmount: number) => {
            this.healthBar.updateHealth(currentHealth, maxHealth, damageAmount);
            // 同步更新防御力显示
            this.healthBar.updateDefense(this.HealthComponent.GetDefense());
        });

        console.log('[TS Enemy] 血条组件初始化完成');
    }

    /**
     * 订阅攻击事件
     */
    private subscribeToAttackEvent(): void {
        EventSystem.subscribe('Attack', this.onAttackTriggered.bind(this));
        console.log('[TS Enemy] 已订阅攻击广播事件');
    }

    /**
     * 处理攻击事件
     */
    private onAttackTriggered(data: any): void {
        console.log('[TS Enemy] ========== 收到攻击广播 ==========');

        if (!data || !data.cardInfo) {
            console.error('[TS Enemy] 攻击事件数据无效');
            return;
        }

        const cardInfo = data.cardInfo as UE.Game.Data.Structs.S_CardInfo.S_CardInfo;
        console.log(`[TS Enemy] 攻击卡牌名称: ${cardInfo.Name}`);

        const damageValue = this.parseDamageValue(cardInfo.Description);
        if (damageValue > 0) {
            console.log(`[TS Enemy] 解析到伤害值: ${damageValue}`);
            this.TakeDamage(damageValue);
        } else {
            console.warn(`[TS Enemy] 无法从卡牌描述中解析伤害值: ${cardInfo.Description}`);
        }
    }

    /**
     * 从卡牌描述中解析伤害值
     * 支持格式：<Damage>5</> 或 <Damage>5</Damage>
     */
    private parseDamageValue(description: string): number {
        if (!description) return 0;
        const match = description.match(/<Damage>(\d+)<\/?(?:Damage)?>/i);
        if (match && match[1]) {
            const value = parseInt(match[1], 10);
            console.log(`[TS Enemy] 从XML标签中提取到伤害值: ${value}`);
            return value;
        }
        return 0;
    }

    /**
     * 受到伤害（可供外部调用）
     */
    public TakeDamage(damageAmount: number, damageInstigator?: UE.Actor): void {
        if (!this.HealthComponent) return;
        this.HealthComponent.TakeDamage(damageAmount, damageInstigator);
        console.log(`[TS Enemy] 受到伤害: ${damageAmount}`);

        // 死亡时从回合管理器注销
        if (this.IsDead && this.IsDead()) {
            TS_EnemyTurnManager.getInstance().unregisterEnemy(this);
            console.log(`[TS Enemy] ${this.GetName()} 已死亡，从 EnemyTurnManager 注销`);
        }
    }

    /**
     * 更新防御力（可供外部调用）
     */
    public UpdateDefense(newDefense: number): void {
        if (!this.HealthComponent) {
            console.warn('[TS Enemy] HealthComponent 不存在，无法更新防御力');
            return;
        }
        this.HealthComponent.SetDefense(newDefense);
        this.healthBar.updateDefense(newDefense);
        console.log(`[TS Enemy] 防御力更新为: ${newDefense}`);
    }
}

blueprint.mixin(jsClass, TS_BaseEnemy);