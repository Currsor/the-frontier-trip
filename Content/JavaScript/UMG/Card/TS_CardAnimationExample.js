"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.CardAnimationExample = void 0;
const UE = require("ue");
const TS_CardAnimationSystem_1 = require("./TS_CardAnimationSystem");
const GameConfig_1 = require("../../Config/GameConfig");
/**
 * 卡牌动画系统使用示例
 *
 * 这个文件展示了如何使用CardAnimationSystem
 */
class CardAnimationExample {
    animationSystem;
    owningWidget;
    constructor(owningWidget) {
        this.owningWidget = owningWidget;
        this.animationSystem = new TS_CardAnimationSystem_1.CardAnimationSystem(owningWidget);
    }
    /**
     * 示例1：基本的卡牌出场动画
     */
    Example1_BasicCardAnimation(cardWidget, startHook) {
        // 定义目标位置
        const targetPos = new UE.Vector2D(100, 200);
        const targetRotation = 5; // 5度旋转
        const targetScale = new UE.Vector2D(1.0, 1.0);
        // 启动动画
        this.animationSystem.StartCardAnimation(cardWidget, startHook, targetPos, targetRotation, targetScale, 0.5 // 0.5秒动画时长
        );
    }
    /**
     * 示例2：批量卡牌动画（模拟抽多张牌）
     */
    Example2_MultipleCardAnimation(cardWidgets, startHook) {
        const spacing = GameConfig_1.GameConfig.CARD_CONFIG.CARD_SPACING;
        const totalWidth = (cardWidgets.length - 1) * spacing;
        const startX = -totalWidth / 2;
        // 为每张卡牌启动动画，添加延迟以产生连续效果
        cardWidgets.forEach((widget, index) => {
            setTimeout(() => {
                const x = startX + index * spacing;
                const normalizedX = (index - (cardWidgets.length - 1) / 2) / Math.max(cardWidgets.length - 1, 1);
                const y = -Math.abs(normalizedX) * GameConfig_1.GameConfig.CARD_CONFIG.HAND_CURVE_HEIGHT;
                const rotation = normalizedX * 5;
                this.animationSystem.StartCardAnimation(widget, startHook, new UE.Vector2D(x, y), rotation, new UE.Vector2D(1.0, 1.0), GameConfig_1.GameConfig.CARD_CONFIG.CARD_DRAW_DURATION);
            }, index * 100); // 每张卡牌延迟100ms
        });
    }
    /**
     * 示例3：动态调整目标位置（新增卡牌时）
     */
    Example3_DynamicTargetUpdate(existingWidgets, newWidget, startHook) {
        // 添加新卡牌到列表
        const allWidgets = [...existingWidgets, newWidget];
        // 重新计算所有卡牌的目标位置
        const newTargets = this.CalculateTargetPositions(allWidgets);
        // 更新正在动画的卡牌的目标位置
        this.animationSystem.UpdateAllTargetPositions(newTargets);
        // 为新卡牌启动动画
        const target = newTargets.get(newWidget);
        if (target) {
            this.animationSystem.StartCardAnimation(newWidget, startHook, target.pos, target.rotation, target.scale, GameConfig_1.GameConfig.CARD_CONFIG.CARD_DRAW_DURATION);
        }
    }
    /**
     * 示例4：检查动画状态
     */
    Example4_CheckAnimationState(cardWidget) {
        if (this.animationSystem.IsAnimating(cardWidget)) {
            console.log('卡牌正在动画中...');
            // 可以选择停止动画
            // this.animationSystem.StopAnimation(cardWidget);
        }
        else {
            console.log('卡牌动画已完成或未开始');
        }
    }
    /**
     * 示例5：自定义动画时长和效果
     */
    Example5_CustomAnimation(cardWidget, startHook) {
        // 快速动画（0.2秒）
        this.animationSystem.StartCardAnimation(cardWidget, startHook, new UE.Vector2D(0, 0), 0, new UE.Vector2D(1.0, 1.0), 0.2 // 快速动画
        );
        // 或者慢速动画（1.0秒）
        // duration: 1.0
    }
    /**
     * 示例6：清理资源
     */
    Example6_Cleanup() {
        // 停止所有动画
        this.animationSystem.StopAllAnimations();
        // 清理资源
        this.animationSystem.Cleanup();
    }
    /**
     * 辅助方法：计算目标位置
     */
    CalculateTargetPositions(widgets) {
        const targetMap = new Map();
        const spacing = GameConfig_1.GameConfig.CARD_CONFIG.CARD_SPACING;
        const curveHeight = GameConfig_1.GameConfig.CARD_CONFIG.HAND_CURVE_HEIGHT;
        const cardScale = GameConfig_1.GameConfig.CARD_CONFIG.CARD_SCALE;
        const totalWidth = (widgets.length - 1) * spacing;
        const startX = -totalWidth / 2;
        widgets.forEach((widget, index) => {
            const x = startX + index * spacing;
            const normalizedX = (index - (widgets.length - 1) / 2) / Math.max(widgets.length - 1, 1);
            const y = -Math.abs(normalizedX) * curveHeight;
            const rotation = normalizedX * 5;
            targetMap.set(widget, {
                pos: new UE.Vector2D(x, y),
                rotation: rotation,
                scale: new UE.Vector2D(cardScale, cardScale)
            });
        });
        return targetMap;
    }
}
exports.CardAnimationExample = CardAnimationExample;
/**
 * 使用方法：
 *
 * 1. 在你的Widget类中创建实例：
 *    this.animExample = new CardAnimationExample(this);
 *
 * 2. 调用示例方法：
 *    this.animExample.Example1_BasicCardAnimation(cardWidget, startHook);
 *
 * 3. 在Destruct时清理：
 *    this.animExample.Example6_Cleanup();
 */
//# sourceMappingURL=TS_CardAnimationExample.js.map