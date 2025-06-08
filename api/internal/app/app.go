package app

import (
	"context"
	"fmt"

	"github.com/jackc/pgx/v5/pgxpool"
	"github.com/lc-tut/hdd-music-web/config"
	"github.com/lc-tut/hdd-music-web/internal/controller"
	"github.com/lc-tut/hdd-music-web/internal/presenter"
	"github.com/lc-tut/hdd-music-web/internal/repo/db"
	"github.com/lc-tut/hdd-music-web/internal/repo/wav2mid"
	router "github.com/lc-tut/hdd-music-web/internal/router"
	"github.com/lc-tut/hdd-music-web/internal/usecase"
	"github.com/lc-tut/hdd-music-web/pkg/logger"
)

// run
func Run(ctx context.Context, cfg *config.Config) {
	l := logger.New(cfg.Log.LogLevel)

	l.Info("App Name: %s", cfg.App.Name)

	// データベース接続設定
	dbConfig, err := pgxpool.ParseConfig("postgres://user:password@db:5432/midi")
	if err != nil {
		l.Fatal("Unable to parse database config: %v", err)
	}

	// 接続プールの設定
	dbConfig.MaxConns = 10                 // 最大接続数
	dbConfig.MinConns = 1                  // 最小接続数（アイドル状態の接続を維持）
	dbConfig.MaxConnLifetime = 1 * 60 * 60 // 接続の最大寿命（秒）
	dbConfig.MaxConnIdleTime = 15 * 60     // アイドル接続の最大時間（秒）

	// 接続プールの作成
	pool, err := pgxpool.NewWithConfig(ctx, dbConfig)
	if err != nil {
		l.Fatal("Unable to create connection pool: %v", err)
	}
	defer pool.Close()

	// 接続テスト
	if err := pool.Ping(ctx); err != nil {
		l.Fatal("Unable to ping database: %v", err)
	}
	l.Info("Successfully connected to database")

	// Queriesインスタンスの作成
	queries := db.New(pool)

	// usecase
	useCase := usecase.NewUseCase(
		queries,
		wav2mid.NewAPIConverter(
			"http://localhost:3000", // WAV2MIDのAPIエンドポイント
		),
	)

	// コントローラーの作成
	mediaController := controller.NewMediaController(
		useCase,
		presenter.NewMediaPresenter(),
		presenter.NewErrorPresenter(),
	)

	// ルーターの初期化と設定
	r := router.NewRouter(
		mediaController,
	)
	r.SetupRoutes()

	// サーバー起動
	serverAddr := fmt.Sprintf("0.0.0.0:%d", cfg.App.Port) // ポート設定を追加
	l.Info("Starting server at %s", serverAddr)
	if err := r.Start(serverAddr); err != nil {
		l.Fatal("Server error: %v", err)
	}
}
