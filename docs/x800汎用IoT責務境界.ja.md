# FAN800のx800汎用IoT責務境界

- 状態：設計整理
- 日付：2026-08-10

## 位置づけ

FAN800はOND800専用の物理演出端末に限定せず、x800系の上位Orchestratorからevent/commandを受けて物理世界へ安全に反映する、交換可能な汎用IoT実行ノードとして扱う。

上位はOND800に限定しない。将来のAstro Runner、農林制御、舞台制御、その他のAI/非AI Orchestratorからも利用可能な中立ノードとする。

```text
KUMA800 / 各種センサー / 外部source
        │ observation/event
        ▼
OND800 / Astro Runner / その他Orchestrator
        │ decision / command
        ▼
FAN800
        │ local hardware control
        ▼
電気柵 / ポンプ / 照明 / MIDI / DMX / その他設備
```

## FAN800が背負う責務

- 物理出力の最終安全境界
- ローカルセンサー・ハードウェアのエラーハンドル
- 上位通信断時の縮退
- 内部クロック・既存ルールによる継続動作
- 設備固有の安全制約
- 上位AIやOrchestratorの誤命令を、そのまま物理危険へ昇格させないこと

## fail-throughput

FAN800は、上位AI・通信・Orchestratorが失われたときに、無条件で全停止することを既定としない。

原則は次である。

> AIが消えたら、AI導入以前から成立していた既存制御・ローカルルールへ縮退する。

これを本書では **fail-throughput** と呼ぶ。

例：

- 農林ポンプ：AI信号断だけを理由に給水系全体を停止せず、既存の水位・タイマー・圧力等のローカル制御へ戻す。
- 電気柵：AI信号断を直接「柵停止命令」に変換せず、その設備で事前に定義した既存安全状態へ縮退する。
- 神楽・舞台照明：上位AIや演出エンジンが落ちても、FAN800内部クロックやローカルシーケンスで成立する最低限の演出・安全動作を維持する。

具体的な設備ごとの縮退先は、機種・法令・現場設計によって異なるため、FAN800 coreで一つに固定しない。

## AIは追加能力であり生存条件ではない

**AI failure must degrade intelligence, not reality.**

AIは判断能力・最適化・演出能力を上乗せできるが、既に生存している物理設備の基本機能までAIの生存に依存させない。

したがって、次のような連鎖を避ける。

```text
AIが不安定 / 通信断
  ↓
ポンプ停止
  ↓
農業被害
```

```text
AIが不安定 / 通信断
  ↓
電気柵停止
  ↓
獣害
```

```text
AIが不安定 / 通信断
  ↓
神楽・舞台の物理系まで全停止
```

## KUMA800との境界

KUMA800はクマ索敵・観測・提示を担当し、FAN800を直接操作する権限を持たない。

KUMA800の `bear_nearby`、`STALE`、`SOURCE_UNAVAILABLE` 等は観測eventであり、FAN800への物理commandではない。

KUMA800のeventを物理作用へ翻訳する必要がある場合は、OND800、Astro Runner等の別Orchestratorが判断・承認・地域規則適用を担う。

```text
KUMA800 = epistemic authority
Orchestrator = decision authority
FAN800 = actuator authority
```

この三権を同一componentへ潰さない。

## 交換可能性

FAN800は物理実行ノードの一実装であり、商用狩猟IoT、林業IoT、既存設備PLC、他社ゲートウェイ等と交換可能である。

同様にFAN800はOND800に従属せず、上位Orchestratorも交換可能とする。FAN800にとって重要なのは「どのモビルスーツが正しいか」ではなく、受け取ったcommandがローカルの能力・安全則・許可範囲に収まるかである。

## 原則

> FAN800は上位AIを信じ切らず、物理安全を最後に守る。
>
> AIがビビった、落ちた、通信が切れた、という理由だけで、飯・獣害防止・現場設備・演出を巻き添えにしない。
