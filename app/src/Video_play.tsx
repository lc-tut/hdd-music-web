import { useState } from 'react';
import './Video_play.css'
import {Button} from "@/components/ui/button"

function Video_play() {
  const [videoRef, setVideoRef] = useState(null);
  const [isPlaying, setIsPlaying] = useState(false);

  const handlePlayPause = () => {
    if (videoRef) {
      if (isPlaying) {
        videoRef.pause();
      } else {
        videoRef.play();
      }
      setIsPlaying(!isPlaying);
    }
  };

  return (
    <div className="video-player">
      <video ref={setVideoRef} src="/path/to/your/video.mp4" controls>
        Your browser does not support the video tag.
      </video>
    </div>
  );
}

export default Video_play;