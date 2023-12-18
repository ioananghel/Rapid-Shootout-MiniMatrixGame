#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "custom_chars.h"
#include "player.h"
#include "memory.h"
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int pinX = A0;
const int pinY = A1;
const int pinLDR = A3;
const int pinSW = 2;
const int buzzerPin = 3;

const byte rs = 13;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const int lcdBacklight = 9;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte lcdBrightness = 5;

unsigned long lastChangeX = 0, lastChangeY = 0, lastChangeSW;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte matrixBrightness = 7.5;
int xPos = 0;
int yPos = 0;
int xLastPos = 0;
int yLastPos = 0;
int xValue, yValue;

const int lowerThresholdX = 400, lowerThresholdY = 400;
const int upperThresholdX = 650, upperThresholdY = 650;

const int startUpTime = 2000;
int startUpAt = 0;
const int debounceTime = 300, debounceTimeBack = 500;
const int shootDebounceTime = 500;
const int second = 1000;
const int bulletBlinkingTime = 100, bulletSpeed = 200, playerBlinkingTime = 400;
unsigned long lastBulletMove = 0;
unsigned long lastBulletBlink = 0, lastPlayerBlink = 0, lastBulletSound = 0, lastHitSound = 0;
bool bulletState = 0, playerState = 0;
const int maxBulletTravel = 15;

// const byte matrixSize = 8;
bool matrixChanged = true;

bool menuDisplayed = false, waitingForInput = false, finished = false, lost = false, playDestroySound = false, playShootSound = false;
int automaticBrightness = false;
bool selectedName = false, selectName = false;
bool inMenu = true, standby = false;
int selected = 0, option = 0;
int soundOn = true;
bool start = 0, uncovered = 0;
int noWalls = 0, initialNoWalls = 0, streak = 0;
unsigned long streakStart = 0, streakTime = 900;
unsigned long startTime = 0;

const int menu = 0, play = 1, easy = 10, medium = 11, hard = 12, settings = 2, setLCDBrightness = 20, lcdLow = 200, lcdMed = 201, lcdHigh = 202, setMatrixBrightness = 21, matrixLow = 210, matrixMed = 211, matrixHigh = 212, matrixAuto = 213, soundSettings = 22, about = 3, expandedAboutGameName = 30, expandedAboutCreatorName = 31, expandedAboutGitHub = 32, expandedAboutLinkedin = 33, nameSelect = 4, howToPlay = 5, leaderBoard = 6, firstHighScore = 60, secondHighScore = 61, thirdHighScore = 62;
const int select = 10;
const unsigned long easyTime = 90 * second, mediumTime = 60 * second, hardTime = 30 * second;
const int mediumScoreMultiplier = 2, hardScoreMultiplier = 3;
int scoreMultiplier = 1;
const int easyLives = 5, mediumLives = 4, hardLives = 3;
int lives = 3, timerIndex = 0, scoreIndex = 0, currentScore = 0;
unsigned long roundTime = easyTime;
unsigned long lastUpdateTime = 0;
int menuNo = 4, aboutNo = 3, settingsNo = 2, lcdNo = 2, matrixNo = 3, highScoresNo = 2, difficultiesNo = 2;
bool lockMatrix = false;

const int soundFrequencies = 3;
int currentFrequency = 0;
int bulletSoundFrequencies[] = {800, 1000, 1200};
int bulletSoundDurations[] = {80, 50, 40};
int wallHitSoundFrequencies[] = {600, 400, 200};
int wallHitSoundDurations[] = {40, 50, 60};

struct direction {
    int x, y;
};
direction up = {0, -1};
direction down = {0, 1};
direction left = {-1, 0};
direction right = {1, 0};
direction currentDirection = {0, 0};
direction currentPlayerPosition =  {0, 0};
direction currentRoom = {0, 0};

Player currentPlayer;
Player highScores[3];
bool newHighScore = false;

byte matrix00[matrixSize][matrixSize] = {
  {2, 2, 2, 0, 0, 2, 2, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 2, 2, 0, 0, 2, 2, 2}  
};
byte matrix01[matrixSize][matrixSize] = {
  {2, 2, 2, 0, 0, 2, 2, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 2, 2, 0, 0, 2, 2, 2}  
};
byte matrix10[matrixSize][matrixSize] = {
  {2, 2, 2, 0, 0, 2, 2, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 2, 2, 0, 0, 2, 2, 2}  
};
byte matrix11[matrixSize][matrixSize] = {
  {2, 2, 2, 0, 0, 2, 2, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 2, 2, 0, 0, 2, 2, 2}  
};

byte *currentMatrix = matrix00[0];

void updateMatrix();
void pulseMatrix();
void printMenu(int subMenu = 0);
void selectInMenu(bool fromJoystick = false);
void inGameLCD(bool startNow = false);

class Bullet {
    int xPos, yPos;
    int xLastPos, yLastPos;
    direction dir;
    int currentTravel;

    public:
        Bullet(int xPos, int yPos, direction dir, int currentTravel = 0) {
            this->xPos = xPos;
            this->yPos = yPos;
            this->dir = dir;
            this->currentTravel = currentTravel;

            // Serial.println("Bullet created");
            playShootSound = true;
        }
        Bullet& operator=(const Bullet& other) {
            direction position = other.getPosition();
            this->xPos = position.x;
            this->yPos = position.y;
            position = other.getLastPosition();
            this->xLastPos = position.x;
            this->yLastPos = position.y;
            this->dir = other.getDirection();
            return *this;
        }
        direction getPosition() {
            return {xPos, yPos};
        }
        direction getLastPosition() {
            return {xLastPos, yLastPos};
        }
        direction getDirection() {
            return this->dir;
        }

