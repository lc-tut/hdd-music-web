/*
 * FT232RLからの入力をSerialモニタに表示
 * - Serial: USBシリアル通信（表示用）
 * - Serial2: FT232RL接続用のハードウェアシリアル（受信専用）
 */

// FT232RL接続用のピン定義
#define FT232_RX_PIN 16  // ESP32のGPIO16をFT232RLのTXに接続
#define FT232_TX_PIN 17  // ESP32のGPIO17をFT232RLのRXに接続

void setup() {
  // USBシリアル通信を初期化（表示用）
  Serial.begin(115200);
  
  // FT232RL用のシリアル通信を初期化
  Serial2.begin(115200, SERIAL_8N1, FT232_RX_PIN, FT232_TX_PIN);
  
  // 起動完了まで少し待機
  delay(1000);
  
  Serial.println("=== FT232RL入力表示開始 ===");
  Serial.println("FT232RLからの入力を待機中...");
  Serial.println();
}

void loop() {
  // FT232RLシリアルからの受信をチェック
  if (Serial2.available()) {
    // 受信したデータをSerialモニタに表示
    Serial.println(Serial2.read());
  }
  
  delay(10); // CPUの負荷を軽減
}

/*
 * 接続方法:
 * 
 * ESP32    <-->  FT232RL
 * GPIO16   <-->  TX
 * GPIO17   <-->  RX (使用しませんが定義は必要)
 * GND      <-->  GND
 * 
 * 使用方法:
 * 1. ESP32をPCにUSB接続してSerial Monitorを開く (115200 bps)
 * 2. FT232RLから送信されたデータがSerial Monitorに表示される
 */