from basic_pitch.inference import predict_and_save
from basic_pitch import ICASSP_2022_MODEL_PATH
from pathlib import Path
import shutil
import separate

def debug_files():
    """利用可能なファイルを確認"""
    keep_dir = Path("converter/keep")
    print("=== ファイル確認 ===")
    print(f"入力ディレクトリ: {keep_dir.absolute()}")
    print(f"ディレクトリ存在: {keep_dir.exists()}")
    
    if keep_dir.exists():
        all_files = list(keep_dir.iterdir())
        wav_files = list(keep_dir.glob("*.wav"))
        
        print(f"全ファイル数: {len(all_files)}")
        print(f"WAVファイル数: {len(wav_files)}")
        
        print("\n利用可能なWAVファイル:")
        for wav_file in wav_files:
            size = wav_file.stat().st_size
            print(f"  - {wav_file.name} ({size:,} bytes)")
    print("=" * 30)

def main(input_audio):
    debug_files()
    keep_dir = Path("converter/keep")
    if keep_dir.exists():
        shutil.rmtree(keep_dir)
    keep_dir.mkdir(parents=True, exist_ok=True)

    output_dir = Path("converter/output")
    if output_dir.exists():
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    # 音声分離を実行
    print("音声分離を実行中...")
    separate.main(input_audio)

    print("WAVをMIDIに変換します。")
    wav_files = list(keep_dir.glob("*.wav"))
    if not wav_files:
        print("変換対象のWAVファイルが見つかりません")
        return []

    midi_files = []
    for audio_file in wav_files:
        if not audio_file.exists():
            print(f"ファイルが存在しません: {audio_file}")
            continue
        print(f"変換中: {audio_file.name}")
        try:
            predict_and_save(
                [str(audio_file.absolute())],
                str(output_dir),
                True, False, False, False,
                ICASSP_2022_MODEL_PATH,
            )
            midi_path = output_dir / f"{audio_file.stem}.mid"
            midi_files.append(str(midi_path))
            print(f"完了: {audio_file.name} → {audio_file.stem}.mid")
        except Exception as e:
            print(f"エラー ({audio_file.name}): {e}")
            import traceback
            traceback.print_exc()
    print("変換処理が完了しました。")
    return midi_files  # ← ここでMIDIファイルのパスリストを返す

if __name__ == "__main__":
    main()  # テスト用のWAVファイルを指定