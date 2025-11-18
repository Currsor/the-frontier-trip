import * as UE from 'ue';
import { GameConfig } from '../../Config/GameConfig';

/**
 * 卡牌动画数据
 */
interface CardAnimationData {
    widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C;
    startPos: UE.Vector2D;
    startRotation: number;
    startScale: UE.Vector2D;
    targetPos: UE.Vector2D;
    targetRotation: number;
    targetScale: UE.Vector2D;
    currentTime: number;
    duration: number;
    isAnimating: boolean;
}

/**
 * 卡牌出场插值动画系统
 * 负责管理卡牌从startHook位置到目标位置的动画
 */
export class CardAnimationSystem {
    private animatingCards: Map<UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C, CardAnimationData> = new Map();
    private owningWidget: UE.UserWidget;
    private updateHandle: any = null;
    
    constructor(owningWidget: UE.UserWidget) {
        this.owningWidget = owningWidget;
    }
    
    /**
     * 开始卡牌出场动画
     * @param widget 卡牌Widget
     * @param startHookWidget 起始位置的Widget（通常是抽牌堆按钮）
     * @param targetPos 目标位置
     * @param targetRotation 目标旋转角度
     * @param targetScale 目标缩放
     * @param duration 动画时长（秒）
     */
    public StartCardAnimation(
        widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C,
        startHookWidget: UE.Widget,
        targetPos: UE.Vector2D,
        targetRotation: number,
        targetScale: UE.Vector2D,
        duration: number = GameConfig.CARD_CONFIG.CARD_DRAW_DURATION
    ): void {
        // 获取startHook的世界位置
        const startPos = this.GetWidgetPosition(startHookWidget);
        
        // 创建动画数据
        const animData: CardAnimationData = {
            widget: widget,
            startPos: startPos,
            startRotation: 0,
            startScale: new UE.Vector2D(0.5, 0.5), // 从小开始
            targetPos: targetPos,
            targetRotation: targetRotation,
            targetScale: targetScale,
            currentTime: 0,
            duration: duration,
            isAnimating: true
        };
        
        // 设置初始状态
        const slot = widget.Slot as UE.CanvasPanelSlot;
        if (slot) {
            slot.SetPosition(startPos);
        }
        widget.SetRenderScale(animData.startScale);
        widget.SetRenderTransformAngle(animData.startRotation);
        
        // 添加到动画列表
        this.animatingCards.set(widget, animData);
        
        // 启动更新循环
        this.StartUpdateLoop();
    }
    
    /**
     * 更新所有正在动画的卡牌的目标位置
     * @param newTargets 新的目标位置映射 (widget -> targetPos)
     */
    public UpdateAllTargetPositions(
        newTargets: Map<UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C, {
            pos: UE.Vector2D,
            rotation: number,
            scale: UE.Vector2D
        }>
    ): void {
        for (const [widget, animData] of this.animatingCards) {
            const newTarget = newTargets.get(widget);
            if (newTarget && animData.isAnimating) {
                // 更新目标位置，但保持当前进度
                animData.targetPos = newTarget.pos;
                animData.targetRotation = newTarget.rotation;
                animData.targetScale = newTarget.scale;
            }
        }
    }
    
    /**
     * 为不在动画的卡牌启动插值动画到新位置
     * @param widget 卡牌Widget
     * @param targetPos 目标位置
     * @param targetRotation 目标旋转角度
     * @param targetScale 目标缩放
     * @param duration 动画时长（秒）
     */
    public StartRepositionAnimation(
        widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C,
        targetPos: UE.Vector2D,
        targetRotation: number,
        targetScale: UE.Vector2D,
        duration: number = 0.3
    ): void {
        const currentPos = this.GetWidgetPosition(widget);
        const currentRotation = widget.GetRenderTransformAngle();
        
        // 尝试从现有动画数据中获取当前缩放
        let currentScale = targetScale;
        const existingAnim = this.animatingCards.get(widget);
        if (existingAnim) {
            currentScale = existingAnim.targetScale;
        }
        
        // 创建动画数据
        const animData: CardAnimationData = {
            widget: widget,
            startPos: currentPos,
            startRotation: currentRotation,
            startScale: currentScale,
            targetPos: targetPos,
            targetRotation: targetRotation,
            targetScale: targetScale,
            currentTime: 0,
            duration: duration,
            isAnimating: true
        };
        
        // 添加到动画列表（如果已存在则覆盖）
        this.animatingCards.set(widget, animData);
        
        // 启动更新循环
        this.StartUpdateLoop();
    }
    
