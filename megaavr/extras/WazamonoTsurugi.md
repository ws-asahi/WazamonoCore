# Wazamono 剣（Tsurugi）

**Arduino Uno R3 後継機 — AVR64DU32 / USB-C**

Wazamono Tsurugi は、Arduino Uno R3 と同じピン配置・フォームファクタを USB ネイティブな AVR `AVR64DU32` で再設計したボードです。Uno R3 が別チップ（USB-シリアル変換）を必要としたのに対し、Tsurugi はマイコン単体で USB-C により PC と直接つながります。さらに、産業用途を想定した **DC ジャック（最大 24V）入力＋同期バックコンバータ**を搭載しています。

> このページは Wazamono Tsurugi 1 機種のドキュメントです。コア全体の概要は [README](../../README.md) を参照してください。
> **状態: 開発中。** ピン定義・ブートローダは変更される可能性があります。

---

## 概要

| 項目 | 内容 |
|------|------|
| 由来 | Arduino Uno R3 後継 |
| MCU | AVR64DU32（VQFN-32） |
| フォームファクタ | Uno R3 互換（D0–D13、A0–A5、電源・ICSP ヘッダ） |
| USB | USB-C（USB 2.0 Full-Speed、マイコン内蔵） |
| クロック | 24 MHz 水晶（既定）／内蔵オシレータ切替可 |
| 電源 | USB 5V ／ DC ジャック（最大 24V、同期バックで 5V 生成）／基板上 LDO で 3.3V（シールド用） |
| 書き込み | USB CDC ブートローダ（STK500v1、1200bps タッチ） |

---

## ボード諸元（AVR64DU32）

| 項目 | 値 |
|------|----|
| Flash | 64 KB（うちスケッチ用 60 KB／USB ブートローダ 4 KB） |
| SRAM | 8 KB |
| EEPROM | 256 B |
| 最大動作周波数 | 24 MHz |
| USB | USB 2.0 Full-Speed デバイス（16 エンドポイントアドレス／最大 32 エンドポイント） |
| ADC | 10-bit 170 ksps × 1（21 チャネル） |
| DAC | なし（AC 内部の DACREF のみ） |
| タイマ | TCA0 ×1（PWM 6ch）、TCB ×2 ／ TCD は USB が占有のため非搭載 |
| USART | 2（USART0 / USART1） |
| SPI / TWI(I2C) | 各 1 |
| CCL（LUT） | 4 |
| イベントシステム | 6 チャネル |
| アナログコンパレータ（AC） | 1 |
| OPAMP | なし |

<sub>諸元はデータシート DS40002548A（AVR64DU32）に基づく。スケッチ用 Flash サイズ・SRAM サイズは boards.txt の設定値。</sub>

---

## ATmega328P（Arduino Uno R3）との比較

Wazamono Tsurugi が置き換える Arduino Uno R3 は **ATmega328P**（8-bit AVR、ネイティブ USB なし）を搭載しています。AVR64DU32 は新世代の **AVRxt コア**で、USB 内蔵・クロック・メモリ・周辺機能のすべてが強化されています。

| 項目 | Wazamono Tsurugi (AVR64DU32) | Arduino Uno R3 (ATmega328P) |
|------|------------------------------|------------------------------|
| コア | AVRxt（命令タイミング改善） | 旧来 AVR |
| 最大クロック | 24 MHz（1.8–5.5V 全域） | 20 MHz（4.5V 以上）／Uno は 16 MHz |
| **USB** | **マイコン内蔵**（CDC/HID/MIDI、変換チップ不要） | なし（基板上に別 USB チップが必要） |
| Flash | 64 KB | 32 KB |
| SRAM | 8 KB | 2 KB |
| EEPROM | 256 B | 1 KB |
| ADC | 10-bit・21 ch・170 ksps | 10-bit・6 ch（Uno ヘッダ） |
| タイマ | 16-bit TCA ×1 + TCB ×2 | 8/16/8-bit ×3 |
| USART / SPI / I2C | 2 / 1 / 1 | 1 / 1 / 1 |
| CCL（論理ブロック） | 4 LUT | なし |
| イベントシステム | 6 ch | なし |

### 性能上の主な利点

- **USB がマイコン内蔵** — Uno R3 は USB-シリアル変換用の別チップを基板に載せていましたが、Tsurugi は不要。`Serial` がそのまま USB CDC 仮想シリアルとなり、USB HID（キーボード/マウス）や USB-MIDI にもなれます。
- **クロックと処理速度** — 24 MHz 動作（Uno の 16 MHz 比で 1.5 倍）に加え、AVRxt コアは一部命令のタイミングが改善されています。
- **メモリ** — Flash 2 倍（64 KB）、SRAM 4 倍（8 KB）。
- **新世代の周辺機能** — CCL（4 論理ブロック）とイベントシステム（6 チャネル）により、CPU を介さないハードウェア信号処理が可能。ATmega328P にはいずれもありません。
- **広い入力電源範囲** — DC ジャックから最大 24V を入力でき、基板上の同期バックで 5V を生成します（Uno R3 のリニアレギュレータより高効率）。
- タイマの構成上ServoやTone使用時にはTCB1を使用し、そのためD3ピンのPWMが無効化されます（Uno R3ではD3とD11が無効）。