        bool move() {
            xLastPos = xPos;
            yLastPos = yPos;
            xPos = (xPos + dir.x);
            yPos = (yPos + dir.y);
            if(xPos >= matrixSize) {
                // matrix[xPos][yPos] = 0;
                xPos = 0;
                updateMatrix();
                // return 0;
            }
            if(yPos >= matrixSize) {
                // matrix[xPos][yPos] = 0;
                yPos = 0;
                updateMatrix();
                // return 0;
            }
            if(xPos < 0) {
                // matrix[xPos][yPos] = 0;
                xPos = matrixSize - 1;
                updateMatrix();
                // return 0;
            }
            if(yPos < 0) {
                // matrix[xPos][yPos] = 0;
                yPos = matrixSize - 1;
                updateMatrix();
                // return 0;
            }

            if((currentMatrix + xPos * matrixSize)[yPos] == 1) {
                playDestroySound = true;
                if(xPos == currentPlayerPosition.x && yPos == currentPlayerPosition.y) {
                    lcd.setCursor(4 + lives - 1, 0);
                    lcd.write(byte(6));
                    lives--;
                }
                else {
                    noWalls--;
                    if(millis() - streakStart < streakTime) {
                        streak++;
                        lcd.setCursor(15, 0);
                        lcd.print(streak);
                        // pulseMatrix();
                        streakStart = millis();
                    }
                    else {
                        currentScore += streak * scoreMultiplier;
                        streak = 0;
                        lcd.setCursor(15, 0);
                        lcd.print(streak);
                    }
                    streakStart = millis();
                    currentScore += scoreMultiplier;
                    lcd.setCursor(scoreIndex + 1, 1);
                    lcd.print(currentScore);
                }
                // Serial.println(noWalls);
                (currentMatrix + xPos * matrixSize)[yPos] = 0;
                (currentMatrix + xLastPos * matrixSize)[yLastPos] = 0;
                updateMatrix();
                return 0;
            }
            if((currentMatrix + xPos * matrixSize)[yPos] == 2 || (currentMatrix + xPos * matrixSize)[yPos] == 3) {
                playDestroySound = true;
                (currentMatrix + xLastPos * matrixSize)[yLastPos] = 0;
                updateMatrix();
                return 0;
            }
            if(currentTravel == maxBulletTravel) {
                (currentMatrix + xLastPos * matrixSize)[yLastPos] = 0;
                updateMatrix();
                return 0;
            }

            currentTravel++;

            updateMatrix();
            return 1;
        }
        void blink(bool state) {
            lc.setLed(0, xPos, yPos, state);
        }
};

class BulletNode {
    Bullet *bullet;
    BulletNode* next;

    public:
        BulletNode(Bullet* bullet) {
            this->bullet = bullet;
            this->next = NULL;
        }
        void setNext(BulletNode* next) {
            this->next = next;
        }
        BulletNode* getNext() {
            return this->next;
        }
        Bullet* getBullet() {
            return this->bullet;
        }
        ~BulletNode() {
            delete this->bullet;
        }
};

class BulletList {
    BulletNode* head;
    BulletNode* tail;

    public:
        BulletList() {
            this->head = NULL;
            this->tail = NULL;
        }
        BulletNode* getHead() {
            return this->head;
        }
        BulletNode* getTail() {
            return this->tail;
        }
        void addNode(BulletNode* node) {
            if(this->head == NULL) {
                this->head = node;
                this->tail = node;
            }
            else {
                this->tail->setNext(node);
                this->tail = node;
            }
        }
        void removeNode(BulletNode* node) {
            if(node == this->head) {
                this->head = this->head->getNext();
                delete node;
            }
            else {
                BulletNode* prev = this->head;
                while(prev->getNext() != node) {
                    prev = prev->getNext();
                }
                prev->setNext(node->getNext());
                delete node;
            }
        }
};

BulletList bullets;

void setup() {
    // putDummyHighscores();
    Serial.begin(9600);
    lcd.begin(16, 2);

    createLCDChars();

    getSettings();
    getHighScores();

    lc.shutdown(0, false);
    lc.setIntensity(0, matrixBrightness);
    lc.clearDisplay(0);
    coverMatrix();

    randomSeed(analogRead(A2));
    randomStartPos();
    (currentMatrix + xPos * matrixSize)[yPos] = 1;
    generateWalls();
    // updateMatrix();

    pinMode(pinX, INPUT);
    pinMode(pinY, INPUT);
    pinMode(pinSW, INPUT_PULLUP);
    pinMode(lcdBacklight, OUTPUT);

    analogWrite(lcdBacklight, lcdBrightness);
    lcd.setCursor(0, 0);
    lcd.print("Hello, player!");
    lcd.setCursor(0, 2);
    lcd.print("Remember:be fast!");
    startUpAt = millis();
    option = 0;
    selected = 0;
}

