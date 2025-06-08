import { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import './Video_list.css'

function Video_list() {
  return (
    <>
      <Header />
      <div className="table-container">
        <Table_func />
      </div>
    </>
  )
}

function Header() {
  return (
    <header className="App-header">
      <h1>動画一覧</h1>
    </header>
  )
}

import {
  Table,
  TableBody,
  TableCell,
  TableRow,
} from "@/components/ui/table"

// Define the interface for video data
interface Video {
  id: string;
  title: string;
  created_at: string;
}

function Table_func() {
  const navigate = useNavigate(); // React Routerの移動用フック
  // 1. 取得した動画リストを保存するための「状態(state)」を定義します。
  //    isLoadingは、データ取得中であることを示すためのものです。
  const [videos, setVideos] = useState<Video[]>([]);
  const [isLoading, setIsLoading] = useState(true);

  // 2. コンポーネントが最初に表示された時に一度だけ実行される処理
  useEffect(() => {
    // APIからデータを取得する非同期処理
    const fetchVideoData = async () => {
      try {
        // 【重要】ここに実際のAPIサーバーのURLを指定します
        const url = 'http://localhost:8080/media/movies'; // 今は仮のURLです

        // --- ここから下は、APIがない場合でも動作確認するためのサンプルコードです ---
        // 実際のAPIがある場合は、このシミュレーション部分は削除してください。
        // console.log("データの取得を開始します...");
        // await new Promise(resolve => setTimeout(resolve, 1000)); // 1秒待機して通信をシミュレート
        // const mockData = [
        //   { id: 1, name: 'Test1', date: '2025/06/08 09:00:00' },
        //   { id: 2, name: 'Test2', date: '2025/06/07 18:30:00' },
        //   { id: 3, name: 'Test3', date: '2025/06/06 12:15:00' },
        //   { id: 4, name: 'Test4', date: '2025/06/05 21:45:00' },
        // ];
        // setVideos(mockData); // サンプルデータを状態にセット
        // --- ここまでサンプルコード ---


        // 実際のAPIを叩く場合は、以下のコードのコメントを解除してください
        const response = await fetch(url);
        if (!response.ok) {
          throw new Error('データの取得に失敗しました');
        }
        const data = await response.json();
        setVideos(data.movies); // 取得したデータを状態にセット

      } catch (error) {
        console.error("エラーが発生しました:", error);
      } finally {
        setIsLoading(false); // ローディング状態を解除
      }
    };

    fetchVideoData();
  }, []); // 第2引数の配列を空にすると、この中の処理は最初に1回だけ実行されます

  // 行がクリックされた時のハンドラ
  const handleRowClick = (videoId: string) => {
    navigate(`/play/${videoId}`); // 動画再生ページへ遷移
  };

  // データ取得中であれば、ローディングメッセージを表示
  if (isLoading) {
    return <p>データを読み込み中...</p>;
  }

  // 3. 取得したデータ(videos)を `map` で展開し、TableRowを動的に生成
  return (
    <Table className='table'>
      <TableBody>
        {videos.map((video) => (
          // mapでリスト表示する際は、各要素を区別するためにユニークな `key` を指定します
          <TableRow
            key={video.id}
            className="cursor-pointer hover:bg-gray-100"
            onClick={() => handleRowClick(video.id)}
          >
            <TableCell className='video_name'>{video.title}</TableCell>
            <TableCell className='video_date'>
              {new Date(video.created_at).toLocaleString('ja-JP', {
                year: 'numeric',
                month: '2-digit',
                day: '2-digit',
                hour: '2-digit',
                minute: '2-digit'
              })}
            </TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  );
}

export default Video_list
