package wav2mid

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
)

// WAV2MIDConverter はWAVをMIDIに変換するためのインターフェース
type WAV2MIDConverter interface {
	ConvertWAVToMID(audioPath string) (map[string]interface{}, error)
}

// APIConverter はAPIを使ってWAVをMIDIに変換する実装
type APIConverter struct {
	apiURL string
}

// NewAPIConverter は新しいAPIConverterインスタンスを作成します
func NewAPIConverter(apiURL string) *APIConverter {
	return &APIConverter{
		apiURL: apiURL,
	}
}

// ConvertWAVToMID は指定されたWAVファイルパスをAPIに送信してMIDIに変換します
// 戻り値: Midiのファイルパス
func (ac *APIConverter) ConvertWAVToMID(audioPath string) (map[string]interface{}, error) {
	payload := map[string]string{"audio_path": audioPath}
	jsonData, err := json.Marshal(payload)
	if err != nil {
		return nil, fmt.Errorf("failed to marshal JSON: %w", err)
	}

	convertURL := fmt.Sprintf("%s/convert", ac.apiURL)

	resp, err := http.Post(convertURL, "application/json", bytes.NewBuffer(jsonData))
	if err != nil {
		return nil, fmt.Errorf("API request failed: %w", err)
	}
	defer resp.Body.Close()

	var result map[string]interface{}
	if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
		return nil, fmt.Errorf("failed to decode response: %w", err)
	}

	return result, nil
}