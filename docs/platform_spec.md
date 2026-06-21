# FAN800_platform_spec.md
# FAN800 プラットフォーム仕様

---

## 概要

FAN800はESP32ベースの演出IoTプラットフォーム。  
全機種が共通のBLE GATTプロトコルでOND800と通信する。  
アクチュエータ・出力が異なるだけで、OND800から見た通信仕様は同一。

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

---

## 共通仕様

### ハードウェア

```
MCU: ESP32-S3（USB-MIDIネイティブ対応）
通信: BLE 5.0 GATT
電源: バスパワー（USB）or 外部5V
サイズ目標: FAN800-MD/DMX はUSBメモリ相当
```

### BLE GATT通信

**接続確立時：自己申告パケット送信（必須）**

OND800が発見・接続したら即座に自己申告する。これによりOND800はデバイスの役割・能力・安全制約を把握する。

```json
{
  "event_schema_version": "1.0.0",
  "uuid": "FAN-MD-xxxx",
  "role": "midi_instrument",
  "display_name": "FAN800-MD ドングル",
  "capabilities": [
    { "event": "MIDI_NOTE_OUT", "params": { "channel": "1-16", "note": "0-127", "velocity": "0-127" } },
    { "event": "MIDI_CLOCK_OUT", "params": {} },
    { "event": "MIDI_NOTE_IN",  "params": {} }
  ],
  "mutex_group": null,
  "base_recast_ms": 0,
  "location_hint": null
}
```

### 物理安全チェック（FAN800責務）

OND800からのコマンドを受け取っても、**自身の物理状態に基づいてリジェクトできる**。

```
リジェクト条件例
  FAN800-AC: クールダウン中・温度センサー異常
  FAN800-PWM: 過電流検出
  FAN800-MD: MIDI接続未確立

リジェクト時はACKで reason を返す
  { "result": "rejected", "reason": "cooldown" }
```

OND800を信用しない。物理的最終防衛ラインはFAN800。

---

## 自律動作モデル

### BLEルール受信フェーズ（遅延許容）

```json
{
  "event_schema_version": "1.0.0",
  "type": "midi_rule",
  "bpm": 120,
  "quantize": "1/4",
  "swing": 0.0,
  "sequence": [
    { "beat": 1, "event": "LIGHT_STROBE" },
    { "beat": 3, "event": "FIRE_SLIME_MEDIUM" }
  ],
  "loop": true,
  "start_at": "next_bar"
}
```

### 内部RTCフェーズ（完全自律）

ルール受信完了後、FAN800は内部リアルタイムクロックでBPMをカウントし、シーケンスを自律実行する。**OND800との通信が切れても動作継続。**

### クロック同期

```
BLE MIDIクロック（24ppqn）をOND800から定期受信
  └─ FAN800がドリフト補正に使用
  └─ 多少の揺れはFAN800側で吸収

精度が必要な場面
  └─ USB-MIDI（アンビリカル）でOND800と直結
  └─ MIDIクロックマスター/スレーブ切替可
```

---

## FAN800-MD 詳細仕様

### USB-MIDI

```
規格: USB MIDI Class Compliant
ドライバ: 不要（刺すだけで認識）
電源: バスパワー
遅延: 1ms以下
```

### 操縦桿トスモード

```
通常: OND800がマスター → FAN800-MD → 外部機器
トス: 外部DJ・VJのMIDI OUTをFAN800-MD MIDI INで受ける
       └─ 外部操縦者がBPMマスターになる
       └─ OND800はスレーブとして同期
       └─ 投げ銭キューはOND800が管理したまま
```

### ジャムセッション参加

```toml
# FAN800が楽器としてMIDIノートを返す設定

[[midi_out]]
trigger_event = "FIRE_SLIME_MEDIUM"
note = 36
velocity_map = "impact_sensor"  # 発射衝撃をvelocityに変換
channel = 10                    # ドラムチャンネル
```

スライムバズーカの発射がドラムヒットとして共有MIDIバスに乗る。

---

## スタジオジャック手順

1. FAN800-MDをスタジオのMIDI IN端子に挿入（5秒）
2. OND800がBLEでFAN800-MDを発見・接続
3. FAN800-MDが自己申告パケットを送信
4. OND800のLoadoutに「スタジオMIDI」として自動登録
5. 投げ銭 → OND800 → BLE → FAN800-MD → MIDI → スタジオ設備

**MIDI INがあれば既存設備は何も変えなくていい。**

---

## イベント言語リファレンス

### 照明系

| イベント | パラメータ |
|---|---|
| `LIGHT_ON` | - |
| `LIGHT_OFF` | - |
| `LIGHT_DIM` | `brightness: 0-100` |
| `LIGHT_COLOR` | `rgb: hex` |
| `LIGHT_STROBE` | `hz: 1-20` |
| `LIGHT_STAGE_RED` | - |
| `LIGHT_STAGE_BLUE` | - |

### 特効系

| イベント | パラメータ | mutex_group |
|---|---|---|
| `FIRE_SLIME_SMALL` | - | pyro |
| `FIRE_SLIME_MEDIUM` | - | pyro |
| `FIRE_SLIME_LARGE` | - | pyro |
| `RAIN_START` | `intensity: 1-5` | wet |
| `RAIN_STOP` | - | wet |
| `SMOKE_START` | `density: 1-5` | - |
| `SMOKE_STOP` | - | - |

### MIDI系

| イベント | パラメータ |
|---|---|
| `MIDI_NOTE_OUT` | `channel, note, velocity` |
| `MIDI_CLOCK_OUT` | - |
| `MIDI_CC_OUT` | `channel, cc, value` |

---

## mutex_groupリファレンス

| グループ | 競合 | 理由 |
|---|---|---|
| `pyro` | `wet` | 水と火工系は同時禁止 |
| `wet` | `pyro` | 同上 |
| `lighting_main` | なし | 照明は他と競合しない |
