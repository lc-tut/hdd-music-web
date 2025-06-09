// オクターブ1から10までの全半音を再生するテストコード

// スピーカー/ブザーを接続するピン
const int speakerPin = 18;

// === 周波数定義 (Hz) ===
// 国際標準ピッチ A4=440Hz に基づく平均律
// C0 = 16.35 Hz (参考)

// オクターブ 1 (C1 - B1) - 再生は非常に困難
#define NOTE_C1   32.70
#define NOTE_CS1  34.65  // C#1
#define NOTE_D1   36.71
#define NOTE_DS1  38.89  // D#1
#define NOTE_E1   41.20
#define NOTE_F1   43.65
#define NOTE_FS1  46.25  // F#1
#define NOTE_G1   49.00
#define NOTE_GS1  51.91  // G#1
#define NOTE_A1   55.00
#define NOTE_AS1  58.27  // A#1
#define NOTE_B1   61.74

// オクターブ 2 (C2 - B2) - 低音域、ブザーによっては再生困難
#define NOTE_C2   65.41
#define NOTE_CS2  69.30
#define NOTE_D2   73.42
#define NOTE_DS2  77.78
#define NOTE_E2   82.41
#define NOTE_F2   87.31
#define NOTE_FS2  92.50
#define NOTE_G2   98.00
#define NOTE_GS2 103.83
#define NOTE_A2  110.00
#define NOTE_AS2 116.54
#define NOTE_B2  123.47

// オクターブ 3 (C3 - B3)
#define NOTE_C3  130.81
#define NOTE_CS3 138.59
#define NOTE_D3  146.83
#define NOTE_DS3 155.56
#define NOTE_E3  164.81
#define NOTE_F3  174.61
#define NOTE_FS3 185.00
#define NOTE_G3  196.00
#define NOTE_GS3 207.65
#define NOTE_A3  220.00
#define NOTE_AS3 233.08
#define NOTE_B3  246.94

// オクターブ 4 (C4 - B4) - 中央のドを含むオクターブ
#define NOTE_C4  261.63
#define NOTE_CS4 277.18
#define NOTE_D4  293.66
#define NOTE_DS4 311.13
#define NOTE_E4  329.63
#define NOTE_F4  349.23
#define NOTE_FS4 369.99
#define NOTE_G4  392.00
#define NOTE_GS4 415.30
#define NOTE_A4  440.00
#define NOTE_AS4 466.16
#define NOTE_B4  493.88

// オクターブ 5 (C5 - B5)
#define NOTE_C5  523.25
#define NOTE_CS5 554.37
#define NOTE_D5  587.33
#define NOTE_DS5 622.25
#define NOTE_E5  659.25
#define NOTE_F5  698.46
#define NOTE_FS5 739.99
#define NOTE_G5  783.99
#define NOTE_GS5 830.61
#define NOTE_A5  880.00
#define NOTE_AS5 932.33
#define NOTE_B5  987.77

// オクターブ 6 (C6 - B6)
#define NOTE_C6  1046.50
#define NOTE_CS6 1108.73
#define NOTE_D6  1174.66
#define NOTE_DS6 1244.51
#define NOTE_E6  1318.51
#define NOTE_F6  1396.91
#define NOTE_FS6 1479.98
#define NOTE_G6  1567.98
#define NOTE_GS6 1661.22
#define NOTE_A6  1760.00
#define NOTE_AS6 1864.66
#define NOTE_B6  1975.53

// オクターブ 7 (C7 - B7)
#define NOTE_C7  2093.00
#define NOTE_CS7 2217.46
#define NOTE_D7  2349.32
#define NOTE_DS7 2489.02
#define NOTE_E7  2637.02
#define NOTE_F7  2793.83
#define NOTE_FS7 2959.96
#define NOTE_G7  3135.96
#define NOTE_GS7 3322.44
#define NOTE_A7  3520.00
#define NOTE_AS7 3729.31
#define NOTE_B7  3951.07

