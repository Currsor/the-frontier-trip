import * as UE from 'ue';
import { $ref, $Ref, $set, $Nullable, blueprint } from 'puerts';
import { EventSystem } from '../../Systems/EventSystem';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Common/Widget_Common_HPBar.Widget_Common_HPBar_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_Common_HPBar extends UE.Game.UI.Blueprints.Common.Widget_Common_HPBar.Widget_Common_HPBar_C {}

export class TS_Common_HPBar extends jsClass {
    
    // 绑定的玩家对象引用
    private boundPlayer: UE.Actor | null = null;
    
    // 事件回调函数引用（用于取消订阅）
    private onDefenseChangedCallback: Function | null = null;
    private onHealthChangedCallback: Function | null = null;

    // 血量动画相关
    private currentHPPercent: number = 1.0;  // 当前真实血量百分比
    private targetHPPercent: number = 1.0;   // 目标血量百分比
    private lostHPPercent: number = 1.0;     // 掉血动画条百分比
    private backHPPercent: number = 1.0;     // 回血动画条百分比
    
    private isAnimating: boolean = false;
    private animationSpeed: number = 2.0;    // 动画速度（每秒变化的百分比）
    private updateTimerHandle: any = null;
    
    /**
     * 绑定防御力值到UI（一次性设置）
     * @param defenseValue 防御力数值
     */
    public BindDefense(defenseValue: number): void {
        if (this.bp_DefenseNum) {
            this.bp_DefenseNum.SetText(defenseValue.toString());
        }
    }

    /**
     * 绑定血量和防御力（一次性设置）
     * @param currentHP 当前血量
     * @param maxHP 最大血量
     * @param defenseValue 防御力数值
     * @param immediate 是否立即设置（不播放动画），默认false
     */
    public BindHealthAndDefense(currentHP: number, maxHP: number, defenseValue: number, immediate: boolean = false): void {
        // 设置防御力
        this.BindDefense(defenseValue);
        
        // 设置血量
        const newHPPercent = maxHP > 0 ? Math.max(0, Math.min(1, currentHP / maxHP)) : 0;
        
        if (immediate) {
            // 立即设置，不播放动画
            this.SetHealthImmediate(newHPPercent);
        } else {
            // 播放动画
            this.SetHealthWithAnimation(newHPPercent);
        }
    }

    /**
     * 立即设置血量（不播放动画）
     * @param hpPercent 血量百分比 (0-1)
     */
    private SetHealthImmediate(hpPercent: number): void {
        this.currentHPPercent = hpPercent;
        this.targetHPPercent = hpPercent;
        this.lostHPPercent = hpPercent;
        this.backHPPercent = hpPercent;

        if (this.bp_progHP) {
            this.bp_progHP.SetPercent(hpPercent);
        }
        if (this.bp_progHPLost) {
            this.bp_progHPLost.SetPercent(hpPercent);
        }
        if (this.bp_progHPBack) {
            this.bp_progHPBack.SetPercent(hpPercent);
        }

        this.StopHealthAnimation();
    }

    /**
     * 设置血量并播放动画
     * @param newHPPercent 新的血量百分比 (0-1)
     */
    private SetHealthWithAnimation(newHPPercent: number): void {
        const oldHPPercent = this.currentHPPercent;
        this.targetHPPercent = newHPPercent;

        if (newHPPercent < oldHPPercent) {
            // 掉血：真实血条直接到位，掉血条插值跟随
            this.currentHPPercent = newHPPercent;
            if (this.bp_progHP) {
                this.bp_progHP.SetPercent(newHPPercent);
            }
            // 掉血条从旧位置插值到新位置
            this.lostHPPercent = oldHPPercent;
            
        } else if (newHPPercent > oldHPPercent) {
            // 回血：回血条先到位，真实血条插值跟随
            this.backHPPercent = newHPPercent;
            if (this.bp_progHPBack) {
                this.bp_progHPBack.SetPercent(newHPPercent);
            }
            // 真实血条从旧位置插值到新位置
            this.currentHPPercent = oldHPPercent;
        } else {
            // 血量没有变化
            return;
        }

        // 开始动画
        this.StartAnimation();
    }

    /**
     * 开始动画更新循环
     */
    private StartAnimation(): void {
        if (this.isAnimating) {
            return;
        }

        this.isAnimating = true;
        this.UpdateAnimation();
    }

    /**
     * 停止动画更新循环
     */
    private StopHealthAnimation(): void {
        this.isAnimating = false;
        if (this.updateTimerHandle) {
            clearTimeout(this.updateTimerHandle);
            this.updateTimerHandle = null;
        }
    }