void loop() {
    if(millis() - startUpAt < startUpTime) {
        return;
    }

    if(standby) {
        if(digitalRead(pinSW) == 1 && millis() - lastChangeSW > debounceTime) {
            start = 0;
            standby = false;
            inMenu = true;
            menuDisplayed = false;
        }
    }
    if(lives == 0 && !finished) {
        standby = true;
        coverMatrix();
        Serial.println("You lost!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("You lost!");
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print(currentScore);
        resetBoard();
    }

    if(!menuDisplayed && !start) {
        selected = 0;
        printMenu();
    }

    if(automaticBrightness) {
        matrixBrightness = map(analogRead(pinLDR), 0, 1023, 0, 15);
        lc.setIntensity(0, matrixBrightness);
    }

    if(start && selectedName) {
        playGame();
    }

    if(start && !selectedName) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Name not set");
        lcd.setCursor(0, 1);
        lcd.print("Playing as aaa");
        selectedName = true;
        for(int i = 0; i < 3; i++) {
            currentPlayer.name[i] = 'a';
        }
    }

    if(selectName == true) {
        inputName();
    }

    if(playDestroySound && soundOn) {
        destroySound();
    }

    if(playShootSound && soundOn) {
        shootSound();
    }
    if (!start && inMenu) {
        readJoystick();
        navigateMenu();
        selectInMenu();
    }

    analogWrite(lcdBacklight, lcdBrightness);
}

void playGame() {
    if(!uncovered) {
        coverMatrix();
        uncoverMatrix();
        uncovered = 1;
        Serial.println("Game started");
        inGameLCD(true);
    }

    if(millis() - lastUpdateTime > second) {
        lastUpdateTime = millis();
        lcd.setCursor(timerIndex + 1, 0);
        lcd.print("   ");
        lcd.setCursor(timerIndex + 1, 0);
        lcd.print((roundTime - (millis() - startTime)) / second);
        lcd.setCursor(15, 0);
        if(millis() - streakStart > streakTime) {
            streak = 0;
        }
        lcd.print(streak);
        // Serial.print(F("Time left: "));
        // Serial.println((roundTime - (millis() - startTime)) / second);
        if((roundTime - (millis() - startTime)) / second == 6) {
            last5Seconds();
        }
        if((roundTime - (millis() - startTime)) / second == 5) {
            inGameLCD();
        }

        if((roundTime - (millis() - startTime)) / second == 0) {
            standby = true;
            coverMatrix();
            displayAnimation(flippedTrophyMatrix);
            currentPlayer.score = currentScore;
            Serial.print(F("Congrats, you finished in "));
            Serial.print((millis() - startTime) / second);
            Serial.println(" seconds");
            animateLCD(3);
            Serial.println("Time's up!");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Time's up!");
            lcd.setCursor(0, 1);
            lcd.print("Score: ");
            lcd.print(currentScore);

            checkHighScores();
            Serial.print(F("Going into reset"));
            resetBoard();
        }
    }
    
    if(noWalls == 0 && !finished) {
        currentPlayer.score = currentScore;

        checkHighScores();

        standby = true;
        coverMatrix();
        displayAnimation(flippedTrophyMatrix);
        resetBoard();
        Serial.print(F("Congrats, "));
        Serial.print(currentPlayer.name);
        Serial.print(F(" you finished in "));
        Serial.print((millis() - startTime) / second);
        Serial.println(" seconds");
    }

    blinkLEDs();
    readJoystick();
    actOnJoystick();
    actOnSW();
    bulletsTravel();
}

void inputName() {
    lcd.setCursor(1, 1);
    lcd.print(alphabet[playerName[0]]);
    lcd.print(alphabet[playerName[1]]);
    lcd.print(alphabet[playerName[2]]);

    readJoystick();
    if(xValue < lowerThresholdX && millis() - lastChangeX > inputDebounceTime) {
        lastChangeX = millis();
        if(playerName[playerNameSelectIndex] == 0) {
            playerName[playerNameSelectIndex] = alphabetLength - 1;
        }
        else {
            playerName[playerNameSelectIndex]--;
        }
        Serial.print(F("values: "));
        Serial.print(playerName[0]);
        Serial.print(playerName[1]);
        Serial.println(playerName[2]);
    }
    else if(xValue > upperThresholdX && millis() - lastChangeX > inputDebounceTime) {        
        lastChangeX = millis();
        if(playerName[playerNameSelectIndex] == alphabetLength - 1) {
            playerName[playerNameSelectIndex] = 0;
        }
        else {
            playerName[playerNameSelectIndex]++;
        }
        Serial.print(F("values: "));
        Serial.print(playerName[0]);
        Serial.print(playerName[1]);
        Serial.println(playerName[2]);
    }
    if(yValue < lowerThresholdY && millis() - lastChangeY > inputDebounceTime) {
        lastChangeY = millis();
        playerNameSelectIndex--;
        if(playerNameSelectIndex < 0) {
            playerNameSelectIndex = 2;
        }
    }
    else if(yValue > upperThresholdY && millis() - lastChangeY > inputDebounceTime) {
        lastChangeY = millis();
        playerNameSelectIndex++;
        if(playerNameSelectIndex > 2) {
            playerNameSelectIndex = 0;
        }
    }

    if(digitalRead(pinSW) == 1 && millis() - lastChangeSW > debounceTime) {
        for(int i = 0; i < 3; i++) {
            currentPlayer.name[i] = alphabet[playerName[i]];
        }
        lastChangeSW = millis();
        selectedName = true;
        selectName = false;
        inMenu = true;
        Serial.print(F("Selected name: "));
        Serial.print(alphabet[playerName[0]]);
        Serial.print(alphabet[playerName[1]]);
        Serial.println(alphabet[playerName[2]]);
    }
}

