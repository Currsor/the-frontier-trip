import * as UE from 'ue';
import { $Ref, $ref, $set, $unref, blueprint } from 'puerts';
import { gameSystemManager } from '../../../GameSystemManager';
import { AttackSystem } from '../../../Systems/AttackSystem';
import { EventSystem } from '../../../Systems/EventSystem';
import { HealthBarComponent } from '../../../Components/HealthBarComponent';
import { TS_CardMainUI } from '../../../UMG/Card/TS_CardMainUI';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_CurrsorCharacter extends UE.Game.Blueprints.Character.Player.BP_CurrsorCharacter.BP_CurrsorCharacter_C {}

export class TS_CurrsorCharacter extends jsClass {
    private attackSystem!: AttackSystem;
    private healthBar: HealthBarComponent = new HealthBarComponent();

    ReceiveBeginPlay() {
        // 确保血条组件已实例化（防止字段初始化器未执行）
        if (!this.healthBar) {
            this.healthBar = new HealthBarComponent();
        }

        // 初始化游戏系统
        this.initializeGameSystems();

        // 订阅防御/消耗广播
        this.subscribeEvent();

        // 初始化血条组件
        this.initializeHealthBar();
    }

    private initializeGameSystems(): void {
        try {
            if (!gameSystemManager.isSystemInitialized('AttackSystem')) {
                gameSystemManager.initialize();
            }
            this.attackSystem = gameSystemManager.getSystem('AttackSystem');
        } catch (error) {
            console.error('[TS Character] 游戏系统初始化失败：', error);
        }
    }

    BndEvt__BP_CurrsorCharacter_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent: UE.PrimitiveComponent, OtherActor: UE.Actor, OtherComp: UE.PrimitiveComponent, OtherBodyIndex: number, bFromSweep: boolean, SweepResult: UE.HitResult): void {
        console.log(`[TS Character] 攻击判定框重叠 ${OtherActor.GetName()}`);
        // 玩家主动攻击敌人 → 玩家先手（Ambushed）
        TS_CardMainUI.combatInitiator = 'player';
        if (this.attackSystem) {
            this.attackSystem.processAttackHit(this, OtherActor, SweepResult);
        } else {
            console.error('[TS Character] 攻击系统不可用');
        }
    }

    // ─── 事件订阅 ────────────────────────────────────────────────────────────

    private subscribeEvent(): void {
        EventSystem.subscribe('Defense', this.onDefenseTriggered.bind(this));
        EventSystem.subscribe('Consumption', this.onConsumptionTriggered.bind(this));
        EventSystem.subscribe('EnemyAttackPlayer', this.onEnemyAttackPlayer.bind(this));
    }

    /**
     * 敌人攻击玩家：先用护盾抵消伤害，再扣血
     */
    private onEnemyAttackPlayer(data: { enemy: any, attackType: number, damage: number }): void {
        const damage = data.damage ?? 0;
        console.log(`[TS Character] 受到敌人攻击，伤害: ${damage}`);

        if (!this.HealthComponent) {
            console.error('[TS Character] HealthComponent 不存在，无法扣血');
            return;
        }

        // 先用护盾抵消伤害
        const currentDefense = this.HealthComponent.GetDefense();
        if (currentDefense > 0) {
            const remainingDamage = Math.max(0, damage - currentDefense);
            const newDefense = Math.max(0, currentDefense - damage);
            console.log(`[TS Character] 护盾抵消: ${currentDefense - newDefense}，剩余伤害: ${remainingDamage}，剩余护盾: ${newDefense}`);
            this.UpdateDefense(newDefense);
            if (remainingDamage <= 0) return;
            // 剩余伤害扣血
            this.HealthComponent.TakeDamage(remainingDamage, data.enemy);
        } else {
            this.HealthComponent.TakeDamage(damage, data.enemy);
        }
    }

    private onDefenseTriggered(data: any): void {
        console.log('[TS Character] ========== 收到防御广播 ==========');

        if (!data || !data.cardInfo) {
            console.error('[TS Character] 防御事件数据无效');
            return;
        }

        const cardInfo = data.cardInfo as UE.Game.Data.Structs.S_CardInfo.S_CardInfo;
        const defenseValue = this.parseDefenseValue(cardInfo.Description);

        if (defenseValue > 0) {
            const currentDefense = this.HealthComponent ? this.HealthComponent.GetDefense() : 0;
            const newDefense = currentDefense + defenseValue;
            console.log(`[TS Character] 当前护盾: ${currentDefense}, 增加: ${defenseValue}, 新护盾: ${newDefense}`);
            this.UpdateDefense(newDefense);
        } else {
            console.warn(`[TS Character] 无法从卡牌描述中解析防御值: ${cardInfo.Description}`);
        }
    }

    private onConsumptionTriggered(data: any): void {
        console.log('[TS Character] ========== 收到消耗广播 ==========');
        if (this.GetMana() - data.cardInfo.Consumption < 0) {
            console.log('[TS Character] 消耗失败');
            return;
        }
        this.UseMana(data.cardInfo.Consumption);
        EventSystem.emit('UpdateMana', { Amount: this.GetMana() });

        const cardType = data.cardInfo.Type;
        if (cardType === '攻击' || cardType === 'Attack') {
            EventSystem.emit('Attack', { cardInfo: data.cardInfo, target: 'Enemy' });
        } else if (cardType === '防御' || cardType === 'Defense') {
            EventSystem.emit('Defense', { cardInfo: data.cardInfo, target: 'Player' });
        } else {
            console.log(`[PlayingCardArea] 未知的卡牌类型: ${cardType}`);
        }
    }

    /**
     * 从卡牌描述中解析防御值
     * 支持格式：<Defense>5</> 或 <Defense>5</Defense>
     */
    private parseDefenseValue(description: string): number {
        if (!description) return 0;
        const match = description.match(/<Defense>(\d+)<\/?(?:Defense)?>/i);
        if (match && match[1]) {
            const value = parseInt(match[1], 10);
            console.log(`[TS Character] 从XML标签中提取到防御值: ${value}`);
            return value;
        }
        return 0;
    }

    // ─── 血条 ────────────────────────────────────────────────────────────────

    /**
     * 初始化血条组件
     */
    private initializeHealthBar(): void {
        if (!this.bp_HPBar) {
            console.error('[TS Character] bp_HPBar 组件不存在！');
            return;
        }

        const healthComp = this.HealthComponent;
        if (!healthComp) {
            console.error('[TS Character] HealthComponent 不存在！');
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

        console.log('[TS Character] 血条组件初始化完成');
    }

    /**
     * 更新防御力（可供外部调用）
     */
    public UpdateDefense(newDefense: number): void {
        if (!this.HealthComponent) return;
        this.HealthComponent.SetDefense(newDefense);
        this.healthBar.updateDefense(newDefense);
        console.log(`[TS Character] 防御力更新为: ${newDefense}`);
    }

    // ─── 其他 ────────────────────────────────────────────────────────────────

    public GetDataFromName(RowName: string): UE.Game.Data.Structs.S_CardInfo.S_CardInfo {
        let cardInfo = {} as UE.Game.Data.Structs.S_CardInfo.S_CardInfo;
        cardInfo = this.BP_GetDataFromName(RowName);
        return cardInfo;
    }
}

blueprint.mixin(jsClass, TS_CurrsorCharacter);