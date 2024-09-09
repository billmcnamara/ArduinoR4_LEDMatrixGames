#include <ezButton.h>
#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

// Define the pin numbers
#define VRX_PIN  A0  // Arduino pin connected to VRX pin
#define VRY_PIN  A1  // Arduino pin connected to VRY pin
#define SW_PIN   D2  // Arduino pin connected to SW pin (button)
ezButton button(SW_PIN);
int joyx = 0; // To store value of the X axis
int joyy = 0; // To store value of the Y axis
int joyb = 0; // To store value of the button

// Define matrix dimensions (adjust as needed)
const int MATRIX_HEIGHT = 8;
const int MATRIX_WIDTH = 12;

// Game state variables
bool inMenu = true;
int currentGame = 0;
bool menuDisplayed = true;

// Timing variables
unsigned long pressStartTime = 0;
unsigned long releaseTime = 0;

// Menu bitmaps
uint8_t pong[MATRIX_HEIGHT][MATRIX_WIDTH] = {
  { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }
};

uint8_t snake[MATRIX_HEIGHT][MATRIX_WIDTH] = {
  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 }
};

uint8_t flappy[MATRIX_HEIGHT][MATRIX_WIDTH] = {
  { 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
  { 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0 }
};

uint8_t sadface[MATRIX_HEIGHT][MATRIX_WIDTH] = {
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 }
};

uint8_t happyface[MATRIX_HEIGHT][MATRIX_WIDTH] = {
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 }
};

// Snake variables
byte frame[8][12];
byte flatSnakeFrame[8 * 12];  // Flattened frame for matrix.loadPixels()
struct snakePoint {
      byte x;
      byte y;
};
snakePoint snakePoints[100];
int snakeLength = 3;
snakePoint snakeFood;
int direction = 0;  // 0=up, 1=right, 2=down, 3=left

// Pong variables
int ballX = 6, ballY = 4;        // Ball position
int ballSpeedX = 1, ballSpeedY = 1;  // Ball movement direction
int leftPaddleY = 3;             // Y position of the left paddle
int rightPaddleY = 3;            // Y position of the right paddle
const int PADDLE_HEIGHT = 2;      // Paddle height
int playerScoreLeft = 0;
int playerScoreRight = 0;
const int HIGHESTSCORE = 10;    // highest score before reset to 0

// Flappy Bird variables
int birdY = 4;                   // Bird's vertical position
int birdVelocity = 0;            // Bird's velocity
const int gravity = 1;           // Gravity that pulls the bird down
const int jumpStrength = -2;      // Jump strength (negative value makes the bird go up)
int pipeX = MATRIX_WIDTH - 1;    // Pipe's horizontal position
int gapY = 3;                    // Vertical position of the gap in the pipe
const int gapHeight = 3;         // Height of the gap in the pipe
bool gameOver = false;           // To track if the game is over
int flappyscore = 0;


void setup() {
  Serial.begin(9600);
  Serial.println("Setup started");
  pinMode(VRX_PIN, INPUT);  // joystick X-axis
  pinMode(VRY_PIN, INPUT);  // joystick Y-axis
  button.setDebounceTime(50); // Set debounce time to 50 milliseconds
  
  // Initialize LED matrix
  matrix.clear();
  matrix.begin();
  Serial.println("Setup done. Now entering loop.");
}

