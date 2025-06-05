from basic_pitch.inference import predict_and_save
from basic_pitch import ICASSP_2022_MODEL_PATH
import os


# 入力ファイルのパス
input_audio = "converter/input/lovedream.mp3"

# 出力先ディレクトリ
output_dir = "converter/output"



# 結果を保存しながら予測
predict_and_save(
    [input_audio],  # 入力音声ファイルパスのリスト
    output_dir,  # 出力ディレクトリ
    True,  # save_midi .midを保存
    False,  # sonify_midi MIDIをwavに変換して保存
    False,  # save_model_outputs MIDIに音を付けて出力（.wav）
    False,  # save_notes ノート情報を保存
    ICASSP_2022_MODEL_PATH,
)
