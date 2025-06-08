package controller

import (
	"github.com/labstack/echo/v4"

	"github.com/lc-tut/hdd-music-web/internal/presenter"
	"github.com/lc-tut/hdd-music-web/internal/usecase"
)

type MediaController struct {
	useCase   usecase.UseCaseInterface
	presenter presenter.IMadiaPresenter
	errorPresenter presenter.IErrorPresenter
}

func NewMediaController(
	useCase usecase.UseCaseInterface,
	presenter presenter.IMadiaPresenter,
	errorPresenter presenter.IErrorPresenter,
) *MediaController {
	return &MediaController{
		useCase:   useCase,
		presenter: presenter,
		errorPresenter: errorPresenter,
	}
}

func (media *MediaController) GetMusicMovies(c echo.Context) error {
	movies, err := media.useCase.GetMusicMovies(c.Request().Context())
	if err != nil {
		return media.errorPresenter.PresentInternalServerError(c, err)
	}

	return media.presenter.PresentMusicMovies(c, movies)
}

func (media *MediaController) GetMusicMoviePath(c echo.Context) error {
	id := c.Param("id")
	if id == "" {
		return media.errorPresenter.PresentBadRequest(c, "id is required")
	}

	path, err := media.useCase.GetMusicMoviePath(c.Request().Context(), id)
	if err != nil {
		return media.errorPresenter.PresentInternalServerError(c, err)
	}

	return media.presenter.PresentMusicMoviePath(c, path.MovieFilePath)
}