package entity

type (
	Obs struct {
		path string
	}

	ObsRepository interface {
		recording() error
		stopRecording() error
	}
)