void loop() {
  button.loop();  // MUST call button.loop() to handle debounce

  // Read the button value (pressed or not)
  joyb = button.getState();

  // Check if we are in the menu or in a game
  if (inMenu) {
    displayMenu();
  } else {
    // Game is running, handle game logic
    switch (currentGame) {
      case 0:
        if (menuDisplayed) {
          pongReset();  // Initialize the bats only once
          menuDisplayed = false;  // Prevent re-initialization on every loop
        }
        runPong();
        break;
      case 1:
        if (menuDisplayed) {
          snakeReset();  // Initialize the snake only once
          menuDisplayed = false;  // Prevent re-initialization on every loop
        }
        runSnake();
        break;
      case 2:
        if (menuDisplayed) {
          flappyReset();  // Initialize the bird only once
          menuDisplayed = false;  // Prevent re-initialization on every loop
        }
        runFlappy();
        break;
    }
  }
}
void displayMenu() {
  static unsigned long lastTime = millis();
  static unsigned long lastButtonCheck = 0;
  const unsigned long buttonCheckInterval = 50;    // Check button every 50ms
  const unsigned long gameSwitchInterval = 2000;   // Switch game every 2 seconds

  if (!menuDisplayed) {
    return;
  }

  // Read the button state
  bool buttonIsPressed = (button.getState() == LOW); // LOW means button is pressed for active low logic
  unsigned long currentTime = millis();

  // Show the current game option
  switch (currentGame) {
    case 0:
      matrix.renderBitmap(pong, 8, 12);
      break;
    case 1:
      matrix.renderBitmap(snake, 8, 12);
      break;
    case 2:
      matrix.renderBitmap(flappy, 8, 12);
      break;
  }

  // Detect if button is pressed
  if (buttonIsPressed) {
    Serial.println("Button pressed in menu, launching game.");
    menuDisplayed = false;
    inMenu = false;  // Exit menu and start the game
    return;
  }

  // Check for button state changes
  if (currentTime - lastButtonCheck >= buttonCheckInterval) {
    lastButtonCheck = currentTime;

    // Change to the next game option every 2 seconds
    if (currentTime - lastTime >= gameSwitchInterval) {
      lastTime = currentTime;
      currentGame = (currentGame + 1) % 3;
    }
  }
}

/*****************************************************
 *
 *
 * SNAKE
 *
 */

void runSnake() {
  if (exitConditionMet()) {
    inMenu = true;
    menuDisplayed = true;
    Serial.println("Exiting Snake. Returning to menu.");
  } else {
    //Serial.println("Snake Game Running");
    // Check joystick axes and move snake
    joyx = analogRead(VRX_PIN);
    joyy = analogRead(VRY_PIN);
    if (joyx > 600 && direction != 3) direction = 1;
    else if (joyx < 400 && direction != 1) direction = 3;
    else if (joyy > 600 && direction != 0) direction = 2;
    else if (joyy < 400 && direction != 2) direction = 0;

    snakeMove();

    // Check for collision with food
    if (snakePoints[0].x == snakeFood.x && snakePoints[0].y == snakeFood.y) {
      snakePoints[snakeLength] = snakePoints[snakeLength - 1];  // Initialize the new segment
      snakeLength++;
      snakeGenerateFood();
    }

    // Check for collision with self
    for (int i = 1; i < snakeLength; i++) {
      if (snakePoints[0].x == snakePoints[i].x && snakePoints[0].y == snakePoints[i].y) {
        showFace(snakeLength);  // Call face when collision happens
        snakeReset();  // Call reset when collision happens
      }
    }

    snakeDrawFrame();
    delay(200);  // Control speed
  }
}

void snakeReset() {
  snakeLength = 3;
  snakePoints[0] = { 6, 4 };
  snakePoints[1] = { 6, 5 };
  snakePoints[2] = { 6, 6 };
  direction = 0;
  snakeGenerateFood();
}

void snakeMove() {
  for (int i = snakeLength - 1; i > 0; i--) {
    snakePoints[i] = snakePoints[i - 1];
  }
  // Move the head of the snake based on the direction
  switch (direction) {
    case 0:
      snakePoints[0].y = (snakePoints[0].y - 1 + MATRIX_HEIGHT) % MATRIX_HEIGHT;  // Wrap around at the top and bottom edges
      break;
    case 1:
      snakePoints[0].x = (snakePoints[0].x + 1) % MATRIX_WIDTH;  // Wrap around at the right and left edges
      break;
    case 2:
      snakePoints[0].y = (snakePoints[0].y + 1) % MATRIX_HEIGHT;  // Wrap around at the bottom and top edges
      break;
    case 3:
      snakePoints[0].x = (snakePoints[0].x - 1 + MATRIX_WIDTH) % MATRIX_WIDTH;  // Wrap around at the left and right edges
      break;
  }
}

void snakeGenerateFood() {
  snakePoint possibleLocations[MATRIX_HEIGHT * MATRIX_WIDTH];
  int idx = 0;

  // Generate all possible locations for the food
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      bool overlap = false;

      // Check for overlap with the snake
      for (int i = 0; i < snakeLength; i++) {
        if (snakePoints[i].x == x && snakePoints[i].y == y) {
          overlap = true;
          break;
        }
      }

      if (!overlap) {
        possibleLocations[idx++] = { x, y };
      }
    }
  }

  // Randomly choose a location for the food from the possible locations
  int choice = random(0, idx);
  snakeFood = possibleLocations[choice];
}

