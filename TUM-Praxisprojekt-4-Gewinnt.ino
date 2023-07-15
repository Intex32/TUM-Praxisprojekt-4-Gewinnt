#include <Adafruit_NeoPixel.h>

int LED_COUNT = 64;
int LED_PIN = 8;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int brightness = 128; // 256
long rot = strip.Color(brightness, 0, 0);
long gruen = strip.Color(0, 0, brightness);
long black = strip.Color(0, 0, 0);

byte zw1 = 0; int zw2 = 0; // werden in "Ausgabe Spielfeld" benutzt
byte zw3 = 0; // wird in Absenken der Spielsteine benutzt
byte zw4 = 0; // Startfeld beim Testen
byte zw5 = 0; // Zaelwert innere Schleife, testet 4 Felder
byte zw6 = 0; // Zaelwert mittlere Schleife, testet eine Spalte
byte zw7 = 0; // Zaelwert außere Schleife, testet alle Spalten
byte zw8 = 0; // Gefallene Steine
byte zw9 = 0; // Ausgabe Siegerfarbe
byte zw10 = 0; //Loeschen Anzeige und Spielfeld
byte zw11 = 0; //Blinken Siegersteine
long farbe = 0;
long farbe_1 = 0; long farbe_2 = 0; // Siegerfarbe
byte position_1 = 0; byte position_2 = 0; byte position_3 = 0; byte position_4 = 0; //Position Gewinnersteine

// 0 -> nicht belegt; 1 -> rot; 2 -> gruen
byte spielfeld [64] = {
  1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

byte richtung = 0 ; // 1 -> rechts; 2 -> links; 3 -> runter
byte status_start = 1; // 1 -> rot; 2 -> gruen
byte status_spiel = 1 ; // 1 -> rot; 2 -> gruen
byte position_0 = 0; // Zeiger fuer Spielfeld
byte test_rot = 0; byte test_gruen = 0; byte sieger = 0; // Benoetigt zum Ermitteln des Siegers

#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
int joystickThreshold = 375;
int joystickButtonPin = 2;
bool joystickButtonPressed = false;

void setPixelColor(int pos, long rgb) {
  int row0 = pos / 8;
  int col0 = pos % 8;
  int row = 7 - col0;
  int indexInRow = (8-1) - row0;
  if(row % 2 == 1)
    indexInRow = (8-1) - indexInRow;
  strip.setPixelColor(row*8+indexInRow, rgb);
}

void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.show();

  pinMode(joystickButtonPin, INPUT_PULLUP);

  ausgabe_spielfeld ();
}

void loop() {
  do {
    int xValue = analogRead(VRX_PIN) - 512;
    int yValue = analogRead(VRY_PIN) - 512;
    bool pressed = digitalRead(joystickButtonPin) == LOW;
    Serial.println(pressed);

    bool right = xValue <= -joystickThreshold;
    bool left = xValue >= joystickThreshold;
    bool down = yValue >= joystickThreshold;

    if(pressed && !joystickButtonPressed) {
      for(int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, rot);
      }
      strip.show();
      delay(500);
      ausgabe_spielfeld();
    }
    joystickButtonPressed = pressed;

    if (right && left) {
      zw8 = 56;
      test_spielende ();
    };
    if (right) {
      richtung = 1; rechts_0 ();
    }
    if (left) {
      richtung = 2; links_0 ();
    }
    if (down) {
      if (spielfeld [position_0 + 8] == 0) {
        richtung = 3;
        runter_0 (); zw8 ++; // Anzahl Durchlaeufe + 1
        test_spielende ();
      };
    }
  }
  while ( richtung == 0);
  richtung = 0; // Loeschender Richtung

  ausgabe_spielfeld ();
  delay(300);
}

