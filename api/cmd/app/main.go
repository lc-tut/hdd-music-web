package main

import (
	"context"
	"log"

	"github.com/lc-tut/hdd-music-web/internal/app"
	"github.com/lc-tut/hdd-music-web/config"
)

func main() {
	ctx := context.Background()

	cfg, err := config.NewConfig()
	if err != nil {
		log.Fatalf("Config error: %s",err)
	}

	app.Run(ctx, cfg)
}