import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/Blueprints/BP_Cube.BP_Cube_C");
const jsClass = blueprint.tojs<typeof UE.Game.Blueprints.BP_Cube.BP_Cube_C>(uclass);

interface TS_Cube extends UE.Game.Blueprints.BP_Cube.BP_Cube_C {}

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
    ReceiveTick(deltaSeconds: number): void {
        this.K2_AddActorLocalRotation(new UE.Rotator(0, deltaSeconds * 5, deltaSeconds * 10), false, $ref<UE.HitResult>(), false);
    }
}

blueprint.mixin(jsClass, TS_Cube);