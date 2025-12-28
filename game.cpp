#include <iostream>
#include <conio.h>
#include <ctime>
#include <windows.h>

//main game display array
//dimensions: 18x7
char gameScreen[126] = {
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' '     //Middle bottom (excluding the wall) col is 122
                                    //To move up a row 122 - 7
};


class Ship{
  protected:
    bool isHit = 0;
    int currPosition;

  public:

    int getCurrPosition(){
      return currPosition;
    }

    int checkHit(){
      return isHit;
    }

    int updatePosition(int newpos){
      currPosition = newpos;
    }

    virtual int shoot(){
      int bulletPos = currPosition + 7;
      return currPosition;
    }
};

//Player Ship
class HeroShip: public Ship{
  protected:

  public:
    HeroShip(){
      currPosition = 115;
    }

    ~HeroShip(){
    }

    int shoot(){
      int bulletPos = currPosition + 7;
      return currPosition;
    }

};

class Enemy: public Ship{
  protected:

  public:
    Enemy(){
      currPosition = 3; 
    }

    ~Enemy(){
    }
};

//grid banaune function
int makeGrid(int heroPos, int prevPos, int enemyPos, int prevEnemyPos, int score){
  std::cout << "SCORE: " << score << '\n';
  std::cout << "||";
  for(int i=0; i<126; i++){

    if((prevPos != heroPos) && (prevPos != 0)){
      gameScreen[prevPos] = ' ';
    }

    if((prevEnemyPos != enemyPos) && (prevEnemyPos != 0)){
      gameScreen[prevEnemyPos] = ' ';
    }

    gameScreen[heroPos] = 'H';
    gameScreen[enemyPos] = 'W';

    std::cout << gameScreen[i] << "    ";

    if((i+1) % 7 == 0){
      std::cout << '\b' << '\b' << '\b' << '\b';
      std::cout << "||";
      std::cout << '\n';
      if(i!=125){
        std::cout << "||";
      }
    }
  } 
}


int makeRandomNumber(){

  srand(time(NULL));

  int num1 = rand() % 5;
  int num2 = rand() % 3;
  int num3 = rand() % 2;

  return num1+num2;
}

void hideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void showCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = true; // Show the cursor
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void moveCursorToTopLeft() {
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    SetConsoleCursorPosition(hOut, coord);
}

int main(){

  Ship *pHero = new HeroShip; 
  Ship *pEnemy = new Enemy;

  char movementInput;

  int heroPosition;
  int enemyPosition;
  int prevPosition = 0;
  int prevEnemyPosition = 0;
  int bulletPosition;
  int isBulletShot = 0;
  int isGameOver = 0;
  int enemySpeed = 0;
  int score = 0;
  int explosionPosition = 125;
  int isHit = 0;

  hideCursor();

  while(1){

    moveCursorToTopLeft();

    heroPosition = pHero->getCurrPosition();

    enemyPosition = pEnemy->getCurrPosition();

    if(enemyPosition > 125){
      Beep(750,1500);
      system("cls");
      std::cout << "GAME OVER" << '\n';
      std::cout << "SCORE: " << score << '\n';
      isGameOver = 1;
      delete pEnemy;
      break;
    }

    if(enemyPosition == heroPosition){
      Beep(750,1500);
      system("cls");
      std::cout << "GAME OVER" << '\n';
      std::cout << "SCORE: " << score << '\n';
      isGameOver = 1;
      delete pEnemy;
      delete pHero;
      break;
    }


    // system("cls");

    makeGrid(heroPosition, prevPosition, enemyPosition, prevEnemyPosition, score);

    gameScreen[explosionPosition] = ' ';

    if(isHit){
      Beep(750,300);
      isHit = 0;
    }

    //bullet position calculating shit
    if(isBulletShot){
      if(bulletPosition<0){
        isBulletShot = 0;
        gameScreen[bulletPosition] = ' ';
      }else{
        bulletPosition = bulletPosition - 7;
        gameScreen[bulletPosition] = '^';
        gameScreen[bulletPosition + 7] = ' ';
      }
    }

    //key hit detector stuff
    if(kbhit()){
      movementInput = getch(); 

      switch (movementInput) {

        case 'a':
          if(heroPosition>112){
            pHero->updatePosition(heroPosition - 1);
            prevPosition = heroPosition;
          }
          break;

        case 'd':
          if(heroPosition<118){
            pHero->updatePosition(heroPosition + 1);
            prevPosition = heroPosition;
          }
          break;

        case 'j':
          if(!isBulletShot){
            bulletPosition = pHero->shoot();
            isBulletShot = 1;
          }
          break;

        default:
          break;
      }
    }

    if(enemySpeed % 3 == 0){
      pEnemy->updatePosition(enemyPosition + 7);
      prevEnemyPosition = enemyPosition;
      enemySpeed = 0;
    }

    enemySpeed = enemySpeed + 1;

    //HIT DETECTION(is glitchy sometimes)
    if(enemyPosition == bulletPosition){
      gameScreen[enemyPosition] = ' ';
      gameScreen[bulletPosition] = ' ';
      gameScreen[enemyPosition] = '*';
      explosionPosition = enemyPosition;
      pEnemy->updatePosition(makeRandomNumber());
      isHit = 1;
      score++;
    }
  }

  showCursor();

}
