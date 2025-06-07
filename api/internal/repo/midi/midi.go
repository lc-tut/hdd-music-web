package midi

import (
	"bytes"
	"runtime"
	"slices"
	"time"

	"github.com/lc-tut/hdd-music-web/internal/entity"
	"gitlab.com/gomidi/midi/v2"
	"gitlab.com/gomidi/midi/v2/smf"
)

type midiRepo struct{}

func NewRepository() entity.MidiRepository {
	return midiRepo{}
}

type (
	noteKey    uint8
	startOrEnd bool
)
type noteEndpoint struct {
	startOrEnd startOrEnd
	startTick  uint32
	endTick    uint32
	key        uint8
	velocity   uint8
}
type noteStart = struct {
	startTick uint32
	velocity  uint8
}
type messageWithAbsTick = struct {
	msg  smf.Message
	tick uint32
}

func (p noteEndpoint) getTick() uint32 {
	switch p.startOrEnd {
	case constNoteStart:
		return uint32(p.startTick)
	default /* self.startOrEnd is constNoteEnd */ :
		return uint32(p.endTick)
	}
}

const (
	constNoteStart = startOrEnd(true)
	constNoteEnd   = startOrEnd(false)
)

// ひとつのMIDIトラックを，複数の和音のないトラックに分ける
func splitTrackIntoSingleNoteTracks(track smf.Track) []smf.Track {
	// 最大同時発音数
	var numberOfSimultaneousTones = 0
	{
		var ch, key, vel uint8
		var tmp = 0
		for _, event := range track {
			switch {
			case event.Message.GetNoteStart(&ch, &key, &vel):
				tmp++
				if numberOfSimultaneousTones < tmp {
					numberOfSimultaneousTones = tmp
				}
			case event.Message.GetNoteEnd(&ch, &key):
				tmp--
			}
		}
	}

	if numberOfSimultaneousTones == 0 {
		return []smf.Track{}
	}

	// ノートの開始と停止
	var noteEndPoints []noteEndpoint
	{
		var ch, key, vel uint8
		currentTick := uint32(0)

		// 本来 StartTick は配列ではなく単一の値でいいはずだが，
		// 同時に同じ音高のノートがオンになる変なMIDIがよくあるので，
		// 配列として持つ。
		currentNotes := map[noteKey][]noteStart{}

		for _, event := range track {
			currentTick += event.Delta
			switch {
			case event.Message.GetNoteStart(&ch, &key, &vel):
				currentNotes[noteKey(key)] = append(currentNotes[noteKey(key)], noteStart{
					startTick: currentTick,
					velocity:  vel,
				})
			case event.Message.GetNoteEnd(&ch, &key):
				st := currentNotes[noteKey(key)][0]
				currentNotes[noteKey(key)] = currentNotes[noteKey(key)][1:]
				noteEndPoints = append(noteEndPoints, noteEndpoint{
					startTick:  st.startTick,
					endTick:    currentTick,
					startOrEnd: constNoteStart,
					key:        key,
					velocity:   st.velocity,
				}, noteEndpoint{
					startTick:  st.startTick,
					endTick:    currentTick,
					startOrEnd: constNoteEnd,
					key:        key,
					velocity:   st.velocity,
				})
			}
		}
	}
	slices.SortStableFunc(noteEndPoints, func(a, b noteEndpoint) int {
		if a.getTick() > b.getTick() {
			return 1
		} else if a.getTick() < b.getTick() {
			return -1
		} else if a.startOrEnd == constNoteEnd && b.startOrEnd == constNoteStart {
			return -1
		} else if a.startOrEnd == constNoteStart && b.startOrEnd == constNoteEnd {
			return 1
		} else if a.key > b.key {
			return 1
		} else if a.key < b.key {
			return -1
		} else {
			return 0
		}
	})

	// トラックごとに，NoteON/OFFとMetaTempo
	msgsWithTickPerTrack := make([][]messageWithAbsTick, numberOfSimultaneousTones)
	{
		var tmp = 0
		for _, p := range noteEndPoints {
			switch p.startOrEnd {
			case constNoteStart:
				msgsWithTickPerTrack[tmp] = append(msgsWithTickPerTrack[tmp], messageWithAbsTick{
					msg:  smf.Message(midi.NoteOn(0, p.key, p.velocity)),
					tick: p.startTick,
				}, messageWithAbsTick{
					msg:  smf.Message(midi.NoteOff(0, p.key)),
					tick: p.endTick,
				})
				tmp++
			default /* self.startOrEnd is constNoteEnd */ :
				tmp--
			}
		}
	}
	for i := range msgsWithTickPerTrack {
		slices.SortStableFunc(msgsWithTickPerTrack[i], func(a, b messageWithAbsTick) int {
			if a.tick > b.tick {
				return 1
			} else if a.tick < b.tick {
				return -1
			} else {
				return 0
			}
		})
	}

	res := make([]smf.Track, 0, numberOfSimultaneousTones)
	for _, msgsWithTick := range msgsWithTickPerTrack {
		t := smf.Track{}
		currentTick := uint32(0)
		for _, msgWithTick := range msgsWithTick {
			delta := msgWithTick.tick - currentTick
			currentTick = msgWithTick.tick
			t.Add(delta, msgWithTick.msg)
		}
		t.Close(0)
		res = append(res, t)
	}

	return res
}

