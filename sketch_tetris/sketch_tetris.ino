#include <IRremote.h>
#include <LedControl.h>
int RECV_PIN = 4;  //IR receiver pin

int DATA_IN = 12;    //data in
int CLOCK_PIN = 11;  //clock
int CS_PIN = 10;     //chip select

int NUM_DEV = 4;  //number of devices

//FF10EF left 4
//FF5AA5 right 6
//FF38C7 enter 5
//FF4AB5 down 8

LedControl lc = LedControl(DATA_IN, CLOCK_PIN, CS_PIN, NUM_DEV);
IRrecv irrecv(RECV_PIN);
decode_results results;

void clear_displays() {
  for (int i = 0; i < NUM_DEV; i++) {
    lc.clearDisplay(i);
  }
}

void displaySetup() {
  for (int i = 0; i < NUM_DEV; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 1);  //brightness 1-8
    clear_displays();
  }
}

void scoreScreen(int score){
  int thousands_score = score/1000;
  int hundreads_score = (score%1000)/100;
  int tens_score = (score%100)/10;
  int ones_score = score%10;

  int num0[] = {
    B00000000,
    B00000000,
    B01111110,
    B01000010,
    B01000010,
    B01000010,
    B01111110,
    B00000000
  };

  // 1
  int num1[] = {
    B00000000,
    B00000000,
    B00000100,
    B00001100,
    B00010100,
    B00000100,
    B00001110,
    B00000000
  };

  // 2
  int num2[] = {
    B00000000,
    B00000000,
    B01111110,
    B01000010,
    B00000100,
    B00001000,
    B01111110,
    B00000000
  };

  // 3
  int num3[] = {
    B00000000,
    B00000000,
    B01111110,
    B00000010,
    B00001110,
    B01000010,
    B00111100,
    B00000000
  };

  // 4
  int num4[] = {
    B00000000,
    B00000000,
    B00001000,
    B00011000,
    B00101000,
    B01111110,
    B00001000,
    B00000000
  };

  // 5
  int num5[] = {
    B00000000,
    B00000000,
    B01111100,
    B01000000,
    B01111000,
    B00000100,
    B00111000,
    B00000000
  };

  // 6
  int num6[] = {
    B00000000,
    B00000000,
    B00111100,
    B01000000,
    B01111100,
    B01000010,
    B00111100,
    B00000000
  };

  // 7
  int num7[] = {
    B00000000,
    B00000000,
    B01111110,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B00000000
  };

  // 8
  int num8[] = {
    B00000000,
    B00000000,
    B00111100,
    B01000010,
    B00111100,
    B01000010,
    B00111100,
    B00000000
  };

  // 9
  int num9[] = {
    B00000000,
    B00000000,
    B00111100,
    B01000010,
    B00111110,
    B00000010,
    B00111100,
    B00000000
  };

  int* number_arrays[] = { num0, num1, num2, num3, num4, num5, num6, num7, num8, num9};

  for (int rownum = 0; rownum < 8; rownum++) {
    lc.setRow(0, rownum, number_arrays[ones_score][rownum]); //1
    lc.setRow(1, rownum, number_arrays[tens_score][rownum]); //10
    lc.setRow(2, rownum, number_arrays[hundreads_score][rownum]); //100
    lc.setRow(3, rownum, number_arrays[thousands_score][rownum]); //1000
  }

}

void splashScreen() {
  int screen3[] = {
    B00000000,
    B00000000,
    B10000101,
    B10000101,
    B11110101,
    B10000101,
    B10000111,
    B00000000
  };

  int screen2[] = {
    B00000000,
    B00000000,
    B01010000,
    B01010000,
    B01011110,
    B01010000,
    B11010000,
    B00000000
  };

  int screen1[] = {
    B00000000,
    B00000000,
    B01001000,
    B10110000,
    B10100111,
    B10100000,
    B11111000,
    B00000000
  };

  int screen0[] = {
    B00000000,
    B00000010,
    B00010101,
    B00010101,
    B11010101,
    B00010101,
    B00001000,
    B00000000
  };

  for (int rownum = 0; rownum < 8; rownum++) {
    lc.setRow(0, rownum, screen0[rownum]);
    lc.setRow(1, rownum, screen1[rownum]);
    lc.setRow(2, rownum, screen2[rownum]);
    lc.setRow(3, rownum, screen3[rownum]);
  }
}

