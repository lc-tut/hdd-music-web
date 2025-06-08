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

	return c.JSON(http.StatusOK, response)
}

func (p *MediaPresenter) PresentMusicMoviePath(c echo.Context, path string) error {
	response := struct {
		MovieFilePath string `json:"movie_file_path"`
	}{
		MovieFilePath: path,
	}

	return c.JSON(http.StatusOK, response)
}
