import os
import sys
from pathlib import Path
import torchaudio
from demucs.api import Separator

def debug_file_system():
    """ファイルシステムの状況をデバッグ"""
    print("=== ファイルシステム デバッグ ===")
    print(f"現在のディレクトリ: {os.getcwd()}")
    print(f"Python実行パス: {sys.executable}")
    
    # inputディレクトリの確認
    input_dir = Path("converter/separate/input")
    print(f"\n入力ディレクトリ: {input_dir}")
    print(f"ディレクトリ存在: {input_dir.exists()}")
    
    if input_dir.exists():
        print("ディレクトリ内のファイル:")
        for file_path in input_dir.iterdir():
            print(f"  - {file_path.name} (サイズ: {file_path.stat().st_size} bytes)")
            
            # ファイルの詳細チェック
            if file_path.suffix.lower() in ['.wav', '.mp3', '.flac']:
                print(f"    パス: {file_path.absolute()}")
                print(f"    読み取り可能: {os.access(file_path, os.R_OK)}")

def test_audio_loading():
    """音声ファイルの読み込みテスト"""
    print("\n=== 音声ファイル読み込みテスト ===")
    
    input_dir = Path("converter/separate/input")
    if not input_dir.exists():
        print("入力ディレクトリが見つかりません")
        return
    
    # WAVファイルを探す
    wav_files = list(input_dir.glob("*.wav"))
    if not wav_files:
        print("WAVファイルが見つかりません")
        return
    
    sr = None
    for wav_file in wav_files:
        print(f"\nテスト対象: {wav_file.name}")
        
        # torchaudioでの読み込みテスト
        try:
            wav, sr = torchaudio.load(str(wav_file))
            print(f"  torchaudio: 成功 (shape: {wav.shape}, sr: {sr})")
            return sr
        except Exception as e:
            print(f"  torchaudio: 失敗 - {e}")
        
        # ffmpegでの読み込みテスト
        try:
            import subprocess
            result = subprocess.run([
                'ffmpeg', '-i', str(wav_file), 
                '-f', 'null', '-'
            ], capture_output=True, text=True)
            if result.returncode == 0:
                print("  ffmpeg: 成功")
            else:
                print(f"  ffmpeg: 失敗 - {result.stderr}")
        except Exception as e:
            print(f"  ffmpeg: テストできません - {e}")

def test_demucs_separation(sr):
    """Demucsでの分離テスト"""
    print("\n=== Demucs分離テスト ===")
    
    input_dir = Path("converter/separate/input")
    wav_files = list(input_dir.glob("*.wav"))
    
    if not wav_files:
        print("テスト用WAVファイルがありません")
        return
    
    test_file = wav_files[0]  # 最初のWAVファイルを使用
    print(f"テストファイル: {test_file}")
    
    try:
        print("Separatorを初期化中...")
        separator = Separator(model="htdemucs")
        print("初期化完了")
        
        # 絶対パスを使用
        abs_path = str(test_file.absolute())
        print(f"絶対パス: {abs_path}")
        
        print("音声分離を開始...")
        original, stems = separator.separate_audio_file(abs_path)
        output_dir = Path("converter/separate/output")
        output_dir.mkdir(parents=True, exist_ok=True)
        
        print("分離完了！")
        print(f"Original shape: {original.shape}")
        print(f"Stems:")

        for name, tensor in stems.items():
            # tensor は torch.Tensor なので shape 属性あり
            print(f"  - {name}: {tensor.shape}")

            out_path = output_dir / f"{test_file.stem}_{name}.wav"
            torchaudio.save(str(out_path), tensor, sr)
            print(f"  - {name} → {out_path}")        
        
        return True
        
    except Exception as e:
        print(f"エラー: {e}")
        import traceback
        traceback.print_exc()
        return False


def main():
    input_dir = Path("converter/separate/input")
    if not input_dir.exists():
        print(f"入力ディレクトリ '{input_dir}' が存在しません。")
        return
    print("Demucs音声分離 デバッグツール")
    print("=" * 50)
    
    # ステップ1: ファイルシステムのデバッグ
    debug_file_system()
    
    # ステップ2: 音声ファイルの読み込みテスト
    sr = test_audio_loading()
    
    # ステップ3: 既存ファイルでの分離テスト
    success = test_demucs_separation(sr)
    if success:
        print("音声分離テストが成功しました！")
    else:
        print("音声分離テストに失敗しました。")
    

if __name__ == "__main__":
    main()