"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_CurrsorCharacter = void 0;
const UE = require("ue");
const puerts_1 = require("puerts");
const GameSystemManager_1 = require("../../../GameSystemManager");
const EventSystem_1 = require("../../../Systems/EventSystem");
const HealthBarComponent_1 = require("../../../Components/HealthBarComponent");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorCharacter extends jsClass {
    attackSystem;
    healthBar = new HealthBarComponent_1.HealthBarComponent();
    ReceiveBeginPlay() {
        // 确保血条组件已实例化（防止字段初始化器未执行）
        if (!this.healthBar) {
            this.healthBar = new HealthBarComponent_1.HealthBarComponent();
        }
        // 初始化游戏系统
        this.initializeGameSystems();
        // 订阅防御/消耗广播
        this.subscribeEvent();
        // 初始化血条组件
        this.initializeHealthBar();
    }
    initializeGameSystems() {
        try {
            if (!GameSystemManager_1.gameSystemManager.isSystemInitialized('AttackSystem')) {
                GameSystemManager_1.gameSystemManager.initialize();
            }
            this.attackSystem = GameSystemManager_1.gameSystemManager.getSystem('AttackSystem');
        }
        catch (error) {
            console.error('[TS Character] 游戏系统初始化失败：', error);
        }
    }
    BndEvt__BP_CurrsorCharacter_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult) {
        console.log(`[TS Character] 攻击判定框重叠 ${OtherActor.GetName()}`);
        if (this.attackSystem) {
            this.attackSystem.processAttackHit(this, OtherActor, SweepResult);
        }
        else {
            console.error('[TS Character] 攻击系统不可用');
        }
    }
    // ─── 事件订阅 ────────────────────────────────────────────────────────────
    subscribeEvent() {
        EventSystem_1.EventSystem.subscribe('Defense', this.onDefenseTriggered.bind(this));
        EventSystem_1.EventSystem.subscribe('Consumption', this.onConsumptionTriggered.bind(this));
    }
    onDefenseTriggered(data) {
        console.log('[TS Character] ========== 收到防御广播 ==========');
        if (!data || !data.cardInfo) {
            console.error('[TS Character] 防御事件数据无效');
            return;
        }
        const cardInfo = data.cardInfo;
        const defenseValue = this.parseDefenseValue(cardInfo.Description);
        if (defenseValue > 0) {
            const currentDefense = this.HealthComponent ? this.HealthComponent.GetDefense() : 0;
            const newDefense = currentDefense + defenseValue;
            console.log(`[TS Character] 当前护盾: ${currentDefense}, 增加: ${defenseValue}, 新护盾: ${newDefense}`);
            this.UpdateDefense(newDefense);
        }
        else {
            console.warn(`[TS Character] 无法从卡牌描述中解析防御值: ${cardInfo.Description}`);
        }
    }
    onConsumptionTriggered(data) {
        console.log('[TS Character] ========== 收到消耗广播 ==========');
        if (this.GetMana() - data.cardInfo.Consumption < 0) {
            console.log('[TS Character] 消耗失败');
            return;
        }
        this.UseMana(data.cardInfo.Consumption);
        EventSystem_1.EventSystem.emit('UpdateMana', { Amount: this.GetMana() });
        const cardType = data.cardInfo.Type;
        if (cardType === '攻击' || cardType === 'Attack') {
            EventSystem_1.EventSystem.emit('Attack', { cardInfo: data.cardInfo, target: 'Enemy' });
        }
        else if (cardType === '防御' || cardType === 'Defense') {
            EventSystem_1.EventSystem.emit('Defense', { cardInfo: data.cardInfo, target: 'Player' });
        }
        else {
            console.log(`[PlayingCardArea] 未知的卡牌类型: ${cardType}`);
        }
    }
    /**
     * 从卡牌描述中解析防御值
     * 支持格式：<Defense>5</> 或 <Defense>5</Defense>
     */
    parseDefenseValue(description) {
        if (!description)
            return 0;
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
    initializeHealthBar() {
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
        healthComp.OnHealthChanged.Add((currentHealth, maxHealth, damageAmount) => {
            this.healthBar.updateHealth(currentHealth, maxHealth, damageAmount);
            // 同步更新防御力显示
            this.healthBar.updateDefense(this.HealthComponent.GetDefense());
        });
        console.log('[TS Character] 血条组件初始化完成');
    }
    /**
     * 更新防御力（可供外部调用）
     */
    UpdateDefense(newDefense) {
        if (!this.HealthComponent)
            return;
        this.HealthComponent.SetDefense(newDefense);
        this.healthBar.updateDefense(newDefense);
        console.log(`[TS Character] 防御力更新为: ${newDefense}`);
    }
    // ─── 其他 ────────────────────────────────────────────────────────────────
    GetDataFromName(RowName) {
        let cardInfo = {};
        cardInfo = this.BP_GetDataFromName(RowName);
        return cardInfo;
    }
}
exports.TS_CurrsorCharacter = TS_CurrsorCharacter;
puerts_1.blueprint.mixin(jsClass, TS_CurrsorCharacter);
//# sourceMappingURL=TS_CurrsorCharacter.js.map