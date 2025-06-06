#include <MIDI.h> // Arduino MIDI Library (FortySevenEffects)

// --- 定数定義 ---
// LED
#define ONBOARD_LED 2 // ESP32のオンボードLEDピン (通常はGPIO2)

// A4988 ステッピングモータードライバーピン
const int STEP_PIN = 4;  // STEPパルス出力ピン
// const int DIR_PIN = 35; // 方向制御ピン (今回は使用しない前提)
const int MS1_PIN = 25;  // マイクロステップ設定ピン1
const int MS2_PIN = 26;  // マイクロステップ設定ピン2
const int MS3_PIN = 27;  // マイクロステップ設定ピン3
// const int ENABLE_PIN = 14; // ENABLEピン (必要に応じて制御。GND接続で常時有効も可)

// MIDI設定
const int MIDI_BAUD_RATE = 115200;      // Hairless MIDI<->Serialで使用するボーレート
// MIDI_CHANNEL_OMNI を使用すると全チャンネルのMIDIメッセージを受信
// 特定のチャンネルのみ受信する場合は 1 から 16 の値を指定
// MIDI.begin() で設定するので、ここでは定数化は必須ではない

// モーター制御関連
const int MIN_MIDI_NOTE = 40; // 対応する最低MIDIノート番号 (C3)
const int MAX_MIDI_NOTE = 51; // 対応する最高MIDIノート番号 (B3)
const int NUM_SUPPORTED_NOTES = MAX_MIDI_NOTE - MIN_MIDI_NOTE + 1; // 12音

// 各ノートに対応するパルス周期 (マイクロ秒) ド(40)～シ(51)
// 値は元のコードを参考にし、12音階分に調整。正確な音程には実機での調整が必要です。
// C3, C#3, D3, D#3, E3, F3, F#3, G3, G#3, A3, A#3, B3
const unsigned int motor_pulse_periods[NUM_SUPPORTED_NOTES] = {
    1897, // 40 (ド)
    1791, // 41 (ド#)
    1691, // 42 (レ)
    1597, // 43 (レ#)
    1508, // 44 (ミ)
    1424, // 45 (ファ)
    1344, // 46 (ファ#)
    1269, // 47 (ソ)
    1198, // 48 (ソ#)
    1131, // 49 (ラ)
    1068, // 50 (ラ#) - 元のコードを参考に調整 (例: (1131+1008)/2付近)
    1008  // 51 (シ)  - 元のコードの最後の値
};

// A4988 マイクロステップモード定数
enum StepResolution {
    STEP_MODE_STOP = 0,      // モーター停止 (駆動パルスなし)
    STEP_MODE_FULL = 1,      // フルステップ
    STEP_MODE_HALF = 2,      // ハーフステップ
    STEP_MODE_QUARTER = 3,   // 1/4 ステップ
    STEP_MODE_EIGHTH = 4,    // 1/8 ステップ
    STEP_MODE_SIXTEENTH = 5  // 1/16 ステップ
};

// --- グローバル変数 ---
unsigned long last_pulse_time_micros = 0;       // 最後のパルス生成時刻 (マイクロ秒)
unsigned int current_motor_pulse_period = 0;  // 現在のモーターパルス周期 (マイクロ秒)
StepResolution current_step_resolution = STEP_MODE_STOP; // 現在のステッピングモード

// MIDIライブラリのインスタンス生成
// Hairless MIDI<->Serial Bridge でESP32のUSBシリアルポートを指定する場合、
// `Serial` (つまり `Serial0`) を使用します。
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);
// もし別のUART (Serial1 or Serial2) を使用する場合は、以下のように変更
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // Serial2の場合

// --- プロトタイプ宣言 ---
void handleNoteOn(byte channel, byte note, byte velocity);
void handleNoteOff(byte channel, byte note, byte velocity);
void setA4988StepResolution(StepResolution resolution);
void driveMotor();

