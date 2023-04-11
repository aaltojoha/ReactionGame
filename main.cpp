// Reaction Game 2022
// Johanna Aaltonen ja Havu Kirjalainen

#include <LiquidCrystal.h>
#include <EEPROM.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


const int buttons[] = { 19, 20, 21 };

bool buttonPushed1 = false;
bool buttonPushed2 = false;
bool buttonPushed3 = false;

int debounceTime = 20; // variable to prevent debounce from buttons

long unsigned int timeFromLed;

int timeVariable = 2000;


// define the leds
#define LED1 48
#define LED2 49
#define LED3 50

// define the frequencies of the notes
#define C 523
#define Cis 554
#define D 587
#define Dis 622
#define E 659
#define note_F 698
#define Fis 740
#define G 784
#define Gis 831
#define A 880
#define Ais 932
#define B 988
#define C2 1047
#define zero 0


// firework animations
byte fire1[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00100
};

byte fire2[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00100,
  B00000,
  B00000
};

byte fire3[] = {
  B00000,
  B00000,
  B00000,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000
};

byte fire4[] = {
  B00000,
  B00000,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte fire5[] = {
  B00000,
  B00100,
  B01010,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000
};

byte fire6[] = {
  B00100,B01010,
  B10101,
  B01010,
  B00100,
  B00000,
  B00000,
  B00000
};


// enum states to change between functions
enum {
  STATE_START_MENU,
  STATE_GAME,
  STATE_GAME_OVER
};


int score = 0;
int highscore = 0;
int state = STATE_START_MENU; // set the current state to STATE_START_MENU


void setup() {
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  // enable pull up resistors for button pins
  for (int i = 0; i < 3; i++)
    pinMode(buttons[i], INPUT_PULLUP);
  
  readHighscore();
  
  // set up the number of LCD's columns and rows
  lcd.begin(16, 2);
  
  // if the state of any button changes, the program goes to the button's
  // assigned ISR function
  attachInterrupt(digitalPinToInterrupt(buttons[0]), ISR_button1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(buttons[1]), ISR_button2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(buttons[2]), ISR_button3, CHANGE);
  
  // create the firework characters
  lcd.createChar(1, fire1);
  lcd.createChar(2, fire2);
  lcd.createChar(3, fire3);
  lcd.createChar(4, fire4);
  lcd.createChar(5, fire5);
  lcd.createChar(6, fire6);
  lcd.home();
}


// ISR for button 1 being pushed
void ISR_button1() {
  state = STATE_GAME;// => game starts
  buttonPushed1 = true; // button has been pushed
}

// ISR for button 2 being pushed
void ISR_button2() {
  state = STATE_GAME;
  buttonPushed2 = true;
}

// ISR for button 3 being pushed
void ISR_button3() {
  state = STATE_GAME;
  buttonPushed3 = true;
}


// firework animations
void firework() {
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.setCursor(15, 1);
  lcd.write(1);
  delay(100);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(15, 1);
  lcd.write(2);
  delay(100);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.write(3);
  lcd.setCursor(15, 1);
  lcd.write(3);
  delay(100);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.write(4);
  lcd.setCursor(15, 1);
  lcd.write(4);
  delay(100);lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.write(5);
  lcd.setCursor(15, 1);
  lcd.write(5);
  delay(100);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.write(6);
  lcd.setCursor(15, 1);
  lcd.write(6);
  delay(200);
}


void introMusic() {
  
  // notes in the melody
  int melody[] = {
    C2, C2, A, note_F, G, C, E, G, C2, C2, A, note_F, G, Fis, G, Fis, note_F, E, note_F, G, A, B, C
  };
  
  // note durations: 4 = quarter note, 8 = eighth note...
  int noteDurations[] = {
    8, 8, 8, 8, 4, 16, 16, 16, 8, 8, 8, 8, 4, 4, 2, 2, 2, 2, 32, 32, 32, 32
  };
  
  // let's repeat the melody
  // iterate over the notes of the melody
  for (int thisNote = 0; thisNote < 22; thisNote++) {
    
    // to calculate the note duration, take one second
    // divided by the note type
    // quarter note = 1000 / 4, eighth note = 1000/8...
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);
    
    // to distinguish the notes, set a minimum time between them
    // the note's duration + 30% seems to work well
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    
    // stop the tone playing:
    noTone(8);
    
  }
}


void gameOverMusic() {
  
  int melody[] = {
    G, zero, Fis, zero, note_F, zero, E
  };
  
  int noteDurations[] = {
    4, 8, 4, 8, 4, 8, 2
  };
  
  for (int thisNote = 0; thisNote < 7; thisNote++) {
    
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(8);
    
  }
}


void winMusic() {
  
  int melody[] = {
    D, E, G, C, E, C, E, C2, C2, zero
  };
  
  int noteDurations[] = {
    4, 4, 4, 32, 32, 32, 32, 8, 2, 2
  };
  
  for (int thisNote = 0; thisNote < 10; thisNote++) {
    
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(8);
    
  }
}


// read highscore value from EEPROM
void readHighscore() {
  
  highscore = (EEPROM.read(0) << 8) + EEPROM.read(1);
  
  // EEPROM initial value is FFFF
  if (highscore == 0xffff)
    highscore = 0;
    
}


// save new highscore value to EEPROM
// play win music
// display new highscore on LCD and play firework animations
// set current state to STATE_START_MENU
void writeHighscore() {
  
  EEPROM.write(0, highscore >> 8);
  EEPROM.write(1, highscore & 0xff);
  
  winMusic();
  
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("New high score");
  lcd.setCursor(6, 1);
  lcd.print(highscore);
  
  for (int i = 6; i > 0; i--) {
    firework();
    state = STATE_START_MENU;
  }
  
}


