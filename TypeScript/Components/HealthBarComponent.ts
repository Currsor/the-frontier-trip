import * as UE from 'ue';

/**
 * 血条组件
 * 负责管理角色血条 UI 的显示、隐藏与数据更新。
 * - 默认隐藏
 * - 战斗状态变为 "Combat" 时显示（监听 GameMode.OnCombatStateChanged）
 * - 战斗状态变为 "Victory" / "Defeat" 时隐藏
 */
export class HealthBarComponent {
    /** 血条 WidgetComponent */
    private widgetComponent: UE.WidgetComponent | null = null;
    /** 血条 Widget 实例（强转为 any 以访问蓝图属性） */
    private widgetInstance: any = null;
    /** 当前血量百分比（用于动画起始值） */
    private currentHPPercent: number = 1.0;
    /** 所属角色的唯一标识（用于死亡事件匹配） */
    private ownerId: string = '';

    // GameMode 委托回调引用（用于取消绑定）
    private onCombatStateChangedCb: ((combatEventType: string) => void) | null = null;

    /**
     * 初始化血条组件
     * @param widgetComp  角色上的 WidgetComponent（bp_HPBar）
     * @param ownerId     所属角色名称，用于死亡事件匹配
     * @param currentHP   初始当前血量
     * @param maxHP       初始最大血量
     * @param defense     初始防御力
     */
    public initialize(
        widgetComp: UE.WidgetComponent,
        ownerId: string,
        currentHP: number,
        maxHP: number,
        defense: number
    ): void {
        this.widgetComponent = widgetComp;
        this.ownerId = ownerId;

        const widget = widgetComp.GetWidget();
        if (!widget) {
            console.error(`[HealthBarComponent] ${ownerId}: 无法获取 Widget 实例`);
            return;
        }
        this.widgetInstance = widget;

        // 设置初始数据
        this.setDefense(defense);
        const hpPercent = maxHP > 0 ? currentHP / maxHP : 0;
        this.currentHPPercent = hpPercent;
        this.setAllBarsImmediate(hpPercent);

        // 默认隐藏血条
        this.setVisible(false);

        // 绑定 GameMode 的战斗状态变化委托
        this.bindCombatStateDelegate();

        console.log(`[HealthBarComponent] ${ownerId}: 初始化完成，血条已隐藏`);
    }

    /**
     * 更新血量（带动画）
     * @param currentHealth 当前血量
     * @param maxHealth     最大血量
     * @param damageAmount  伤害量（正数=受伤，负数=回血，0=直接设置）
     */
    public updateHealth(currentHealth: number, maxHealth: number, damageAmount: number): void {
        if (!this.widgetInstance) return;

        const newHPPercent = maxHealth > 0 ? currentHealth / maxHealth : 0;
        const prevPercent = this.currentHPPercent;

        if (damageAmount > 0) {
            // 受伤：真实血条直接到位，掉血条插值跟随
            this.setBarPercent('bp_progHP', newHPPercent);
            this.currentHPPercent = newHPPercent;
            this.animateBar('bp_progHPLost', prevPercent, newHPPercent, 0.5);
        } else if (damageAmount < 0) {
            // 回血：回血条先到位，真实血条插值跟随
            this.setBarPercent('bp_progHPBack', newHPPercent);
            this.animateBar('bp_progHP', prevPercent, newHPPercent, 0.5);
        } else {
            // 直接设置
            this.currentHPPercent = newHPPercent;
            this.setAllBarsImmediate(newHPPercent);
        }
    }

    /**
     * 更新防御力显示
     * @param defense 防御力数值
     */
    public updateDefense(defense: number): void {
        this.setDefense(defense);
    }

    /**
     * 销毁时清理委托绑定
     */
    public destroy(): void {
        if (this.onCombatStateChangedCb) {
            const gameMode = this.getGameMode();
            if (gameMode) {
                gameMode.OnCombatStateChanged.Remove(this.onCombatStateChangedCb);
            }
            this.onCombatStateChangedCb = null;
        }
        this.widgetInstance = null;
        this.widgetComponent = null;
        console.log(`[HealthBarComponent] ${this.ownerId}: 已销毁`);
    }

    // ─── 私有辅助方法 ────────────────────────────────────────────────────────

    /**
     * 获取 GameMode 实例
     */
    private getGameMode(): UE.CurrsorGameMode | null {
        const world = UE.GameplayStatics.GetGameInstance(this.widgetComponent as any)?.GetWorld();
        if (!world) return null;
        return UE.GameplayStatics.GetGameMode(this.widgetComponent as any) as UE.CurrsorGameMode;
    }

    /**
     * 绑定 GameMode 的 OnCombatStateChanged 委托
     * - "Combat"  → 显示血条
     * - "Victory" / "Defeat" → 隐藏血条
     */
    private bindCombatStateDelegate(): void {
        const gameMode = this.getGameMode();
        if (!gameMode) {
            console.error(`[HealthBarComponent] ${this.ownerId}: 无法获取 GameMode，战斗状态监听失败`);
            return;
        }

        this.onCombatStateChangedCb = (combatEventType: string) => {
            console.log(`[HealthBarComponent] ${this.ownerId}: 收到战斗状态变化 -> ${combatEventType}`);
            if (combatEventType === 'Combat') {
                this.setVisible(true);
            } else if (combatEventType === 'Victory' || combatEventType === 'Defeat') {
                this.setVisible(false);
            }
        };

        gameMode.OnCombatStateChanged.Add(this.onCombatStateChangedCb);
        console.log(`[HealthBarComponent] ${this.ownerId}: 已绑定 OnCombatStateChanged 委托`);
    }

    private setVisible(visible: boolean): void {
        if (!this.widgetInstance) return;
        this.widgetInstance.SetVisibility(
            visible ? UE.ESlateVisibility.Visible : UE.ESlateVisibility.Hidden
        );
    }

    private setDefense(defense: number): void {
        if (this.widgetInstance?.bp_DefenseNum) {
            this.widgetInstance.bp_DefenseNum.SetText(defense.toString());
        }
    }

    private setBarPercent(barName: string, percent: number): void {
        if (this.widgetInstance?.[barName]) {
            this.widgetInstance[barName].SetPercent(percent);
        }
    }

    private setAllBarsImmediate(percent: number): void {
        this.setBarPercent('bp_progHP', percent);
        this.setBarPercent('bp_progHPLost', percent);
        this.setBarPercent('bp_progHPBack', percent);
    }

    /**
     * 进度条插值动画（EaseOutCubic）
     */
    private animateBar(barName: string, startPercent: number, endPercent: number, duration: number): void {
        const startTime = Date.now();
        const delta = endPercent - startPercent;

        const tick = () => {
            const elapsed = (Date.now() - startTime) / 1000;
            const t = Math.min(elapsed / duration, 1.0);
            const eased = 1 - Math.pow(1 - t, 3); // EaseOutCubic
            this.setBarPercent(barName, startPercent + delta * eased);

            if (t < 1.0) {
                setTimeout(tick, 16);
            } else {
                // 动画完成，同步所有进度条
                this.currentHPPercent = endPercent;
                this.setAllBarsImmediate(endPercent);
            }
        };

        tick();
    }
}
