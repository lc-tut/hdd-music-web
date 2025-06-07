from basic_pitch.inference import predict_and_save
from basic_pitch import ICASSP_2022_MODEL_PATH
from pathlib import Path
import separate

def debug_files():
    """利用可能なファイルを確認"""
    input_dir = Path("converter/keep")
    print("=== ファイル確認 ===")
    print(f"入力ディレクトリ: {input_dir.absolute()}")
    print(f"ディレクトリ存在: {input_dir.exists()}")
    
    if input_dir.exists():
        all_files = list(input_dir.iterdir())
        wav_files = list(input_dir.glob("*.wav"))
        
        print(f"全ファイル数: {len(all_files)}")
        print(f"WAVファイル数: {len(wav_files)}")
        
        print("\n利用可能なWAVファイル:")
        for wav_file in wav_files:
            size = wav_file.stat().st_size
            print(f"  - {wav_file.name} ({size:,} bytes)")
    print("=" * 30)

def main():
    # デバッグ情報を表示
    debug_files()
    
    # 入力・出力ディレクトリを統一してPathで管理
    input_dir = Path("converter/keep")
    input_dir.mkdir(parents=True, exist_ok=True)
    output_dir = Path("converter/output")
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # 音声分離を実行
    print("音声分離を実行中...")
    separate.main()
    
    print("WAVをMIDIに変換します。")
    
    # WAVファイルのみを取得
    wav_files = list(input_dir.glob("*.wav"))
    
    if not wav_files:
        print("変換対象のWAVファイルが見つかりません")
        return
    
    print(f"見つかったファイル数: {len(wav_files)}")
    
    # 各ファイルを処理
    for audio_file in wav_files:
        if not audio_file.exists():
            print(f"ファイルが存在しません: {audio_file}")
            continue
            
        print(f"変換中: {audio_file.name}")
        
        try:
            # 絶対パスを文字列で渡す
            predict_and_save(
                [str(audio_file.absolute())],  # 絶対パスのリスト
                str(output_dir),  # 出力ディレクトリ（文字列）
                True,  # save_midi .midを保存
                False,  # sonify_midi MIDIをwavに変換して保存
                False,  # save_model_outputs MIDIに音を付けて出力（.wav）
                False,  # save_notes ノート情報を保存
                ICASSP_2022_MODEL_PATH,
            )
            print(f"完了: {audio_file.name} → {audio_file.stem}.mid")
            
        except Exception as e:
            print(f"エラー ({audio_file.name}): {e}")
            # 詳細なエラー情報が必要な場合
            import traceback
            traceback.print_exc()
    
    print("変換処理が完了しました。")

if __name__ == "__main__":
    main()