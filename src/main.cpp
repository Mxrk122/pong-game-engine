#include <SDL_keyboard.h>
#include <iostream>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <string>
#include <SDL.h>
#include <vector>
#include <random>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_FPS = 60;
const int BALL_SPEED = 200;
const int BALL_SIZE = 100;

const int PLAYER_SIZE = 150;
const int PLAYER_SPEED = 350;

const int ENEMY_SIZE_X = 100;
const int ENEMY_SIZE_Y = 50;

struct Rect {
  SDL_Rect rect = {0, 0, 100, 100};
  int vx = 0;
  int vy = 0;
  SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};
};

std::vector<Rect> cubes;

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};


Rect player = {{(SCREEN_WIDTH/2) - PLAYER_SIZE, SCREEN_HEIGHT - 10, PLAYER_SIZE, 10}, 0, 0, red};
Rect ball = {{SCREEN_WIDTH - BALL_SIZE, 0, 20, 20}, -BALL_SPEED, BALL_SPEED, blue};


void renderRect(SDL_Renderer* renderer, Rect& ball) {
  SDL_SetRenderDrawColor(renderer, ball.color.r, ball.color.g, ball.color.b, ball.color.a);
  SDL_RenderFillRect(renderer, &ball.rect);
}

bool checkColission(const SDL_Rect& a, const SDL_Rect& b) {
  return (
    a.x < b.x + b.w &&
    a.x + a.w > b.x &&
    a.y < b.y + b.h &&
    a.y + a.h > b.y 
  );
}

bool checkWin(const std::vector<Rect>& array) {
    return array.empty();
}

void handleInput(SDL_Event& e) {
  // resolve
  const Uint8* ks = SDL_GetKeyboardState(NULL);

  player.vx = 0;
  player.vy = 0;

  if (ks[SDL_SCANCODE_A]) {
    player.vx = -PLAYER_SPEED;
  }
  // if (ks[SDL_SCANCODE_W]) {
  //   player.vy = -BALL_SPEED;
  // }
  // if (ks[SDL_SCANCODE_S]) {
  //   player.vy = BALL_SPEED;
  // }
  if (ks[SDL_SCANCODE_D]) {
    player.vx = PLAYER_SPEED;
  }
}

void update(float dT) {

  if(checkWin(cubes)){
    std::cout << "WIN!" << std::endl;
    exit(0);
  }
  
  if (ball.rect.x < 0) {
    ball.vx *= -1;
  }
  // if (player.rect.y < 0) {
  //   player.vy *= -1;
  // }
  if (ball.rect.y < 0) {
    ball.vy *= -1;
  }
  if (player.rect.x + player.rect.w > SCREEN_WIDTH) {
    player.vx *= 0;
  }
  if (ball.rect.x + ball.rect.w > SCREEN_WIDTH) {
    ball.vx *= -1;
  }
  // if (player.rect.y + player.rect.h > SCREEN_HEIGHT) {
  //   player.vy *= -1;
  // }
  if (ball.rect.y + ball.rect.h > SCREEN_HEIGHT) {
    std::cout << "Game Over!" << std::endl;
    exit(0);
  }

  if (checkColission(player.rect, ball.rect)) {
    ball.vy *= -1;
  }

  for (size_t i = 0; i < cubes.size(); ) {
    if (checkColission(ball.rect, cubes[i].rect)) {
        cubes.erase(cubes.begin() + i);
    } else {
        ++i;
    }
}

  // posible position
  int newx = player.rect.x + player.vx * dT;
  if (newx > 0 && newx < SCREEN_WIDTH - PLAYER_SIZE){
    player.rect.x = newx;
  }

  ball.rect.x += ball.vx * dT;
  ball.rect.y += ball.vy * dT;

}

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window* window = SDL_CreateWindow("", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  std::random_device rd;
  std::mt19937 gen(rd());

  // Llenar el vector con objetos enemy
  for (int i = 0; i < 5; ++i) {
    // Definir un distribuidor para números enteros en el rango [1, 100]
    std::uniform_int_distribution<> disInt(1, 250);
    SDL_Color color = {disInt(gen), disInt(gen), disInt(gen), disInt(gen)};

    Rect enemy = {{(ENEMY_SIZE_X * 1.2 * i), 0, ENEMY_SIZE_X, ENEMY_SIZE_Y}, 0, 0, color};
    cubes.push_back(enemy);
  }

  bool quit = false;
  SDL_Event e;

  Uint32 frameCount = 1;
  Uint32 frameStartTimestamp;
  Uint32 frameEndTimestamp;
  Uint32 lastFrameTime = SDL_GetTicks();
  Uint32 lastUpdateTime = 0;
  float frameDuration = (1.0/MAX_FPS) * 1000.0;
  float actualFrameDuration;
  int FPS = MAX_FPS;
  
  while (!quit) {
    frameStartTimestamp = SDL_GetTicks();

    // delta time
    Uint32 currentFrameTime = SDL_GetTicks();
    float dT = (currentFrameTime - lastFrameTime) / 1000.0; 
    lastFrameTime = currentFrameTime;

    // poll events
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      handleInput(e);
    }
    // update
    update(dT);
    // render
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    renderRect(renderer, player);
    renderRect(renderer, ball);

    for (size_t i = 0; i < cubes.size(); ++i) {
      renderRect(renderer, cubes[i]);
    }


    SDL_RenderPresent(renderer);
    
    frameEndTimestamp = SDL_GetTicks();
    actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

    if (actualFrameDuration < frameDuration) {
      SDL_Delay(frameDuration - actualFrameDuration);
    }

    // fps calculation
    frameCount++;
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - lastUpdateTime;
    if (elapsedTime > 1000) {
      FPS = (float)frameCount / (elapsedTime / 1000.0);
      lastUpdateTime = currentTime;
      frameCount = 0;
    }
    SDL_SetWindowTitle(window, ("FPS: " + std::to_string(FPS)).c_str());
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
