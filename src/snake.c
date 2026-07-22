#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 640
#define PIXEL_SIZE 40
#define GRID_WIDTH (SCREEN_WIDTH / PIXEL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / PIXEL_SIZE)
#define BODY_MAX (GRID_WIDTH * GRID_HEIGHT)
#define SNAKE_SPEED 10

typedef struct Snake {
  size_t head;
  size_t tail;
  Vector2 *body;
  Vector2 direction;
  bool collisionGrid[GRID_WIDTH][GRID_HEIGHT];
} Snake;

Snake *newSnake() {
  Snake *snake = malloc(sizeof(Snake));
  snake->head = 0;
  snake->tail = 0;
  snake->body = malloc(sizeof(Vector2) * BODY_MAX);
  snake->body[snake->head] = (Vector2){0, 0};
  snake->direction = (Vector2){1, 0};
  memset(snake->collisionGrid, false, sizeof(snake->collisionGrid));
  snake->collisionGrid[0][0] = true;
  return snake;
}

void drawSnake(Snake *snake) {
  size_t current = snake->tail;
  
  while (true) {
    DrawRectangle(
      snake->body[current].x * PIXEL_SIZE,
      snake->body[current].y * PIXEL_SIZE,
      PIXEL_SIZE,
      PIXEL_SIZE,
      GREEN
    );
    
    if (current == snake->head) break;
    current = (current + 1) % BODY_MAX;
  }
}

Vector2 nextSnakePosition(Snake *snake) {
  return (Vector2){
    (int)(snake->body[snake->head].x + snake->direction.x + GRID_WIDTH) % GRID_WIDTH,
    (int)(snake->body[snake->head].y + snake->direction.y + GRID_HEIGHT) % GRID_HEIGHT
  };
}

bool willSnakeEat(Snake *snake, Vector2 foodPosition) {
  Vector2 nextPosition = nextSnakePosition(snake);
  return nextPosition.x == foodPosition.x && nextPosition.y == foodPosition.y;
}

bool willSnakeCollide(Snake *snake) {
  bool willCollide = false;
  Vector2 nextPosition = nextSnakePosition(snake);

  if (snake->collisionGrid[(int)nextPosition.x][(int)nextPosition.y]) {
    willCollide = true;
  }

  if (
    snake->body[snake->tail].x == nextPosition.x &&
    snake->body[snake->tail].y == nextPosition.y
  ) {
    willCollide = false;
  }

  return willCollide;
}

void growSnake(Snake *snake) {
  Vector2 nextPosition = nextSnakePosition(snake);
  size_t nextIndex = (snake->head + 1) % BODY_MAX;
  snake->body[nextIndex].x = nextPosition.x;
  snake->body[nextIndex].y = nextPosition.y;
  snake->head = nextIndex;
  snake->collisionGrid[(int)nextPosition.x][(int)nextPosition.y] = true;
}

void moveSnake(Snake *snake) {
  growSnake(snake);
  snake->collisionGrid[(int)snake->body[snake->tail].x][(int)snake->body[snake->tail].y] = false;
  snake->tail = (snake->tail + 1) % BODY_MAX;
}

void turnSnake(Snake *snake, Vector2 newDirection) {
  if (newDirection.x == -snake->direction.x && newDirection.y == -snake->direction.y) {
    return;
  }
  snake->direction = newDirection;
}

void killSnake(Snake *snake) {
  free(snake->body);
  free(snake);
}

bool isFoodOnSnake(Snake *snake, Vector2 foodPosition) {
  return snake->collisionGrid[(int)foodPosition.x][(int)foodPosition.y];
}

Vector2 newFood(Snake *snake) {
  Vector2 foodPosition = (Vector2){
    GetRandomValue(0, GRID_WIDTH - 1),
    GetRandomValue(0, GRID_HEIGHT - 1)
  };

  while (isFoodOnSnake(snake, foodPosition)) {
    foodPosition.x = GetRandomValue(0, GRID_WIDTH - 1);
    foodPosition.y = GetRandomValue(0, GRID_HEIGHT - 1);
  }

  return foodPosition;
}

void drawFood(Vector2 foodPosition) {
  DrawRectangle(
    foodPosition.x * PIXEL_SIZE,
    foodPosition.y * PIXEL_SIZE,
    PIXEL_SIZE,
    PIXEL_SIZE,
    RED
  );
}

int main(void)
{
  bool gameOver = true;
  Snake *snake = NULL;
  Vector2 currFood = {0, 0};
  float deltaAccumulator = 1.0f / SNAKE_SPEED;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground(RAYWHITE);

    if (gameOver) {
      DrawText(
        "Press SPACE to start",
        SCREEN_WIDTH / 2 - MeasureText("Press SPACE to start", 20) / 2,
        SCREEN_HEIGHT / 2 - 10, 20, BLACK
      );
      if (IsKeyPressed(KEY_SPACE)) {
        gameOver = false;
        snake = newSnake();
        currFood = newFood(snake);
        deltaAccumulator = 1.0f / SNAKE_SPEED;
      }
    } else {
      if (IsKeyPressed(KEY_W)) turnSnake(snake, (Vector2){0, -1});
      if (IsKeyPressed(KEY_S)) turnSnake(snake, (Vector2){0, 1});
      if (IsKeyPressed(KEY_A)) turnSnake(snake, (Vector2){-1, 0});
      if (IsKeyPressed(KEY_D)) turnSnake(snake, (Vector2){1, 0});

      deltaAccumulator += GetFrameTime();
      while (deltaAccumulator >= 1.0f / SNAKE_SPEED) {
        deltaAccumulator -= 1.0f / SNAKE_SPEED;

        if (willSnakeEat(snake, currFood)) {
          growSnake(snake);
          currFood = newFood(snake);
        } else if (willSnakeCollide(snake)) {
          killSnake(snake);
          snake = NULL;
          gameOver = true;
          break;
        } else {
          moveSnake(snake);
        }
      }

      if (snake) {
        drawSnake(snake);
        drawFood(currFood);
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}