//  ------------- Test ob es einen Sieger gibt  ------------------
//  ---------- Programm wird nach Absenken eines Steines aufgerufen
void test_spielende () {

  // ----------- Test diagonal oben ------------
  // ----------3 verschaltelte Zyklen ----------

  zw4 = 32; // Startfeld beim Testen
  sieger = 0; zw7 = 0;

  do {
    zw6 = 0;

    do {
      zw5 = 0; // Zaelschleife
      test_rot = 0; test_gruen = 0;

      // Testen 4 Zellen diegonal oben
      do {
        if ( spielfeld [zw4] == 1) {
          test_rot ++;
        };
        if ( spielfeld [zw4] == 2) {
          test_gruen ++;
        };

        // Merken der Position beim Sieg
        if (sieger == 0) {
          if (zw5 == 0) {
            position_1 = zw4;
          };
          if (zw5 == 1) {
            position_2 = zw4;
          };
          if (zw5 == 2) {
            position_3 = zw4;
          };
          if (zw5 == 3) {
            position_4 = zw4;
          };
        };

        zw4 = zw4 - 7; zw5++;
      }
      while (zw5 != 4);

      if (test_rot == 4) {
        sieger = 1;
      } else {
        test_rot = 0;
      };
      if (test_gruen == 4) {
        sieger = 2;
      } else {
        test_gruen = 0;
      };
      zw4 = zw4 + 36; zw6++;
      if (sieger != 0) {
        zw6 = 4;
      };
    }
    while ( zw6 != 4);

    zw4 = zw4 - 31; zw7++;
    if (sieger != 0) {
      zw7 = 5;
    };
  }
  while (zw7 != 5);

  // ----------- Test diagonal unten -----------
  // ----------3 verschachtelte Zyklen ----------

  zw4 = 8; // Startfeld beim Testen
  zw7 = 0;

  do {
    zw6 = 0;

    do {
      zw5 = 0; //zaehlschleife
      test_rot = 0; test_gruen = 0;

      // Testen 4 Zellen diagonal unten
      do {
        if ( spielfeld [zw4] == 1) {
          test_rot ++;
        };
        if ( spielfeld [zw4] == 2) {
          test_gruen ++;
        };

        // Merken der Position beim Sieg
        if (sieger == 0) {
          if (zw5 == 0) {
            position_1 = zw4;
          };
          if (zw5 == 1) {
            position_2 = zw4;
          };
          if (zw5 == 2) {
            position_3 = zw4;
          };
          if (zw5 == 3) {
            position_4 = zw4;
          };
        };

        zw4 = zw4 + 9; zw5++;
      }
      while (zw5 != 4);

      if (test_rot == 4) {
        sieger = 1;
      } else {
        test_rot = 0;
      };
      if (test_gruen == 4) {
        sieger = 2;
      } else {
        test_gruen = 0;
      };
      zw4 = zw4 - 28; zw6++;
      if (sieger != 0) {
        zw6 = 4;
      };
    }
    while ( zw6 != 4);

    zw4 = zw4 - 31; zw7++;
    if (sieger != 0) {
      zw7 = 5;
    };
  }
  while (zw7 != 5);

  // ------------ Test senkrecht ---------------
  // ----------3 verschaltelte Zyklen ----------

  zw4 = 8; // Startfeld beim Testen
  zw7 = 0;
  do {
    zw6 = 0;

    do {
      zw5 = 0; //zaelschleife
      test_rot = 0; test_gruen = 0;

      // Testen 4 Zellen senkrecht
      do {
        if ( spielfeld [zw4] == 1) {
          test_rot ++;
        };
        if ( spielfeld [zw4] == 2) {
          test_gruen ++;
        };

        // Merken der Position beim Sieg
        if (sieger == 0) {
          if (zw5 == 0) {
            position_1 = zw4;
          };
          if (zw5 == 1) {
            position_2 = zw4;
          };
          if (zw5 == 2) {
            position_3 = zw4;
          };
          if (zw5 == 3) {
            position_4 = zw4;
          };
        };

        zw4 = zw4 + 8; zw5++;
      }
      while (zw5 != 4);

      if (test_rot == 4) {
        sieger = 1;
      } else {
        test_rot = 0;
      };
      if (test_gruen == 4) {
        sieger = 2;
      } else {
        test_gruen = 0;
      };
      zw4 = zw4 - 24; zw6++;
      if (sieger != 0) {
        zw6 = 4;
      };
    }
    while ( zw6 != 4);

    zw4 = zw4 - 31; zw7++;
    if (sieger != 0) {
      zw7 = 8;
    };
  }
  while (zw7 != 8);

  // ------------- Test waagerecht --------------
  // ----------3 verschachtelte Zyklen ----------
  zw4 = 8; // Startfeld beim Testen
  zw7 = 0;
  do {
    zw6 = 0;

    do {
      zw5 = 0;
      test_rot = 0; test_gruen = 0;

      // Testen 4 Zellen waagerecht
      do {
        if ( spielfeld [zw4] == 1) {
          test_rot ++;
        };
        if ( spielfeld [zw4] == 2) {
          test_gruen ++;
        };

        // Merken der Position beim Sieg
        if (sieger == 0) {
          if (zw5 == 0) {
            position_1 = zw4;
          };
          if (zw5 == 1) {
            position_2 = zw4;
          };
          if (zw5 == 2) {
            position_3 = zw4;
          };
          if (zw5 == 3) {
            position_4 = zw4;
          };
        };

        zw4++; zw5++;
      }
      while (zw5 != 4);

      if (test_rot == 4) {
        sieger = 1;
      } else {
        test_rot = 0;
      };
      if (test_gruen == 4) {
        sieger = 2;
      } else {
        test_gruen = 0;
      };
      zw4 = zw4 + 4; zw6++;
      if (sieger != 0) {
        zw6 = 7;
      };
    }
    while ( zw6 != 7);

    zw4 = zw4 - 55; zw7++;
    if (sieger != 0) {
      zw7 = 5;
    };
  }
  while (zw7 != 5);

  // Festlegen Siegerfarben
  if (sieger == 1) {
    farbe_1 = rot;
    farbe_2 = rot;
    ende (); // Ausgabe Siegerfarbe Rot
  };
  if (sieger == 2) {
    farbe_1 = gruen;
    farbe_2 = gruen;
    ende (); // Ausgabe Siegerfarbe Gruen
  };
  if (zw8 == 56 ) {
    farbe_1 = gruen;
    farbe_2 = rot;
    ende (); // Ausgabe Unentschieden
  };

  position_1 = 0; position_2 = 0; position_3 = 0; position_4 = 0;
  test_rot = 0; test_gruen = 0;
}