// updates display with current score
void updateDisplay(int score) {
  
  lcd.clear();
  lcd.print("Your score:");
  lcd.print(score);
  delayMicroseconds(500);
  
}


// prepares game state for a new game
// set current state to STATE_GAME
void startNewGame() {
  
  buttonPushed1 = false;
  buttonPushed2 = false;
  buttonPushed3 = false;
  
  score = 0;
  debounceTime = 20;
  timeFromLed = 0;
  timeVariable = 2000;
  
  state = STATE_GAME;
  
}


// plays intro music
// updates the start menu.
// start a new game when some button is pressed
void startMenu() {
  
  buttonPushed1 = false;
  buttonPushed2 = false;
  buttonPushed3 = false;
  
  // plays a cheerful melody
  introMusic();
  
  // print "Welcome, Push any button to start" message to LCD
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Welcome");
  delay(2000);
  
  while(buttonPushed1 == false) {
    
    lcd.setCursor(1, 0);
    lcd.print("Push 1st button");
    delay(1000);
    lcd.setCursor(4, 1);
    lcd.print("to start");
    
  }
  
  while(buttonPushed1 == true){
    
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Ready to start");
    delay(2000);
    startNewGame();
    
  }
  
}


// game over
// play a game over music and print "GAME OVER"
void gameOver() {
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  
  lcd.clear();
  gameOverMusic();
  lcd.setCursor(3, 0);
  lcd.print("GAME OVER!");
  delay(3000);
  
  // checks if new highscore
  if (score > highscore) {
    highscore = score;
    writeHighscore();
  }
  
  // prints score
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Your score:");
  lcd.print(score);
  lcd.setCursor(1, 1);
  lcd.print("High score:");
  lcd.print(highscore);
  delay(3000);
  
  // enter menu
  state = STATE_START_MENU;
}


// to set buttons to false state between game rouds
void buttonsFalse() {
  
  buttonPushed1 = false;
  buttonPushed2 = false;
  buttonPushed3 = false;
  
}


// the game
void playGame() {
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  
  //sets all buttons to state false
  buttonsFalse();
  timeFromLed = 0;
  delay(100); // delay so that false button flags have time to set in
  
  randomSeed(analogRead(0)); // seed so the numbers won't be same every time
  int randNum = random(1,4); // random number 1 to 3
  
  // updates the score while gaming
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(0, 7);
  lcd.print(score);
  
  if(randNum == 1){
    timeFromLed = millis(); // Save current millis() in a variable
    digitalWrite(LED1, HIGH); // Write led as high
    
    do {
    // wait while button hasn't been pushed
    } while (buttonPushed1 == false && (millis() - timeFromLed) < timeVariable); // do when button hasn't been pushed, there is still time
 
    if (buttonPushed1 == true && debounceTime < (millis() - timeFromLed) && (millis() - timeFromLed) < timeVariable) { // do when button has been pushed in time and it's been pushed after debounceTime
      digitalWrite(LED1, LOW);
      score++; // add a point to the score
      randNum = 0;
    }
    
    else if (buttonPushed1 == false && timeVariable >= (millis() - timeFromLed)) { // do when button hasn't been pushed in time
      digitalWrite(LED1, LOW);
      gameOver();
    }
    
    else if (buttonPushed2 == true || buttonPushed3 == true) { // do when wrong button has been pushed
      digitalWrite(LED1, LOW);
      gameOver();
    }
    
    buttonsFalse();
    
  }

  if(randNum == 2){
    
    timeFromLed = millis(); 
    digitalWrite(LED2, HIGH); 
    
    do {
    // wait while button hasn't been pushed
    } while (buttonPushed2 == false && (millis() - timeFromLed) < timeVariable);
    
    if (buttonPushed2 == true && debounceTime < (millis() - timeFromLed) && (millis() - timeFromLed) < timeVariable) {
      digitalWrite(LED2, LOW);
      score++; 
      randNum = 0;
    }
    
    else if (buttonPushed2 == false && timeVariable >= (millis() - timeFromLed)) { 
      digitalWrite(LED2, LOW);
      gameOver();
    }
    
    else if (buttonPushed1 == true || buttonPushed3 == true) {
      digitalWrite(LED2, LOW);
      gameOver();
    }
    
    buttonsFalse();
    
  }
  
  if(randNum == 3){
    timeFromLed = millis();
    digitalWrite(LED3, HIGH);
    
    do {
    // wait while button hasn't been pushed
    } while (buttonPushed3 == false && (millis() - timeFromLed) < timeVariable); 
 
    if (buttonPushed3 == true && debounceTime < (millis() - timeFromLed) && (millis() - timeFromLed) < timeVariable) {
      digitalWrite(LED3, LOW);
      score++;
      randNum = 0;
    }
    
    else if (buttonPushed3 == false && timeVariable >= (millis() - timeFromLed)) {
      digitalWrite(LED3, LOW);
      gameOver();
    }
    
    else if (buttonPushed1 == true || buttonPushed2 == true) {
      digitalWrite(LED3, LOW);
      gameOver();
    }
    
    buttonsFalse();
    
  }
}


// Main loop. Update menu, game or game over depending on current state.
void loop() {
  
  if (state == STATE_START_MENU)
    startMenu();
    
  else if (state == STATE_GAME)
    playGame();
    
  else
    gameOver();
    
}
