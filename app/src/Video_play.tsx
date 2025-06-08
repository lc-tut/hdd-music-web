import { useState, useEffect } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import './Video_play.css';
import { Button } from "@/components/ui/button";

function Video_play() {
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();
  const [videoUrl, setVideoUrl] = useState<string | null>(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    // APIから動画のファイルパスを取得する
    const fetchVideoPath = async () => {
      if (!id) {
        setError('動画IDが指定されていません');
        setIsLoading(false);
        return;
      }

      try {
        const response = await fetch(`http://localhost:8080/media/movies/${id}`);

        if (!response.ok) {
          throw new Error('動画情報の取得に失敗しました');
        }

        const data = await response.json();

        if (!data || !data.movie_file_path) {
          throw new Error('動画のパスが見つかりません');
        }

        // APIから取得したファイルパスを /m エンドポイントと組み合わせる
        setVideoUrl(`http://localhost:8080/m/${data.movie_file_path}`);
      } catch (err) {
        setError(err instanceof Error ? err.message : '不明なエラーが発生しました');
      } finally {
        setIsLoading(false);
      }
    };

    fetchVideoPath();
  }, [id]);

  const handleBack = () => {
    navigate('/');
  };

  if (isLoading) {
    return <div className="loading">動画情報を読み込み中...</div>;
  }

  if (error) {
    return (
      <div className="error">
        <h2>エラーが発生しました</h2>
        <p>{error}</p>
        <Button onClick={handleBack}>一覧に戻る</Button>
      </div>
    );
  }

  return (
    <div className="video-container">
      <div className="video-header">
        <Button onClick={handleBack} className="back-button">← 一覧に戻る</Button>
      </div>

      <div className="video-player">
        {videoUrl ? (
          <video
            src={videoUrl}
            controls
          >
            お使いのブラウザは動画再生をサポートしていません。
          </video>
        ) : (
          <div className="no-video">
            動画が見つかりませんでした
          </div>
        )}
      </div>
    </div>
  );
}

export default Video_play;