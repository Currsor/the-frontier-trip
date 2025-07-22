"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/Blueprints/BP_Cube.BP_Cube_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_Cube extends jsClass {
    /**
     * 注意：
     * 1. 蓝图对象的实例化由引擎完成，TypeScript 侧的构造函数（constructor）不会被自动调用。
     * 2. 只有在 TypeScript 代码中主动 new TS_Cube() 时，才会执行构造函数。
     * 3. 通常情况下，建议在 ReceiveBeginPlay 等生命周期函数中做初始化逻辑。
     *
     * 示例：
     * constructor() {
     *     super();
     *     console.log("BP_Cube instance created");
     *
     *     // 在这里添加初始化逻辑
     * }
     */
    ReceiveTick(deltaSeconds) {
        this.K2_AddActorLocalRotation(new UE.Rotator(0, deltaSeconds * 5, deltaSeconds * 10), false, (0, puerts_1.$ref)(), false);
    }
}
puerts_1.blueprint.mixin(jsClass, TS_Cube);
//# sourceMappingURL=BP_Cube.js.map