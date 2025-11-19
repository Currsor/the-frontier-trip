"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.CardAnimationSystem = void 0;
const UE = require("ue");
const GameConfig_1 = require("../../Config/GameConfig");
/**
 * 卡牌出场插值动画系统
 * 负责管理卡牌从startHook位置到目标位置的动画
 */
class CardAnimationSystem {
    animatingCards = new Map();
    hoverStates = new Map();
    owningWidget;
    updateHandle = null;
    // 悬浮效果配置
    HOVER_SCALE = GameConfig_1.GameConfig.CARD_CONFIG.CARD_HOVER_SCALE; // 1.1倍放大
    HOVER_OFFSET_Y = -80; // 向上偏移80像素
    HOVER_Z_ORDER = 1000; // 最高层级
    HOVER_DURATION = 0.2; // 悬浮动画时长（秒）
    constructor(owningWidget) {
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
    StartCardAnimation(widget, startHookWidget, targetPos, targetRotation, targetScale, duration = GameConfig_1.GameConfig.CARD_CONFIG.CARD_DRAW_DURATION) {
        // 获取startHook的世界位置
        const startPos = this.GetWidgetPosition(startHookWidget);
        // 创建动画数据
        const animData = {
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
        const slot = widget.Slot;
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
    UpdateAllTargetPositions(newTargets) {
        for (const [widget, animData] of this.animatingCards) {
            const newTarget = newTargets.get(widget);
            if (newTarget && animData.isAnimating) {
                // 更新目标位置，但保持当前进度
                animData.targetPos = newTarget.pos;
                animData.targetRotation = newTarget.rotation;
                animData.targetScale = newTarget.scale;
                // 立即设置层级（不需要插值）
                const slot = widget.Slot;
                if (slot) {
                    slot.SetZOrder(newTarget.zOrder);
                }
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
    StartRepositionAnimation(widget, targetPos, targetRotation, targetScale, duration = 0.3) {
        // 检查是否已经在动画中
        const existingAnim = this.animatingCards.get(widget);
        let startPos;
        let startRotation;
        let startScale;
        if (existingAnim && existingAnim.isAnimating) {
            // 如果正在动画中，从当前的目标位置作为新的起点
            // 这样可以避免瞬移，实现平滑过渡
            startPos = existingAnim.targetPos;
            startRotation = existingAnim.targetRotation;
            startScale = existingAnim.targetScale;
        }
        else {
            // 如果不在动画中，从当前实际位置作为起点
            startPos = this.GetWidgetPosition(widget);
            startRotation = widget.GetRenderTransformAngle();
            startScale = targetScale; // 使用目标缩放作为起始缩放
        }
        // 创建动画数据
        const animData = {
            widget: widget,
            startPos: startPos,
            startRotation: startRotation,
            startScale: startScale,
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
    StartUpdateLoop() {
        if (this.updateHandle)
            return; // 已经在运行
        // 使用定时器更新动画
        this.updateHandle = setInterval(() => {
            this.UpdateAnimations();
        }, 16); // 约60fps
    }
    /**
     * 停止更新循环
     */
    StopUpdateLoop() {
        if (this.updateHandle) {
            clearInterval(this.updateHandle);
            this.updateHandle = null;
        }
    }
    /**
     * 更新所有动画
     */
    UpdateAnimations() {
        const deltaTime = 0.016; // 约60fps
        let hasActiveAnimations = false;
        for (const [widget, animData] of this.animatingCards) {
            if (!animData.isAnimating)
                continue;
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
            const slot = widget.Slot;
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
    OnAnimationComplete(widget) {
        // 清理已结束的悬浮状态
        const hoverState = this.hoverStates.get(widget);
        if (hoverState && !hoverState.isHovered) {
            // 如果悬浮状态已标记为非悬浮，且动画已完成，则清理状态
            this.hoverStates.delete(widget);
        }
        // console.log('卡牌动画完成');
    }
    /**
     * 清理已完成的动画
     */
    CleanupCompletedAnimations() {
        const toRemove = [];
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
    GetWidgetPosition(widget) {
        const slot = widget.Slot;
        if (slot) {
            return slot.GetPosition();
        }
        return new UE.Vector2D(0, 0);
    }
    /**
     * 线性插值
     */
    Lerp(a, b, t) {
        return a + (b - a) * t;
    }
    /**
     * Vector2D插值
     */
    LerpVector2D(a, b, t) {
        return new UE.Vector2D(this.Lerp(a.X, b.X, t), this.Lerp(a.Y, b.Y, t));
    }
    /**
     * EaseOutCubic缓动函数
     */
    EaseOutCubic(t) {
        return 1 - Math.pow(1 - t, 3);
    }
    /**
     * 检查Widget是否正在动画
     */
    IsAnimating(widget) {
        const animData = this.animatingCards.get(widget);
        return animData ? animData.isAnimating : false;
    }
    /**
     * 停止指定Widget的动画
     */
    StopAnimation(widget) {
        const animData = this.animatingCards.get(widget);
        if (animData) {
            animData.isAnimating = false;
        }
    }
    /**
     * 停止所有动画
     */
    StopAllAnimations() {
        for (const animData of this.animatingCards.values()) {
            animData.isAnimating = false;
        }
        this.StopUpdateLoop();
        this.animatingCards.clear();
    }
    /**
     * 开始卡牌悬浮动画
     * @param widget 卡牌Widget
     */
    StartHoverAnimation(widget) {
        // 检查是否已经悬浮
        const existingHoverState = this.hoverStates.get(widget);
        if (existingHoverState && existingHoverState.isHovered)
            return;
        // 保存原始状态
        const slot = widget.Slot;
        if (!slot)
            return;
        let originalPos;
        let originalRotation;
        let originalScale;
        let originalZOrder;
        // 如果之前有悬浮状态（正在恢复动画中），复用之前保存的原始位置
        // 这样可以避免快速划过时位置累积
        if (existingHoverState) {
            originalPos = existingHoverState.originalPosition;
            originalRotation = existingHoverState.originalRotation;
            originalScale = existingHoverState.originalScale;
            originalZOrder = existingHoverState.originalZOrder;
        }
        else {
            // 首次悬浮，保存当前状态
            originalPos = slot.GetPosition();
            originalRotation = widget.GetRenderTransformAngle();
            originalZOrder = slot.GetZOrder();
            // 获取当前缩放：如果卡牌正在动画中，使用动画目标缩放；否则使用默认缩放
            const animData = this.animatingCards.get(widget);
            if (animData && animData.isAnimating) {
                originalScale = animData.targetScale;
            }
            else {
                originalScale = new UE.Vector2D(1.0, 1.0);
            }
        }
        // 保存悬浮状态
        this.hoverStates.set(widget, {
            widget: widget,
            originalPosition: originalPos,
            originalRotation: originalRotation,
            originalScale: originalScale,
            originalZOrder: originalZOrder,
            isHovered: true
        });
        // 计算目标状态
        const targetPos = new UE.Vector2D(originalPos.X, originalPos.Y + this.HOVER_OFFSET_Y);
        const targetRotation = 0; // 摆正
        const targetScale = new UE.Vector2D(this.HOVER_SCALE, this.HOVER_SCALE);
        // 立即设置最高层级
        slot.SetZOrder(this.HOVER_Z_ORDER);
        // 启动插值动画
        this.StartRepositionAnimation(widget, targetPos, targetRotation, targetScale, this.HOVER_DURATION);
    }
    /**
     * 结束卡牌悬浮动画
     * @param widget 卡牌Widget
     */
    EndHoverAnimation(widget) {
        // 获取悬浮状态
        const hoverState = this.hoverStates.get(widget);
        if (!hoverState || !hoverState.isHovered)
            return;
        // 标记为非悬浮（但保留状态，以便快速重新悬浮时使用）
        hoverState.isHovered = false;
        // 恢复原始层级
        const slot = widget.Slot;
        if (slot) {
            slot.SetZOrder(hoverState.originalZOrder);
        }
        // 启动恢复动画
        this.StartRepositionAnimation(widget, hoverState.originalPosition, hoverState.originalRotation, hoverState.originalScale, this.HOVER_DURATION);
        // 注意：不立即删除悬浮状态，保留它以便快速重新悬浮时使用
        // 状态会在一段时间后自动清理，或在下次布局更新时清理
    }
    /**
     * 检查卡牌是否处于悬浮状态
     * @param widget 卡牌Widget
     */
    IsHovered(widget) {
        const hoverState = this.hoverStates.get(widget);
        return hoverState ? hoverState.isHovered : false;
    }
    /**
     * 更新悬浮卡牌的原始状态（当布局改变时调用）
     * @param widget 卡牌Widget
     * @param newPosition 新的原始位置
     * @param newRotation 新的原始旋转
     * @param newScale 新的原始缩放
     */
    UpdateHoverOriginalState(widget, newPosition, newRotation, newScale) {
        const hoverState = this.hoverStates.get(widget);
        if (hoverState && hoverState.isHovered) {
            // 更新原始状态，但保持悬浮效果
            hoverState.originalPosition = newPosition;
            hoverState.originalRotation = newRotation;
            hoverState.originalScale = newScale;
            // 更新悬浮目标位置
            const targetPos = new UE.Vector2D(newPosition.X, newPosition.Y + this.HOVER_OFFSET_Y);
            // 更新动画目标
            const animData = this.animatingCards.get(widget);
            if (animData && animData.isAnimating) {
                animData.targetPos = targetPos;
                animData.targetRotation = 0;
                animData.targetScale = new UE.Vector2D(this.HOVER_SCALE, this.HOVER_SCALE);
            }
        }
    }
    /**
     * 清理资源
     */
    Cleanup() {
        this.StopAllAnimations();
        this.hoverStates.clear();
    }
}
exports.CardAnimationSystem = CardAnimationSystem;
//# sourceMappingURL=TS_CardAnimationSystem.js.map