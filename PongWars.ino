/** 
 * PongWars
 * ========
 * orig idea: https://github.com/vnglst/pong-wars
**/

#include <Arduino.h>
#include <M5StickCPlus2.h>
#include <math.h>

/* Definitions */

uint16_t MYWHITE=0x0001;
uint16_t MYBLACK=0xFFFF;
const uint16_t DAY_COLOR = MYWHITE;
const uint16_t DAY_BALL_COLOR = MYBLACK;
const uint16_t NIGHT_COLOR = MYBLACK;
const uint16_t NIGHT_BALL_COLOR = MYWHITE;
const int SQUARE_SIZE = 10;
const int HALF_SQUARE_SIZE = 5;
const int MIN_SPEED = 5;
const int MAX_SPEED = 10;
const int BALL_WIDTH = 2;
const int BALL_HEIGHT = 2;

int dayScore = 0;
int nightScore = 0;

const int32_t canvas_width = 135;
const int32_t canvas_height = 240;
const float numSquaresX = canvas_width / SQUARE_SIZE;
const float numSquaresY = canvas_height / SQUARE_SIZE;
const int nsx = (int)numSquaresX;
const int nsy = (int)numSquaresY;
int squares[nsx][nsy];

struct BALL {
  int x;
  int y;
  int dx;
  int dy;
  uint16_t reverseColor;
  uint16_t ballColor;
};

BALL balls[] = {
  {canvas_width / 4 , canvas_height / 2, 4, -4, NIGHT_COLOR, 0x03E0},
  {(canvas_width / 4) * 3, canvas_height / 2, -4, 4, DAY_COLOR, 0x867D},
};

int iteration = 0;

/* Helper Methods */

void intro()
{
  StickCP2.Display.fillScreen(WHITE);
  delay(500);
  StickCP2.Display.fillScreen(RED);
  delay(500);
  StickCP2.Display.fillScreen(GREEN);
  delay(500);
  StickCP2.Display.fillScreen(BLUE);
  delay(500);
  StickCP2.Display.fillScreen(BLACK);
  delay(500);
}

void drawBall(BALL *ball, LovyanGFX* gfx)
{
  StickCP2.Display.fillCircle(ball->x, ball->y, 2, ball->ballColor);
}

void drawSquares(LovyanGFX* gfx)
{
  dayScore = 0;
  nightScore = 0;

  for (int i = 0; i < numSquaresX; i++) 
  {
    for (int j = 0; j < numSquaresY; j++) 
    {

      gfx->fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, squares[i][j]);

      // Update scores
      if (squares[i][j] == DAY_COLOR) dayScore++;
      if (squares[i][j] == NIGHT_COLOR) nightScore++;
    }
  }
}


void checkBoundaryCollision(BALL *ball) 
{
  if (ball->x + ball->dx > canvas_width - SQUARE_SIZE / 2 || ball->x + ball->dx < SQUARE_SIZE / 2) 
  {
    ball->dx = -ball->dx;
  }

  if (ball->y + ball->dy > canvas_height - SQUARE_SIZE / 2 || ball->y + ball->dy < SQUARE_SIZE / 2) 
  {
    ball->dy = -ball->dy;
  }
}

void addRandomness(BALL *ball) 
{
  ball->dx += random(MIN_SPEED, MAX_SPEED) * 0.01 - 0.005;
  ball->dy += random(MIN_SPEED, MAX_SPEED) * 0.01 - 0.005;

  // Make sure the ball always maintains a minimum speed
  if (abs(ball->dx) < MIN_SPEED)
  {
    ball->dx = ball->dx > 0 ? MIN_SPEED : -MIN_SPEED;
  }

  if (abs(ball->dy) < MIN_SPEED)
  {
    ball->dy = ball->dy > 0 ? MIN_SPEED : -MIN_SPEED;
  }
}


void draw(LovyanGFX* gfx)
{
  gfx->clear(); // with this the white area flickers
  //StickCP2.Display.fillScreen(WHITE); // with this the black area flickers

  drawSquares(&StickCP2.Display);

  StickCP2.Display.setCursor(90, 100);
  StickCP2.Display.printf("%i", dayScore);
  StickCP2.Display.setCursor(20, 100);
  StickCP2.Display.printf("%i", nightScore);

  for(int i = 0; i < 2; i++)
  {
    drawBall(&balls[i], &StickCP2.Display);
    checkSquareCollision(&balls[i]);
    checkBoundaryCollision(&balls[i]);
    balls[i].x += balls[i].dx;
    balls[i].y += balls[i].dy;
    addRandomness(&balls[i]);
  }

  iteration++;
}

void checkSquareCollision(BALL *ball) 
{
  // Check multiple points around the ball's circumference
  for (int angle = 0; angle < PI * 2; angle += PI / 2)
  {
    float checkX = ball->x + cos(angle) * HALF_SQUARE_SIZE;
    float checkY = ball->y + sin(angle) * HALF_SQUARE_SIZE;

    int i = floor(checkX / SQUARE_SIZE);
    int j = floor(checkY / SQUARE_SIZE);

    if (i >= 0 && i < nsx && j >= 0 && j < nsy) 
    {
      if (squares[i][j] != ball->reverseColor) 
      {
        // Square hit! Update square color
        squares[i][j] = ball->reverseColor;

        // Determine bounce direction based on the angle
        if (abs(cos(angle)) > abs(sin(angle))) 
        {
          ball->dx = -ball->dx;
        } else 
        {
          ball->dy = -ball->dy;
        }
      }
    }
  }
}

/* MAIN Methods */

void setup() 
{
  auto cfg = M5.config();
  StickCP2.begin(cfg);
  StickCP2.Display.setRotation(2);
  StickCP2.Display.setTextColor(RED);
  StickCP2.Display.setTextSize(1);

  int h = StickCP2.Display.width();
  int w = StickCP2.Display.height();
  const int32_t canvas_width = w; // not sure if correct?! refactor!
  const int32_t canvas_height = h;

  intro();

  StickCP2.Display.clear();

  // Populate the fields, one half day, one half night
  for (int i = 0; i < nsx; i++)
  {
    for (int j = 0; j < nsy; j++)
    {
      squares[i][j] = i < nsx / 2 ? NIGHT_COLOR : DAY_COLOR;
    }
  }
}

void loop() 
{
  draw(&StickCP2.Display);
  delay(100);
}
