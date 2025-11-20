import * as UE from 'ue';
import { $ref, $Ref, $set, $Nullable, blueprint } from 'puerts';
import { TS_Common_HPBar } from '../../../UMG/Common/TS_Common_HPBar';
import { EventSystem } from '../../../Systems/EventSystem';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Enemy/Base/BP_BaseEnemy.BP_BaseEnemy_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_BaseEnemy extends UE.Game.Blueprints.Character.Enemy.Base.BP_BaseEnemy.BP_BaseEnemy_C {}

export class TS_BaseEnemy extends jsClass {
    private hpBarWidget: TS_Common_HPBar | null = null;
    private currentHPPercent: number = 1.0; // 当前血量百分比

    ReceiveBeginPlay() {
        // 初始化血条UI
        this.initializeHealthBar();
        
        // 订阅攻击广播
        this.subscribeToAttackEvent();
    }

    /**
     * 初始化血条UI
     */
    private initializeHealthBar(): void {
        try {
            // 检查bp_HPBar组件是否存在
            if (!this.bp_HPBar) {
                console.error("[TS Enemy] bp_HPBar 组件不存在！");
                return;
            }

            // 从WidgetComponent获取Widget实例
            const widget = this.bp_HPBar.GetWidget();
            if (!widget) {
                console.error("[TS Enemy] 无法从 bp_HPBar 获取 Widget 实例！");
                return;
            }

            console.log(`[TS Enemy] Widget 类型：${widget.GetClass().GetName()}`);

            // 获取HealthComponent
            const healthComp = this.HealthComponent;
            if (!healthComp) {
                console.error("[TS Enemy] HealthComponent 不存在！");
                return;
            }

            // 获取初始数据
            const currentHP = healthComp.GetCurrentHealth();
            const maxHP = healthComp.GetMaxHealth();
            const defense = healthComp.GetDefense();

            console.log(`[TS Enemy] 血量数据 - HP: ${currentHP}/${maxHP}, Defense: ${defense}`);

            // 直接设置防御力文本和血量进度条
            const widgetInstance = widget as UE.Game.UI.Blueprints.Common.Widget_Common_HPBar.Widget_Common_HPBar_C;
            if (widgetInstance.bp_DefenseNum) {
                widgetInstance.bp_DefenseNum.SetText(defense.toString());
                console.log(`[TS Enemy] 防御力文本已设置：${defense}`);
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
            console.log(`[TS Enemy] 血量进度条已设置：${hpPercent * 100}%`);

            // 订阅HealthComponent的OnHealthChanged委托
            healthComp.OnHealthChanged.Add((currentHealth: number, maxHealth: number, damageAmount: number) => {
                this.onHealthChanged(currentHealth, maxHealth, damageAmount);
            });

            // 保存widget引用用于后续更新
            this.hpBarWidget = widget as any;

            console.log(`[TS Enemy] 血条UI初始化成功！`);

        } catch (error) {
            console.error("[TS Enemy] 血条初始化失败：", error);
        }
    }

    /**
     * 当血量变化时触发
     */
    private onHealthChanged(currentHealth: number, maxHealth: number, damageAmount: number): void {
        console.log(`[TS Enemy] 血量变化 - 当前: ${currentHealth}/${maxHealth}, 伤害: ${damageAmount}`);

        if (!this.HealthComponent || !this.hpBarWidget) {
            return;
        }

        const defense = this.HealthComponent.GetDefense();
        const hpPercent = maxHealth > 0 ? currentHealth / maxHealth : 0;

        // 直接更新UI组件
        const widgetInstance = this.hpBarWidget as any;
        if (widgetInstance.bp_DefenseNum) {
            widgetInstance.bp_DefenseNum.SetText(defense.toString());
        }

        // 更新血量进度条（带动画）
        this.updateHealthBarWithAnimation(widgetInstance, hpPercent, damageAmount);

        console.log(`[TS Enemy] UI已更新 - HP: ${hpPercent * 100}%, Defense: ${defense}`);
    }

    /**
     * 更新防御力（当防御力变化时调用）
     */
    public UpdateDefense(newDefense: number): void {
        if (!this.HealthComponent) {
            console.warn("[TS Enemy] HealthComponent 不存在，无法更新防御力");
            return;
        }

        this.HealthComponent.SetDefense(newDefense);

        // 直接更新UI
        if (this.hpBarWidget) {
            const widgetInstance = this.hpBarWidget as any;
            if (widgetInstance.bp_DefenseNum) {
                widgetInstance.bp_DefenseNum.SetText(newDefense.toString());
            }
        }

        console.log(`[TS Enemy] 防御力更新为: ${newDefense}`);
    }

    /**
     * 订阅攻击事件
     */
    private subscribeToAttackEvent(): void {
        EventSystem.subscribe("onCardAttack", this.onAttackTriggered.bind(this));
        console.log("[TS Enemy] 已订阅攻击广播事件");
        console.log(`[TS Enemy] 订阅后监听器数量: ${EventSystem.getListenerCount("onCardAttack")}`);
    }

    /**
     * 处理攻击事件
     */
    private onAttackTriggered(data: any): void {
        console.log("[TS Enemy] ========== 收到攻击广播 ==========");
        
        if (!data || !data.cardInfo) {
            console.error("[TS Enemy] 攻击事件数据无效");
            return;
        }

        const cardInfo = data.cardInfo as UE.Game.Data.Structs.S_CardInfo.S_CardInfo;
        console.log(`[TS Enemy] 攻击卡牌名称: ${cardInfo.Name}`);
        console.log(`[TS Enemy] 攻击卡牌描述: ${cardInfo.Description}`);

        // 从卡牌描述中解析伤害值
        const damageValue = this.parseDamageValue(cardInfo.Description);
        
        if (damageValue > 0) {
            console.log(`[TS Enemy] 解析到伤害值: ${damageValue}`);
            
            // 对敌人造成伤害
            this.TakeDamage(damageValue);
        } else {
            console.warn(`[TS Enemy] 无法从卡牌描述中解析伤害值: ${cardInfo.Description}`);
        }
    }

    /**
     * 从卡牌描述中解析伤害值
     * @param description 卡牌描述文本
     * @returns 解析出的伤害值，如果解析失败返回0
     */
    private parseDamageValue(description: string): number {
        if (!description) {
            return 0;
        }

        // 匹配XML标签格式：<Damage>5</> 或 <Damage>5</Damage>
        const xmlMatch = description.match(/<Damage>(\d+)<\/?(?:Damage)?>/i);
        if (xmlMatch && xmlMatch[1]) {
            const value = parseInt(xmlMatch[1], 10);
            console.log(`[TS Enemy] 从XML标签中提取到伤害值: ${value}`);
            return value;
        }

        return 0;
    }

    /**
     * 受到伤害时调用（可选，用于外部调用）
     */
    public TakeDamage(damageAmount: number, damageInstigator?: UE.Actor): void {
        if (!this.HealthComponent) {
            return;
        }

        this.HealthComponent.TakeDamage(damageAmount, damageInstigator);
        console.log(`[TS Enemy] 受到伤害: ${damageAmount}`);
    }

    /**
     * 更新血量进度条（带动画效果）
     */
    private updateHealthBarWithAnimation(widgetInstance: any, newHPPercent: number, damageAmount: number): void {
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
        } else if (damageAmount < 0) {
            // 回血：回血条先到位，真实血条插值跟随
            if (widgetInstance.bp_progHPBack) {
                widgetInstance.bp_progHPBack.SetPercent(newHPPercent);
            }
            
            // 启动回血动画（回血完成后会更新currentHPPercent）
            this.animateProgressBar(widgetInstance.bp_progHP, currentPercent, newHPPercent, 0.5);
        } else {
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
    private animateProgressBar(progressBar: UE.ProgressBar, startPercent: number, endPercent: number, duration: number): void {
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
            } else {
                // 动画完成，确保所有进度条同步
                this.currentHPPercent = endPercent; // 更新当前血量百分比
                if (this.hpBarWidget) {
                    const widget = this.hpBarWidget as any;
                    if (widget.bp_progHP) widget.bp_progHP.SetPercent(endPercent);
                    if (widget.bp_progHPLost) widget.bp_progHPLost.SetPercent(endPercent);
                    if (widget.bp_progHPBack) widget.bp_progHPBack.SetPercent(endPercent);
                }
            }
        };

        updateAnimation();
    }

    /**
     * 缓动函数：EaseOutCubic
     */
    private easeOutCubic(t: number): number {
        return 1 - Math.pow(1 - t, 3);
    }
}

blueprint.mixin(jsClass, TS_BaseEnemy);