### 留意点

- **EEPROM 容量は ATmega328P のほうが大きい**（1 KB 対 256 B）。多くの不揮発データを保存する用途では保存方法の見直し（User Row やフラッシュの活用）が必要になる場合があります。

- **Tx / Rx LEDが無い** (ピン不足による削減)。 Serialに対する出力をボード上でモニターすることはできません。

---

## ピンマッピング

Arduino Uno R3 と同じ番号付け（D0–D13、A0–A5）です。A0–A5 はデジタル D14–D19 を兼ねます。

| D# | MCU | アナログ別名 | ADC ch | 主な機能 |
|----|-----|--------------|--------|----------|
| D0 | PA5 | A6 | AIN25 | **RX**（Serial0 / USART0 ALT1） |
| D1 | PA4 | A7 | AIN24 | **TX**（Serial0 / USART0 ALT1） |
| D2 | PA6 | A8 | AIN26 | USART0 XCK |
| D3 | PF5 | A9 | AIN21 | ~PWM(TCB1 ALT1) ／ tone() |
| D4 | PF4 | A10 | AIN20 | 汎用 I/O（PWM なし。TCB0 は millis） |
| D5 | PD1 | A11 | AIN1 | ~PWM(TCA0 WO1) ／ CCL |
| D6 | PD0 | A12 | AIN0 | ~PWM(TCA0 WO0) ／ CCL |
| D7 | PD7 | A13 | AIN7 | 汎用 I/O（旧 AREF。VREFA 無効化） |
| D8 | PA7 | A14 | AIN27 | 汎用 I/O（5V ネイティブ） |
| D9 | PD2 | A15 | AIN2 | ~PWM(TCA0 WO2) ／ CCL ／ AC0 AINP0 ／ EVOUTD |
| D10 | PD3 | A16 | AIN3 | ~PWM(TCA0 WO3) ／ CCL ／ AC0 AINN0 ／ **SS** |
| D11 | PD4 | A17 | AIN4 | ~PWM(TCA0 WO4) ／ SPI **MOSI** |
| D12 | PD5 | A18 | AIN5 | ~PWM(TCA0 WO5) ／ SPI **MISO** |
| D13 | PD6 | A19 | AIN6 | SPI **SCK** ／ **LED_BUILTIN**
| D14 / A0 | PF0 | A0 | AIN16 | アナログ A0 ／ CCL |
| D15 / A1 | PF1 | A1 | AIN17 | アナログ A1 ／ CCL |
| D16 / A2 | PF2 | A2 | AIN18 | アナログ A2 ／ CCL ／ EVOUTF |
| D17 / A3 | PF3 | A3 | AIN19 | アナログ A3 ／ CCL |
| D18 / A4 | PA2 | A4 | AIN22 | アナログ A4 ／ **SDA**（I2C） |
| D19 / A5 | PA3 | A5 | AIN23 | アナログ A5 ／ **SCL**（I2C） |

**ヘッダの専用ピン:** ICSP（PD4/PD5/PD6 = MOSI/MISO/SCK + RESET）。
**内部・非ヘッダピン:** PC3（LED_BUILTIN ドライバ, AIN31, 3.3V駆動）／PA0・PA1（24 MHz 水晶）／PF6（RESET）／PF7（UPDI）。

> `~` は PWM 出力可能ピン。各デジタルピンは ADC チャネルを持つため、A6–A19 としても参照できます。

---

## ペリフェラル割り当て

variant 側でピン割り当てが確定済みのため、スケッチで `swap()` を指定する必要はありません。

### シリアルポート

| オブジェクト | 実体 | ピン | 備考 |
|--------------|------|------|------|
| `Serial` | USB CDC | USB-C | シリアルモニタ（仮想 COM）。日常の `Serial.print()` は Uno と同じ感覚で使えます |
| `Serial1` | USART0（ALT1 固定） | D0(RX) / D1(TX) | Uno R3 の D0/D1 ハードウェア UART |

> **名前について:** この基板では D0/D1 の UART が **USART0** に配線されているため、ハードウェア UART は本来 `Serial1` ではなく **`Serial0`** ですが、ボード設定でSerial1へリネームしています。（ハードウェア定義のSerial1はピン構成上使用不可）
`Serial` は USB CDC なので、シリアルモニタ用途は Uno R3 と同じく `Serial` を使います。外部機器と D0/D1 でやり取りする場合のみ `Serial0` を使ってください。