void destroySound() {
    tone(buzzerPin, wallHitSoundFrequencies[currentFrequency], wallHitSoundDurations[currentFrequency]);
    if(millis() - lastHitSound > wallHitSoundDurations[currentFrequency]) {
        lastHitSound = millis();
        noTone(buzzerPin);
        currentFrequency++;
        if(currentFrequency == soundFrequencies) {
            currentFrequency = 0;
            playDestroySound = false;
        }
    }
}

void shootSound() {
    tone(buzzerPin, bulletSoundFrequencies[currentFrequency], bulletSoundDurations[currentFrequency]);
    if(millis() - lastBulletSound > bulletSoundDurations[currentFrequency]) {
        lastBulletSound = millis();
        noTone(buzzerPin);
        currentFrequency++;
        if(currentFrequency == soundFrequencies) {
            currentFrequency = 0;
            playShootSound = false;
        }
    }
}


void blinkLEDs() {
    if(millis() - lastPlayerBlink > playerBlinkingTime) {
        lastPlayerBlink = millis();
        playerState = !playerState;
        lc.setLed(0, currentPlayerPosition.x, currentPlayerPosition.y, playerState);
    }
    if(millis() - lastBulletBlink > bulletBlinkingTime) {
        lastBulletBlink = millis();
        bulletState = !bulletState;
        BulletNode* node = bullets.getHead();
        while(node != NULL) {
            node->getBullet()->blink(bulletState);
            node = node->getNext();
        }
    }
}

void coverMatrix() {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, true);
            delay(25);
        }
    }
}

void uncoverMatrix() {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            if((currentMatrix + row * matrixSize)[col] == 0) {
                lc.setLed(0, row, col, false);
            }
            delay(25);
        }
    }
}

void readJoystick() {
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
}

void actOnJoystick() {
    if(xValue > upperThresholdX && millis() - lastChangeX > debounceTime) {
        lastChangeX = millis();
        currentDirection = up;
        move(up);
    }
    else if(xValue < lowerThresholdX && millis() - lastChangeX > debounceTime) {        
        lastChangeX = millis();
        currentDirection = down;
        move(down);
    }
    if(yValue > upperThresholdY && millis() - lastChangeY > debounceTime) {
        lastChangeY = millis();
        currentDirection = right;
        move(right);
    }
    else if(yValue < lowerThresholdY && millis() - lastChangeY > debounceTime) {
        lastChangeY = millis();
        currentDirection = left;
        move(left);
    }
}

void navigateMenu() {
    if(option == 10) {
        menuNo = difficultiesNo;
    }
    else if(option == 20) {
        menuNo = settingsNo;
    }
    else if(option == 30) {
        menuNo = aboutNo;
    }
    else if(option == 60) {
        menuNo = highScoresNo;
    }
    else if(option == 200 || option == 210) {
        menuNo = option == 200 ? lcdNo : matrixNo;
    }
    else {
        menuNo = 6;
    }
    if(xValue < lowerThresholdX && millis() - lastChangeX > debounceTime) {
        lastChangeX = millis();
        selected++;
        if(selected > menuNo) {
            selected = 0;
        }
        Serial.print(F("Navigating to: "));
        Serial.println(option + selected);
        printMenu(option + selected);
    }
    else if(xValue > upperThresholdX && millis() - lastChangeX > debounceTime) {        
        lastChangeX = millis();
        selected--;
        if(selected < 0) {
            selected = menuNo;
        }
        Serial.print(F("Navigating to: "));
        Serial.println(option + selected);
        printMenu(option + selected);
    }

    if(yValue < lowerThresholdY && millis() - lastChangeY > debounceTimeBack) {
        if(option != 0) {
            selected = (option / 10) % 10;
            option /= 100;
            printMenu(option + selected);
        }
    }
    else if(selected != 1 && yValue > upperThresholdY && millis() - lastChangeY > debounceTime) {
        selectInMenu(true);
    }
}

void actOnSW() {
    if(digitalRead(pinSW) == 1 && millis() - lastChangeSW > shootDebounceTime) {
        lastChangeSW = millis();

        Bullet* bullet = new Bullet(currentPlayerPosition.x, currentPlayerPosition.y, currentDirection);
        BulletNode* node = new BulletNode(bullet);
        bullets.addNode(node);
    }
}

void selectInMenu(bool fromJoystick = false) {
    if((digitalRead(pinSW) == 1 || fromJoystick) && millis() - lastChangeSW > debounceTime) {
        Serial.print(F("Selecting in menu: selected = "));
        Serial.print(selected);
        Serial.print(F(", option = "));
        Serial.println(option);
        lcd.createChar(4, downwardArrow);
        lastChangeSW = millis();
        if(option == 0 && selected == 0 || option / 10 == 3) {
            return;
        }
        else if(option == 0 || option / 10 < 10 || option == 200 || option == 210) {
            option = ((option / 10) * 10 + selected) * 10;
            selected = 0;
            printMenu(option + selected);
        }
    }
}

