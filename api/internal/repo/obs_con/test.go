package main

import (
	"fmt"
	"log"
	"net/http"
	"github.com/gorilla/websocket"
)

// Hello メッセージの構造体
type HelloMessage struct {
	Op int `json:"op"`
	D  struct {
		RpcVersion int `json:"rpcVersion"`
	} `json:"d"`
}

func main() {
	http.HandleFunc("/start", func(w http.ResponseWriter, r *http.Request) {
		go startOBS()
		fmt.Fprintln(w, "配信開始コマンドを送信しました")
	})

	http.HandleFunc("/stop", func(w http.ResponseWriter, r *http.Request) {
		go stopOBS()
		fmt.Fprintln(w, "配信停止コマンドを送信しました")
	})

	// 録画開始
	http.HandleFunc("/start-record", func(w http.ResponseWriter, r *http.Request) {
		go startRecording()
		fmt.Fprintln(w, "録画開始コマンドを送信しました")
	})

	// 録画停止
	http.HandleFunc("/stop-record", func(w http.ResponseWriter, r *http.Request) {
		go stopRecording()
		fmt.Fprintln(w, "録画停止コマンドを送信しました")
	})

	http.Handle("/", http.FileServer(http.Dir(".")))

	fmt.Println("サーバー起動中: http://localhost:8080")
	http.ListenAndServe(":8080", nil)
}

// WebSocket接続して認証まで行う共通関数
func connectAndIdentify() (*websocket.Conn, error) {
    conn, _, err := websocket.DefaultDialer.Dial("ws://localhost:4455", nil)
    if err != nil {
        return nil, err
    }

    // Hello受信
    _, _, err = conn.ReadMessage()
    if err != nil {
        conn.Close()
        return nil, err
    }

    // Identify送信
    identify := `{
        "op": 1,
        "d": {
            "rpcVersion": 1
        }
    }`
    if err := conn.WriteMessage(websocket.TextMessage, []byte(identify)); err != nil {
        conn.Close()
        return nil, err
    }

    // Identified受信
    _, _, err = conn.ReadMessage()
    if err != nil {
        conn.Close()
        return nil, err
    }

    return conn, nil
}

// StartStream
func startOBS() {
    conn, err := connectAndIdentify()
    if err != nil {
        log.Println("接続失敗:", err)
        return
    }
    defer conn.Close()

    start := `{
        "op": 6,
        "d": {
            "requestType": "StartStream",
            "requestId": "start"
        }
    }`
    conn.WriteMessage(websocket.TextMessage, []byte(start))
    log.Println("配信開始を送信")
}

// StopStream
func stopOBS() {
    conn, err := connectAndIdentify()
    if err != nil {
        log.Println("接続失敗:", err)
        return
    }
    defer conn.Close()

    stop := `{
        "op": 6,
        "d": {
            "requestType": "StopStream",
            "requestId": "stop"
        }
    }`
    conn.WriteMessage(websocket.TextMessage, []byte(stop))
    log.Println("配信停止を送信")
}

// 録画開始
func startRecording() {
    conn, err := connectAndIdentify()
    if err != nil {
        log.Println("接続失敗:", err)
        return
    }
    defer conn.Close()

    start := `{
        "op": 6,
        "d": {
            "requestType": "StartRecord",
            "requestId": "start-record"
        }
    }`
    conn.WriteMessage(websocket.TextMessage, []byte(start))
    log.Println("録画開始を送信")
}

// 録画停止
func stopRecording() {
    conn, err := connectAndIdentify()
    if err != nil {
        log.Println("接続失敗:", err)
        return
    }
    defer conn.Close()

    stop := `{
        "op": 6,
        "d": {
            "requestType": "StopRecord",
            "requestId": "stop-record"
        }
    }`
    conn.WriteMessage(websocket.TextMessage, []byte(stop))
    log.Println("録画停止を送信")
}