### SPI（ホスト）

| 信号 | ピン |
|------|------|
| MOSI | D11（PD4） |
| MISO | D12（PD5） |
| SCK | D13（PD6） |
| SS | D10（PD3、Uno 慣例） |

Uno R3 と同じ D11–D13 / D10 に配置されています（SPI0 ALT4）。
ボードは SPI ホストで、チップセレクトはソフトウェア制御（任意の GPIO）です。
ALT4 のハードウェア SS は PD7（= D7）ですが、`SPI0.CTRLB.SSD = 1`（Client Select Disable）で運用するため、D7 が Low になってもホスト→クライアントに切り替わることはありません。

### I2C（Wire）

| 信号 | ピン |
|------|------|
| SDA | A4（PA2） |
| SCL | A5（PA3） |

Uno R3 と同じ A4/A5 に配置されています（TWI0 既定位置）。
通常の `Wire.begin()` でそのまま使えます。

### PWM（`analogWrite()`）

- **D5, D6, D9, D10, D11, D12** … TCA0（PORTD へ割り当て、WO0–WO5）
- **D3** … TCB1（ALT1 = PF5）
- Uno R3に対してD12へPWM機能が追加されています。

> **Uno R3 / tone() との関係:** `tone()` はタイマー **TCB1** を使います。
TCB1 は D3 PWM と共用のため、`tone()` 実行中は D3 の PWM のみ停止します（TCA0 の PWM と millis は動作継続）。
これは Uno R3 の Timer2（`tone()` 実行中は D3 および D11が無効）に相当する挙動です。

### アナログ入力

- Uno R3 ヘッダの **A0–A5**（= D14–D19）
- 各デジタルピンも ADC チャネルを持ち、A6–A19 として参照可能

---

## クロック

USB 用の 48 MHz（CLK_USB）は内蔵 PLL48M が生成し USB の SOF に同期して自動調整されます。**システムクロックの選択とは独立**しているため、内蔵オシレータ動作でも USB は機能します。

| 選択肢 | クロック源 | 備考 |
|--------|-----------|------|
| 24 MHz external crystal（既定） | 外部 24 MHz 水晶（PA0/PA1） | 製品の標準設定 |
| 24 MHz internal | 内蔵 OSCHF | 水晶なしで動作確認する場合 |
| 20 MHz internal | 内蔵 OSCHF | |
| 16 MHz internal | 内蔵 OSCHF | Uno R3 と同じ動作周波数 |

外部水晶を選択した場合、PA0/PA1 は GPIO として使用できなくなります。

---

## 電源

Tsurugi は **2 系統の電源入力**を持ち、いずれからでも 5V を得られます。

- **USB-C（5V）:** 理想ダイオード（Torex XC8110AA01）で逆流保護し、ホストを破損させずに 5V を供給します。
- **DC ジャック（最大 24V）:** φ5.5/2.1mm の DC ジャック（J10）から入力し、ショットキー（Rohm RB068MM-60、60V/2A）で逆接続保護後、**同期バックコンバータ（Renesas ISL854102FRZ）**で 5V（VOUT = 5.000V、FB 分圧 110k/15k）を生成します。EN 分圧（100k/15k）により起動電圧は約 9.2V です。入力側 TVS（DF2B36FU）で過電圧から保護します。
- **3.3V（シールドピン用）:** 基板上の LDO（日清紡 NJM2881F33、3.3V/300mA）。
- USB データライン（D+/D-）は TVS（Toshiba DF2B6M4CT）で ESD 保護。


---

## LED とスイッチ

| 部品 | 接続 | 用途 |
|------|------|------|
| 電源 LED | 電源ライン（D3／黄緑） | 通電表示 |
| LED_BUILTIN | D13（PD6）→ PC3 へミラー（D4／橙） | オンボード LED（Uno R3 慣例） |
| リセット | RESET（PF6） | タクトスイッチ（SKRPACE010） |

`LED_BUILTIN` は **D13（PD6）** です。D13 は SPI SCK と共用のため、Uno R3 と同様に SPI 使用中は LED が SCK のトラフィックで点滅します。

> **LED 駆動の仕組み（PC3 ハードミラー）:** Tsurugi は D13（PD6）の状態を **CCL LUT1 + イベントシステム**で **PC3** にハードウェアミラーし、PC3 側で実際の LED を点灯させます（[wazamono_tsurugi_init.cpp](../variants/WazamonoTsurugi/wazamono_tsurugi_init.cpp)）。
これにより Uno R3 で必要だった D13 のバッファ用オペアンプを置き換え、LED が SCK ラインに負荷をかけないようにしています。
> 経路: `PD6 ピンレベル → PORTD EVGEN0 → EVSYS CH0 → CCL LUT1（バッファ）→ LUT1-OUT = PC3 → LED`。
> CPU を介さないハードウェアパスで、`digitalWrite(13, …)` でも SPI（SCK）でも PD6 を追従します。
> EVSYS チャネル 0 と CCL LUT1 を消費するため、Event / Logic ライブラリを使うスケッチではこれらを避けてください。

