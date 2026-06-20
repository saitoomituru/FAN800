# FAN800 — Fannel Ad-hoc Network 800

**サイコミュファンネルネットワーク**: OND800（モビルスーツ）が展開するESP32ベースの自律演出IoTプラットフォーム。

BLE GATTでOND800からイベント言語を受け取り、物理演出（照明・特効・MIDI・DMX）を自律実行する。  
OND800が落ちてもFAN800はループし続ける。物理安全チェックはFAN800が最終防衛ラインを守る。

---

## システム構成における位置づけ

```
saitoomituru/FAN800   # サイコミュファンネルネットワーク（ESP32群）← このリポジトリ
saitoomituru/OND800   # モビルスーツ本体（Raspberry Pi 5）
saitoomituru/SAO800   # バトルシップ拡張（OBSプラグイン C++）
```

| レイヤー | 担当 | 比喩 |
|---|---|---|
| FAN800 | 物理演出・センサー・MIDI出力 | ファンネル |
| OND800 | 判断・調停・演出エンジン | モビルスーツ |
| SAO800（optional） | エンコードオフロード・BPM解析・AIオフロード | バトルシップ |
| OBS | 配信・シーン管理 | コロニーレーザー |

**FAN800は申告する、OND800は裁定する、物理安全はFAN800が守る。**

---

## 機種バリエーション

| 型番 | 出力 | 用途 |
|---|---|---|
| FAN800-AC | ACスイッチング | 一般照明・スモークマシン・バズーカ電源 |
| FAN800-PWM | PWM制御 | フルカラーLEDテープ・ピストンマシン・サーボ |
| FAN800-IR | リモコン信号クローン | 既製品エフェクター乗っ取り |
| FAN800-MD | USB-MIDI + DIN5 送受信 | DJ・VJ・シンセへの操縦桿トス |
| FAN800-DMX | DMX512 送受信 | 舞台照明卓直結 |
| FAN800-MD/DMX | MIDI↔DMX変換 | ブリッジドングル |

**FAN800-MDをスタジオのMIDI INに刺すと、そのスタジオの照明・シンセ・VJが投げ銭とビートに応答し始める。  
箱の許可は要らない。MIDI INがあれば十分。**

---

## 共通仕様

```
MCU: ESP32-S3（USB-MIDIネイティブ対応）
通信: BLE 5.0 GATT
電源: バスパワー（USB）or 外部5V
サイズ目標: FAN800-MD/DMX はUSBメモリ相当
```

詳細は [`docs/platform_spec.md`](docs/platform_spec.md) 参照。  
インターフェース仕様の正本は [OND800/docs/interface_spec.md](https://github.com/saitoomituru/OND800/blob/main/docs/interface_spec.md)。

---

## 設計哲学

**BLEは楽譜を渡す、内部クロックが演奏する、USB-MIDIは一緒にセッションする。**

- 遅延許容のBLE通信でルールを受け取り、FAN800内部RTCがBPMを自律タイムキープ
- OND800との通信が切れてもシーケンスはループし続ける
- 精度が必要な場面はUSB-MIDIアンビリカルで割り切る
- OND800を信用しない。物理的最終防衛ラインはFAN800

---

## ディレクトリ構成

```
/docs       - プラットフォーム仕様・インターフェース参照
/firmware   - ESP32ファームウェア（機種別サブディレクトリ）
/notes      - 実験ノート・作業ログ（AIエージェント/人間共通）
```

---

## ライセンス

MIT License

## 作者

齋藤みつる（ふさもふ）/ ZeroRoomLab  
https://github.com/saitoomituru