int pilot;

void pilot_num() {
  if (irrecv.decode(&results)) {
    if (results.value != 0xFFFFFFFF) {
      Serial.println(results.value, HEX);
      if (String(results.value, HEX) == "ff38c7") {
        pilot = 5;
        irrecv.resume();
      } else if (String(results.value, HEX) == "ff10ef") {
        pilot = 4;
        irrecv.resume();
      } else if (String(results.value, HEX) == "ff5aa5") {
        pilot = 6;
        irrecv.resume();
      } else if (String(results.value, HEX) == "ff4ab5") {
        pilot = 8;
        irrecv.resume();
      } else if (String(results.value, HEX) == "ffa25d") {  //top-left button "escape"
        pilot = 8008;
        irrecv.resume();
      }
    }
    irrecv.resume();
  }
}

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  displaySetup();
}

void loop() {
  int dispfinal[8][16] = { 0 };
  int disp[8][16] = { 0 };
  pilot_num();
  splashScreen();
  int delay_delay = 1000;
  boolean gameStart = false;
  boolean gameOver = false;
  boolean score_tetris = false;
  int leftsteps = 0;
  int rightsteps = 0;
  bool nextbox = false;

  if (pilot == 5 || pilot == 4 || pilot == 6 || pilot == 8 || pilot == 8008) {
    Serial.println(pilot);
  }

  int totalscore = 0;
  if (pilot == 5) {
    gameStart = true;
    clear_displays();
    pilot = 0;
  }
  while (gameStart == true) {
    pilot_num();
    totalscore += 1;
    /////////////////////////////////////////////
    //int block = rand() % 5 + 1;
    randomSeed(analogRead(5));
    int block = random(1,6);
    switch (block) {
      case 1:  // - block
        disp[2][0] = 1;
        disp[3][0] = 1;
        disp[4][0] = 1;
        disp[5][0] = 1;
        leftsteps = 2;
        rightsteps = 2;
        break;
      case 2:  // I block
        disp[3][0] = 1;
        disp[3][1] = 1;
        disp[3][2] = 1;
        disp[3][3] = 1;
        leftsteps = 3;
        rightsteps = 4;
        break;
      case 3:  // square block
        disp[3][0] = 1;
        disp[3][1] = 1;
        disp[4][0] = 1;
        disp[4][1] = 1;
        leftsteps = 3;
        rightsteps = 3;
        break;
      case 4:  // dot block
        disp[3][0] = 1;
        leftsteps = 3;
        rightsteps = 4;
        break;
      case 5:  // T block
        disp[3][0] = 1;
        disp[3][1] = 1;
        disp[2][1] = 1;
        disp[4][1] = 1;
        leftsteps = 2;
        rightsteps = 3;
        break;
    }
    //game over condition
    bool can_place_block = true;
    for (int y = 0; y <= 15; y++) {
      for (int x = 0; x <= 7; x++) {
        if (disp[x][y] == 1 && dispfinal[x][y] == 1) {
          can_place_block = false;
          gameOver = true;
          break;
        }
      }
      if (!can_place_block) {
        break;
      }
    }
    if (!can_place_block) {
      //cout << "Game over" << endl;
      break;
    }
    delay_delay = 1000;
    //main loop. 16 times because of screen size
    for (int i = 0; i <= 15; i++) {
      pilot_num();
      //move left and check if there is collison
      if (pilot == 6 && leftsteps != 0) {
        pilot = 0;
        bool can_move_left = true;
        for (int y = 0; y < 16; y++) {
          for (int x = 0; x < 8; x++) {
            if (x - 1 >= 0 && disp[x][y] == 1 && dispfinal[x - 1][y] == 1) {
              can_move_left = false;
              break;
            }
          }
          if (!can_move_left) {
            break;
          }
        }
        if (can_move_left) {
          for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 8; x++) {
              if (x - 1 >= 0) {
                int temp = disp[x - 1][y];
                disp[x - 1][y] = disp[x][y];
                disp[x][y] = temp;
              }
            }
          }
          rightsteps++;
          leftsteps--;
        }
      }

      //move right and check if there is collison
      if (pilot == 4 && rightsteps != 0) {
        pilot = 0;
        bool can_move_right = true;
        for (int y = 0; y < 16; y++) {
          for (int x = 7; x >= 0; x--) {
            if (x + 1 <= 7 && disp[x][y] == 1 && dispfinal[x + 1][y] == 1) {
              can_move_right = false;
              break;
            }
          }
          if (!can_move_right) {
            break;
          }
        }

        if (can_move_right) {
          for (int y = 0; y < 16; y++) {
            for (int x = 7; x >= 0; x--) {
              if (x + 1 <= 7) {
                int temp = disp[x + 1][y];
                disp[x + 1][y] = disp[x][y];
                disp[x][y] = temp;
              }
            }
          }
          rightsteps--;
          leftsteps++;
        }
      }

      if(pilot == 8){
        pilot = 0;
        delay_delay = 0;
      }


      Serial.println(i);
      //check if the block fell down or if it hit saved blocks
      for (int x = 0; x < 8; x++) {
        if (i == 15 || (disp[x][i] == 1 && dispfinal[x][i + 1] == 1)) {
          nextbox = true;
          break;
        }
        if (disp[x][i + 1] == 1 && dispfinal[x][i + 2] == 1) {
          nextbox = true;
          break;
        }
        if (disp[x][i + 2] == 1 && dispfinal[x][i + 3] == 1) {
          nextbox = true;
          break;
        }
        if (disp[x][i + 3] == 1 && dispfinal[x][i + 4] == 1) {
          nextbox = true;
          break;
        }
        if (block == 2 && i == 12) {
          nextbox = true;
          break;
        }
        if ((block == 3 || block == 5) && i == 14) {
          nextbox = true;
          break;
        }
      }
      Serial.println(nextbox);
      //check if the next block should be handled, save the previous one and reset display
      if (nextbox == true) {
        nextbox = false;
        for (int y = 0; y <= 15; y++) {
          for (int x = 0; x <= 7; x++) {
            dispfinal[x][y] = disp[x][y] | dispfinal[x][y];
          }
        }

        for (int y = 0; y <= 15; y++) {
          for (int x = 0; x <= 7; x++) {
            disp[x][y] = 0;
          }
        }
        //cout<<"break";
        break;
      }

      //print tetris on led matrix

      for (int y = 0; y <= 15; y++) {
        for (int x = 0; x <= 7; x++) {
          if (y >= 8) {
            lc.setLed(0, x, y % 8, (disp[x][y] | dispfinal[x][y] == 1 ? 1 : 0));
          } else {
            lc.setLed(1, x, y, (disp[x][y] | dispfinal[x][y] == 1 ? 1 : 0));
          }
        }
      }
      /*
        //debug print tetris
        for(int j = 0; j<16; j++){
            for(int z = 0; z<8; z++){
                int dispint = dispfinal[z][j] | disp[z][j];
                cout << dispint << " ";
            }
            cout << endl;
        }
        */

      //check if line should be cleared and move set blocks down
      for (int y = 0; y <= 15; y++) {
        bool line_cleared = true;
        for (int x = 0; x <= 7; x++) {
          if (dispfinal[x][y] != 1) {
            line_cleared = false;
            break;
          }
        }

        if (line_cleared) {
          for (int j = y; j > 0; j--) {
            for (int k = 0; k < 8; k++) {
              dispfinal[k][j] = dispfinal[k][j - 1];
              totalscore += 5;
            }
          }
          for (int k = 0; k < 8; k++) {
            dispfinal[k][0] = 0;
          }
        }
      }

      //handle block moving down
      for (int x = 0; x < 8; x++) {
        for (int j = i + 3; j >= i; j--) {
          if (disp[x][j] == 1 && j < 15) {
            int temp = disp[x][j + 1];
            disp[x][j + 1] = disp[x][j];
            disp[x][j] = temp;
          }
        }
      }

      //cin>>keybind;
      delay(delay_delay);
    }
  }

  while (gameOver == true) {
    pilot_num();
    lc.setLed(random(0, 4), random(0, 8), random(0, 8), true);
    if (pilot == 5) {
      pilot = 0;
      gameOver = false;
      score_tetris = true;
      clear_displays();
    }
  }
  while (score_tetris) {
    pilot_num();
    scoreScreen(totalscore);
    if (pilot == 5) {
      pilot = 0;
      score_tetris = false;
      clear_displays();
      break;
    }
  }
}