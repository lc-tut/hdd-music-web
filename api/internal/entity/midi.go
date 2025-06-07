// midi出力に関わるrepositoryの定義

package entity

type (

	Midi struct {
		path string
	}
	MidiRepository interface {                    // midiの和音分離
		playt(hn int) (bool, error) // 出力ポートの演奏
	}

)