void inGameLCD(bool startNow = false) {
    lcd.clear();
    int setLives = 0;
    lcd.setCursor(0, 1);
    if(scoreMultiplier == 1) {
        lcd.print("Easy ");
        timerIndex = 10;
        scoreIndex = 11;
        setLives = easyLives;
    }
    else if(scoreMultiplier == 2) {
        lcd.print("Medium ");
        timerIndex = 9;
        scoreIndex = 13;
        setLives = mediumLives;
    }
    else {
        lcd.print("Hard ");
        timerIndex = 8;
        scoreIndex = 11;
        setLives = hardLives;
    }
    lcd.print("Score: ");
    lcd.print(currentScore);


    lcd.setCursor(0, 0);
    for(int i = 0; i < 3; i++) {
        lcd.print(currentPlayer.name[i]);
    }
    // lcd.print(currentPlayer.name);
    lcd.setCursor(4, 0);
    for(int i = 0; i < lives; i++) {
        lcd.write(byte(5));
    }
    if(lives < setLives) {
        lcd.setCursor(4 + lives, 0);
        lcd.write(byte(6));
    }
    for(int i = 0; i < setLives - lives; i++) {
        lcd.write(byte(7));
    }

    lcd.print(" ");
    if(startNow)
        startTime = millis();
    lcd.write(byte(0));
    lcd.print((roundTime - (millis() - startTime)) / second);
    lastUpdateTime = millis();

    lcd.setCursor(14, 0);
    lcd.print("X0");
}

void bulletsTravel() {
    if(millis() - lastBulletMove > bulletSpeed) {
        lastBulletMove = millis();
        BulletNode* node = bullets.getHead();
        while(node != NULL) {
            if(node->getBullet()->move() == 0) {
                bullets.removeNode(node);
            }
            node = node->getNext();
        }
    }
}

void move(direction dir) {
    xPos = currentPlayerPosition.x + dir.x;
    yPos = currentPlayerPosition.y + dir.y;
    if(xPos < 0) {
        xPos = matrixSize - 1;
        currentRoom.y--;
        if(currentRoom.y < 0) {
            currentRoom.x == 0 ? currentMatrix = matrix01[0] : currentMatrix = matrix11[0];
            currentRoom.y = 1;
        }
        else {
            currentRoom.x == 0 ? currentMatrix = matrix00[0] : currentMatrix = matrix10[0];
        }
        Serial.print("In room: ");
        Serial.print(currentRoom.x);
        Serial.print(", ");
        Serial.println(currentRoom.y);
    }
    if(yPos < 0) {
        yPos = matrixSize - 1;
        currentRoom.x--;
        if(currentRoom.x < 0) {
            currentRoom.y == 0 ? currentMatrix = matrix10[0] : currentMatrix = matrix11[0];
            currentRoom.x = 1;
        }
        else {
            currentRoom.y == 0 ? currentMatrix = matrix00[0] : currentMatrix = matrix01[0];
        }
        Serial.print("In room: ");
        Serial.print(currentRoom.x);
        Serial.print(", ");
        Serial.println(currentRoom.y);
    }
    if(xPos > matrixSize - 1) {
        xPos = 0;
        currentRoom.y++;
        if(currentRoom.y > roomSize - 1) {
            currentRoom.x == 0 ? currentMatrix = matrix00[0] : currentMatrix = matrix10[0];
            currentRoom.y = 0;
        }
        else {
            currentRoom.x == 0 ? currentMatrix = matrix01[0] : currentMatrix = matrix11[0];
        }
        Serial.print("In room: ");
        Serial.print(currentRoom.x);
        Serial.print(", ");
        Serial.println(currentRoom.y);
        
    }
    if(yPos > matrixSize - 1) {
        yPos = 0;
        currentRoom.x++;
        if(currentRoom.x > roomSize - 1) {
            currentRoom.y == 0 ? currentMatrix = matrix00[0] : currentMatrix = matrix01[0];
            currentRoom.x = 0;
        }
        else {
            currentRoom.y == 0 ? currentMatrix = matrix10[0] : currentMatrix = matrix11[0];
        }
        Serial.print("In room: ");
        Serial.print(currentRoom.x);
        Serial.print(", ");
        Serial.println(currentRoom.y);
    }

    if((currentMatrix + xPos * matrixSize)[yPos]) {
        return;
    }

    // currentPlayerPosition.x = xPos;
    // currentPlayerPosition.y = yPos;

    (currentMatrix + xPos * matrixSize)[yPos] = 1;
    (currentMatrix + currentPlayerPosition.x * matrixSize)[currentPlayerPosition.y] = 0;
    updateMatrix();
    currentPlayerPosition.x = xPos;
    currentPlayerPosition.y = yPos;
}

void updateMatrix() {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, (currentMatrix + row * matrixSize)[col]);
        }
    }
}

void randomStartPos() {
    randomSeed(analogRead(A2));

    xPos = random() % matrixSize;
    yPos = random() % matrixSize;

    if((currentMatrix + xPos * matrixSize)[yPos]) {
        randomStartPos();
    }
    else {
        (currentMatrix + xPos * matrixSize)[yPos] = 1;
    }

    currentPlayerPosition.x = xPos;
    currentPlayerPosition.y = yPos;
}

