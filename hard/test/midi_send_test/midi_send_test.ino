// ESP32のオンボードLEDのピン番号を指定します。
// 多くのESP32ボードではGPIO 2に接続されています。
// 使用するボードに合わせてピン番号を変更してください。
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

void setup() {
  // シリアル通信を115200 bpsで開始します。
  Serial.begin(115200);

  // LEDピンを出力モードに設定します。
  pinMode(LED_BUILTIN, OUTPUT);
  // 初期状態としてLEDを消灯しておきます。
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("ESP32 MIDI受信準備完了 (115200 bps)");
  Serial.println("MIDIデータを受信するとLEDが点灯します。");
}

void loop() {
  // シリアルポートに受信データがあるか確認します。
  if (Serial.available() > 0) {
    // データがあれば、バッファにあるデータをすべて読み込みます。
    // (今回はMIDIメッセージの内容を解析するわけではないので、読み飛ばすだけでもOK)
    while (Serial.available() > 0) {
      byte incomingByte = Serial.read();
      // 必要であればここで受信したバイトデータを処理できます。
      // Serial.print("受信: 0x");
      // Serial.println(incomingByte, HEX);
    }

    // オンボードLEDを点灯させます。
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("MIDIデータ受信！ LED点灯");

    // 短時間 (例: 200ミリ秒) 点灯させた後、消灯します。
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LED消灯");
  }
}