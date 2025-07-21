import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorPlayerController.BP_CurrsorPlayerController_C");

const jsClass = blueprint.tojs<typeof UE.Game.Blueprints.Character.Player.BP_CurrsorPlayerController.BP_CurrsorPlayerController_C>(uclass);

class TS_CurrsorPlayerController extends jsClass {
    private moveSpeed: number = 0;

    ReceiveBeginPlay(): void {
        this.moveSpeed = 600;
        this.Initialize();
    }

    public Initialize(): void {
        if (isNaN(this.moveSpeed)) {
            console.error("moveSpeed is NaN! Setting to default value.");
            this.moveSpeed = 600;
        }
        this.SetMovementSpeedFromTS(this.moveSpeed);
        console.log("Initialized with speed:", this.moveSpeed);
    }

    public UpdateMovementSpeed(newSpeed: number): void {
        this.moveSpeed = newSpeed;
        this.SetMovementSpeedFromTS(newSpeed);
    }
}

blueprint.mixin(jsClass, TS_CurrsorPlayerController);