func (midiRepo) ParseMidiFile(f entity.MidiFileInfo) ([]entity.ProcessedMidiData, error) {
	mid, err := smf.ReadFile(f.Path)
	if err != nil {
		return nil, err
	}

	tempoMarkers := []messageWithAbsTick{}
	for _, track := range mid.Tracks {
		currentTick := uint32(0)
		for _, event := range track {
			currentTick += event.Delta
			var bpm float64
			switch {
			case event.Message.GetMetaTempo(&bpm):
				tempoMarkers = append(tempoMarkers, messageWithAbsTick{
					msg:  smf.MetaTempo(bpm),
					tick: currentTick,
				})
			}
		}
	}
	slices.SortStableFunc(tempoMarkers, func(a, b messageWithAbsTick) int {
		if a.tick > b.tick {
			return 1
		} else if a.tick < b.tick {
			return -1
		} else {
			return 0
		}
	})
	tempoTrack := smf.Track{}
	currentTick := uint32(0)
	for _, msgWithTick := range tempoMarkers {
		delta := msgWithTick.tick - currentTick
		currentTick = msgWithTick.tick
		tempoTrack.Add(delta, msgWithTick.msg)
	}
	tempoTrack.Close(0)

	newMid := smf.NewSMF1()

	err = newMid.Add(tempoTrack)
	if err != nil {
		return nil, err
	}

	for _, track := range mid.Tracks {
		for _, track1 := range splitTrackIntoSingleNoteTracks(track) {
			newMid.TimeFormat = mid.TimeFormat
			err = newMid.Add(track1)
			if err != nil {
				return nil, err
			}
		}
	}

	buf := bytes.NewBuffer(nil)
	newMid.WriteTo(buf)
	newMidTracks := smf.ReadTracksFrom(buf)
	if err != nil {
		return nil, err
	}

	midis := make([]entity.ProcessedMidiData, int(newMidTracks.SMF().NumTracks()))
	newMidTracks.Do(func(te smf.TrackEvent) {
		track := &midis[te.TrackNo]
		(*track).Events = append((*track).Events, entity.MidiEvent{
			Msg:         entity.MidiMessage(te.Message.Bytes()),
			AbsMicroSec: uint64(te.AbsMicroSeconds),
		})
	})
	return midis, nil
}

func (midiRepo) PlayMidi(m entity.ProcessedMidiData, hn entity.HardwereNumber) error {
	last := time.Duration(0)
	out := midi.GetOutPorts()[int(hn)]

	err := out.Open()
	if err != nil {
		return err
	}
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()

	for _, e := range m.Events {
		current := (time.Microsecond * time.Duration(e.AbsMicroSec))
		diff := current - last
		time.Sleep(diff)
		err := out.Send(e.Msg)
		if err != nil {
			return err
		}
		last = current
	}
	return nil
}
