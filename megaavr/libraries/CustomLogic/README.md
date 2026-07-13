# CustomLogic — Wazamono用ハードウェアロジックゲートライブラリ

AVR DUシリーズ内蔵のCCL(Configurable Custom Logic)を、SerialやSDと同じ感覚で
使えるようにしたWazamonoCore専用ライブラリです。`begin()`した瞬間から
**CPUを一切使わずに**ハードウェアだけで論理ゲートが動き続けます。

```cpp
#include <CustomLogic.h>

void setup() {
  CustomLogic.begin(AND);                  // OUT = IN0 AND IN1
  // CustomLogic.begin(OR, OR);            // 3入力OR: (IN0 OR IN1) OR IN2
  // CustomLogic.begin(AND, OR);           // (IN0 AND IN1) OR IN2
  // CustomLogic.beginTruthTable(0x96, 3); // 任意の真理値表(3入力XOR)
}

void loop() {
  // 何も書かなくてもゲートは動き続けます
}
```

## ユニットとピン(ハードウェア固定)

| | IN0 | IN1 | IN2 | OUT |
|---|---|---|---|---|
| **CustomLogic** (Tachi) | A3 (PD0) | A2 (PD1) | A1 (PD2) | A0 (PD3) |
| **CustomLogic** (Tsurugi) | D5 (PD0) | D6 (PD1) | D9 (PD2) | D10 (PD3) |
| **CustomLogic** (Kunai) | D4 (PA0) | D5 (PA1) | D3 (PA2) | D2 (PA3) |
| **CustomLogic1** (Tachi) | D5 (PF0) | D6 (PF1) | D7 (PF2) | D8 (PF3) |
| **CustomLogic1** (Tsurugi) | A0 (PF0) | A1 (PF1) | A2 (PF2) | A3 (PF3) |

- Kunaiは1ユニットのみ(CustomLogic1はありません)
- 入力ピンは自動的にプルアップされるので、ボタンをGNDへ直結するだけで実験できます
  (駆動されたロジック信号はプルアップに勝つのでそのまま接続可)

## API

| メソッド | 説明 |
|---|---|
| `begin(logic1)` | 2入力ゲート: OUT = IN0 (logic1) IN1。`LogicType`は`AND/OR/XOR/NAND/NOR/XNOR/NOT`(NOTはIN0のみの1入力インバータ) |
| `begin(logic1, logic2)` | 3入力ロジック: OUT = (IN0 logic1 IN1) logic2 IN2。例: `begin(AND, OR)`=A・B+C(NOTは合成不可) |
| `beginTruthTable(table, n)` | 真理値表を直接指定。bit iが「入力の並びが数値iのときの出力」(IN2=bit2, IN1=bit1, IN0=bit0) |
| `read()` | OUTピンの現在の状態 |
| `attachInterrupt(fn, mode)` | 出力変化で関数呼び出し(`RISING/FALLING/CHANGE`) |
| `detachInterrupt()` | 割り込み解除 |
| `end()` | 停止してピンを解放 |

## サンプル

- **TwoInputAND** — 2入力ANDゲート(ボタン2個+LED)
- **ThreeInputOR** — 3入力OR。ゲート名を書き換えるだけで他のゲートに
- **TruthTable** — 真理値表の直接指定(3入力XOR=0x96)
- **EdgeInterrupt** — ゲート出力の変化で割り込み
- **DualUnits** — 2ユニット同時動作(Tachi/Tsurugi)

## 注意

- **AnalogCompとのピン共有(Tachi/Tsurugi)**: CustomLogicのIN2/OUT(PD2/PD3)は
  AnalogCompの初期入力と同じピンです。同時に使う場合は
  `AnalogComp.setInputs(PIN_PD6, PIN_PD7)`で退避するか、CustomLogic1を使ってください。
- **KunaiのI2Cとの共有**: KunaiのCustomLogic(PA0〜PA3)はI2Cピン(D4/D5および
  代替のD3/D2)と重なるため、Wireとの同時使用はできません。
- ゲートの応答はハードウェア直結です(フィルタなし)。チャタリングのある機械接点を
  割り込みで数える場合はその点に留意してください。

## 実装の出自

本ライブラリはAVR64DU28/32データシート(DS40002548A)とMicrochip公式デバイスヘッダのみを
情報源とした独立実装です。DxCore/megaTinyCoreのLogicライブラリを含む既存の
CCLライブラリのコードは使用・参照していません。
