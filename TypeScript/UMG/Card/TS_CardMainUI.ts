import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardMainUI.Widget_CardMainUI_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_CardMainUI extends UE.Game.UI.Blueprints.Cards.Widget_CardMainUI.Widget_CardMainUI_C {}

export class TS_CardMainUI implements TS_CardMainUI {

    static pawn: UE.CurrsorCharacter;
    static PlayerState: UE.CurrsorPlayerState;
    
    Construct() {
        TS_CardMainUI.pawn = this.GetOwningPlayerPawn() as UE.CurrsorCharacter;
        TS_CardMainUI.PlayerState = TS_CardMainUI.pawn.PlayerState as UE.CurrsorPlayerState;
        
        this.InitCard();
    }
    
    private InitCard(): void {
        const deckCardNames = TS_CardMainUI.PlayerState.DeckCardNames;
            
        for (let i = 0; i < deckCardNames.Num(); i++) {
            const cardName = deckCardNames.Get(i);
            
            const cardInfo = ((TS_CardMainUI.pawn as any).GetDataFromName(cardName)) as UE.Game.Data.Structs.S_CardInfo.S_CardInfo;
                    
            this.drawPile.Add(cardInfo);
        }

        this.AddCard(5);
        this.Widget_CardListButton.AmountText.SetText(this.drawPile.Num().toString());
    }

    // 抽取手牌
    public AddCard(numCards: number = 1): void {
        if (this.drawPile.Num() === 0) return;
        
        const cardsToAdd = Math.min(numCards, this.drawPile.Num());
        for (let i = 0; i < cardsToAdd; i++) {
            const randomIndex = Math.floor(Math.random() * this.drawPile.Num());
            const card = this.drawPile.Get(randomIndex);
            this.handCards.Add(card);
            this.drawPile.RemoveAt(randomIndex);
        }
        
        this.Widget_CardListButton.AmountText.SetText(this.drawPile.Num().toString());
    }
    
    // 弃牌（将手牌移动到弃牌堆）
    public DiscardCard(cardIndex: number): void {
        
    }
    
    // 洗牌（将弃牌堆放回抽牌堆并洗牌）
    public ShuffleDiscardPile(): void {
        
    }
    
    // 洗抽牌堆
    private ShuffleDrawPile(): void {
        
    }
}

blueprint.mixin(jsClass, TS_CardMainUI);