void generateWalls() {
    // before room walls:
    // 8x8 matrix => 64 cells
    // 50% - 75% walls => 32 - 48 walls

    // room walls take up 20 cells out of the 64
    // free cells = 64 - 20 = 44
    // 50% - 75% walls => 22 - 33 walls
    randomSeed(analogRead(A2));

    // noWalls = random() % 17 + 32;
    noWalls = random() % 11 + 22;
    // noWalls = 2;
    // noWalls = 0;
    initialNoWalls = noWalls * 4;
    for(int i = 0; i < noWalls; i++) {
        int x = random() % matrixSize;
        int y = random() % matrixSize;

        if(matrix00[x][y]) {
            i--;
        }
        else{
            matrix00[x][y] = 1;
            matrix01[x][y] = 1;
            matrix10[x][y] = 1;
            matrix11[x][y] = 1;
        }
    }
    noWalls *= 4;
}

void printMenu(int subMenu = 0) {
    lcd.clear();
    switch(subMenu) {
        case menu:
            Serial.println("Main menu:");
            lcd.setCursor(0, 0);
            lcd.print("Rapid Shootout");
            lcd.write(byte(7));
            menuDisplayed = true;
            break;
        case play:
            lcd.createChar(4, playButton);
            lcd.setCursor(0, 0);
            lcd.print("> Play ");
            lcd.write(byte(4));
            lcd.print("       ");
            lcd.write(byte(7));
            Serial.println("Play");
            break;
        case easy:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Play");
            lcd.print("       ");
            lcd.write(byte(7));
            lcd.setCursor(1, 1);
            lcd.print("Easy         ");
            break;
        case easy * select:
            roundTime = easyTime;
            Serial.print(F("Round time: "));
            Serial.println(roundTime);
            scoreMultiplier = 1;
            start = 1;
            finished = 0;
            lost = 0;
            currentScore = 0;
            lives = easyLives;
            startTime = millis();
            inMenu = false;
            break;
        case medium:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Play");
            lcd.print("       ");
            lcd.write(byte(7));
            lcd.setCursor(1, 1);
            lcd.print("Medium       ");
            break;
        case medium * select:
            roundTime = mediumTime;
            Serial.print(F("Round time: "));
            Serial.println(roundTime);
            scoreMultiplier = mediumScoreMultiplier;
            start = 1;
            finished = 0;
            lost = 0;
            currentScore = 0;
            lives = mediumLives;
            startTime = millis();
            inMenu = false;
            break;
        case hard:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Play");
            lcd.print("       ");
            lcd.write(byte(7));
            lcd.setCursor(1, 1);
            lcd.print("Hard         ");
            break;
        case hard * select:
            roundTime = hardTime;
            Serial.print(F("Round time: "));
            Serial.println(roundTime);
            scoreMultiplier = hardScoreMultiplier;
            start = 1;
            finished = 0;
            lost = 0;
            currentScore = 0;
            lives = hardLives;
            startTime = millis();
            inMenu = false;
            break;
        case settings:
            lcd.setCursor(0, 0);
            lcd.print("> Settings ");
            lcd.write(byte(2));
            lcd.print("   ");
            Serial.println("Settings");
            lcd.write(byte(7));
            waitingForInput = true;
            break;
        case setLCDBrightness:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Settings ");
            lcd.write(byte(2));
            lcd.setCursor(1, 1);
            lcd.print("LCD Glow     ");
            lcd.write(byte(7));
            Serial.println("LCD Glow");
            break;
        case lcdLow:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" LCD Glow");
            lcd.setCursor(1, 1);
            lcd.print("Low          ");
            lcd.write(byte(7));
            break;
        case lcdLow * select:
            lcdBrightness = 100;
            EEPROM.put(lcdBrightnessAddress, lcdBrightness);
            // setLcdBrightness();
            option = lcdLow;
            selected = lcdLow % 10;
            printMenu(option + selected);
            break;
        case lcdMed:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" LCD Glow");
            lcd.setCursor(1, 1);
            lcd.print("Medium       ");
            lcd.write(byte(7));
            break;
        case lcdMed * select:
            lcdBrightness = 190;
            EEPROM.put(lcdBrightnessAddress, lcdBrightness);
            // setLcdBrightness();
            option = lcdLow;
            selected = lcdMed % 10;
            printMenu(option + selected);
            break;
        case lcdHigh:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" LCD Glow");
            lcd.setCursor(1, 1);
            lcd.print("High         ");
            lcd.write(byte(7));
            break;
        case lcdHigh * select:
            lcdBrightness = 254;
            EEPROM.put(lcdBrightnessAddress, lcdBrightness);
            // setLcdBrightness();
            option = lcdLow;
            selected = lcdHigh % 10;
            printMenu(option + selected);
            break;
        case setMatrixBrightness:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Settings ");
            lcd.write(byte(2));
            lcd.setCursor(1, 1);
            lcd.print("Matrix Glow  ");
            lcd.write(byte(7));
            Serial.println("Matrix Glow");
            break;
        case matrixLow:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Matrix Glow");
            lcd.setCursor(1, 1);
            lcd.print("Low          ");
            lcd.write(byte(7));
            break;
        case matrixLow * select:
            automaticBrightness = false;
            matrixBrightness = 2;
            EEPROM.put(matrixBrightnessAddress, matrixBrightness);
            lc.setIntensity(0, matrixBrightness);
            option = matrixLow;
            selected = matrixLow % 10;
            printMenu(option + selected);
            break;
        case matrixMed:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Matrix Glow");
            lcd.setCursor(1, 1);
            lcd.print("Medium       ");
            lcd.write(byte(7));
            break;
        case matrixMed * select:
            automaticBrightness = false;
            matrixBrightness = 7.5;
            EEPROM.put(matrixBrightnessAddress, matrixBrightness);
            lc.setIntensity(0, matrixBrightness);
            option = matrixLow;
            selected = matrixMed % 10;
            printMenu(option + selected);
            break;
        case matrixHigh:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Matrix Glow");
            lcd.setCursor(1, 1);
            lcd.print("High         ");
            lcd.write(byte(7));
            break;
        case matrixHigh * select:
            automaticBrightness = false;
            matrixBrightness = 15;
            EEPROM.put(matrixBrightnessAddress, matrixBrightness);
            lc.setIntensity(0, matrixBrightness);
            option = matrixLow;
            selected = matrixHigh % 10;
            printMenu(option + selected);
            break;
        case matrixAuto:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Matrix Glow");
            lcd.setCursor(1, 1);
            lcd.print("Automatic    ");
            lcd.write(byte(7));
            break;
        case matrixAuto * select:
            automaticBrightness = !automaticBrightness;
            EEPROM.put(matrixBrightnessAutoAddress, automaticBrightness);
            // Serial.print(F("Automatic brightness: "));
            // Serial.println(automaticBrightness);
            option = matrixLow;
            selected = matrixAuto % 10;
            printMenu(option + selected);
            break;
        case soundSettings:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Settings ");
            lcd.write(byte(2));
            lcd.setCursor(1, 1);
            lcd.print("Sound ");
            lcd.print(soundOn ? "Off    " : "On     ");
            lcd.write(byte(7));
            Serial.println("Sound");
            break;
        case soundSettings * select:
            soundOn = !soundOn;
            EEPROM.put(soundSettingsAddress, soundOn);
            option = setLCDBrightness;
            selected = soundSettings % 10;
            printMenu(option + selected);
            break;
        case about:
            lcd.setCursor(0, 0);
            lcd.print("> About ");
            lcd.write(byte(5));
            lcd.print("      ");
            lcd.write(byte(7));
            Serial.println("About");
            break;
        case expandedAboutGameName:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" About");
            lcd.print("      ");
            // lcd.write(byte(7));
            lcd.setCursor(1, 1);
            lcd.print("Rapid Shootout");
            lcd.write(byte(7));
            break;
        case expandedAboutCreatorName:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" About");
            lcd.print("      ");
            // lcd.write(byte(7));
            lcd.setCursor(1, 1);
            lcd.print("Ioan Anghel   ");
            lcd.write(byte(7));
            break;
        case expandedAboutGitHub:
            lcd.createChar(3, github);
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" About");
            lcd.print("      ");
            // lcd.write(byte(7));
            lcd.setCursor(1, 1);
            lcd.write(byte(3));
            lcd.print(" ioananghel  ");
            lcd.write(byte(7));
            break;
        case expandedAboutLinkedin:
            lcd.createChar(3, linkedin);
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" About");
            lcd.print("      ");
            // lcd.write(byte(7));
            lcd.setCursor(1, 1);
            lcd.write(byte(3));
            lcd.print(" ioananghel  ");
            lcd.write(byte(7));
            break;
        case nameSelect:
            lcd.setCursor(0, 0);
            lcd.print("> Select Name ");
            lcd.write(byte(5));
            lcd.write(byte(7));
            Serial.println("Name");
            break;
        case nameSelect * select:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Select Name");
            lcd.setCursor(13, 1);
            lcd.print("<");
            lcd.write(byte(7));
            lcd.print(">");
            selectName = true;
            inMenu = false;
            break;
        case howToPlay:
            lcd.setCursor(0, 0);
            lcd.print("> How to play ");
            lcd.write(byte(5));
            lcd.write(byte(7));
            Serial.println("How to play");
            break;
        case howToPlay * select:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" How to play");
            lcd.setCursor(13, 1);
            lcd.print("<");
            lcd.write(byte(7));
            lcd.print(">");
            Serial.println("How to play");
            break;
        case leaderBoard:
            lcd.setCursor(0, 0);
            lcd.print("> Leaderboard ");
            lcd.write(byte(1));
            lcd.write(byte(7));
            Serial.println("Leaderboard");
            break;
        case firstHighScore:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Leaderboard");
            lcd.setCursor(1, 1);
            lcd.print("1. ");
            lcd.print(highScores[2].name);
            lcd.print(" ");
            lcd.print(highScores[2].score);
            lcd.print(" ");
            lcd.write(byte(7));
            break;
        case secondHighScore:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Leaderboard");
            lcd.setCursor(1, 1);
            lcd.print("2. ");
            lcd.print(highScores[1].name);
            lcd.print(" ");
            lcd.print(highScores[1].score);
            lcd.print(" ");
            lcd.write(byte(7));
            break;
        case thirdHighScore:
            lcd.setCursor(0, 0);
            lcd.write(byte(4));
            lcd.print(" Leaderboard");
            lcd.setCursor(1, 1);
            lcd.print("3. ");
            lcd.print(highScores[0].name);
            lcd.print(" ");
            lcd.print(highScores[0].score);
            lcd.print(" ");
            lcd.write(byte(7));
            break;
        default:
            Serial.print(F("Invalid options: "));
            Serial.println(subMenu);
            Serial.print(F("\n"));
            break;
    }
}

