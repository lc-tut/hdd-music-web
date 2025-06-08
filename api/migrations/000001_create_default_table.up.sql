BEGIN;

CREATE TABLE IF NOT EXISTS musics (
    id UUID DEFAULT gen_random_uuid() PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    midi_file_path VARCHAR(255) NOT NULL,
    movie_file_path VARCHAR(255) DEFAULT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO musics (title, midi_file_path, movie_file_path)
VALUES
    ('Default Music', '/var/midi/test1/default.mid', 'test1/default.mp4');

COMMIT;

