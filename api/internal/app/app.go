package app

import (
	"context"
	"fmt"

	"github.com/jackc/pgx/v5"
	"github.com/lc-tut/hdd-music-web/config"
	// "github.com/lc-tut/hdd-music-web/internal/controller"
	router "github.com/lc-tut/hdd-music-web/internal/router"
	// "github.com/lc-tut/hdd-music-web/internal/presenter"
	// "github.com/lc-tut/hdd-music-web/internal/repo/db"
	// "github.com/lc-tut/hdd-music-web/internal/repo/webapi"
	// "github.com/lc-tut/hdd-music-web/internal/usecase"
	"github.com/lc-tut/hdd-music-web/pkg/logger"
)

// run
func Run(ctx context.Context, cfg *config.Config) {
	l := logger.New(cfg.Log.LogLevel)

	l.Info("App Name: %s", cfg.App.Name)

	// Repositroy
	conn, err := pgx.Connect(ctx, "postgres://user:password@postgis:5432/gis")
	if err != nil {
		l.Fatal("Unable to connect to database: %v\n", err)
	}
	defer conn.Close(ctx)

	// Queriesインスタンスの作成
	// queries := db.New(conn)

	// usecase

	// コントローラーの作成


	// ルーターの初期化と設定
	r := router.NewRouter(
		// TODO: ルーターの追加
	)
	r.SetupRoutes()

	// サーバー起動
	serverAddr := fmt.Sprintf("0.0.0.0:%d", cfg.App.Port) // ポート設定を追加
	l.Info("Starting server at %s", serverAddr)
	if err := r.Start(serverAddr); err != nil {
		l.Fatal("Server error: %v", err)
	}
}