void displayAnimation(byte matrix[matrixSize][matrixSize]) {
    for(int row = 0; row < matrixSize; row++) {
        for(int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, matrix[row][col]);
       }
    }
}

void resetBoard() {
    for(int row = 0; row < matrixSize; row++) {
        for(int col = 0; col < matrixSize; col++) {
            if((currentMatrix + row * matrixSize)[col] != 2) {
                (currentMatrix + row * matrixSize)[col] = 0;
            }
        }
    }

    // menuDisplayed = 0;
    uncovered = 0;
    finished = 1;
    option = 0;
    selected = 0;
    start = 0;
    lockMatrix = false;
    randomSeed(analogRead(A2));
    randomStartPos();
    (currentMatrix + xPos * matrixSize)[yPos] = 1;
    generateWalls();
}

void animateLCD(int ownChar) {
    lcd.clear();
    int lcdRows = 2, lcdCols = 16;
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < lcdCols; j++) {

            lcd.setCursor(j, i);
            lcd.write(byte(ownChar));
            delay(50);
        }
    }
    lcd.clear();
}

// void setLcdBrightness() {
//     analogWrite(lcdBacklight, lcdBrightness);
// }

void getSettings() {
    EEPROM.get(matrixBrightnessAddress, matrixBrightness);
    Serial.print(F("Matrix brightness: "));
    Serial.println(matrixBrightness);
    EEPROM.get(matrixBrightnessAutoAddress, automaticBrightness);
    Serial.print(F("Automatic brightness: "));
    Serial.println(automaticBrightness);
    EEPROM.get(lcdBrightnessAddress, lcdBrightness);
    Serial.print(F("LCD brightness: "));
    Serial.println(lcdBrightness);
    EEPROM.get(soundSettingsAddress, soundOn);
    Serial.print(F("Sound: "));
    Serial.println(soundOn);
}

