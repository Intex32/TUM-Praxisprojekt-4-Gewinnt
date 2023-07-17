#include <Adafruit_NeoPixel.h>

#define PIN 8
#define NUMPIXELS 64

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define VRX_PIN A0 // Arduino pin connected to VRX pin
#define VRY_PIN A1 // Arduino pin connected to VRY pin
int joystickThreshold = 375;
int joystickButtonPin = 2;


int board[NUMPIXELS];

//RGB LED values for player 1 (index 0) and player 2 (index 1)
//Player 1 = RED, Player 2 = BLUE
int r[2] = { 255, 0 };
int g[2] = { 0, 0 };
int b[2] = { 0, 255 };

boolean demoMode = true;  //run in demo mode until reset

int whoGoesFirst = 1;  //player to go first.

void setup() {
  pixels.begin();
  pixels.setBrightness(128);
  pixels.show();

  Serial.begin(9600);  // open the serial port at 9600 bps:
  randomSeed(7);

  pinMode(joystickButtonPin, INPUT_PULLUP);

  //press any button during start up to enter demo mode
  checkSwapGameMode();
}

void checkSwapGameMode() {
  bool pressed = digitalRead(joystickButtonPin) == LOW;
  if (pressed) {
    for (int i = 0; i < NUMPIXELS; i++) {
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

void fillBoard() {  //for testing purpose only
  int player = 2;

  for (int i = 8; i < NUMPIXELS; i++) {

    if (player == 1) {
      player = 2;
    } else {
      player = 1;
    }

    board[i] = player;
    setPixelColor(i, pixels.Color(r[player - 1], g[player - 1], b[player - 1]));  // set color
  }
  pixels.show();
}

void loop() {
  if (demoMode) {  //run game demo
    runDemo();
  } else {            //play regular game
    twoPlayerGame();  //two player game
  }
  Serial.println("cjecl");
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

void showPixel(int player, int location) {
  setPixelColor(location, pixels.Color(r[player - 1], g[player - 1], b[player - 1]));  //on
  pixels.show();                                                                       // This sends the updated pixel color to the hardware.
}

void movePixel(int player, int currentLoc, int nextLoc) {
  setPixelColor(nextLoc, pixels.Color(r[player - 1], g[player - 1], b[player - 1]));  //on
  setPixelColor(currentLoc, pixels.Color(0, 0, 0));                                   // off
  pixels.show();                                                                      // This sends the updated pixel color to the hardware.
}


int isButtonPressed() {

  int xValue = analogRead(VRX_PIN) - 512;
  int yValue = analogRead(VRY_PIN) - 512;
  bool pressed = digitalRead(joystickButtonPin) == LOW;

  bool right = xValue <= -joystickThreshold;
  bool left = xValue >= joystickThreshold;
  bool down = yValue >= joystickThreshold;

  // check if the pushbutton is pressed. If it is, the buttonState is LOW:
  if (left) {
    return 1;
  } else if (down) {
    return 2;  //normal return
  } else if (right) {
    return 3;
  } else return 0;  //nothing pressed
}

int dropChip(int currentLoc, int player) {

  boolean notOccupied = true;
  int checkLoc = currentLoc + 8;  //check next location
  int prevLoc;

  //light the top row first
  setPixelColor(currentLoc, pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
  pixels.show();  // This sends the updated pixel color to the hardware.

  while ((checkLoc < 64) && (notOccupied)) {
    if (board[checkLoc] == 0) {  //not occupied
      prevLoc = currentLoc;
      currentLoc = checkLoc;
      //move light

      movePixel(player, prevLoc, currentLoc);

      //      setPixelColor(prevLoc, pixels.Color(0, 0, 0)); // off
      //      setPixelColor(currentLoc, pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
      //      pixels.show(); // This sends the updated pixel color to the hardware.
      checkLoc = checkLoc + 8;  //check lower level
    } else {                    //occupied
      notOccupied = false;
    }
    delay(25);  //slow down the drop
  }

  board[currentLoc] = player;

  return currentLoc;
}

void showConnect4(int connect4[]) {

  int player = board[connect4[0]];  //find out whose connect4
  /*
    Serial.print("showConnect4: Connected dots are :");
    for (int l = 0; l <= 3; l++) {
      Serial.print(connect4[l]);
      Serial.print(" ");
    }
    Serial.println();
  */
  //blink for 4 times
  for (int i = 0; i <= 4; i++) {

    //off all connect 4
    for (int j = 0; j <= 3; j++) {
      setPixelColor(connect4[j], pixels.Color(0, 0, 0));
    }

    //    setPixelColor(connect4[0], pixels.Color(0, 0, 0));
    //    setPixelColor(connect4[1], pixels.Color(0, 0, 0));
    //    setPixelColor(connect4[2], pixels.Color(0, 0, 0));
    //    setPixelColor(connect4[3], pixels.Color(0, 0, 0));
    pixels.show();
    delay(200);
    //turn on all connect 4
    for (int j = 0; j <= 3; j++) {
      setPixelColor(connect4[j], pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
    }
    //    setPixelColor(connect4[0], pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
    //    setPixelColor(connect4[1], pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
    //    setPixelColor(connect4[2], pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
    //    setPixelColor(connect4[3], pixels.Color(r[player - 1], g[player - 1], b[player - 1]));
    pixels.show();
    delay(200);
  }
}

void clearBoard() {
  for (int i = 0; i < NUMPIXELS; i++) {
    board[i] = 0;
    setPixelColor(i, pixels.Color(0, 0, 0));  // off
  }
  pixels.show();
}

void melt() {

  int color;

  //Serial.println("initiate melting effect");

  for (int k = 0; k <= 7; k++) {
    //first time 55 to

    for (int i = 55; i >= 8 * k; i--) {  //64-9=55 always starts from the 2nd to the bottom row

      color = board[i];

      //change row below to the same color as top
      if (color == 0) {
        setPixelColor(i + 8, pixels.Color(0, 0, 0));  // black
        board[i + 8] = 0;
      } else {
        setPixelColor(i + 8, pixels.Color(r[color - 1], g[color - 1], b[color - 1]));  // set color
        board[i + 8] = color;
      }
    }

    pixels.show();
    delay(80);

    //change top rows to black //h = 0, from 7 to 0, h=1, from 15 to 8, when h=2, 23-16
    for (int i = (7 + (8 * k)); i >= (8 * k); i--) {
      setPixelColor(i, pixels.Color(0, 0, 0));  // black
      board[i] = 0;
    }
    //Serial.println("outloop");

    pixels.show();
    //drawBoard();
    //delay(50);
  }

  delay(500);
}


void twoPlayerGame() {

  int whoseTurn = whoGoesFirst;  //global var

  //fillBoard(); //for testing

  //while (eachTurn(whoseTurn) == 0) {  //returns 1 if end of game.. TODO recode this
  while (!endGame(whoseTurn)) {
    if (whoseTurn == 1) {
      whoseTurn = 2;  //next turn is player 2
    } else {
      whoseTurn = 1;  //next turn is player 1
    }
  }

  melt();

  //who goes first next turn: first player has an advantage so loser goes first
  if (whoseTurn == 1) {
    whoGoesFirst = 2;  //player 2 starts the next game
  } else {
    whoGoesFirst = 1;  //player 1 starts the next game
  }
}

/*
    Find a starting spot for each player
    Some of the top row columns  may be played already so find an empty spot to start
    If nothing found, return 9 (board full)
    TODO: auto play last chip
*/
int startingSpot(int player) {

  int i = 0;
  int pixelLoc;
  int prevLoc;
  boolean foundStartLoc = false;

  if (player == 1) {  //player1 start from left
    int i = 0;
    //find a starting pixel: normally at 0
    while ((i < 8) && (!foundStartLoc)) {
      if (board[i] == 0) {  //found an empty spot to start
        pixelLoc = i;
        showPixel(player, pixelLoc);
        foundStartLoc = true;
      } else {  //occupied, go to next
        i++;
      }
    }
  } else {  //player2 starts from the right side
    int i = 7;
    while ((i >= 0) && (!foundStartLoc)) {
      if (board[i] == 0) {  //found an empty spot to start
        pixelLoc = i;
        showPixel(player, pixelLoc);
        foundStartLoc = true;
      } else {  //occupied, go to next
        i--;
      }
    }
  }

  if (foundStartLoc) {
    return pixelLoc;
  } else {
    return 9;  //9 means no more space to play
  }
}

boolean endGame(int player) {

  int prevLoc;

  //find a starting spot for each player
  int pixelLoc = startingSpot(player);

  if (pixelLoc == 9) {  // no more spot to play. board is full
    Serial.println("No more place to play. Game Over");
    return true;
  }

  int WhichButton = isButtonPressed();  //find out which button is pressed

  while (WhichButton != 2) {  //not a down button

    boolean okToMove = false;  //reset at each loop

    if (WhichButton == 1) {

      //make sure left spot is not occupied
      int chkLoc = pixelLoc - 1;  //check the left spot

      while ((chkLoc >= 0) && (!okToMove)) {
        if (board[chkLoc] == 0) {
          prevLoc = pixelLoc;
          pixelLoc = chkLoc;  //shift to left
          okToMove = true;
        } else {  //occupied, go to next one
          chkLoc = chkLoc - 1;
        }
      }

      movePixel(player, prevLoc, pixelLoc);
      delay(200);
    } else if (WhichButton == 3) {

      int chkLoc = pixelLoc + 1;  //check the right spot

      while ((chkLoc <= 7) && (!okToMove)) {
        if (board[chkLoc] == 0) {
          prevLoc = pixelLoc;
          pixelLoc = chkLoc;  //shift to right
          okToMove = true;
        } else {  //occupied, go to next one
          chkLoc = chkLoc + 1;
        }
      }

      movePixel(player, prevLoc, pixelLoc);
      delay(200);
    }

    delay(1);  //pause before reading next switch position?
    WhichButton = isButtonPressed();
  }

  if (WhichButton == 2) {
    //Serial.println("drop");
    int playedLoc = dropChip(pixelLoc, player);

    if (isWinningMove(player, playedLoc)) {
      //gameOver = true;
      //Serial.println("eachturn: winningmove set gameover");
      return true;
    } else {
      return false;
    }
  }

  //Serial.println(pixelLoc);
  //return false;
}


void runDemo() {
  randomPlay(50);  //play 50 turns or until game over
  delay(1000);     //pause before "melt" the screen
  melt();
  delay(500);  //pause after "melt"
}

void randomPlay(int turns) {

  boolean gameOver = false;

  int player = 2;
  int i = 1;

  //Serial.println("random play");

  while ((i < turns) && (!gameOver)) {
    if (player == 1) {
      player = 2;
    } else {
      player = 1;
    }

    //pick a column that's not full
    int chooseColumn = randomColumn();
    while (board[chooseColumn] > 0) {  //occupied - pick another number
      //Serial.println("@randomplay - column is full: ");
      //delay(5000);
      //pick another column
      chooseColumn = randomColumn();
    }

    //wait for down button: for debugging one turn at a time
    /*
      while (isButtonPressed() != 2 ) {
      //Serial.println("waiting for down button");
      delay(10);
      }*/

    //Serial.println(chooseColumn);
    int playedLoc = dropChip(chooseColumn, player);
    delay(75);

    if (isWinningMove(player, playedLoc)) {
      gameOver = true;
    }
    i++;

    //pause after each turn
    delay(100);
  }
  //Serial.println("********** random play - game over **********");
}

int randomColumn() {

  int randomNum;
  //random(min, max)
  //Parameters
  //min - lower bound of the random value, inclusive (optional)
  //max - upper bound of the random value, exclusive

  //generate random # from 2 to 5 that matches led Pins
  //2 = red, 3 = blue, 4 = yellow, 5=green

  randomNum = random(0, 8);
  return randomNum;
}


boolean isWinningMove(int player, int playedLoc) {

  /*
    Serial.println("-----------------------------");
    Serial.print("isWinningMove: playedLoc = ");
    Serial.println(playedLoc);
  */

  int connected[4];  //store all connected location
  /*
    //test
    if (playedLoc < 40) {
    Serial.println("Connect4 - veritcal");
    connected[0] = playedLoc;
    connected[1] = playedLoc + 8;
    connected[2] = playedLoc + 16;
    connected[3] = playedLoc + 24;
    showConnect4(connected);
    return true;
    }
  */

  //check for veritcal connect4
  if (chkVericalWin(player, playedLoc)) {
    return true;
  } else if (chkHorizontalWin(player, playedLoc)) {
    return true;
  } else if (chkDiagonalWin(player, playedLoc)) {  //up & right dirction TODO: combine with below
    return true;
  } else if (chkDiagonalWin2(player, playedLoc)) {  //up and left direction
    return true;
  } else {
    return false;
  }
}

boolean chkVericalWin(int player, int playedLoc) {

  int connected[4];  //init arrayf

  //Serial.println("chkVericalWin: ");

  if ((playedLoc < 40) && (board[playedLoc + 8] == player) && (board[playedLoc + 16] == player) && (board[playedLoc + 24] == player)) {
    //Serial.println("********** Connect4 - veritcal**********");
    //make array
    connected[0] = playedLoc;
    connected[1] = playedLoc + 8;
    connected[2] = playedLoc + 16;
    connected[3] = playedLoc + 24;
    showConnect4(connected);
    return true;
  } else {
    //Serial.println("  not connected");
    return false;
  }
}

boolean chkDiagonalWin2(int player, int playedLoc) {

  int connected[4] = { 0, 0, 0, 0 };  //init array

  int connectedIndex = 0;  //starting with one connection index - self
  connected[connectedIndex] = playedLoc;

  //Serial.println("chkDiagonal  reverse");

  //check row above & left -> \ direction
  boolean keepChecking = true;
  int chkLoc;

  //check for right edge condition
  if ((playedLoc) % 8 == 0) {
    //playedLoc is on the left edge, no need to check further
    //Serial.println("left edge");
    keepChecking = false;
  } else {
    chkLoc = playedLoc - 9;  //56
  }

  while (keepChecking) {
    //Serial.print("  chkloc(up & left) = ");
    //Serial.println(chkLoc);

    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;
      //Serial.println("  diag \ connected ");
      //Serial.println(connected[0]);
      //Serial.println(connected[1]);
      //Serial.println(connected[2]);
      //Serial.println(connected[3]);

      if (connectedIndex >= 3) {
        //Serial.println("**********Connect4 - diagonal**********");
        showConnect4(connected);
        return true;
      }

      //deal with boundary issue
      if ((chkLoc) % 8 == 0) {
        //chkLoc is on the left edge, no need to check further
        //Serial.println("left edge");
        keepChecking = false;
      } else {
        chkLoc = chkLoc - 9;  //56
      }

    } else {
      //Serial.println("  diag \ not connected ");
      keepChecking = false;
    }
  }

  // check going down diagonally in "\" direction
  keepChecking = true;

  //check for right edge condition ex: 55,47,63
  if ((playedLoc + 1) % 8 == 0) {
    //playedLoc is on the left edge, no need to check further
    //Serial.println("right edge");
    keepChecking = false;
  } else {
    chkLoc = playedLoc + 9;  //54 + 9 = 63
  }

  while ((keepChecking) && (chkLoc <= 63)) {
    //Serial.print("  chkloc(down & right) = ");
    //Serial.println(chkLoc);
    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;
      //Serial.println("  diag \ connected ");
      //Serial.println(connected[0]);
      //Serial.println(connected[1]);
      //Serial.println(connected[2]);
      //Serial.println(connected[3]);

      if (connectedIndex >= 3) {
        //Serial.println("**********Connect4 - diagonal**********");
        showConnect4(connected);
        return true;
      }

      //edge = (playedLoc) % 8;
      if ((chkLoc + 1) % 8 == 0) {  //ex 47
        //chkLoc is on the right edge, no need to check further
        //Serial.println("right edge");
        keepChecking = false;
      } else {
        chkLoc = chkLoc + 9;  //38+9=47
      }
    } else {
      //Serial.println("  diag \ not connected ");
      keepChecking = false;
    }
  }

  //Serial.print("connectedIndex: ");
  //Serial.println(connectedIndex);

  //drawBoard();

  return false;
}

boolean chkDiagonalWin(int player, int playedLoc) {

  int connected[4] = { 0, 0, 0, 0 };  //init array

  int connectedIndex = 0;  //starting with one connection index - self
  connected[connectedIndex] = playedLoc;

  //Serial.println("chkDiagonal /");

  //check row above & right -> / direction
  boolean keepChecking = true;
  int chkLoc;

  //check for right edge condition
  //int edge = (playedLoc + 1) % 8;
  if ((playedLoc + 1) % 8 == 0) {
    //playedLoc is on the right edge, no need to check further
    //Serial.println("right edge");
    keepChecking = false;
  } else {
    chkLoc = playedLoc - 7;  //56
  }

  while (keepChecking) {
    //Serial.print("  chkloc(up & right) = ");
    //Serial.println(chkLoc);

    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;
      //Serial.println("  diag / connected ");
      //Serial.println(connected[0]);
      //Serial.println(connected[1]);
      //Serial.println(connected[2]);
      //Serial.println(connected[3]);

      if (connectedIndex >= 3) {
        //Serial.println("**********Connect4 - diagonal**********");
        showConnect4(connected);
        return true;
      }

      //deal with boundary issue
      if ((chkLoc + 1) % 8 == 0) {
        //playedLoc is on the right edge, no need to check further
        //Serial.println("right edge");
        keepChecking = false;
      } else {
        chkLoc = chkLoc - 7;  //56
      }

    } else {
      //Serial.println("  diag / not connected ");
      keepChecking = false;
    }
  }

  keepChecking = true;

  //check for left edge condition 56 is edge
  //edge = (playedLoc) % 8;
  if ((playedLoc) % 8 == 0) {
    //playedLoc is on the left edge, no need to check further
    //Serial.println("left edge");
    keepChecking = false;
  } else {
    chkLoc = playedLoc + 7;  //56
  }

  // check going down diagonally in "/" direction
  while ((keepChecking) && (chkLoc <= 63)) {
    //Serial.print("  chkloc(down & left) = ");
    //Serial.println(chkLoc);
    if (board[chkLoc] == player) {
      connectedIndex++;
      connected[connectedIndex] = chkLoc;
      //Serial.println("  diag / connected ");
      //Serial.println(connected[0]);
      //Serial.println(connected[1]);
      //Serial.println(connected[2]);
      //Serial.println(connected[3]);

      if (connectedIndex >= 3) {
        //Serial.println("**********Connect4 - diagonal**********");
        showConnect4(connected);
        return true;
      }

      //edge = (playedLoc) % 8;
      if ((chkLoc) % 8 == 0) {
        //playedLoc is on the left edge, no need to check further
        //Serial.println("left edge");
        keepChecking = false;
      } else {
        chkLoc = chkLoc + 7;  //56
      }
    } else {
      //Serial.println("  diag / not connected ");
      keepChecking = false;
    }
  }

  //drawBoard();

  return false;
}

boolean chkHorizontalWin(int player, int playedLoc) {

  int connected[4];  //init array

  int connectedIndex = 0;  //starting with one connection index - self
  connected[connectedIndex] = playedLoc;

  int leftWall = int(playedLoc / 8) * 8;

  //Serial.print("chkHorizontalWin: leftWall =  ");
  //Serial.println(leftWall);

  //until hit a wall, keep checking left side
  int chkLoc = playedLoc - 1;
  boolean keepChecking = true;

  while ((leftWall <= chkLoc) && (keepChecking)) {
    //Serial.print("  chkloc = ");
    //Serial.println(chkLoc);
    if (board[chkLoc] == player) {
      //left connected
      connectedIndex++;
      connected[connectedIndex] = chkLoc;
      //Serial.println("  left connected ");  //33
      //Serial.println(chkLoc);
      if (connectedIndex >= 3) {
        //Serial.println("**********Connect4 - horizontal**********");
        showConnect4(connected);
        return true;
      }
    } else {
      //Serial.println("  left not connected");
      keepChecking = false;
    }
    chkLoc--;  //32
  }            //while to check left

  //check right: ex: for 61, right wall is 63
  int rightWall = leftWall + 7;

  //Serial.print("chkHorizontalWin: rightWall =  ");
  //Serial.println(rightWall);

  //until hit a wall, keep checking left side
  chkLoc = playedLoc + 1;
  keepChecking = true;

  while ((rightWall >= chkLoc) && (keepChecking)) {
    //Serial.print("  chkloc = ");
    //Serial.println(chkLoc);
    if (board[chkLoc] == player) {
      //right connected
      connectedIndex++;  //keep accumulating connected points
      connected[connectedIndex] = chkLoc;
      //Serial.println("  right connected ");  //33
      //Serial.println(chkLoc);
      if (connectedIndex >= 3) {
        //Serial.println("**********Connect4 - horizontal**********");
        showConnect4(connected);
        return true;
      }
    } else {
      //Serial.println("  right not connected");
      keepChecking = false;
    }
    chkLoc++;  //32
  }            //while to check left

  return false;
}