---

## 書き込み

1. ボードを USB で接続します。
2. Arduino IDE からスケッチを書き込みます。書き込み開始時に **1200bps タッチ**が行われ、USB CDC ブートローダへ自動遷移します。
3. 自動遷移しない場合は、**リセットボタンのダブルタップ**でブートローダに入れます。

初回のみ、または USB ブートローダを書き込み直す場合は、UPDI プログラマ（PICkit 4/5、Atmel-ICE、jtag2updi 等）を UPDI パッド（PF7）に接続して書き込みます。

<sub>開発用 VID/PID は pid.codes のテスト範囲（アプリ `0x1209:0x0008` / ブートローダ `0x1209:0x0007`）を使用しています。製品出荷前に正式な VID/PID へ置き換えてください。</sub>

> **ブートローダ hex について:** Tsurugi 用のブートローダ hex（`usbcdcboot_wazamonotsurugi.hex`）はボード固有（VID/PID・LED ピンが Tachi と異なり、LED は PC3／アクティブ HIGH）のため別途ビルドが必要です。`megaavr/bootloaders/usbcdcboot/` で `build_wazamono.bat`（または `.sh`）を実行すると Tachi/Kunai とともに生成されます。

---

## ソフトウェア互換性（Arduino Uno R3）

Tsurugi は Uno R3 からの移植の手間を最小化することを目指しています。
`Serial` は USB CDC、I2C は A4/A5、SPI は D11–D13、`LED_BUILTIN` は D13 と、Uno R3 の作法がそのまま通用します。

> 注: レジスタを直接読み出す動作に関しては変更が必要です。

---

## 主要部品

確定 BOM の主要部品です。
**MCU とピンソケット以外全て日本製部品で統一**されています。

| 記号 | 種別 | 型番 (MPN) | 備考 |
|------|------|-----------|------|
| U1 | MCU | Microchip AVR64DU32-I/RXB（VQFN-32） | メインマイコン |
| U2 | 理想ダイオード | Torex XC8110AA01MR-G（SOT-25） | USB 電源 逆流保護 |
| U3 | LDO | 日清紡 NJM2881F33-TE1（SOT-23-5） | シールド 3.3V ピン用（3.3V/300mA） |
| U4 | 同期バック DC-DC | Renesas ISL854102FRZ（DFN-12） | 24V→5V 変換（Vref=0.6V、1.2A） |
| X1 | 水晶振動子 | EPSON TSX-3225 24.0000MF20G-AC3 | システムクロック 24 MHz |
| L1 | パワーインダクタ | TDK SPM6530T-220M（22µH） | バック用 Isat≥2.5A |
| D1, D2 | ESD/TVS | Toshiba DF2B6M4CT（SOD-882） | USB D+/D- |
| D3 | LED（黄緑） | Rohm SML-D12M8WT86（1608） | 電源表示 |
| D4 | LED（橙） | Rohm SML-D12D1W（1608） | LED_BUILTIN（PC3 駆動） |
| D10 | ショットキー | Rohm RB068MM-60（SOD-123FL） | DC 入力 逆接続保護（60V/2A） |
| D11 | TVS | Toshiba DF2B36FU（SOD-323） | DC 入力過電圧保護 ※耐圧(>24V)要確認 |
| SW1 | タクトスイッチ | ALPS Alpine SKRPACE010 | リセット |
| USB1 | USB コネクタ | Hirose CX90B-16P | USB-C トップマウント（USB 2.0 FS） |
| J10 | DC ジャック | マル信 MJ-179PH（φ5.5/2.1mm） | DC 24V 入力 |
| J5 | ピンヘッダ | 2×3 / 2.54mm | ICSP / SPI ブレークアウト |
| R3, R4 | 抵抗 5.1kΩ | KOA RK73B1ETTP512J（1005） | USB-C CC（Rd） |
| R12 / R13 | 抵抗 110kΩ / 15kΩ（1%） | KOA RK73H1E…（1005） | バック FB 分圧（VOUT=5.000V） |

> 上記のほか、デカップリング用 MLCC（0.1µF ×9 ほか）、水晶負荷容量（10pF ×2）、バック入出力容量（22µF 系）、各種電流制限・分圧抵抗を実装します。
> 完全な BOM は別途部品リストを参照してください。

---

## 公式ドキュメント

- AVR64DU32 製品ページ: <https://www.microchip.com/en-us/product/AVR64DU32>
- データシート: DS40002548A（AVR64DU32）
- 降圧 DC-DC: Renesas ISL854102 データシート
