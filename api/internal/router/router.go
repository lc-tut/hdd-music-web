package router

import (
	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
	"github.com/lc-tut/hdd-music-web/internal/controller"
)

// Router はルート設定を行う構造体
type Router struct {
	e              *echo.Echo
	authController *controller.AuthController
	userController *controller.UserController
}

// NewRouter は新しいRouterインスタンスを作成する
func NewRouter(
	authController *controller.AuthController,
	userController *controller.UserController,
) *Router {
	e := echo.New()

	// ミドルウェアの設定
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())

	return &Router{
		e:              e,
		authController: authController,
		userController: userController,
	}
}

func health(c echo.Context) error {
	return c.String(200, "OK")
}

// SetupRoutes はルート設定を行う
func (r *Router) SetupRoutes() {

	r.e.GET("/health", health)
}

// Start はサーバーを起動する
func (r *Router) Start(address string) error {
	return r.e.Start(address)
}

// Echo はEchoインスタンスを返す
func (r *Router) Echo() *echo.Echo {
	return r.e
}
