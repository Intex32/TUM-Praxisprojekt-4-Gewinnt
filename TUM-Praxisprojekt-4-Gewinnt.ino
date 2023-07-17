#include <Adafruit_NeoPixel.h>

#define PIN 8 // led matrix pin
#define PIXEL_COUNT 64

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIN, NEO_GRB + NEO_KHZ800);

#define VRX_PIN A0
#define VRY_PIN A1
int joystickThreshold = 375;
int joystickButtonPin = 2;


int board[PIXEL_COUNT];

// player 1 (index 0, red); player 2 (index 1, blue)
int r[2] = { 255, 0 };
int g[2] = { 0, 0 };
int b[2] = { 0, 255 };

boolean demoMode = true;
int playerWhoStarts = 1;

void setup() {
  pixels.begin();
  pixels.setBrightness(128);
  pixels.show();

  Serial.begin(9600);
  randomSeed(7);
  pinMode(joystickButtonPin, INPUT_PULLUP);
  checkSwapGameMode();
}

void loop() {
  if (demoMode) {
    runDemo();
  } else {
    multiplayerGame();
  }
  delay(100);
  checkSwapGameMode();
}


void setPixelColor(int pos, long rgb) {
  int row0 = pos / 8;
  int col0 = pos % 8;
  int row = 7 - col0;
  int indexInRow = (8 - 1) - row0;
  if (row % 2 == 1)
    indexInRow = (8 - 1) - indexInRow;
  pixels.setPixelColor(row * 8 + indexInRow, rgb);
}

void checkSwapGameMode() {
  bool pressed = digitalRead(joystickButtonPin) == LOW;
  if (pressed) {
    for (int i = 0; i < PIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(32, 32, 32));
    }
    pixels.show();
    delay(200);
    clearBoard();

    demoMode = !demoMode;
    Serial.print("Demo mode: ");
    Serial.println(demoMode);
  }
}

