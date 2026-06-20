# FAN800 インターフェース参照

**正本:** [OND800/docs/interface_spec.md](https://github.com/saitoomituru/OND800/blob/main/docs/interface_spec.md)

変更はOND800リポジトリ側が行う。このファイルはFAN800ファームウェア実装者向けの参照索引。

---

## FAN800が実装すべきインターフェース

### 1. 自己申告パケット（接続確立時・必須）

OND800がBLEで発見・接続したら**即座に**送信する。

```json
{
  "uuid": "FAN-XX0-xxxx",
  "role": "slime_bazooka",
  "display_name": "スライムバズーカ左",
  "capabilities": [
    { "event": "FIRE_SLIME_SMALL",  "params": {} },
    { "event": "FIRE_SLIME_MEDIUM", "params": {} },
    { "event": "FIRE_SLIME_LARGE",  "params": {} }
  ],
  "mutex_group": "pyro",
  "mutex_with": [],
  "base_recast_ms": 5000,
  "cooldown_ms": 5000,
  "location_hint": "スタジオA"
}
```

`capabilities` には実際に対応しているイベントのみ列挙する。

### 2. ACKパケット（イベント受信時）

```json
{
  "uuid": "FAN-XX0-xxxx",
  "type": "ack",
  "event": "FIRE_SLIME_MEDIUM",
  "result": "ok",
  "timestamp_ms": 1234567890
}
```

物理安全チェックでリジェクトする場合:

```json
{
  "uuid": "FAN-XX0-xxxx",
  "type": "ack",
  "event": "FIRE_SLIME_MEDIUM",
  "result": "rejected",
  "reason": "cooldown"
}
```

`reason` の値: `"cooldown"` / `"temperature"` / `"overcurrent"` / `"midi_not_connected"`

### 3. MIDIルール受信（低頻度・BLE GATT）

```json
{
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

受信後は内部RTCでBPMをカウントし、**OND800との接続が切れても自律実行継続**する。

---

## FAN800が知らなくていいこと

- Identity Store（顔データ・演者情報）
- OBSのシーン構成
- リキャストタイム計算（OND800が計算して発火命令を送る）
- SAO800の存在

---

## 物理安全チェック（FAN800の責務）

OND800からコマンドを受け取っても、**自身の物理状態に基づいてリジェクトできる**。

| 機種 | リジェクト条件 |
|---|---|
| FAN800-AC | クールダウン中・温度センサー異常 |
| FAN800-PWM | 過電流検出 |
| FAN800-MD | MIDI接続未確立 |

**OND800を信用しない。物理的最終防衛ラインはFAN800。**

---

## イベント言語リファレンス（サマリ）

正本は `platform_spec.md` および OND800 `interface_spec.md` を参照。

| カテゴリ | イベント例 |
|---|---|
| 照明 | `LIGHT_ON` / `LIGHT_OFF` / `LIGHT_STROBE` / `LIGHT_COLOR` |
| 特効 | `FIRE_SLIME_SMALL/MEDIUM/LARGE` / `RAIN_START/STOP` / `SMOKE_START/STOP` |
| MIDI | `MIDI_NOTE_OUT` / `MIDI_CLOCK_OUT` / `MIDI_CC_OUT` |
| 汎用 | `SSR_ON` / `SSR_OFF` / `PUMP_STOP` / `UNIT_STATUS_REQ` |
