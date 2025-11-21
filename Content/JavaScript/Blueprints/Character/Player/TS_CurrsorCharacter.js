"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_CurrsorCharacter = void 0;
const UE = require("ue");
const puerts_1 = require("puerts");
const GameSystemManager_1 = require("../../../GameSystemManager");
const EventSystem_1 = require("../../../Systems/EventSystem");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorCharacter {
    attackSystem;
    hpBarWidget = null;
    currentHPPercent = 1.0; // 当前血量百分比
    ReceiveBeginPlay() {
        // 初始化游戏系统
        this.initializeGameSystems();
        // 订阅防御广播
        this.subscribeEvent();
        // 初始化血条UI
        this.initializeHealthBar();
    }
    initializeGameSystems() {
        try {
            // 确保游戏系统管理器已初始化
            if (!GameSystemManager_1.gameSystemManager.isSystemInitialized("AttackSystem")) {
                GameSystemManager_1.gameSystemManager.initialize();
            }
            this.attackSystem = GameSystemManager_1.gameSystemManager.getSystem("AttackSystem");
        }
        catch (error) {
            console.error("[TS Character] 游戏系统初始化失败：", error);
        }
    }
    BndEvt__BP_CurrsorCharacter_AttackHitbox_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult) {
        console.log(`[TS Character] 攻击判定框重叠 ${OtherActor.GetName()}`);
        // 直接转发到 C++ 攻击系统处理
        if (this.attackSystem) {
            this.attackSystem.processAttackHit(this, OtherActor, SweepResult);
        }
        else {
            console.error("[TS Character] 攻击系统不可用");
        }
    }
    // 订阅防御事件
    subscribeEvent() {
        EventSystem_1.EventSystem.subscribe("Defense", this.onDefenseTriggered.bind(this));
        EventSystem_1.EventSystem.subscribe("Consumption", this.onConsumptionTriggered.bind(this));
    }
    // 处理防御事件
    onDefenseTriggered(data) {
        console.log("[TS Character] ========== 收到防御广播 ==========");
        if (!data || !data.cardInfo) {
            console.error("[TS Character] 防御事件数据无效");
            return;
        }
        const cardInfo = data.cardInfo;
        console.log(`[TS Character] 防御卡牌名称: ${cardInfo.Name}`);
        console.log(`[TS Character] 防御卡牌描述: ${cardInfo.Description}`);
        // 从卡牌描述中解析防御值
        // 假设描述格式为 "获得X点护盾" 或 "Gain X Defense"
        const defenseValue = this.parseDefenseValue(cardInfo.Description);
        if (defenseValue > 0) {
            // 获取当前防御值
            const currentDefense = this.HealthComponent ? this.HealthComponent.GetDefense() : 0;
            const newDefense = currentDefense + defenseValue;
            console.log(`[TS Character] 当前护盾: ${currentDefense}, 增加: ${defenseValue}, 新护盾: ${newDefense}`);
            // 更新防御值
            this.UpdateDefense(newDefense);
        }
        else {
            console.warn(`[TS Character] 无法从卡牌描述中解析防御值: ${cardInfo.Description}`);
        }
    }
    onConsumptionTriggered(data) {
        console.log("[TS Character] ========== 收到消耗广播 ==========");
        if (this.GetMana() - data.cardInfo.Consumption < 0) {
            console.log("[TS Character] 消耗失败");
            return;
        }
        this.UseMana(data.cardInfo.Consumption);
        EventSystem_1.EventSystem.emit("UpdateMana", { Amount: this.GetMana() });
        const cardType = data.cardInfo.Type;
        // 根据卡牌类型发出相应的广播
        if (cardType === "攻击" || cardType === "Attack") {
            EventSystem_1.EventSystem.emit("Attack", {
                cardInfo: data.cardInfo,
                target: "Enemy"
            });
        }
        else if (cardType === "防御" || cardType === "Defense") {
            EventSystem_1.EventSystem.emit("Defense", {
                cardInfo: data.cardInfo,
                target: "Player"
            });
        }
        else {
            console.log(`[PlayingCardArea] 未知的卡牌类型: ${cardType}`);
        }
    }
    /**
     * 从卡牌描述中解析防御值
     * @param description 卡牌描述文本
     * @returns 解析出的防御值，如果解析失败返回0
     */
    parseDefenseValue(description) {
        if (!description) {
            return 0;
        }
        // 匹配XML标签格式：<Defense>5</> 或 <Defense>5</Defense>
        const xmlMatch = description.match(/<Defense>(\d+)<\/?(?:Defense)?>/i);
        if (xmlMatch && xmlMatch[1]) {
            const value = parseInt(xmlMatch[1], 10);
            console.log(`[TS Character] 从XML标签中提取到防御值: ${value}`);
            return value;
        }
        return 0;
    }
    // 获取卡牌数据
    GetDataFromName(RowName) {
        let cardInfo = {};
        cardInfo = this.BP_GetDataFromName(RowName);
        return cardInfo;
    }
    /**
     * 初始化血条UI
     */
    initializeHealthBar() {
        try {
            // 检查bp_HPBar组件是否存在
            if (!this.bp_HPBar) {
                console.error("[TS Character] bp_HPBar 组件不存在！");
                return;
            }
            // 从WidgetComponent获取Widget实例
            const widget = this.bp_HPBar.GetWidget();
            if (!widget) {
                console.error("[TS Character] 无法从 bp_HPBar 获取 Widget 实例！");
                return;
            }
            console.log(`[TS Character] Widget 类型：${widget.GetClass().GetName()}`);
            console.log(`[TS Character] Widget 可用方法：`, Object.getOwnPropertyNames(widget));
            // 获取HealthComponent
            const healthComp = this.HealthComponent;
            if (!healthComp) {
                console.error("[TS Character] HealthComponent 不存在！");
                return;
            }
            // 获取初始数据
            const currentHP = healthComp.GetCurrentHealth();
            const maxHP = healthComp.GetMaxHealth();
            const defense = healthComp.GetDefense();
            console.log(`[TS Character] 血量数据 - HP: ${currentHP}/${maxHP}, Defense: ${defense}`);
            // 直接设置防御力文本和血量进度条
            const widgetInstance = widget;
            if (widgetInstance.bp_DefenseNum) {
                widgetInstance.bp_DefenseNum.SetText(defense.toString());
                console.log(`[TS Character] 防御力文本已设置：${defense}`);
            }
            // 设置初始血量（立即设置，不播放动画）
            const hpPercent = maxHP > 0 ? currentHP / maxHP : 0;
            this.currentHPPercent = hpPercent; // 保存当前血量百分比
            if (widgetInstance.bp_progHP) {
                widgetInstance.bp_progHP.SetPercent(hpPercent);
            }
            if (widgetInstance.bp_progHPLost) {
                widgetInstance.bp_progHPLost.SetPercent(hpPercent);
            }
            if (widgetInstance.bp_progHPBack) {
                widgetInstance.bp_progHPBack.SetPercent(hpPercent);
            }
            console.log(`[TS Character] 血量进度条已设置：${hpPercent * 100}%`);
            // 订阅HealthComponent的OnHealthChanged委托
            healthComp.OnHealthChanged.Add((currentHealth, maxHealth, damageAmount) => {
                this.onHealthChanged(currentHealth, maxHealth, damageAmount);
            });
            // 保存widget引用用于后续更新
            this.hpBarWidget = widget;
            console.log(`[TS Character] 血条UI初始化成功！`);
        }
        catch (error) {
            console.error("[TS Character] 血条初始化失败：", error);
        }
    }
    /**
     * 当血量变化时触发
     */
    onHealthChanged(currentHealth, maxHealth, damageAmount) {
        console.log(`[TS Character] 血量变化 - 当前: ${currentHealth}/${maxHealth}, 伤害: ${damageAmount}`);
        if (!this.HealthComponent || !this.hpBarWidget) {
            return;
        }
        const defense = this.HealthComponent.GetDefense();
        const hpPercent = maxHealth > 0 ? currentHealth / maxHealth : 0;
        // 直接更新UI组件
        const widgetInstance = this.hpBarWidget;
        if (widgetInstance.bp_DefenseNum) {
            widgetInstance.bp_DefenseNum.SetText(defense.toString());
        }
        // 更新血量进度条（带动画）
        this.updateHealthBarWithAnimation(widgetInstance, hpPercent, damageAmount);
        console.log(`[TS Character] UI已更新 - HP: ${hpPercent * 100}%, Defense: ${defense}`);
    }
    /**
     * 更新防御力（当防御力变化时调用）
     */
    UpdateDefense(newDefense) {
        if (!this.HealthComponent) {
            return;
        }
        this.HealthComponent.SetDefense(newDefense);
        // 直接更新UI
        if (this.hpBarWidget) {
            const widgetInstance = this.hpBarWidget;
            if (widgetInstance.bp_DefenseNum) {
                widgetInstance.bp_DefenseNum.SetText(newDefense.toString());
            }
        }
        console.log(`[TS Character] 防御力更新为: ${newDefense}`);
    }
    /**
     * 更新血量进度条（带动画效果）
     */
    updateHealthBarWithAnimation(widgetInstance, newHPPercent, damageAmount) {
        if (!widgetInstance.bp_progHP) {
            return;
        }
        const currentPercent = this.currentHPPercent; // 使用内部状态而不是从UI读取
        if (damageAmount > 0) {
            // 受到伤害：真实血条直接到位，掉血条插值跟随
            widgetInstance.bp_progHP.SetPercent(newHPPercent);
            this.currentHPPercent = newHPPercent; // 更新当前血量百分比
            // 启动掉血动画
            this.animateProgressBar(widgetInstance.bp_progHPLost, currentPercent, newHPPercent, 0.5);
        }
        else if (damageAmount < 0) {
            // 回血：回血条先到位，真实血条插值跟随
            if (widgetInstance.bp_progHPBack) {
                widgetInstance.bp_progHPBack.SetPercent(newHPPercent);
            }
            // 启动回血动画（回血完成后会更新currentHPPercent）
            this.animateProgressBar(widgetInstance.bp_progHP, currentPercent, newHPPercent, 0.5);
        }
        else {
            // 没有伤害变化，直接设置
            this.currentHPPercent = newHPPercent; // 更新当前血量百分比
            widgetInstance.bp_progHP.SetPercent(newHPPercent);
            if (widgetInstance.bp_progHPLost) {
                widgetInstance.bp_progHPLost.SetPercent(newHPPercent);
            }
            if (widgetInstance.bp_progHPBack) {
                widgetInstance.bp_progHPBack.SetPercent(newHPPercent);
            }
        }
    }
    /**
     * 进度条插值动画
     */
    animateProgressBar(progressBar, startPercent, endPercent, duration) {
        const startTime = Date.now();
        const deltaPercent = endPercent - startPercent;
        const updateAnimation = () => {
            const elapsed = (Date.now() - startTime) / 1000;
            const progress = Math.min(elapsed / duration, 1.0);
            // 使用缓动函数
            const easedProgress = this.easeOutCubic(progress);
            const currentPercent = startPercent + deltaPercent * easedProgress;
            progressBar.SetPercent(currentPercent);
            if (progress < 1.0) {
                setTimeout(updateAnimation, 16); // 约60fps
            }
            else {
                // 动画完成，确保所有进度条同步
                this.currentHPPercent = endPercent; // 更新当前血量百分比
                if (this.hpBarWidget) {
                    const widget = this.hpBarWidget;
                    if (widget.bp_progHP)
                        widget.bp_progHP.SetPercent(endPercent);
                    if (widget.bp_progHPLost)
                        widget.bp_progHPLost.SetPercent(endPercent);
                    if (widget.bp_progHPBack)
                        widget.bp_progHPBack.SetPercent(endPercent);
                }
            }
        };
        updateAnimation();
    }
    /**
     * 缓动函数：EaseOutCubic
     */
    easeOutCubic(t) {
        return 1 - Math.pow(1 - t, 3);
    }
}
exports.TS_CurrsorCharacter = TS_CurrsorCharacter;
puerts_1.blueprint.mixin(jsClass, TS_CurrsorCharacter);
//# sourceMappingURL=TS_CurrsorCharacter.js.map