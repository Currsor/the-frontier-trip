/**
 * 事件系统
 * 提供发布-订阅模式的事件管理
 */
export class EventSystem {
    private static listeners: Map<string, Function[]> = new Map();
    private static eventHistory: Array<{event: string, data: any, timestamp: number}> = [];
    private static maxHistorySize: number = 100;

    /**
     * 订阅事件
     * @param event 事件名称
     * @param callback 回调函数
     * @param priority 优先级（数字越大优先级越高）
     */
    static subscribe(event: string, callback: Function, priority: number = 0): void {
        if (!this.listeners.has(event)) {
            this.listeners.set(event, []);
        }
        
        const callbacks = this.listeners.get(event)!;
        
        // 根据优先级插入回调函数
        const callbackWithPriority = Object.assign(callback, { priority });
        
        let insertIndex = callbacks.length;
        for (let i = 0; i < callbacks.length; i++) {
            if ((callbacks[i] as any).priority < priority) {
                insertIndex = i;
                break;
            }
        }
        
        callbacks.splice(insertIndex, 0, callbackWithPriority);
        
        console.log(`Event subscribed: ${event} (Priority: ${priority})`);
    }

    /**
     * 取消订阅事件
     * @param event 事件名称
     * @param callback 回调函数
     */
    static unsubscribe(event: string, callback: Function): void {
        const callbacks = this.listeners.get(event);
        if (callbacks) {
            const index = callbacks.indexOf(callback);
            if (index > -1) {
                callbacks.splice(index, 1);
                console.log(`Event unsubscribed: ${event}`);
            }
        }
    }

    /**
     * 发布事件
     * @param event 事件名称
     * @param data 事件数据
     * @param immediate 是否立即执行（否则在下一帧执行）
     */
    static emit(event: string, data?: any, immediate: boolean = true): void {
        // 记录事件历史
        this.addToHistory(event, data);
        
        const callbacks = this.listeners.get(event);
        if (!callbacks || callbacks.length === 0) {
            return;
        }

        if (immediate) {
            this.executeCallbacks(event, callbacks, data);
        } else {
            // 延迟到下一帧执行
            setTimeout(() => {
                this.executeCallbacks(event, callbacks, data);
            }, 0);
        }
    }

    /**
     * 执行回调函数
     * @param event 事件名称
     * @param callbacks 回调函数列表
     * @param data 事件数据
     */
    private static executeCallbacks(event: string, callbacks: Function[], data: any): void {
        console.log(`Event emitted: ${event}`, data);
        
        callbacks.forEach(callback => {
            try {
                callback(data);
            } catch (error) {
                console.error(`Error in event callback for ${event}:`, error);
            }
        });
    }

    /**
     * 添加到事件历史
     * @param event 事件名称
     * @param data 事件数据
     */
    private static addToHistory(event: string, data: any): void {
        this.eventHistory.push({
            event,
            data,
            timestamp: Date.now()
        });

        // 限制历史记录大小
        if (this.eventHistory.length > this.maxHistorySize) {
            this.eventHistory.shift();
        }
    }

    /**
     * 获取事件历史
     * @param event 事件名称（可选，不传则返回所有历史）
     * @param limit 限制数量
     * @returns 事件历史记录
     */
    static getEventHistory(event?: string, limit: number = 10): Array<{event: string, data: any, timestamp: number}> {
        let history = this.eventHistory;
        
        if (event) {
            history = history.filter(record => record.event === event);
        }
        
        return history.slice(-limit);
    }

    /**
     * 清除所有事件监听器
     */
    static clearAllListeners(): void {
        this.listeners.clear();
        console.log("All event listeners cleared");
    }

    /**
     * 清除指定事件的所有监听器
     * @param event 事件名称
     */
    static clearEventListeners(event: string): void {
        this.listeners.delete(event);
        console.log(`Event listeners cleared for: ${event}`);
    }

    /**
     * 获取所有已注册的事件名称
     * @returns 事件名称数组
     */
    static getRegisteredEvents(): string[] {
        return Array.from(this.listeners.keys());
    }

    /**
     * 获取指定事件的监听器数量
     * @param event 事件名称
     * @returns 监听器数量
     */
    static getListenerCount(event: string): number {
        const callbacks = this.listeners.get(event);
        return callbacks ? callbacks.length : 0;
    }

    /**
     * 一次性事件监听（只触发一次后自动取消订阅）
     * @param event 事件名称
     * @param callback 回调函数
     * @param priority 优先级
     */
    static once(event: string, callback: Function, priority: number = 0): void {
        const onceCallback = (data: any) => {
            callback(data);
            this.unsubscribe(event, onceCallback);
        };
        
        this.subscribe(event, onceCallback, priority);
    }

    /**
     * 批量发布事件
     * @param events 事件数组，每个元素包含 {event, data}
     */
    static emitBatch(events: Array<{event: string, data?: any}>): void {
        events.forEach(({event, data}) => {
            this.emit(event, data, false); // 延迟执行以避免阻塞
        });
    }

    /**
     * 调试信息：打印所有事件监听器
     */
    static debugPrintListeners(): void {
        console.log("=== Event System Debug Info ===");
        this.listeners.forEach((callbacks, event) => {
            console.log(`Event: ${event}, Listeners: ${callbacks.length}`);
            callbacks.forEach((callback, index) => {
                const priority = (callback as any).priority || 0;
                console.log(`  [${index}] Priority: ${priority}`);
            });
        });
        console.log("===============================");
    }
}