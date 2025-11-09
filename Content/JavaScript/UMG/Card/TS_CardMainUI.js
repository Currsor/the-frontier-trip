"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_CardMainUI = void 0;
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardMainUI.Widget_CardMainUI_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CardMainUI {
    static pawn;
    static PlayerState;
    Construct() {
        TS_CardMainUI.pawn = this.GetOwningPlayerPawn();
        TS_CardMainUI.PlayerState = TS_CardMainUI.pawn.PlayerState;
        this.InitCard();
    }
    InitCard() {
        const deckCardNames = TS_CardMainUI.PlayerState.DeckCardNames;
        for (let i = 0; i < deckCardNames.Num(); i++) {
            const cardName = deckCardNames.Get(i);
            const cardInfo = (TS_CardMainUI.pawn.GetDataFromName(cardName));
            this.drawPile.Add(cardInfo);
        }
        this.AddCard(5);
        this.Widget_CardListButton.AmountText.SetText(this.drawPile.Num().toString());
    }
    // 抽取手牌
    AddCard(numCards = 1) {
        if (this.drawPile.Num() === 0)
            return;
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
    DiscardCard(cardIndex) {
    }
    // 洗牌（将弃牌堆放回抽牌堆并洗牌）
    ShuffleDiscardPile() {
    }
    // 洗抽牌堆
    ShuffleDrawPile() {
    }
}
exports.TS_CardMainUI = TS_CardMainUI;
puerts_1.blueprint.mixin(jsClass, TS_CardMainUI);
//# sourceMappingURL=TS_CardMainUI.js.map