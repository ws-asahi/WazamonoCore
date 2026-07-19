# WazamonoCore のインストール

WazamonoCore は、Wazamono シリーズ（AVR DU 搭載ボード）を Arduino IDE で開発するための専用コアです。

## 対応 IDE

- **Arduino IDE 1.8.13 以降**（推奨）
- **Arduino IDE 2.x**

> **Linux をお使いの場合:** Arduino IDE は必ず [arduino.cc](https://www.arduino.cc) の配布版を使用してください。ディストリビューションのパッケージマネージャ版は改変されており、サードパーティのボードパッケージが正常に動作しないことが知られています。

## 対応 OS（ツールチェーン）

ボードマネージャ経由のインストールは、現在以下のホストに対応しています。

| ホスト | 状態 |
|--------|------|
| Windows x64 | ✅ 対応 |
| Linux x64 | ✅ 対応 |
| macOS (Intel / Apple Silicon)・Linux ARM64 | 🚧 準備中 |

---

## ボードマネージャ経由（推奨）

1. Arduino IDE の **ファイル > 基本設定** を開き、**追加のボードマネージャの URL** に以下を追加します。

   ```
   https://wazamono.ws-asahi.net/package_wazamono_index.json
   ```

2. **ツール > ボード > ボードマネージャ** を開き、「**Wazamono**」で検索します。

3. **Wazamono Boards (AVR DU series)** を選んで **インストール** します。
   コア本体に加えて、専用ツールチェーン wazamono-toolchain（avr-gcc 15.2.0 / avrdude 8.1）が自動的にダウンロード・設定されます。

4. 「ツール > ボード > WazamonoCore」に **Wazamono Tachi / Tsurugi / Kunai** が表示されればインストール完了です。

> ツールチェーンを含めて数百 MB のダウンロードが発生します。インストール先は
> Arduino IDE の標準パッケージフォルダ（Windows: `%LOCALAPPDATA%\Arduino15\packages\WazamonoCore\`）です。

---

## 手動インストール（開発者向け・hardware フォルダ）

コア自体を開発・改造する場合のインストール方法です。**通常の利用にはボードマネージャ経由を推奨します。**

1. このリポジトリを `git clone` するか、ZIP をダウンロードして展開します。

2. スケッチブックの `hardware` フォルダに、フォルダ名を **`WazamonoCore`** として配置します。階層は以下のようになります。

   ```
   <スケッチブック>/
     └─ hardware/
          └─ WazamonoCore/
               └─ megaavr/
                    ├─ boards.txt
                    ├─ platform.txt
                    ├─ cores/
                    ├─ variants/
                    └─ ...
   ```

   スケッチブックの場所は、Arduino IDE の「ファイル > 環境設定 > スケッチブックの保存場所」で確認できます。

   - Windows 例: `ドキュメント\Arduino\hardware\WazamonoCore\`
   - macOS 例: `~/Documents/Arduino/hardware/WazamonoCore/`
   - Linux 例: `~/Arduino/hardware/WazamonoCore/`

3. **ツールチェーンを設定します（手動インストールでは必須）。**
   手動インストールでは IDE がツールチェーンを自動解決しないため、そのままでは
   IDE 標準の avr-gcc 7.3.0（AVR DU 非対応）が使われてビルドに失敗します。
   `megaavr\make_platform_local.bat` を実行して、ローカルの avr-gcc 15.x を指す
   `platform.local.txt` を生成してください（詳細はバッチファイル冒頭のコメント参照）。

4. Arduino IDE を再起動します。

> **ボードマネージャ版と手動版を併用する場合:** 両方が存在するときは
> スケッチブック（hardware フォルダ）側が優先されます。ボードマネージャ版の
> 動作を確認したいときは、hardware フォルダ側を一時的にリネームしてください。
> どちらが使われているかは、ビルドログ冒頭の `Using board ... from platform in folder:` で確認できます。

---

## 初回のブートローダ書き込み（必要な場合）

Wazamono の製品ボードは出荷時に USB ブートローダが書き込まれているため、通常は USB 接続するだけでスケッチを書き込めます。

自作・修理などでブートローダの書き込みが必要な場合は、UPDI プログラマを使用します。

- 対応プログラマ: PICkit 4 / 5、Atmel-ICE、SerialUPDI アダプタ、jtag2updi など
- 接続先: UPDI ピン / パッド
- 手順: 「ツール > 書き込み装置」でプログラマを選択し、「ブートローダを書き込む」を実行

---

## 動作確認

インストール後、以下のスケッチで動作を確認できます。

```cpp
void setup() {
  Serial.begin(115200);          // Serial = USB CDC（変換チップ不要）
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  Serial.println(millis());
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500);
}
```

ボードを選択して書き込み、シリアルモニタ（115200 bps）に値が表示され、オンボード LED が点滅すれば成功です。
