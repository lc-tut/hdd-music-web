import { useState } from 'react';
import './Midi_upload.css';

import { Button } from "@/components/ui/button";
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";

function Midi_upload() {
  return (
    <>
      <Header />
      <div className='button-container'>
        <Dialog_func />
      </div>
    </>
  );
}

function Header() {
  return (
    <header className="App-header">
      <h1>Upload</h1>
    </header>
  );
}

function Dialog_func() {
  const [selectedFile, setSelectedFile] = useState(null);
  const [uploadStatus, setUploadStatus] = useState('');

  const handleFileChange = (event) => {
    if (event.target.files && event.target.files.length > 0) {
      setSelectedFile(event.target.files[0]);
      setUploadStatus('');
    }
  };

  const handleUpload = async () => {
    if (!selectedFile) {
      setUploadStatus('ファイルを選択してください。');
      return;
    }

    setUploadStatus('アップロード中...');
    const formData = new FormData();
    // サーバー側で受け取るキー名は 'file' のように汎用的にしても良いでしょう
    formData.append('uploadFile', selectedFile);

    try {
      const response = await fetch('/api/upload', {
        method: 'POST',
        body: formData,
      });

      if (response.ok) {
        setUploadStatus('アップロードに成功しました！');
      } else {
        const errorData = await response.json().catch(() => null);
        setUploadStatus(`エラー: ${errorData?.message || 'アップロードに失敗しました。'}`);
      }
    } catch (error) {
      console.error('Upload error:', error);
      setUploadStatus('ネットワークエラーが発生しました。');
    }
  };

  return (
    <Dialog onOpenChange={(open) => {
      if (!open) {
        setSelectedFile(null);
        setUploadStatus('');
      }
    }}>
      <DialogTrigger asChild>
        {/* ▼ 変更: ボタンのテキストを更新 */}
        <Button variant="outline">ファイルをアップロード</Button>
      </DialogTrigger>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          {/* ▼ 変更: タイトルを更新 */}
          <DialogTitle>ファイルをアップロード</DialogTitle>
          {/* ▼ 変更: 説明文を更新 */}
          <DialogDescription>
            ここにアップロードするMIDIまたはWAVファイルを選択してください。
          </DialogDescription>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="upload-file" className="text-right">
              ファイル
            </Label>
            {/* ▼ 変更点: accept属性に .wav を追加 */}
            <Input
              id="upload-file"
              type="file"
              accept=".mid, .midi, .wav" // MIDIとWAVファイル形式を許可
              onChange={handleFileChange}
              className="col-span-3"
            />
          </div>
          {selectedFile && (
            <div className="col-span-4 text-sm text-center text-muted-foreground">
              選択中のファイル: {selectedFile.name}
            </div>
          )}
        </div>
        <DialogFooter>
          {uploadStatus && <p className="text-sm mr-auto">{uploadStatus}</p>}
          <Button onClick={handleUpload} disabled={!selectedFile || uploadStatus === 'アップロード中...'}>
            {uploadStatus === 'アップロード中...' ? '処理中...' : 'アップロード'}
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}

export default Midi_upload;