void showPixel(int player, int location) {
  setPixelColor(location, pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
  pixels.show();
}

void movePixel(int player, int currentLoc, int nextLoc) {
  setPixelColor(nextLoc, pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
  setPixelColor(currentLoc, pixels.Color(0, 0, 0));
  pixels.show();
}


int checkInput() {
  int xValue = analogRead(VRX_PIN) - 512;
  int yValue = analogRead(VRY_PIN) - 512;
  bool pressed = digitalRead(joystickButtonPin) == LOW;

  bool right = xValue <= -joystickThreshold;
  bool left = xValue >= joystickThreshold;
  bool down = yValue >= joystickThreshold;

  if (left) {
    return 1;
  } else if (down) {
    return 2;
  } else if (right) {
    return 3;
  } else return 0;
}

int dropChip(int currentLoc, int player) {
  boolean notOccupied = true;
  int checkLoc = currentLoc + 8;
  int prevLoc;

  setPixelColor(currentLoc, pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
  pixels.show();

  while ((checkLoc < 64) && (notOccupied)) {
    if (board[checkLoc] == 0) {
      prevLoc = currentLoc;
      currentLoc = checkLoc;
      movePixel(player, prevLoc, currentLoc);
      checkLoc = checkLoc + 8;
    } else {
      notOccupied = false;
    }
    delay(25);
  }

  board[currentLoc] = player;

  return currentLoc;
}

void highlightWinner(int connect4[]) {
  int player = board[connect4[0]];

  // light up four times
  for (int i = 0; i <= 4; i++) {
    for (int j = 0; j <= 3; j++) {
      setPixelColor(connect4[j], pixels.Color(0, 0, 0));
    }
    pixels.show();
    delay(200);
    for (int j = 0; j <= 3; j++) {
      setPixelColor(connect4[j], pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
    }
    pixels.show();
    delay(200);
  }
}

void clearBoard() {
  for (int i = 0; i < PIXEL_COUNT; i++) {
    board[i] = 0;
    setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

void showMeltingAnimation() {
  int color;

  for (int k = 0; k <= 7; k++) {
    for (int i = 55; i >= 8 * k; i--) {
      color = board[i];

      if (color == 0) {
        setPixelColor(i + 8, pixels.Color(0, 0, 0));
        board[i + 8] = 0;
      } else {
        setPixelColor(i + 8, pixels.Color(r[color - 1], g[color - 1], b[color - 1]));
        board[i + 8] = color;
      }
    }

    pixels.show();
    delay(80);

    for (int i = (7 + (8 * k)); i >= (8 * k); i--) {
      setPixelColor(i, pixels.Color(0, 0, 0));
      board[i] = 0;
    }

    pixels.show();
  }

  delay(500);
}


void multiplayerGame() {
  int whoseTurn = playerWhoStarts;

  while (!endGame(whoseTurn)) {
    if (whoseTurn == 1) {
      whoseTurn = 2;
    } else {
      whoseTurn = 1;
    }
  }

  showMeltingAnimation();

  // looser starts next game
  if (whoseTurn == 1) {
    playerWhoStarts = 2;
  } else {
    playerWhoStarts = 1;
  }
}

int findStartingSlot(int player) {
  int i = 0;
  int pixelLoc;
  int prevLoc;
  boolean foundStartLoc = false;

  if (player == 1) {
    int i = 0;

    while ((i < 8) && (!foundStartLoc)) {
      if (board[i] == 0) {
        pixelLoc = i;
        showPixel(player, pixelLoc);
        foundStartLoc = true;
      } else {
        i++;
      }
    }
  } else {
    int i = 7;
    while ((i >= 0) && (!foundStartLoc)) {
      if (board[i] == 0) {
        pixelLoc = i;
        showPixel(player, pixelLoc);
        foundStartLoc = true;
      } else {
        i--;
      }
    }
  }

  if (foundStartLoc) {
    return pixelLoc;
  } else {
    return 9; // no free spot left
  }
}

boolean endGame(int player) {
  int prevLoc;
  int pixelLoc = findStartingSlot(player);

  if (pixelLoc == 9) {
    Serial.println("No more place to play. Game Over");
    return true;
  }

  int WhichButton = checkInput();

  while (WhichButton != 2) {
    boolean okToMove = false;

    if (WhichButton == 1) {
      int chkLoc = pixelLoc - 1;

      while ((chkLoc >= 0) && (!okToMove)) {
        if (board[chkLoc] == 0) {
          prevLoc = pixelLoc;
          pixelLoc = chkLoc;
          okToMove = true;
        } else {
          chkLoc = chkLoc - 1;
        }
      }

      movePixel(player, prevLoc, pixelLoc);
      delay(200);
    } else if (WhichButton == 3) {
      int chkLoc = pixelLoc + 1;

      while ((chkLoc <= 7) && (!okToMove)) {
        if (board[chkLoc] == 0) {
          prevLoc = pixelLoc;
          pixelLoc = chkLoc;
          okToMove = true;
        } else {
          chkLoc = chkLoc + 1;
        }
      }

      movePixel(player, prevLoc, pixelLoc);
      delay(200);
    }

    delay(1);
    WhichButton = checkInput();
  }

  if (WhichButton == 2) {
    int playedLoc = dropChip(pixelLoc, player);

    if (isWinningMove(player, playedLoc)) {
      return true;
    } else {
      return false;
    }
  }
}


void runDemo() {
  randomPlay(50);
  delay(1000);
  showMeltingAnimation();
  delay(500);
}

void randomPlay(int turns) {
  boolean gameOver = false;

  int player = 2;
  int i = 1;

  while ((i < turns) && (!gameOver)) {
    if (player == 1) {
      player = 2;
    } else {
      player = 1;
    }

    // pick column that is not full
    int chooseColumn = randomColumn();
    while (board[chooseColumn] > 0) {
      chooseColumn = randomColumn();
    }

    int playedLoc = dropChip(chooseColumn, player);
    delay(75);

    if (isWinningMove(player, playedLoc)) {
      gameOver = true;
    }
    i++;

    delay(100);
  }
}

int randomColumn() {
  int randomNum;
  randomNum = random(0, 8);
  return randomNum;
}


boolean isWinningMove(int player, int playedLoc) {
  int connected[4];
  if (checkWinVertical(player, playedLoc)) {
    return true;
  } else if (checkWinHorizontal(player, playedLoc)) {
    return true;
  } else if (checkWinDiagonal(player, playedLoc)) {
    return true;
  } else if (checkWinDiagonal2(player, playedLoc)) {
    return true;
  } else {
    return false;
  }
}

boolean checkWinVertical(int player, int playedLoc) {
  int connected[4];

  if ((playedLoc < 40) && (board[playedLoc + 8] == player) && (board[playedLoc + 16] == player) && (board[playedLoc + 24] == player)) {
    connected[0] = playedLoc;
    connected[1] = playedLoc + 8;
    connected[2] = playedLoc + 16;
    connected[3] = playedLoc + 24;
    highlightWinner(connected);
    return true;
  } else {
    return false;
  }
}

boolean checkWinDiagonal2(int player, int playedLoc) {
  int connected[4] = { 0, 0, 0, 0 };
  int connectedIndex = 0;
  connected[connectedIndex] = playedLoc;

  boolean keepChecking = true;
  int chkLoc;

  if ((playedLoc) % 8 == 0) {
    keepChecking = false;
  } else {
    chkLoc = playedLoc - 9;  //56
  }

  while (keepChecking) {
    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;

      if (connectedIndex >= 3) {
        highlightWinner(connected);
        return true;
      }

      if ((chkLoc) % 8 == 0) {
        keepChecking = false;
      } else {
        chkLoc = chkLoc - 9;
      }

    } else {
      keepChecking = false;
    }
  }

  keepChecking = true;

  if ((playedLoc + 1) % 8 == 0) {
    keepChecking = false;
  } else {
    chkLoc = playedLoc + 9;
  }

  while ((keepChecking) && (chkLoc <= 63)) {
    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;

      if (connectedIndex >= 3) {
        highlightWinner(connected);
        return true;
      }

      if ((chkLoc + 1) % 8 == 0) {
        keepChecking = false;
      } else {
        chkLoc = chkLoc + 9;
      }
    } else {
      keepChecking = false;
    }
  }

  return false;
}

boolean checkWinDiagonal(int player, int playedLoc) {
  int connected[4] = { 0, 0, 0, 0 };
  int connectedIndex = 0;
  connected[connectedIndex] = playedLoc;

  boolean keepChecking = true;
  int chkLoc;

  if ((playedLoc + 1) % 8 == 0) {
    keepChecking = false;
  } else {
    chkLoc = playedLoc - 7;
  }

  while (keepChecking) {
    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;

      if (connectedIndex >= 3) {
        highlightWinner(connected);
        return true;
      }

      if ((chkLoc + 1) % 8 == 0) {
        keepChecking = false;
      } else {
        chkLoc = chkLoc - 7;
      }
    } else {
      keepChecking = false;
    }
  }

  keepChecking = true;

  if ((playedLoc) % 8 == 0) {
    keepChecking = false;
  } else {
    chkLoc = playedLoc + 7;
  }

  while ((keepChecking) && (chkLoc <= 63)) {
    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;

      if (connectedIndex >= 3) {
        highlightWinner(connected);
        return true;
      }

      if ((chkLoc) % 8 == 0) {
        keepChecking = false;
      } else {
        chkLoc = chkLoc + 7;
      }
    } else {
      keepChecking = false;
    }
  }

  return false;
}

boolean checkWinHorizontal(int player, int playedLoc) {
  int connected[4];
  int connectedIndex = 0;
  connected[connectedIndex] = playedLoc;

  int leftWall = int(playedLoc / 8) * 8;

  int chkLoc = playedLoc - 1;
  boolean keepChecking = true;

  while ((leftWall <= chkLoc) && (keepChecking)) {
    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;

      if (connectedIndex >= 3) {
        highlightWinner(connected);
        return true;
      }
    } else {
      keepChecking = false;
    }
    chkLoc--;
  }

  int rightWall = leftWall + 7;

  chkLoc = playedLoc + 1;
  keepChecking = true;

  while ((rightWall >= chkLoc) && (keepChecking)) {
    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;

      if (connectedIndex >= 3) {
        highlightWinner(connected);
        return true;
      }
    } else {
      keepChecking = false;
    }
    chkLoc++;
  }

  return false;
}