// --- セットアップ関数 ---
void setup() {
  // シリアルモニタ初期化 (デバッグメッセージ用)
  // MIDI通信と兼用するため、MIDI.begin()より先に実行しない、
  // もしくはMIDI通信に別のシリアルポートを使用する。
  // 今回はMIDIもSerialなので、MIDI.beginの後か、レートを合わせる。
  // MIDIライブラリがSerialを初期化するので、ここでは不要な場合もある。
  // ただし、デバッグ用に明示的に開始しておく。
  Serial.begin(MIDI_BAUD_RATE);
  // Serial.println("ESP32 MIDI Stepper Controller Initializing...");

  // LEDピン設定
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW); // 初期状態は消灯

  // A4988制御ピン設定
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW); // 初期状態はLOW
  // if (DIR_PIN is used) pinMode(DIR_PIN, OUTPUT);
  // if (ENABLE_PIN is used) { pinMode(ENABLE_PIN, OUTPUT); digitalWrite(ENABLE_PIN, LOW); /* LOWで有効 */ }

  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);

  // 初期マイクロステップモード設定 (停止時はフルステップ待機)
  setA4988StepResolution(STEP_MODE_FULL); // ドライバのピンは設定しておく
  current_step_resolution = STEP_MODE_STOP; // 実際の動作は停止

  // MIDIライブラリ初期化とコールバック関数登録
  // Serial (USB CDC) を使うため、ボーレートはHairless MIDI Bridgeと合わせる。
  // MIDI.begin()は内部でSerial.begin()を呼び出す場合があるので、
  // 事前にSerial.begin()するか、MIDIライブラリに任せる。
  // ここでは MIDI.begin の前に Serial.begin しているのでボーレートは設定済み。
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI); // 全てのMIDIチャンネルからのメッセージを受信

  // Serial.println("Initialization Complete. Waiting for MIDI messages...");
}

// --- メインループ関数 ---
void loop() {
  MIDI.read();    // MIDIメッセージを継続的に読み込む (コールバックがトリガーされる)
  driveMotor();   // モーター駆動処理を継続的に実行
}

// --- MIDIイベントハンドラ ---
void handleNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(ONBOARD_LED, HIGH); // LED点灯
  // Serial.print("Note On: Ch="); Serial.print(channel);
  // Serial.print(", Note="); Serial.print(note);
  // Serial.print(", Vel="); Serial.println(velocity);

  if (velocity == 0) { // ベロシティ0のノートオンは実質ノートオフ
    handleNoteOff(channel, note, velocity); // ノートオフ処理を呼ぶ
    return;
  }

  if (note >= MIN_MIDI_NOTE && note <= MAX_MIDI_NOTE) {
    int note_index = note - MIN_MIDI_NOTE;
    current_motor_pulse_period = motor_pulse_periods[note_index];

    // === マイクロステップモードの選択 ===
    // ここでノートやベロシティに応じてマイクロステップモードを変更できる
    // 例: 低音はフルステップ、高音はマイクロステップなど
    // 今回は常にフルステップで駆動する例
    current_step_resolution = STEP_MODE_FULL;
    setA4988StepResolution(current_step_resolution);

    // Serial.print("Motor Start. Pulse Period: "); Serial.println(current_motor_pulse_period);
  } else {
    // Serial.println("Note out of supported range. Motor remains stopped or unchanged.");
    // 対応範囲外のノートが来た場合、現在のモーター状態を維持するか、明示的に停止する
    // current_motor_pulse_period = 0;
    // current_step_resolution = STEP_MODE_STOP;
    // setA4988StepResolution(STEP_MODE_FULL); // ピンはフルステップ状態へ
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  digitalWrite(ONBOARD_LED, LOW); // LED消灯
  // Serial.print("Note Off: Ch="); Serial.print(channel);
  // Serial.print(", Note="); Serial.print(note);
  // Serial.print(", Vel="); Serial.println(velocity);

  // TODO: どのノートがオフになったら止めるか？
  // 現在は、どのノートオフでもモーターを停止。
  // もし「最後に押された音がリリースされたら止める」などのポリフォニックな挙動を
  // させたい場合は、現在アクティブなノートを管理するロジックが必要。
  // ここでは、元のコードの挙動（いずれかのノートオフで停止）を踏襲。
  current_motor_pulse_period = 0; // パルス周期を0に = モーター停止指示
  current_step_resolution = STEP_MODE_STOP; // 状態を停止に

  // モーター停止時も、ドライバーのピンは特定のモード (例:フルステップ) に設定しておくのが一般的
  setA4988StepResolution(STEP_MODE_FULL);
  digitalWrite(STEP_PIN, LOW); //念のためSTEPピンをLOWに

  // Serial.println("Motor Stop.");
}

