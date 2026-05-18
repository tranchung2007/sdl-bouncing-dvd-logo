#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdlib.h>

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800
#define DVD_LOGO_SPEED 200

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

typedef struct {
    float x;
    float y;
} Vector2_f;

static Vector2_f dvd_position;
static Vector2_f dvd_direction;

static int dvd_logo_width = 0;
static int dvd_logo_height = 0;

static Uint64 last = 0;

static int sdl_check_bool(bool code) {
    if (!code) {
        SDL_Log("ERROR: %s", SDL_GetError());
    }
    return code;
}

static void *sdl_check_ptr(void *ptr) {
    if (ptr == NULL) {
        SDL_Log("ERROR: %s", SDL_GetError());
    }
    return ptr;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

    SDL_Surface *surface = NULL;
    char *dvd_png_path = "./dvd_logo.png";

    sdl_check_bool(SDL_Init(SDL_INIT_VIDEO));

    sdl_check_bool(SDL_CreateWindowAndRenderer(
        "bouncing dvd logos", WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_VULKAN, &window, &renderer));

    // SDL_Log("window =  %p", window);
    // SDL_Log("renderer =  %p", renderer);

    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX |
                                         SDL_RENDERER_VSYNC_ADAPTIVE);

    surface = SDL_LoadPNG(dvd_png_path);

    last = SDL_GetTicks();

    sdl_check_ptr(surface);
    // SDL_Log("surface =  %p", surface);

    dvd_direction.x = dvd_direction.y = 1.0f;
    dvd_position.x = dvd_position.y = 50.0f;

    dvd_logo_width = surface->w / 2.0f;
    dvd_logo_height = surface->h / 2.0f;

    // SDL_free(&dvd_png_path);

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    sdl_check_ptr(texture);
    // SDL_Log("texture =  %p", surface);

    SDL_DestroySurface(surface);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

    const SDL_FRect dvd_logo = {(int)SDL_roundf(dvd_position.x),
                                (int)SDL_roundf(dvd_position.y), dvd_logo_width,
                                dvd_logo_height};

    const Uint64 now = SDL_GetTicks();
    const float elapsed = (float)(now - last) / 1000.0f;
    // SDL_Log("elapsed = %f", elapsed);
    const float distance = elapsed * DVD_LOGO_SPEED;
    // SDL_Log("distance = %f", distance);

    dvd_position.x += dvd_direction.x * distance;
    if (dvd_position.x <= 0) {
        dvd_position.x = 0;
        dvd_direction.x *= -1.0f;
    }
    if (dvd_position.x >= WINDOW_WIDTH - dvd_logo_width) {
        dvd_position.x = WINDOW_WIDTH - dvd_logo_width;
        dvd_direction.x *= -1.0f;
    }

    dvd_position.y += dvd_direction.y * distance;
    if (dvd_position.y <= 0) {
        dvd_position.y = 0;
        dvd_direction.y *= -1.0f;
    }

    if (dvd_position.y > WINDOW_HEIGHT - dvd_logo_height ||
        dvd_position.y < 0) {
        dvd_position.y = WINDOW_HEIGHT - dvd_logo_height;
        dvd_direction.y *= -1.0f;
    }

    last = now;

    SDL_SetRenderDrawColor(renderer, 18, 18, 18, 1);
    SDL_RenderClear(renderer);

    SDL_RenderTexture(renderer, texture, NULL, &dvd_logo);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult) { SDL_DestroyTexture(texture); }
