-- name: CreateMusicRow :one
INSERT INTO musics (
    title, midi_file_path
) VALUES (
    $1, $2
) RETURNING id, midi_file_path;

-- name: GetMusicRowByID :many
SELECT * FROM musics WHERE id = $1;

-- name: GetMusicMovies :one
SELECT title, created_at, updated_at FROM musics
WHERE movie_file_path IS NOT NULL
AND movie_file_path != ''
ORDER BY created_at DESC;

-- name: UpdateMusicMovieFilePath :one
UPDATE musics
SET movie_file_path = $2
WHERE id = $1
RETURNING id, movie_file_path;