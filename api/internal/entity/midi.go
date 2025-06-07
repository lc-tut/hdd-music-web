// midi出力に関わるrepositoryの定義

package entity

type (
	MidiFileInfo struct {
		Path string
	}
	MidiMessage []byte
	MidiEvent   struct {
		Msg         MidiMessage
		AbsMicroSec uint64
	}
	ProcessedMidiData struct {
		Events []MidiEvent
	}
	HardwereNumber int

	MidiRepository interface {
		ParseMidiFile(MidiFileInfo) ([]ProcessedMidiData, error)
		PlayMidi(ProcessedMidiData, HardwereNumber) error
	}
)