void createLCDChars() {
    lcd.createChar(0, timerChar);
    lcd.createChar(1, trophyChar);
    lcd.createChar(2, wrenchChar);
    lcd.createChar(3, amazedChar);
    // lcd.createChar(4, explosion1Step);
    // lcd.createChar(5, explosion2Step);
    // lcd.createChar(6, explosion3Step);
    // lcd.createChar(7, fullMatrix);
    lcd.createChar(4, playButton);
    lcd.createChar(5, heartChar);
    lcd.createChar(6, skullChar);
    lcd.createChar(7, upDownArrows);
}

void getHighScores() {
    int i = 0;
    for(int currentAddress = highScoresAddress; currentAddress < highScoresAddress + 3 * sizeof(Player); currentAddress += sizeof(Player)) {
        EEPROM.get(currentAddress, highScores[i++]);
    }
    for(int i = 0; i < 3; i++) {
      Serial.print(i);
      Serial.print(" este ");
      Serial.print(highScores[i].name);
    }
}

void putDummyHighscores() {
    Player dummy1 = {"AAA", 10};
    Player dummy2 = {"BBB", 11};
    Player dummy3 = {"CCC", 12};
    EEPROM.put(highScoresAddress, dummy1);
    EEPROM.put(highScoresAddress + sizeof(Player), dummy2);
    EEPROM.put(highScoresAddress + 2 * sizeof(Player), dummy3);
}

void checkHighScores() {
    Serial.print("Checcking ");
    Serial.print(currentPlayer.name);
    Serial.print(" ");
    Serial.println(currentPlayer.score);
    for(int i = 2; i >= 0; i--) {
        Serial.print(highScores[i].name);
        if(currentPlayer.score >= highScores[i].score) {
            Serial.println(" is higher");
            newHighScore = true;
            for(int j = 0; j < i; j++) {
                Serial.print("Moving ");
                Serial.print(highScores[j + 1].name);
                Serial.print(" ");
                Serial.println(highScores[j + 1].score);
                highScores[j] = highScores[j + 1];
            }
            highScores[i] = currentPlayer;
            break;
        }
    }
    if(newHighScore) {
        lcd.clear();
        animateLCD(1);
        lcd.setCursor(0, 0);
        lcd.print("New high score!");
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print(currentPlayer.score);
        newHighScore = false;
    }
    for(int i = 0; i < 3; i++) {
        Serial.print(highScores[i].name);
        Serial.print(" ");
        Serial.println(highScores[i].score);
        EEPROM.put(highScoresAddress + i * sizeof(Player), highScores[i]);
    }
    Serial.println("High scores updated");
}

void last5Seconds() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Last 5 seconds!");
    lcd.setCursor(0, 1);
    lcd.print("DOUBLE POINTS!");
    lcd.write(byte(3));
    scoreMultiplier *= 2;
    lockMatrix = true;

    shutMatrix();
}

void shutMatrix() {
    (currentMatrix)[3] = 3;
    (currentMatrix)[4] = 3;
    (currentMatrix + 3 * matrixSize)[0] = 3;
    (currentMatrix + 3 * matrixSize)[matrixSize - 1] = 3;
    (currentMatrix + 4 * matrixSize)[0] = 3;
    (currentMatrix + 4 * matrixSize)[matrixSize - 1] = 3;
    (currentMatrix + (matrixSize - 1) * matrixSize)[3] = 3;
    (currentMatrix + (matrixSize - 1) * matrixSize)[4] = 3;
}