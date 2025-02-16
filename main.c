#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define TILE_SIZE 20

SDL_FRect drawRect;
int direction;
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

SDL_Point randomTile() {
  return (SDL_Point){rand() % (WINDOW_WIDTH / TILE_SIZE - 1),
                     rand() % (WINDOW_HEIGHT / TILE_SIZE - 1)};
}

SDL_Point* snake;
size_t snakeSize, snakeCap;

void addToSnake(SDL_Point point) {
  if(snakeSize == snakeCap) {
    size_t newCap = (snakeCap == 0) ? 1 : snakeCap * 2;
    SDL_Point *tmp_snake = (SDL_Point *)realloc(snake, newCap * sizeof(SDL_Point));
    if(tmp_snake == NULL) {
      free(snake);
      exit(1);
    }
    snake = tmp_snake;
    snakeCap = newCap;
  }
  snake[snakeSize] = point;
  snakeSize++;
}

void resetGame() {
  snakeSize = 0;
  addToSnake(randomTile());
  direction = (rand() % 4);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if(!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if(!SDL_CreateWindowAndRenderer("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                   &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  drawRect.w = drawRect.h = TILE_SIZE;
  srand(time(NULL));

  resetGame();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_DOWN:
    switch (event->key.scancode) {
    case SDL_SCANCODE_ESCAPE:
    case SDL_SCANCODE_Q:
      return SDL_APP_SUCCESS;
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP:
      direction = UP;
      break;
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
      direction = DOWN;
      break;
    case SDL_SCANCODE_A:
    case SDL_SCANCODE_LEFT:
      direction = LEFT;
      break;
    case SDL_SCANCODE_D:
    case SDL_SCANCODE_RIGHT:
      direction = RIGHT;
      break;
    }
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  if(snakeSize > 0) {
    if(direction == UP) snake[0].y--;
    if(direction == DOWN) snake[0].y++;
    if(direction == LEFT) snake[0].x--;
    if(direction == RIGHT) snake[0].x++;
    if(snake[0].x >= (WINDOW_WIDTH / TILE_SIZE)) snake[0].x = 0;
    if(snake[0].y >= (WINDOW_HEIGHT / TILE_SIZE)) snake[0].y = 0;
    if(snake[0].x < 0) snake[0].x = (WINDOW_WIDTH / TILE_SIZE) - 1;
    if(snake[0].y < 0) snake[0].y = (WINDOW_HEIGHT / TILE_SIZE) - 1;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    drawRect.x = snake[0].x * TILE_SIZE;
    drawRect.y = snake[0].y * TILE_SIZE;
    SDL_RenderFillRect(renderer, &drawRect);
  }

  SDL_RenderPresent(renderer);

  SDL_Delay(90);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  free(snake);
}