// --- モーター駆動関数 ---
void driveMotor() {
  // current_step_resolution が STEP_MODE_STOP でなく、かつ有効なパルス周期が設定されている場合のみ駆動
  if (current_step_resolution != STEP_MODE_STOP && current_motor_pulse_period > 0) {
    unsigned long current_time_micros = micros();
    if (current_time_micros - last_pulse_time_micros >= current_motor_pulse_period) {
      last_pulse_time_micros = current_time_micros;
      
      digitalWrite(STEP_PIN, HIGH);
      // A4988のSTEPパルスの最小High期間は通常1マイクロ秒。
      // digitalWriteの速度は十分速いことが多いが、極稀に問題になる場合は短い遅延を入れる。
      // delayMicroseconds(1); // 最小パルス幅を保証する場合 (通常はなくても動作することが多い)
      digitalWrite(STEP_PIN, LOW);
    }
  }
}

// --- A4988 マイクロステップ設定関数 ---
void setA4988StepResolution(StepResolution resolution) {
  /*
   A4988 Stepping Mode Truth Table (MS1, MS2, MS3):
   L, L, L: Full Step
   H, L, L: Half Step
   L, H, L: 1/4 Step
   H, H, L: 1/8 Step
   H, H, H: 1/16 Step
  */
  // Serial.print("Setting Microstep Resolution to: ");

  switch (resolution) {
    case STEP_MODE_STOP: // 停止命令だが、ピン自体は次の駆動のためにフルステップ状態にしておく
      // Serial.println("STOP (Pins set to Full Step for standby)");
      digitalWrite(MS1_PIN, LOW);
      digitalWrite(MS2_PIN, LOW);
      digitalWrite(MS3_PIN, LOW);
      break;
    case STEP_MODE_FULL:
      // Serial.println("Full Step");
      digitalWrite(MS1_PIN, LOW);
      digitalWrite(MS2_PIN, LOW);
      digitalWrite(MS3_PIN, LOW);
      break;
    case STEP_MODE_HALF:
      // Serial.println("Half Step");
      digitalWrite(MS1_PIN, HIGH);
      digitalWrite(MS2_PIN, LOW);
      digitalWrite(MS3_PIN, LOW);
      break;
    case STEP_MODE_QUARTER:
      // Serial.println("1/4 Step");
      digitalWrite(MS1_PIN, LOW);
      digitalWrite(MS2_PIN, HIGH);
      digitalWrite(MS3_PIN, LOW);
      break;
    case STEP_MODE_EIGHTH:
      // Serial.println("1/8 Step");
      digitalWrite(MS1_PIN, HIGH);
      digitalWrite(MS2_PIN, HIGH);
      digitalWrite(MS3_PIN, LOW);
      break;
    case STEP_MODE_SIXTEENTH:
      // Serial.println("1/16 Step");
      digitalWrite(MS1_PIN, HIGH);
      digitalWrite(MS2_PIN, HIGH);
      digitalWrite(MS3_PIN, HIGH);
      break;
    default: // 不明なモードの場合はフルステップにフォールバック
      // Serial.println("Unknown (Fallback to Full Step)");
      digitalWrite(MS1_PIN, LOW);
      digitalWrite(MS2_PIN, LOW);
      digitalWrite(MS3_PIN, LOW);
      break;
  }
}