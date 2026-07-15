# WazamonoCore

**Wazamono（業物）シリーズ専用 Arduino コア**
USB ネイティブな新世代 AVR（AVR DU シリーズ: `AVR64DU32` / `AVR32DU20`）を搭載した Arduino 互換ボード「Wazamono」シリーズのためのボードサポートパッケージ（Arduino core）です。

![platform](https://img.shields.io/badge/platform-AVR%20DU-blue)
![license](https://img.shields.io/badge/license-LGPL--2.1-green)
![version](https://img.shields.io/badge/core-v0.0.1-orange)
![based on](https://img.shields.io/badge/based%20on-DxCore-lightgrey)

Wazamono シリーズは、定番の Arduino 互換ボードを **USB を内蔵した新世代 AVR** で置き換えることを目指したボード群です。
USB-シリアル変換チップを別途搭載せず、マイコン単体で PC と直接つながります。
WazamonoCore は、これらのボードを Arduino IDE で開発するための専用コアで、[DxCore](https://github.com/SpenceKonde/DxCore) をベースに **Wazamono シリーズに必要な部分だけを残して再構成** しています。

> ⚠️ **開発版（v0.0.1）です。** API・ボード定義・ブートローダは予告なく変更されることがあります。

---

## 対応ボード

| ボード | 由来 | MCU | フォームファクタ | 状態 |
|--------|------|-----|------------------|------|
| [**Wazamono 太刀（Tachi）**](megaavr/extras/WazamonoTachi.md) | Pro Micro 後継 | AVR64DU32 | Pro Micro 互換 / USB-C | ✅ 対応済み |
| [**Wazamono 剣（Tsurugi）**](megaavr/extras/WazamonoTsurugi.md) | Arduino Uno R3 後継 | AVR64DU32 | Uno R3 互換 / USB-C | ✅ 対応済み  |
| [**Wazamono 苦無（Kunai）**](megaavr/extras/WazamonoKunai.md) | Seeeduino XIAO 後継 | AVR32DU20 | XIAO 互換 / USB-C | ✅ 対応済み  |

> このコアには **Wazamono Tachi**、**Wazamono Tsurugi**、**Wazamono Kunai**の variant が含まれています。

---

## 心臓部 - AVR DU シリーズ

Wazamono シリーズは全機種が USB を内蔵した新世代 AVR「**AVR DU**」を採用しています。
USB-シリアル変換チップなしで PC と直接通信できることが最大の特長です。
Tachi / Tsurugi は **AVR64DU32**、小型の Kunai は **AVR32DU20** を搭載します。

| 項目 | AVR64DU32（Tachi / Tsurugi） | AVR32DU20（Kunai） |
|------|------------------------------|--------------------|
| Flash | 64 KB | 32 KB |
| SRAM | 8 KB | 4 KB |
| EEPROM | 256 B | 256 B |
| 最大動作周波数 | 24 MHz | 24 MHz |
| **USB** | USB 2.0 Full-Speed **デバイス** | USB 2.0 Full-Speed **デバイス** |
| ADC | 10-bit 170 ksps × 1（21 チャネル） | 10-bit 170 ksps × 1（ピン数で制限） |
| タイマ | TCA0 ×1、TCB ×2 | TCA0 ×1、TCB ×2 |
| USART / SPI / I2C | 2 / 1 / 1 | 2 / 1 / 1 |
| CCL（LUT）/ EVSYS / AC | 4 / 6ch / 1 | 同等（周辺ブロックは共通） |
| パッケージ | 32 ピン（TQFP / VQFN） | 20 ピン（VQFN） |

<sub>諸元はデータシート DS40002548A（AVR64DU32）/ DS40002576（AVR16/32DU ファミリ）に基づく。両者はピン数・Flash・SRAM が主な差で、周辺機能ブロックは共通です（20 ピンの Kunai は外部に出せるピン数が少なく、同時利用できる機能が制限されます）。</sub>

---

## 特長

- **基礎性能の向上** - 動作クロック1.5倍、プログラム容量約2倍、（※EEPROM容量は1KBから256Bへ減少）
- **USB ネイティブ** - 追加の USB-シリアル変換チップが不要。`Serial` がそのまま USB仮想シリアルポートになります。
- **USB ブートローダ** - USB-CDC（STK500v1）経由でスケッチを書き込み。**1200bps タッチ**でブートローダへ自動遷移するため、Leonardo / ProMicro と同じ手順で書き込めます。
- **HID / MIDI 対応** - USB キーボード・マウス等の HID、および USB-MIDI をサポート。
- **高い互換性** - 同系統のMCUを採用しているためUnoR3やLeonardo / ProMicroのコードをほぼそのまま実装可能（一部ピン配列の変更や未対応のライブラリあり）。
- **各ピンの出力能力** - ピンあたりの電流出力は20mAを維持しており、UnoR4では8mAでは動かせない外部機器も動作可能。
- **全ピンアナログ入力対応** - 全てのデジタル入出力ピンでアナログ値の読取りが可能。
- **7系統の PWM 出力** - UnoR3では6本だったPWMを7本に拡張
- **UPDI 対応** - UPDIデバッガーなどで動作中のMCUにアクセス可能
- **ネイティブ avr-gcc 対応** - DxCore と異なる点として最新の avr-gcc コンパイラを使用（今後も順次更新されます）。

---

## インストール

詳しい手順は [Installation.md](Installation.md) を参照してください。

### 手動インストール（hardware フォルダ）

1. このリポジトリを clone するか、ZIP をダウンロードして展開します。
2. スケッチブックの `hardware` フォルダに、フォルダ名を **`WazamonoCore`** として配置します。

   ```
   <スケッチブック>/hardware/WazamonoCore/megaavr/...
   ```

   - Windows 例: `ドキュメント\Arduino\hardware\WazamonoCore\`
   - macOS / Linux 例: `~/Documents/Arduino/hardware/WazamonoCore/`

3. Arduino IDE を再起動します。

> ボードマネージャ（JSON URL）からのインストールは今後対応予定です。

### 必要環境

- Arduino IDE 1.8.13 以降、または 2.x
- ブートローダーの書き換えにはUPDI プログラマ（PICkit 4/5、Atmel-ICE、jtag2updi 等）が必要になります。

> Linux をお使いの場合、Arduino IDE は必ず [arduino.cc](https://www.arduino.cc) 配布版を使用してください。ディストリのパッケージマネージャ版は改変されており、正常に動作しません。

---

## クイックスタート

1. **ツール > ボード > WazamonoCore** から **Wazamono Tachi (AVR64DU32)** を選択
2. **クロック** は通常「24 MHz external crystal (default)」のまま
3. USB ケーブルで接続し、書き込み

**Lチカ:**
```cpp
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500);
}
```

**USB シリアル:**
```cpp
void setup() {
  Serial.begin(115200);          // Serial = USB CDC（変換チップ不要）
}
void loop() {
  Serial.println(millis());
  delay(1000);
}
```

---

## 書き込み（ブートローダ経由）

1. ボードを USB で接続します。
2. Arduino IDE からスケッチを書き込みます。書き込み開始時に **1200bps タッチ**が行われ、自動的にブートローダへ遷移します。
3. 自動遷移しない場合は、**リセットボタンのダブルタップ**でブートローダに入れます。

---

## ライセンスとクレジット

WazamonoCore は [DxCore](https://github.com/SpenceKonde/DxCore)（© Spence Konde、LGPL 2.1）から派生した **製品専用フォーク**です。本コアも **LGPL 2.1** で配布されます。

- ベースコア: **DxCore** - © Spence Konde 2021–2022、および各 Arduino コア
- Wazamono 向けカスタマイズ・USB スタック・ボード定義: © Workshop Asahi 2026
- 「Wazamono（業物）」「Tachi（太刀）」「Tsurugi（剣）」「Kunai（苦無）」は Workshop Asahi の製品名です。

ライセンス全文は [LICENSE.md](LICENSE.md) を参照してください。一部のファイル・ライブラリは別ライセンスで提供される場合があり、その旨は各ファイル先頭に記載されています。