void snakeDrawFrame() {
  // Clear frame
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      frame[y][x] = 0;
    }
  }

  // Draw snake
  for (int i = 0; i < snakeLength; i++) {
    frame[snakePoints[i].y][snakePoints[i].x] = 1;
  }

  // Draw food
  frame[snakeFood.y][snakeFood.x] = 1;

  // Flatten frame array and load into LED matrix
  int idx = 0;
  for (int height = 0; height < MATRIX_HEIGHT; height++) {
    for (int width = 0; width < MATRIX_WIDTH; width++) {
      flatSnakeFrame[idx++] = frame[height][width];
    }
  }
  matrix.loadPixels(flatSnakeFrame, MATRIX_HEIGHT * MATRIX_WIDTH);
  matrix.renderFrame(0);
}

/*****************************************************
 *
 *
 * PONG
 *
 */

void runPong() {
  if (exitConditionMet()) {
    inMenu = true;
    menuDisplayed = true;
    Serial.println("Exiting Pong. Returning to menu.");
  } else {
    //Serial.println("Pong Game Running");

    // Move paddles based on joystick input
    joyy = analogRead(VRY_PIN);  // Assuming Y-axis controls paddles

    if (joyy > 600 && leftPaddleY < MATRIX_HEIGHT - PADDLE_HEIGHT) leftPaddleY++;   // Move paddle down
    else if (joyy < 400 && leftPaddleY > 0) leftPaddleY--;  // Move paddle up

    // Simulate right paddle AI (follows the ball)
    if (ballY > rightPaddleY + PADDLE_HEIGHT / 2 && rightPaddleY < MATRIX_HEIGHT - PADDLE_HEIGHT) rightPaddleY++;
    else if (ballY < rightPaddleY + PADDLE_HEIGHT / 2 && rightPaddleY > 0) rightPaddleY--;

    // Move the ball
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // Check for collisions with the walls
    if (ballY == 0 || ballY == MATRIX_HEIGHT - 1) {
      ballSpeedY = -ballSpeedY;  // Invert ball vertical direction
    }

    // Check for collisions with the paddles
    if (ballX == 1 && ballY >= leftPaddleY && ballY < leftPaddleY + PADDLE_HEIGHT) {
      ballSpeedX = -ballSpeedX;  // Bounce off left paddle
    } else if (ballX == MATRIX_WIDTH - 2 && ballY >= rightPaddleY && ballY < rightPaddleY + PADDLE_HEIGHT) {
      ballSpeedX = -ballSpeedX;  // Bounce off right paddle
    }

    // Check if the ball goes out of bounds (score)
    if (ballX <= 0) {
      playerScoreRight++;
      if ( playerScoreRight >= HIGHESTSCORE ) {
        // playerRight wins
        showFace(playerScoreLeft);
        playerScoreLeft = 0;
        playerScoreRight = 0;
      }
      pongReset();  // Reset ball and paddles
    } else if (ballX >= MATRIX_WIDTH - 1) {
      playerScoreLeft++;
      if ( playerScoreLeft >= HIGHESTSCORE ) {
        // playerLeft wins
        showFace(playerScoreLeft);
        playerScoreLeft = 0;
        playerScoreRight = 0;
      }
      pongReset();  // Reset ball and paddles
    }

    // Draw the game frame on the LED matrix
    pongDrawFrame();

    // Delay to control speed
    delay(100);
  }
}

void pongDrawFrame() {
  // Clear the frame
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      frame[y][x] = 0;
    }
  }

  // Draw left paddle
  for (int i = 0; i < PADDLE_HEIGHT; i++) {
    frame[leftPaddleY + i][0] = 1;
  }

  // Draw right paddle
  for (int i = 0; i < PADDLE_HEIGHT; i++) {
    frame[rightPaddleY + i][MATRIX_WIDTH - 1] = 1;
  }

  // Draw ball
  frame[ballY][ballX] = 1;

  // Flatten and load the frame into the LED matrix
  int idx = 0;
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      flatSnakeFrame[idx++] = frame[y][x];  // Using flatSnakeFrame for simplicity
    }
  }
  matrix.loadPixels(flatSnakeFrame, MATRIX_HEIGHT * MATRIX_WIDTH);
  matrix.renderFrame(0);
}

void pongReset() {
  // Reset ball and paddle positions
  ballX = MATRIX_WIDTH / 2;
  ballY = MATRIX_HEIGHT / 2;
  ballSpeedX = -ballSpeedX;  // Reverse ball direction
  ballSpeedY = 1;
  leftPaddleY = 3;
  rightPaddleY = 3;
}


