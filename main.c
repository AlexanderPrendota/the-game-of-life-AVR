#include <stdlib.h>
#include <avr/io.h>
#include <time.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

//частота процессора
#define F_CPU 16000000UL 

//задержка отображения
#define DELAY 500
#define SIZE1 5
#define SIZE2 7

char col = 0;
char s_diod[SIZE1][SIZE2] = {
    {0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00}};

int pins[] = { PD2, PD3, PD4, PD5, PD6, PD7,
               PB0, PB1, PB2, PB3, PB4, PB5}; 

int stolb[5] = {pins[0], pins[1], pins[2], pins[3], pins[4]};

int str[7] = {pins[5], pins[6], pins[7], pins[8], pins[9], pins[10], pins[11]};

// массив для хранения текущего и предыдущего поля
char matrix[SIZE1][SIZE2][2]; 

 // вероятность рандома
long density = 55;

void setups_diod(); // инитиализация матрицы
void randomMatrix();
void display_st(int i); //  отобразить столбец

int main(void)
{
    setups_diod();
    srand(time(NULL));

    while(1)
    {
        for (int i = 0; i < SIZE1; i++) {
            for (int j = 0; j < SIZE2; j++) {
		// подготавливаем матрицу к отображению
                s_diod[i][j] = matrix[i][j][0];
            }
        }
        //матричный индикатор
        for (int i = 0; i < SIZE1; ++i) {
            display_st(i);
        }
	// задержка, чтобы юзер видил процесс игры
        _delay_ms(DELAY); 

        for (int x = 0; x < SIZE1; x++) {
            for (int y = 0; y < SIZE2; y++) {
            	// игра собственно)
                matrix[x][y][1] = matrix[x][y][0];
		// считаем соседей
                int count = sosed(x, y);
                if (count == 3 && matrix[x][y][0] == 0) {
		    // три пораждают еще одну
                    matrix[x][y][1] = 1;
                }
                if ((count < 2 || count > 3) && matrix[x][y][0] == 1) {
                // если их(соседей) не три то клетки умирают от перенаселения или от недостатка жизни 
                matrix[x][y][1] = 0;
            }
        }
   }

   for (int x = 0; x < SIZE1; x++) {
        for (int y = 0; y < SIZE2; y++) {
	    // копипастим
            matrix[x][y][0] = matrix[x][y][1];
        }
   }
}


   return 0;
}

int sosed(int x, int y) {
	// считаем соседей банальным суммированием всех клеток по кругу
  return matrix[(x + 1) % SIZE1][y][0] +
          matrix[x][(y + 1) % SIZE2][0] +
          matrix[(x + SIZE1 - 1) % SIZE2][y][0] +
          matrix[x][(y + SIZE1 - 1) % SIZE2][0] +
          matrix[(x + 1) % SIZE1][(y + 1) % SIZE2][0] +
          matrix[(x + SIZE1 - 1) % SIZE2][(y + 1) % SIZE1][0] +
          matrix[(x + SIZE2 - 1) % SIZE1][(y + SIZE2 - 1) % SIZE1][0] +
          matrix[(x + 1) % SIZE1][(y + SIZE2 - 1) % SIZE1][0];
}

void setups_diod() {
 // установить все пины на вывод
 // используем 2 канала 
    DDRD = 0xff;
    DDRB = 0xff;

 // скажет всем установиться на 0
   for (int i = 0; i < SIZE1; i++) {
     PORTD &= ~_BV(stolb[i]);
   }
 // необоходимо различить порты D и B
 // так как пины 7 - относяться к D, а 8-13 к B 
   char cond = 0;
   for (int i = 0; i < SIZE2; i++) {
     if (str[i] == PB0) cond = 1;
     if (!cond) PORTD &= ~_BV(str[i]);
     else PORTB &= ~_BV(str[i]);
   }
}

void clears_diod() {
   // отчистим матрицу
   for (int i = 0; i < SIZE1; i++) {
     for (int j = 0; j < SIZE2; j++) {
       s_diod[i][j] = 0x0;
     }
   }
}

int randomMatrix()
{
    //поле заливается рандомно
    for (int i = 0; i < SIZE1; i++) {
       for (int j = 0; j < SIZE2; j++) {
            if (rand() % 100 < density) {
            	matrix[i][j][0] = 1;
            }
            else {
                matrix[i][j][0] = 0;
            }
            matrix[i][j][1] = 0;
        }
    }
}

void display_st(int c) {
   // функция заставляет светиться один столбец
    // логический ноль - есть земля
   char cond = 0;
   for (int str1 = 0; str1 < SIZE2; str1++) {
     if (str[i] == PB0) cond = 1;
     if (s_diod[col][str1] == 0) {
        if (!cond) PORTD |= _BV(str[i]);
        else PORTB |= _BV(str[i]);
     }
     else {
        if (!cond) PORTD |= _BV(str[i]);
        else PORTB |= _BV(str[i]);
     }
   }
    //зажечь стоблец
    PORTD |= _BV(stolb[c]);
    _delay_ms(2);
    PORTD &= ~_BV(stolb[c]);
    
}
