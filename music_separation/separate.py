from demucs.api import Separator, save_audio
from pathlib import Path

separator = Separator()

input_audio = "music_separation/input/01_かぜのねいろ[1].wav"


original, stems = separator.separate_audio_file(input_audio)

output_dir = Path("separated")
output_dir.mkdir(exist_ok=True)

for name, source in stems.items():
    save_audio(
        source,
        str(output_dir / f"{name}.wav"),
        samplerate=separator.samplerate
    )