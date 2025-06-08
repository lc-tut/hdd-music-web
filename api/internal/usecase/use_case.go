package usecase

import (
	"context"
	"fmt"
	"io"
	"mime/multipart"
	"os"
	"strings"

	"github.com/google/uuid"
	"github.com/jackc/pgx/v5/pgtype"

	"github.com/lc-tut/hdd-music-web/internal/repo/db"
	"github.com/lc-tut/hdd-music-web/internal/repo/wav2mid"
)

type UseCase struct {
	dbRepo  db.Queries
	cnvRepo wav2mid.WAV2MIDConverter
}

type input struct {
	File  *multipart.FileHeader
}

type UseCaseInterface interface {
	UploadFile(ctx context.Context, input input) (string, error)
	GetMusicMovies(ctx context.Context) ([]db.GetMusicMoviesRow, error)
	GetMusicMoviePath(ctx context.Context, id string) (string, error)
}

func NewUseCase(dbRepo db.Queries, cnvRepo wav2mid.WAV2MIDConverter) *UseCase {
	return &UseCase{
		dbRepo:  dbRepo,
		cnvRepo: cnvRepo,
	}
}

func (u *UseCase) UploadFile(ctx context.Context, input input) (string, error) {
	folderID := uuid.New().String()

	src, err := input.File.Open()

	if err != nil {
		return "", err
	}
	defer src.Close()

	var filepaths []string

	parts := strings.Split(input.File.Filename, ".")
	fileName := parts[0]
	fileType := parts[len(parts)-1]

	if fileType == "wav" {
		// WAVファイルの場合、MIDIに変換

		// srcを一時ファイルに保存
		tempFile, err := os.CreateTemp("/tmp", "temp_audio" + folderID + ".wav")
		if err != nil {
			return "", fmt.Errorf("failed to create temp file: %w", err)
		}
		defer os.Remove(tempFile.Name())

		result, err := u.cnvRepo.ConvertWAVToMID("/tmp/temp_audio" + folderID + ".wav")
		if err != nil {
			return "", err
		}
		filepaths = append(filepaths, result["midi_path"].(string))
	} else if strings.Split(input.File.Filename, ".")[1] == "mid" {
		// MIDIファイルの場合、そのまま保存
		filepaths = append(filepaths, input.File.Filename)
	} else {
		return "", fmt.Errorf("unsupported file type: %s", input.File.Filename)
	}

	savePath := "/var/music/" + folderID

	// folderIDのfolderを作成後、直下にmidiファイルを保存
	os.MkdirAll(savePath, os.ModePerm)
	for _, filepath := range filepaths {
		file, err := os.Open(filepath)
		if err != nil {
			return "", err
		}
		defer file.Close()

		destPath := savePath + "/" + filepath
		dest, err := os.Create(destPath)
		if err != nil {
			return "", err
		}
		defer dest.Close()

		_, err = io.Copy(dest, file)
		if err != nil {
			return "", err
		}
	}

	// データベースに保存
	id, err := u.dbRepo.CreateMusicRow(ctx, db.CreateMusicRowParams{
		Title:        fileName,
		MidiFilePath: folderID,
	})

	if err != nil {
		return "", fmt.Errorf("failed to save music data: %w", err)
	}
	return id.String(), nil
}

func (u *UseCase) GetMusicMovies(ctx context.Context) ([]db.GetMusicMoviesRow, error) {
	movies, err := u.dbRepo.GetMusicMovies(ctx)
	if err != nil {
		return nil, fmt.Errorf("failed to get music movies: %w", err)
	}
	return movies, nil
}

func (u *UseCase) GetMusicMoviePath(ctx context.Context, id string) (string, error) {
	var musicID pgtype.UUID

	if musicID.Scan(id) != nil {
		return "", fmt.Errorf("invalid music ID format: %s", id)
	}

	music, err := u.dbRepo.GetMusicRowByID(ctx, musicID)
	if err != nil {
		return "", fmt.Errorf("failed to get music by ID: %w", err)
	}

	if len(music) == 0 || music[0].MovieFilePath == "" {
		return "", fmt.Errorf("no movie file found for music ID: %s", id)
	}

	return music[0].MovieFilePath, nil
}