    /**
     * 启动更新循环
     */
    private StartUpdateLoop(): void {
        if (this.updateHandle) return; // 已经在运行
        
        // 使用定时器更新动画
        this.updateHandle = setInterval(() => {
            this.UpdateAnimations();
        }, 16); // 约60fps
    }
    
    /**
     * 停止更新循环
     */
    private StopUpdateLoop(): void {
        if (this.updateHandle) {
            clearInterval(this.updateHandle);
            this.updateHandle = null;
        }
    }
    
    /**
     * 更新所有动画
     */
    private UpdateAnimations(): void {
        const deltaTime = 0.016; // 约60fps
        let hasActiveAnimations = false;
        
        for (const [widget, animData] of this.animatingCards) {
            if (!animData.isAnimating) continue;
            
            hasActiveAnimations = true;
            
            // 更新时间
            animData.currentTime += deltaTime;
            const progress = Math.min(animData.currentTime / animData.duration, 1.0);
            
            // 使用缓动函数（EaseOutCubic）
            const easedProgress = this.EaseOutCubic(progress);
            
            // 插值位置
            const currentPos = this.LerpVector2D(animData.startPos, animData.targetPos, easedProgress);
            
            // 插值旋转
            const currentRotation = this.Lerp(animData.startRotation, animData.targetRotation, easedProgress);
            
            // 插值缩放
            const currentScale = this.LerpVector2D(animData.startScale, animData.targetScale, easedProgress);
            
            // 应用变换
            const slot = widget.Slot as UE.CanvasPanelSlot;
            if (slot) {
                slot.SetPosition(currentPos);
            }
            widget.SetRenderScale(currentScale);
            widget.SetRenderTransformAngle(currentRotation);
            
            // 检查动画是否完成
            if (progress >= 1.0) {
                animData.isAnimating = false;
                this.OnAnimationComplete(widget);
            }
        }
        
        // 如果没有活动动画，停止更新循环
        if (!hasActiveAnimations) {
            this.StopUpdateLoop();
            this.CleanupCompletedAnimations();
        }
    }
    
    /**
     * 动画完成回调
     */
    private OnAnimationComplete(widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C): void {
        // 可以在这里添加完成回调
        console.log('卡牌动画完成');
    }
    
    /**
     * 清理已完成的动画
     */
    private CleanupCompletedAnimations(): void {
        const toRemove: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C[] = [];
        
        for (const [widget, animData] of this.animatingCards) {
            if (!animData.isAnimating) {
                toRemove.push(widget);
            }
        }
        
        for (const widget of toRemove) {
            this.animatingCards.delete(widget);
        }
    }
    
    /**
     * 获取Widget的位置
     */
    private GetWidgetPosition(widget: UE.Widget): UE.Vector2D {
        const slot = widget.Slot as UE.CanvasPanelSlot;
        if (slot) {
            return slot.GetPosition();
        }
        return new UE.Vector2D(0, 0);
    }
    
    /**
     * 线性插值
     */
    private Lerp(a: number, b: number, t: number): number {
        return a + (b - a) * t;
    }
    
    /**
     * Vector2D插值
     */
    private LerpVector2D(a: UE.Vector2D, b: UE.Vector2D, t: number): UE.Vector2D {
        return new UE.Vector2D(
            this.Lerp(a.X, b.X, t),
            this.Lerp(a.Y, b.Y, t)
        );
    }
    
    /**
     * EaseOutCubic缓动函数
     */
    private EaseOutCubic(t: number): number {
        return 1 - Math.pow(1 - t, 3);
    }
    
    /**
     * 检查Widget是否正在动画
     */
    public IsAnimating(widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C): boolean {
        const animData = this.animatingCards.get(widget);
        return animData ? animData.isAnimating : false;
    }
    
    /**
     * 停止指定Widget的动画
     */
    public StopAnimation(widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C): void {
        const animData = this.animatingCards.get(widget);
        if (animData) {
            animData.isAnimating = false;
        }
    }
    
    /**
     * 停止所有动画
     */
    public StopAllAnimations(): void {
        for (const animData of this.animatingCards.values()) {
            animData.isAnimating = false;
        }
        this.StopUpdateLoop();
        this.animatingCards.clear();
    }
    
    /**
     * 清理资源
     */
    public Cleanup(): void {
        this.StopAllAnimations();
    }
}