// オクターブ 8 (C8 - B8)
#define NOTE_C8  4186.01
#define NOTE_CS8 4434.92
#define NOTE_D8  4698.63
#define NOTE_DS8 4978.03
#define NOTE_E8  5274.04
#define NOTE_F8  5587.65
#define NOTE_FS8 5919.91
#define NOTE_G8  6271.93
#define NOTE_GS8 6644.88
#define NOTE_A8  7040.00
#define NOTE_AS8 7458.62
#define NOTE_B8  7902.13

// オクターブ 9 (C9 - B9) - 超高音域、再生・聴取困難な場合が多い
#define NOTE_C9  8372.02
#define NOTE_CS9 8869.84
#define NOTE_D9  9397.27
#define NOTE_DS9 9956.06
#define NOTE_E9 10548.08
#define NOTE_F9 11175.30
#define NOTE_FS9 11839.82
#define NOTE_G9 12543.85
#define NOTE_GS9 13289.75
#define NOTE_A9 14080.00
#define NOTE_AS9 14917.24
#define NOTE_B9 15804.27

// オクターブ 10 (C10のみ) - 極超高音域、通常は再生・聴取不能
#define NOTE_C10 16744.04
// オクターブ11 (C11 ~33.5kHz), オクターブ12 (C12 ~67kHz) は実用的でないため省略

// 再生する周波数の配列
float frequencies[] = {
  // Octave 1
  NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
  // Octave 2
  NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
  // Octave 3
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
  // Octave 4
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  // Octave 5
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  // Octave 6
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  // Octave 7
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7,
  // Octave 8
  NOTE_C8, NOTE_CS8, NOTE_D8, NOTE_DS8, NOTE_E8, NOTE_F8, NOTE_FS8, NOTE_G8, NOTE_GS8, NOTE_A8, NOTE_AS8, NOTE_B8,
  // Octave 9
  NOTE_C9, NOTE_CS9, NOTE_D9, NOTE_DS9, NOTE_E9, NOTE_F9, NOTE_FS9, NOTE_G9, NOTE_GS9, NOTE_A9, NOTE_AS9, NOTE_B9,
  // Octave 10
  NOTE_C10
};
int numFrequencies = sizeof(frequencies) / sizeof(frequencies[0]);

// 各音を鳴らす時間 (ミリ秒)
int noteDuration = 150;
// 音と音の間の休止時間 (ミリ秒)
int pauseBetweenNotes = 80; // (150ms + 80ms = 230ms per note)

void setup() {
  Serial.begin(115200);
  Serial.println("オクターブ1から10 (C10まで) の全半音テスト開始");
  Serial.print("合計ノート数: ");
  Serial.println(numFrequencies); // 109 notes
  Serial.println("再生時間 約25秒 (昇順)");
  Serial.println("注意: スピーカーや聴覚により、多くの音が再生できない/聞こえない可能性があります。");
}

void loop() {
  Serial.println("\n昇順で再生:");
  for (int i = 0; i < numFrequencies; i++) {
    float currentFrequency = frequencies[i];
    Serial.print(i + 1);
    Serial.print("/");
    Serial.print(numFrequencies);
    Serial.print(" - 周波数: ");
    Serial.print(currentFrequency, 2); // 小数点以下2桁まで表示
    Serial.println(" Hz");

    tone(speakerPin, currentFrequency, noteDuration);
    delay(noteDuration + pauseBetweenNotes);
  }

  Serial.println("\n--- 昇順再生完了 ---");
  delay(3000); // 3秒待つ

  Serial.println("\n降順で再生:");
  for (int i = numFrequencies - 1; i >= 0; i--) {
    float currentFrequency = frequencies[i];
    Serial.print(numFrequencies - i);
    Serial.print("/");
    Serial.print(numFrequencies);
    Serial.print(" - 周波数: ");
    Serial.print(currentFrequency, 2);
    Serial.println(" Hz");

    tone(speakerPin, currentFrequency, noteDuration);
    delay(noteDuration + pauseBetweenNotes);
  }

  Serial.println("\n--- 降順再生完了 ---");
  delay(5000); // 5秒待ってからループを繰り返す
}