/*****************************************************
 *
 *
 * FLAPPY BIRD
 *
 */

void runFlappy() {
  if (exitConditionMet()) {
    inMenu = true;
    menuDisplayed = true;
    Serial.println("Exiting Flappy Bird. Returning to menu.");
  } else if (!gameOver) {
    Serial.println("Flappy Bird Game Running");

    // Read joystick button
    joyb = button.getState();

    // Bird jumps if the button is pressed
    if (joyb == LOW) {
      birdVelocity = jumpStrength;  // Apply upward force
    }

    // Apply gravity to bird's velocity
    birdVelocity += gravity;
    birdY += birdVelocity;

    // Keep bird within matrix bounds
    if (birdY < 0) birdY = 0;
    if (birdY >= MATRIX_HEIGHT) birdY = MATRIX_HEIGHT - 1;

    // Move the pipe to the left
    pipeX--;

    // If pipe goes off the screen, reset it and generate a new gap
    if (pipeX < 0) {
      pipeX = MATRIX_WIDTH - 1;
      gapY = random(1, MATRIX_HEIGHT - gapHeight - 1);

      // Increase score when the pipe is passed
      flappyscore++;
      Serial.print("Score: ");
      Serial.println(flappyscore);
    }

    // Check for collisions with the pipe (bird hitting the pipe or not going through the gap)
    if (pipeX == 1) {
      if (birdY < gapY || birdY > gapY + gapHeight - 1) {
        gameOver = true;
        Serial.println("Game Over! Flappy Bird hit the pipe.");
        Serial.print("Final Score: ");
        Serial.println(flappyscore);
      }
    }

    // Draw the game frame on the LED matrix
    flappyDrawFrame();

    // Delay to control speed
    delay(150);
  } else {
    // If game over, reset on button press
    if (joyb == LOW) {
      flappyReset();
    }
  }
}
void flappyDrawFrame() {
  // Clear the frame
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      frame[y][x] = 0;
    }
  }

  // Draw bird
  frame[birdY][1] = 1;  // Bird is always at x = 1 (left side of the screen)

  // Draw pipe
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    if (y < gapY || y >= gapY + gapHeight) {
      frame[y][pipeX] = 1;  // Pipe is drawn, except for the gap
    }
  }

  // Flatten and load the frame into the LED matrix
  int idx = 0;
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      flatSnakeFrame[idx++] = frame[y][x];  // Using flatSnakeFrame for simplicity
    }
  }
  matrix.loadPixels(flatSnakeFrame, MATRIX_HEIGHT * MATRIX_WIDTH);
  matrix.renderFrame(0);
}

void flappyReset() {
  // Reset the bird, pipe, and game state
  birdY = 4;
  birdVelocity = 0;
  pipeX = MATRIX_WIDTH - 1;
  gapY = 3;
  gameOver = false;
  Serial.println("Flappy Bird Game Reset.");
}

/**********************
 * end
 **********************/
void showFace(int score) {
  if ( score >= 10 ) {
    matrix.renderBitmap(happyface, 8, 12);
  } else {
    matrix.renderBitmap(sadface, 8, 12);
  }
  Serial.println("Game Over");
  Serial.print("Score:");
  Serial.println(score);
  delay(2000);  // Small delay to ensure proper transition
}

bool exitConditionMet() {
  // Track how long the button is held
  static unsigned long pressStartTime = 0;
  static bool buttonWasPressed = false; // Track the previous state of the button

  // Read the current button state
  bool buttonIsPressed = (button.getState() == LOW); // LOW means button is pressed for active low logic

  if (buttonIsPressed) {
    if (!buttonWasPressed) {
      // Button was just pressed
      pressStartTime = millis();  // Record the time when the button press started
      Serial.println("Button pressed, starting timer.");
    }
    buttonWasPressed = true; // Update the button state
  } else {
    if (buttonWasPressed) {
      // Button was just released
      unsigned long elapsedTime = millis() - pressStartTime;
      if (elapsedTime >= 2000) {
        // Button was held for 2 seconds
        Serial.println("Button held for > 2s, exiting.");
        buttonWasPressed = false; // Reset button state for next press
        return true;
      }
      Serial.println("Button released, resetting timer.");
    }
    buttonWasPressed = false; // Update the button state
  }

  return false;  // Button not held long enough
}