    /**
     * 更新动画（每帧调用）
     */
    private UpdateAnimation(): void {
        if (!this.isAnimating) {
            return;
        }

        const deltaTime = 0.016; // 约60fps
        const deltaPercent = this.animationSpeed * deltaTime;

        let animationComplete = true;

        // 更新掉血动画
        if (this.lostHPPercent > this.targetHPPercent) {
            this.lostHPPercent = Math.max(this.targetHPPercent, this.lostHPPercent - deltaPercent);
            if (this.bp_progHPLost) {
                this.bp_progHPLost.SetPercent(this.lostHPPercent);
            }
            animationComplete = false;
        }

        // 更新回血动画
        if (this.currentHPPercent < this.targetHPPercent) {
            this.currentHPPercent = Math.min(this.targetHPPercent, this.currentHPPercent + deltaPercent);
            if (this.bp_progHP) {
                this.bp_progHP.SetPercent(this.currentHPPercent);
            }
            animationComplete = false;
        }

        // 检查动画是否完成
        if (animationComplete) {
            this.OnAnimationComplete();
        } else {
            // 继续下一帧
            this.updateTimerHandle = setTimeout(() => this.UpdateAnimation(), 16);
        }
    }

    /**
     * 动画完成回调
     */
    private OnAnimationComplete(): void {
        // 确保所有进度条都在正确位置
        if (this.bp_progHP) {
            this.bp_progHP.SetPercent(this.targetHPPercent);
        }
        if (this.bp_progHPLost) {
            this.bp_progHPLost.SetPercent(this.targetHPPercent);
        }
        if (this.bp_progHPBack) {
            this.bp_progHPBack.SetPercent(this.targetHPPercent);
        }

        this.currentHPPercent = this.targetHPPercent;
        this.lostHPPercent = this.targetHPPercent;
        this.backHPPercent = this.targetHPPercent;

        this.StopHealthAnimation();
    }

    /**
     * 绑定到玩家对象，自动监听数据变化
     * @param player 玩家对象
     * @param initialDefense 初始防御力值
     * @param initialCurrentHP 初始当前血量（可选）
     * @param initialMaxHP 初始最大血量（可选）
     */
    public BindToPlayer(player: UE.Actor, initialDefense: number, initialCurrentHP?: number, initialMaxHP?: number): void {
        if (this.boundPlayer) {
            this.UnbindFromPlayer();
        }

        this.boundPlayer = player;

        // 设置初始值（立即设置，不播放动画）
        this.BindDefense(initialDefense);
        if (initialCurrentHP !== undefined && initialMaxHP !== undefined) {
            this.BindHealthAndDefense(initialCurrentHP, initialMaxHP, initialDefense, true);
        }

        const playerId = player.GetName();

        // 订阅该玩家专属的防御力变化事件
        this.onDefenseChangedCallback = (data: { newDefense: number }) => {
            this.BindDefense(data.newDefense);
        };
        EventSystem.subscribe(`onPlayerDefenseChanged_${playerId}`, this.onDefenseChangedCallback);

        // 订阅该玩家专属的血量变化事件（后续变化播放动画）
        this.onHealthChangedCallback = (data: { currentHP: number, maxHP: number, defense: number }) => {
            this.BindHealthAndDefense(data.currentHP, data.maxHP, data.defense, false);
        };
        EventSystem.subscribe(`onPlayerHealthChanged_${playerId}`, this.onHealthChangedCallback);

        console.log(`血条UI已绑定到玩家: ${playerId}`);
    }

    /**
     * 解除玩家绑定，停止监听数据变化
     */
    public UnbindFromPlayer(): void {
        if (!this.boundPlayer) {
            return;
        }

        const playerId = this.boundPlayer.GetName();

        // 取消订阅事件
        if (this.onDefenseChangedCallback) {
            EventSystem.unsubscribe(`onPlayerDefenseChanged_${playerId}`, this.onDefenseChangedCallback);
            this.onDefenseChangedCallback = null;
        }

        if (this.onHealthChangedCallback) {
            EventSystem.unsubscribe(`onPlayerHealthChanged_${playerId}`, this.onHealthChangedCallback);
            this.onHealthChangedCallback = null;
        }

        console.log(`血条UI已解绑玩家: ${playerId}`);
        this.boundPlayer = null;
    }

    /**
     * Widget销毁时自动清理
     */
    public Destruct(): void {
        this.StopHealthAnimation();
        this.UnbindFromPlayer();
    }

    /**
     * 设置动画速度
     * @param speed 动画速度（每秒变化的百分比，默认2.0）
     */
    public SetAnimationSpeed(speed: number): void {
        this.animationSpeed = Math.max(0.1, speed);
    }

    /**
     * 获取当前血量百分比
     */
    public GetCurrentHPPercent(): number {
        return this.currentHPPercent;
    }
}

blueprint.mixin(jsClass, TS_Common_HPBar);