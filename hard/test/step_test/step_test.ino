/*
 * ESP32 + A4988 ステッピングモーター制御（シンプル版）
 * 
 * 配線:
 * STEP -> GPIO 2
 * DIR  -> GPIO 4
 * 
 * A4988の電源とモーター接続:
 * VMOT -> 12V (モーター電源)
 * GND  -> GND
 * 2B, 2A, 1A, 1B -> ステッピングモーター
 * VDD -> 3.3V (ロジック電源)
 * GND -> GND
 */

// ピン定義
#define STEP_PIN 2
#define DIR_PIN 4

// 設定
#define STEPS_PER_REV 2000  // NEMA17の場合：200ステップ/回転
#define DELAY_TIME 650    // ステップ間の遅延（マイクロ秒） 650μで

void setup() {
  Serial.begin(115200);
  
  // ピンを出力に設定
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  
  Serial.println("A4988 ステッピングモーター制御開始");
  Serial.println("自動で回転します...");
}

void loop() {
  // 時計回りに1回転
  Serial.println("時計回り 1回転");
  digitalWrite(DIR_PIN, HIGH);  // 方向設定（時計回り）
  
  for(int i = 0; i < STEPS_PER_REV; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(DELAY_TIME);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(DELAY_TIME);
  }
  
  delay(1000);  // 1秒待機
  
  // 反時計回りに1回転
  Serial.println("反時計回り 1回転");
  digitalWrite(DIR_PIN, LOW);   // 方向設定（反時計回り）
  
  for(int i = 0; i < STEPS_PER_REV; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(DELAY_TIME);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(DELAY_TIME);
  }
  
  delay(1000);  // 1秒待機
}