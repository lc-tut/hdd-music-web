// FT232RL接続用のピン定義
#define FT232_RX_PIN 16
#define FT232_TX_PIN 17
// ステッピングモーター制御ピン (A4988)
#define STEP_PIN 2
#define DIR_PIN 4
#define ENABLE_PIN 5

// マルチコア用変数
TaskHandle_t stepperTaskHandle = NULL;
volatile bool midi_flag = false;
volatile uint8_t currentNote = 60;
volatile uint8_t currentVelocity = 0;

// 排他制御用
SemaphoreHandle_t xMutex = NULL;

const unsigned long noteFrequencies[132] = {
  // C0からC10まで128音程分
  30578, 28862, 27242, 25713, 24270, 22908, 21622, 20408, 19263, 18182, 17161, 16198, // C0-B0
  15289, 14431, 13621, 12856, 12135, 11454, 10811, 10204, 9631, 9091, 8581, 8099,   // C1-B1
  7645, 7215, 6810, 6428, 6067, 5727, 5405, 5102, 4816, 4545, 4290, 4050,         // C2-B2
  3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551, 2408, 2273, 2145, 2025,         // C3-B3
  1911, 1804, 1703, 1607, 1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,         // C4-B4
  956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506,                     // C5-B5
  478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253,                     // C6-B6
  239, 226, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127,                     // C7-B7
  119, 113, 106, 100, 95, 89, 84, 80, 75, 71, 67, 63,                            // C8-B8
  60, 56, 53, 50, 47, 45, 42, 40, 38, 35, 33, 32,                                // C9-B9
  30, 28, 27, 25, 24, 22, 21, 20, 19, 18, 17, 16                                 // C10-B10
};
int arrayNumber = sizeof noteFrequencies / sizeof noteFrequencies[0];

// ステッピングモーター制御タスク（Core 0で実行）
void stepperTask(void *parameter) {
  bool localMidiFlag = false;
  uint8_t localNote = 60;
  unsigned long stepInterval = 800; // マイクロ秒
  
  while (true) {
    // 共有変数を安全に読み取り
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      localMidiFlag = midi_flag;
      localNote = currentNote;
      xSemaphoreGive(xMutex);
    }
    
    if (localMidiFlag) {
      // 音程に応じてステップ間隔を調整（オプション）
      stepInterval = 1000000 / noteFrequencies[arrayNumber-localNote];
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(stepInterval);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(stepInterval);
    } else {
      // アイドル時は少し待機
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, FT232_RX_PIN, FT232_TX_PIN);
  
  // モーターピンの初期化
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);
  
  // ミューテックス作成
  xMutex = xSemaphoreCreateMutex();
  
  // ステッピングモーター制御タスクをCore 0で作成
  xTaskCreatePinnedToCore(
    stepperTask,           // タスク関数
    "StepperTask",         // タスク名
    2048,                  // スタックサイズ
    NULL,                  // パラメータ
    1,                     // 優先度
    &stepperTaskHandle,    // タスクハンドル
    0                      // Core 0で実行
  );
  
  Serial.println("Setup completed. MIDI task on Core 1, Stepper task on Core 0");
  delay(1000);
}

// MIDIイベントは3バイト
uint8_t midiBuffer[3];
uint8_t midiIndex = 0;

// メインループ（Core 1で実行）- MIDI処理専用
void loop() {
  if (Serial2.available()) {
    uint8_t incoming = Serial2.read();
    midiBuffer[midiIndex++] = incoming;
    
    // 3バイト溜まったら処理
    if (midiIndex >= 3) {
      uint8_t status = midiBuffer[0];
      uint8_t note   = midiBuffer[1];
      uint8_t vel    = midiBuffer[2];
      midiIndex = 0;
      
      // 共有変数を安全に更新
      if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        if ((status & 0xF0) == 0x90 && vel > 0) {
          // Note On
          Serial.print("Note On: "); Serial.println(note);
          digitalWrite(ENABLE_PIN, LOW);
          digitalWrite(DIR_PIN, note >= 60 ? HIGH : LOW);
          currentNote = note;
          currentVelocity = vel;
          midi_flag = true;
        }
        // Note Off または Note On + velocity 0
        else if (((status & 0xF0) == 0x80) || ((status & 0xF0) == 0x90 && vel == 0)) {
          Serial.print("Note Off: "); Serial.println(note);
          midi_flag = false;
          digitalWrite(ENABLE_PIN, HIGH);
        }
        xSemaphoreGive(xMutex);
      }
    }
  }
}