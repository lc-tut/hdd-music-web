from flask import Flask, request, jsonify
import wav_to_midi

app = Flask(__name__)

@app.route('/convert', methods=['POST'])
def convert():
    data = request.get_json()
    if not data or 'audio_path' not in data:
        return jsonify({'error': 'No audio_path provided'}), 400
    audio_path = data['audio_path']
    try:
        midi_files = wav_to_midi.main(audio_path)
        return jsonify({'result_files': midi_files}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=3000)