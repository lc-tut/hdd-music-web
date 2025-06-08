package db

import (
	"context"
	"testing"

	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgtype"
)

func TestDB(t *testing.T) {
	ctx := context.Background()

	conn, err := pgx.Connect(ctx, "postgres://user:password@localhost:5432/midi")

	if err != nil {
		t.Fatalf("Unable to connect to database: %v", err)
	}

	defer conn.Close(ctx)

	db := New(conn)

	params := CreateMusicRowParams{
		Title:        "Test Music",
		MidiFilePath: "/var/music/midi/test.mid",
	}

	// Create a music row
	id, err := db.CreateMusicRow(ctx, params)

	if err != nil {
		t.Fatalf("Failed to create music row: %v", err)
	}

	if id == (pgtype.UUID{}) {
		t.Error("Expected non-zero ID, got zero")
	}

	t.Log("Created music row ID: ", id)

	// Retrieve music movies(0 is success)
	movies, err := db.GetMusicMovies(ctx)

	if err != nil {
		t.Fatalf("Failed to get music movies: %v", err)
	}

	if len(movies) != 0 {
		t.Error("Expected at least one music movie, got none")
	}

	t.Log("successfully retrieved music movies: ", len(movies))

	// Update the music row(adding a movie file path)
	updateParams := UpdateMusicMovieFilePathParams{
		ID:            id,
		MovieFilePath: "/var/music/movie/test.mp4",
	}

	_, err = db.UpdateMusicMovieFilePath(ctx, updateParams)

	if err != nil {
		t.Fatalf("Failed to update music movie file path: %v", err)
	}

	t.Log("Updated music movie file path for ID: ", id)

	// Get Music Movies
	musicMovies, err := db.GetMusicMovies(ctx)
	if err != nil {
		t.Fatalf("Failed to get music movies: %v", err)
	}

	if len(musicMovies) == 0 {
		t.Error("Expected at least one music movie, got none")
	}

	t.Log("Successfully retrieved music movies: ", len(musicMovies))
	t.Log("Music Movies: ", musicMovies)

	musicRow, err := db.GetMusicRowByID(ctx, musicMovies[0].ID)

	if err != nil {
		t.Fatalf("Failed to get music row by ID: %v", err)
	}

	if len(musicRow) == 0 {
		t.Error("Expected to find music row by ID, got none")
	}

	t.Log("Successfully retrieved music row by ID: ", musicRow[0].MovieFilePath)
}