// ---------- Ausgabe Siegerfarbe ----------

void ende () {
  zw11 = 0;

  if (zw8 != 56 ) {
    do {
      setPixelColor ( position_1, black);
      setPixelColor ( position_2, black);
      setPixelColor ( position_3, black);
      setPixelColor ( position_4, black);
      strip.show(); delay(200);
      setPixelColor ( position_1, farbe_1);
      setPixelColor ( position_2, farbe_1);
      setPixelColor ( position_3, farbe_1);
      setPixelColor ( position_4, farbe_1);
      strip.show(); delay(200);
      zw11++;
    }
    while ( zw11 != 5);
  }

  zw9 = 0;
  do {
    setPixelColor ( zw9, farbe_1); zw9++;
    setPixelColor ( zw9, farbe_2); zw9++;
  }
  while (zw9 != 64);
  strip.show();

  delay(2000);

  // ---------- Vorbereitung neuer Start -----------

  zw10 = 0;
  do {
    setPixelColor ( zw10, black); spielfeld [zw10] = 0;
    zw10++;
  }
  while ( zw10 != 64);
  strip.show();

  spielfeld [0] = 1; ausgabe_spielfeld ();
  zw1 = 0; zw2 = 0; zw3 = 0; zw4 = 0; zw5 = 0; zw6 = 0; zw7 = 0; zw8 = 0; zw9 = 0; zw10 = 0;
  position_1 = 0; position_2 = 0; position_3 = 0; position_4 = 0;
  position_0 = 0;

  // Wechsel Start-Spielfarbe
  farbe_1 = 0; farbe_2 = 0; sieger = 0;  

  if (status_start == 1) {
    farbe = gruen; status_spiel = 2;status_start=2;
    spielfeld [0] = 2;
  } else {
    farbe = rot;
    spielfeld [0] = 1; status_spiel = 1;status_start=1;
  };
  
  ausgabe_spielfeld ();
}

// ---------- Ein Schritt nach Links ----------
void links_0 () {
  if (position_0 != 0) {
    spielfeld [position_0] = 0;
    position_0--;
    spielfeld [ position_0 ] = status_spiel;
  };
}

// ---------- Ein Schritt nach Rechts ----------
void rechts_0 () {
  if (position_0 != 7) {
    spielfeld [position_0] = 0;
    position_0++;
    spielfeld [ position_0 ] = status_spiel;
  };
}

// ---------- Absenken des Spielsteines ----------
void runter_0 () {
  zw3 = 0;
  do {
    if (spielfeld [position_0 + 8] == 0 ) {
      spielfeld [position_0] = 0; position_0 = position_0 + 8; spielfeld [position_0] = status_spiel;
      zw3++;
      ausgabe_spielfeld (); delay(200);
    }
    else {
      zw3 = 7;
    }
  }
  while (zw3 != 7) ;
  richtung = 0; position_0 = 0;
  if (status_spiel == 1 ) {
    status_spiel = 2;
    spielfeld [position_0] = 2;
  } else {
    status_spiel = 1;
    spielfeld [position_0] = 1;
  };
  ausgabe_spielfeld ();
}

// ---------- Ausgabe Spielfeld ----------
void ausgabe_spielfeld () {
  zw1 = 0;

  do {
    zw2 = spielfeld [zw1];
    if (zw2 == 0) {
      setPixelColor ( zw1, black);
    };
    if (zw2 == 1) {
      setPixelColor ( zw1, rot);
    };
    if (zw2 == 2) {
      setPixelColor ( zw1, gruen);
    };
    zw1++;
  }
  while (zw1 != 64);
  strip.show();
}