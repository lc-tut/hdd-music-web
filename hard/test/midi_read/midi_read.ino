// # include <MIDI.h>
// FT232RL接続用のピン定義
#define FT232_RX_PIN 16  // ESP32のGPIO16をFT232RLのTXに接続
#define FT232_TX_PIN 17  // ESP32のGPIO17をFT232RLのRXに接続

// MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI_IN);

void setup() {
  // USBシリアル通信を初期化（表示用）
  Serial.begin(115200);
  
  // FT232RL用のシリアル通信を初期化
  Serial2.begin(115200, SERIAL_8N1, FT232_RX_PIN, FT232_TX_PIN);


  // MIDI_IN.begin(MIDI_CHANNEL_OMNI);
  
  // 起動完了まで少し待機
  delay(1000);
}

void loop() {
  // FT232RLシリアルからの受信をチェック
  if (Serial2.available()) {
    // 受信したデータをSerialモニタに表示
    Serial.println(Serial2.read());
  }
}
