package presenter

import (
	"net/http"

	"github.com/labstack/echo/v4"
	"github.com/lc-tut/hdd-music-web/internal/usecase"
)

type IMadiaPresenter interface {
	PresentMusicMovies(c echo.Context, movies []usecase.GetMusicMoviesRow) error
	PresentMusicMoviePath(c echo.Context, path string) error
}

type MediaPresenter struct {}

func NewMediaPresenter() *MediaPresenter {
	return &MediaPresenter{}
}

func (p *MediaPresenter) PresentMusicMovies(c echo.Context, movies []usecase.GetMusicMoviesRow) error {
	response := struct {
		Movies []usecase.GetMusicMoviesRow `json:"movies"`
	}{
		Movies: movies,
	}

	c.Response().Header().Set("Access-Control-Allow-Origin", "*")
	c.Response().Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
	c.Response().Header().Set("Access-Control-Allow-Headers", "Content-Type, Authorization")

	return c.JSON(http.StatusOK, response)
}

func (p *MediaPresenter) PresentMusicMoviePath(c echo.Context, path string) error {
	response := struct {
		MovieFilePath string `json:"movie_file_path"`
	}{
		MovieFilePath: path,
	}

	c.Response().Header().Set("Access-Control-Allow-Origin", "*")
	c.Response().Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
	c.Response().Header().Set("Access-Control-Allow-Headers", "Content-Type, Authorization")

	return c.JSON(http.StatusOK, response)
}
