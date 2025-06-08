-- name: CreateMusicRow :one
INSERT INTO musics (
    title, midi_file_path
) VALUES (
    $1, $2
) RETURNING id;

-- name: GetMusicRowByID :one
SELECT * FROM musics WHERE id = $1;

-- name: GetMusicMovies :many
SELECT id, title, created_at FROM musics
WHERE movie_file_path IS NOT NULL
AND movie_file_path != ''
ORDER BY created_at DESC;

-- name: UpdateMusicMovieFilePath :one
UPDATE musics
SET movie_file_path = $2
WHERE id = $1
RETURNING id, movie_file_path;