/*компиляция используя -lncurses
   например, gcc absgraph.c -o absgraph -lncurses
 */
#include <stdio.h>                                /*подкл.библ.ф-й ст.в/выв */
#include <string.h>                               /*подкл.библ.ф-й стр.симв.*/
#include <stdlib.h>                               /*подкл.библ.ф-й преобр.д.*/
#include <ctype.h>                                /*подкл.библ.ф-й преобр.с.*/
#include <curses.h>

#define  NSPIS  5                                 /*разм.списка загр.прогр. */
#define  NOBJ   50                                /*разм.масс.об'ектных карт*/
#define  DOBLZ  1024                              /*длина области загрузки  */
#define  NOP 11                                    /*кол-во обрабатываемых   */
/* команд                 */

char opname[6];
char NFIL [30] = "\x0";

int IOBJC   = 0;                                  /*инд.вакантн.стр. OBJCARD*/
char OBJCARD [NOBJ][80];                          /*масс.хранен.об'ектн.карт*/

int ISPIS   = 0;                                  /*инд.вакантн.стр. SPISOK */
char SPISOK  [NSPIS][80];                         /*масс.хранен.списка прогр*/

WINDOW *wblue, *wgreen, *wred, *wcyan, *wmargenta;

struct STR_BUF_TXT                                /*структ.буфера карты TXT */
{
	unsigned char POLE1;                      /*место для кода 0x02     */
	unsigned char POLE2  [ 3];                /*поле типа об'ектн.карты */
	unsigned char POLE3;                      /*пробел                  */
	unsigned char ADOP   [ 3];                /*относит.адрес опреации  */
	unsigned char POLE5  [ 2];                /*пробелы                 */
	unsigned char DLNOP  [ 2];                /*длина операции          */
	unsigned char POLE7  [ 2];                /*пробелы                 */
	unsigned char POLE71 [ 2];                /*внутренний идент.прогр. */
	unsigned char OPER   [56];                /*тело операции           */
	unsigned char POLE9  [ 8];                /*идентификационное поле  */
};

void writeMemory(const char * fileName);

union                                             /*определить об'единение  */
{
	struct STR_BUF_TXT STR_TXT;               /*структура буфера        */
	unsigned char BUF_TXT [80];               /*буфер карты TXT         */
} TXT;


unsigned char INST [6];                           /*массив, содерж. обрабат.*/
/*команду                 */


/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FRR();                                        /*подпр.обр.опер.RR-форм. */

/*..........................................................................*/

/*п р о т о т и п  обращ.к*/
int FRX();                                        /*подпр.обр.опер.RX-форм. */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FXX();/*подпр.обр.опер.XX-форм. */
/*..........................................................................*/


int X1 = 1;                                       /* инициализация коорд.   */
int Y1 = 15;                                      /* на экране              */
int R1,                                           /*номер 1-го регистра-опе-*/
/*ранда в форматах RR и RX*/
    R2,                                           /*номер 2-го регистра-опе-*/
/*ранда в формате RX      */
    D,D2,                                            /*смещение в формате RX   */
    X,X2,                                            /*номер индексн. регистра */
/*в формате RX            */
    B,B2;                                            /*номер базового регистра */
/*в формате RX            */
unsigned short CC;
int LENGTH, LENGTH2;
unsigned long I,                                  /*счетчик адр.тек.ком-ды  */
              BAS_ADDR,                     /*адрес начала обл.загруз.*/
              I1,ADDR1,ADDR2,ARG,VS;               /*вспомогательные перем.  */
unsigned long VR[16],                             /*массив,содерж.знач.рег. */
              LIGHTPTR;                     /*адрес начала обл.отсвет.*/


int x,y,i,j,k,kk;                                 /*рабочие переменные      */

int CUR_IND;                                      /*индекс масс.обл.загр.,  */
                                                  /*соотв.текущ.ком-де прогр*/
int BAS_IND;                                      /*индекс масс.обл.загр.,  */
                                                  /*соотв.первой ком-ды прог*/

union U1                                          /*постоянная часть шабло- */
{                                                 /*на отсветки регистров на*/
	struct                                    /*экране консоли          */
	{
		char NOMREG  [ 3];
		char ZNEQUEL [1];
	} REGS_ASC [16];

	char BUFR [16][4];
} R_ASC;

union u2                                          /*шаблон для расчета      */
{                                                 /*элементов абсолютного   */
	struct
	{
		unsigned int SMESH;
		unsigned int SEGM;
	} VAL_P;
	unsigned char *P_OBLZ;
} POINT;

unsigned char OBLZ [DOBLZ];                       /*область загрузки трас-  */
/*сируемой программы      */
/*
 ***** ТАБЛИЦА машинных операций
 */

typedef unsigned char byte;
struct TMOP                                       /*структ.стр.табл.маш.опер*/
{
	unsigned char MNCOP [5];                  /*мнемокод операции       */
	unsigned char CODOP;                      /*машинный код операции   */
	unsigned char DLOP;                       /*длина операции в байтах */
	int (*BXPROG)();                          /*указатель на подпр.обраб*/
} T_MOP [NOP]  =                                  /*об'явление табл.маш.опер*/
{
	{{'B', 'A', 'L', 'R', ' '}, '\x05', 2, FRR}, /*инициализация           */
	{{'B', 'C', 'R', ' ', ' '}, '\x07', 2, FRR}, /*строк                   */
	{{'S', 'T', ' ', ' ', ' '}, '\x50', 4, FRX}, /*таблицы                 */
	{{'L', ' ', ' ', ' ', ' '}, '\x58', 4, FRX}, /*машинных                */
	{{'A', ' ', ' ', ' ', ' '}, '\x5A', 4, FRX}, /*операций                */
	{{'S', ' ', ' ', ' ', ' '}, '\x5B', 4, FRX}, /*                        */
    {{'S','H',' ',' ',' '}, '\x5C', 4, FRX},
    {{'S','T','H',' ',' '}, '\x5D', 4, FRX},
    {{'C', 'R', ' ', ' ', ' '}, '\x19', 2, FRR},
    {{'B', 'C', ' ', ' ', ' '}, '\x47', 4, FRX},
    {{'L', 'H', ' ', ' ', ' '}, '\x48', 4, FRX},
};
//..........................................................................
//п р о г р а м м а реализации семантики команды BALR
int P_BALR(void)
{
	if (R2 != 0)
		I = VR[R2];
	if (R1 != 0)
		VR[R1] = I;

	return 0;
}
//..........................................................................
//п р о г р а м м а реализации семантики команды BCR с маской 15
int P_BCR(void)
{
	int ret;

	ret = 1;
	if (R1 == 15)
	{
		ret = 0;
		if ((VR[R2] != 0) && (R2 != 0))
			I = VR[R2];
		else
		{
			if (R2 != 0)
			{
				waddstr(wcyan, "переход по адресу = 0 или завершение трассировки программы после нажатия клавиши");
				wrefresh(wcyan);
				ret = 1;
			}
		}
	}

	return ret;
}
/*..........................................................................*/

int P_ST()                                        /*  п р о г р а м м а     */
/*реализации семантики    */
{                                                 /*команды ST              */
	int sm,i;                                 /*рабочие                 */
	char bytes[2];                            /*переменные              */

	ADDR1 = VR[B] + VR[X] + D;                 /*вычисление абс.адреса и */
	sm = (int) (ADDR1 -I);                     /*смещения                */

//	bytes[0] = (VR[R1] -                      /*преобразование содержим.*/
//	            VR[R1]% 0x1000000L)/0x1000000L; /*РОН, использованного в  */
//	bytes[1] = ((VR[R1] -                     /*качестве первого оп-да, */
//	             VR[R1]%0x10000L)/0x10000L)%0x100; /*к виду, принятому в     */
	bytes[0] = ((VR[R1] % 0x10000L) -         /*ЕС ЭВМ                  */
	            ((VR[R1]%0x10000L)%0x100))/0x100; /*                        */
	bytes[1] = (VR[R1] % 0x10000L) % 0x100;   /*                        */

	for (i=0; i<2; i++)                       /*запись преобразованого  */
		OBLZ[BAS_IND + CUR_IND + sm + i] = bytes[i]; /*значения по адресу 2-г  */
	/*операнда                */
	return 0;                                 /*успешное заверш.прогр.  */
}

int P_STH()                                        /*  п р о г р а м м а     */
/*реализации семантики    */
{                                                 /*команды STH              */
    int sm,i;                                 /*рабочие                 */
    char bytes[4];                            /*переменные              */
    
    ADDR1 = VR[B] + VR[X] + D;                 /*вычисление абс.адреса и */
    sm = (int) (ADDR1 -I);                     /*смещения                */
    
    /*преобразование содержим. к виду, принятому в     */
    bytes[0] = bytes[1] = 0;
    bytes[2] = ((VR[R1] % 0x10000L) -
                ((VR[R1]%0x10000L)%0x100))/0x100;
    /*РОН, использованного в ЕС ЭВМ                  */
    bytes[3] = (VR[R1] % 0x10000L) % 0x100;
    
    for (i=0; i<2; i++)                       /*запись преобразованого  */
        OBLZ[BAS_IND + CUR_IND + sm + i] = bytes[i+2]; /*значения по адресу 2-г  */
    /*операнда                */
    return 0;                                 /*успешное заверш.прогр.  */
}

/*..........................................................................*/

int P_L()                                         /*  п р о г р а м м а     */
/*реализации семантики    */
{                                                 /*команды L               */
	int sm;                                   /*рабочая переменная      */

	ADDR1 = VR[B] + VR[X] + D;                 /*вычисление абс.адреса и */
	sm = (int) ( ADDR1 - I );                  /*смещения                */
	VR[R1] =                                  /*преобразование содержим.*/
	         OBLZ[BAS_IND + CUR_IND + sm] * 0x1000000L + /*второго операнда к виду,*/
	         OBLZ[BAS_IND + CUR_IND + sm + 1] * 0x10000L + /*принятому в IBM PC, и   */
	         OBLZ[BAS_IND + CUR_IND + sm + 2] * 0x100 + /*запись в РОН, использ.в */
	         OBLZ[BAS_IND + CUR_IND + sm + 3]; /*качестве 1-го операнда  */

	return 0;                                 /*успешное заверш.прогр.  */
}

/*..........................................................................*/

int P_LH()                                         /*  п р о г р а м м а     */
/*реализации семантики    */
{                                                 /*команды LH               */
    int sm;                                   /*рабочая переменная      */
    
    ADDR1 = VR[B] + VR[X] + D;                 /*вычисление абс.адреса и */
    sm = (int) ( ADDR1 - I );                  /*смещения                */
    VR[R1] =                                  /*преобразование содержим.*/
    OBLZ[BAS_IND + CUR_IND + sm ] * 0x100 + /*запись в РОН, использ.в */
    OBLZ[BAS_IND + CUR_IND + sm + 1]; /*качестве 1-го операнда  */
    return 0;                                 /*успешное заверш.прогр.  */
}

/*..........................................................................*/

int P_A()                                         /*  п р о г р а м м а     */
/*реализации семантики    */
{                                                 /*команды A               */
	int sm;                                   /*рабочая переменная      */

	ADDR1 = VR[B] + VR[X] + D;                 /*вычисление абс.адреса и */
	sm = ( int ) ( ADDR1 -I );                 /*смещения                */
	ARG = OBLZ[BAS_IND + CUR_IND + sm] * 0x1000000L+/*формирование содержимого*/
	      OBLZ[BAS_IND + CUR_IND + sm + 1] * 0x10000L + /*второго операнда в сог- */
	      OBLZ[BAS_IND + CUR_IND + sm + 2] * 0x100 + /*лашениях ЕС ЭВМ         */
	      OBLZ[BAS_IND + CUR_IND + sm + 3];   /*                        */
	/*и                       */
	VR[R1] = VR[R1] + ARG;                    /*сложение с 1-м операндом*/

	return 0;                                 /*успешное заверш. прогр. */
}

/*..........................................................................*/

int P_S()                                         /* п р о г р а м м а      */
/* реализации семантики   */
{                                                 /* команды S              */
	int sm;                                   /*рабочая переменная      */

	ADDR1 = VR[B] + VR[X] + D;                 /*вычисление рабочего     */
	sm = ( int ) ( ADDR1 - I );                /*адреса и смещения       */

	ARG = OBLZ[BAS_IND + CUR_IND + sm] * 0x1000000L+/*формирование содержимого*/
	      OBLZ[BAS_IND + CUR_IND + sm + 1] * 0x10000L +/*второго операнда в сог- */
	      OBLZ[BAS_IND + CUR_IND + sm + 2] * 0x100 + /*лашениях ЕС ЭВМ         */
	      OBLZ[BAS_IND + CUR_IND + sm + 3];   /*                        */
	/* и                      */
	VR[R1] = VR[R1] - ARG;                    /*выч-ие из 1-го операнда */

	return 0;                                 /*успешное заверш.прогр.  */
}

int P_SH()                                         /* п р о г р а м м а      */
/* реализации семантики   */
{                                                 /* команды SH              */
    int sm;                                   /*рабочая переменная      */
    
    ADDR1 = VR[B] + VR[X] + D;                 /*вычисление рабочего     */
    sm = ( int ) ( ADDR1 - I );                /*адреса и смещения       */
    
    ARG = OBLZ[BAS_IND + CUR_IND + sm] * 0x100 +/*формирование содержимого*/
    OBLZ[BAS_IND + CUR_IND + sm + 1];   /*второго операнда в сог- */

    /* и                      */
    VR[R1] = VR[R1] - ARG;                    /*выч-ие из 1-го операнда */
    
    return 0;                                 /*успешное заверш.прогр.  */
}

int P_MVC()
{
    //PRINT("MVC - change addr - %0lX %d\n", ADDR, L);
    //PRINT("B1 %d, D1 %d, B2 %d, D2 %d, L %d\n", B1, D1, B2, D2, L);
    writeMemory("BEFORE_MVC.txt");
    int sm1, sm2, i;
    
    ADDR1 = VR[B] + D;
    sm1 = (int) ( ADDR1 - I );
    ADDR2 = VR[B2] + D2;
    sm2 = (int) ( ADDR2 - I );
    
    for (i=0; i<LENGTH; i++)
    {
        // PRINT("%d\n", OBLZ[BAS_IND + CUR_IND + sm1 + i]); //OBLZ[VR[B2] + VR[X] + D2 + i];
        OBLZ[BAS_IND + CUR_IND + sm1 + i] = OBLZ[BAS_IND + CUR_IND + sm2 + i];
        //OBLZ[sm1] = OBLZ[sm2]; // совсем неверно, но ближе к нашему случаю. ХЗ, почему
    }
    
    ADDR1 = VR[B] + VR[X] + D;
    FILE * debugFile = fopen("addres.txt", "wt");
    fprintf(debugFile, "addr1 = %d, addr2 = %d",BAS_IND + CUR_IND + sm1, BAS_IND + CUR_IND + sm2);
    fclose(debugFile);
    writeMemory("AFTER_MVC.txt");
    return 0;
}

int P_CVB()
{
    int i, sm;
    
    printf("CVB - change addr - %0lX\n", ADDR1);
    
    ADDR1 = VR[B] + VR[X] + D;
    sm = ( int ) ( ADDR1 - I );
    
    printf("CVB - change addr - %0lX %lu\n", ADDR1, VR[X] + D);
    
    //     for ( i=0 ; i < 8 ; ++i )
    //     {
    //        PRINT("CVB - [%0X]\n", OBLZ[BAS_IND + CUR_IND + sm + i]);
    //     }
    
    VR[R1] =
    (OBLZ[BAS_IND + CUR_IND + sm+4] >> 4) * 1000000  +
    (OBLZ[BAS_IND + CUR_IND + sm+4] % 0x10) * 100000 +
    (OBLZ[BAS_IND + CUR_IND + sm+5] >> 4) * 10000    +
    (OBLZ[BAS_IND + CUR_IND + sm+5] % 0x10) * 1000   +
    (OBLZ[BAS_IND + CUR_IND + sm+6] >> 4) * 100      +
    (OBLZ[BAS_IND + CUR_IND + sm+6] % 0x10) * 10     +
    (OBLZ[BAS_IND + CUR_IND + sm+7] >> 4);
    
    if (OBLZ[BAS_IND + CUR_IND + sm+7] % 0x10 == 0xD)
        VR[R1] = -1 * VR[R1];
    return 0;
}
int P_CR()
{
    if(VR[R1]==VR[R2])
    {
        CC = 0;
    }
    else
    {
        if(VR[R1]>VR[R2])
        {
            CC = 2;
        }
        else//(VR[R1]<VR[R2])
        {
            CC = 1;
        }
    }
    return 0;
}
int P_BC()
{
    int mask=R1;
    if ((mask == 7 && CC != 0)     //branch if not equals
        || (mask == 8 && CC == 0)  //branch if equals
        || (mask == 15) ||
        (mask == 4 && CC == 1))             //branch always
    {
        ADDR1 = VR[B] + VR[X] + D;
        int sm = ( int ) ( ADDR1 -I );
        int idx = BAS_IND + CUR_IND + sm;
        CUR_IND = idx;
        I = ADDR1;
    }
    return 0;
}

//..........................................................................
int FRR(void)
{
	int i, j;

	for (i = 0; i < NOP; i++)
	{
		if (INST[0] == T_MOP[i].CODOP)
		{
			waddstr(wgreen, "      ");
			for (j = 0; j < 5; j++)
				waddch(wgreen, T_MOP[i].MNCOP[j]);
			waddstr(wgreen, " ");

			j = (INST[1] - (INST[1] % 0x10)) / 0x10;
			R1 = j;
			wprintw(wgreen, "%1d, ", j);
			j = INST[1] % 0x10;
			R2 = j;
			wprintw(wgreen, "%1d \n", j);
            break;
		}
	}

	return 0;
}
//...........................................................................
int FRX(void)
{
	int i, j;
	for (i = 0; i < NOP; i++)
	{
		if (INST[0] == T_MOP[i].CODOP)
		{
			waddstr(wgreen, "  ");
			for (j = 0; j < 5; j++)
				waddch(wgreen, T_MOP[i].MNCOP[j]);
			waddstr(wgreen, " ");

			j = INST[1] >> 4;
			R1 = j;
			wprintw(wgreen, "%.1d, ", j);

			j = INST[2] % 16;
			j = j * 256 + INST[3];
			D = j;
			wprintw(wgreen, "X'%.3X'(", j);

			j = INST[1] % 16;
			X = j;
			wprintw(wgreen, "%1d, ", j);

			j = INST[2] >> 4;
			B = j;
			wprintw(wgreen, "%1d)", j);

			ADDR1 = VR[B] + VR[X] + D;
			wprintw(wgreen,"        %.06lX\n", ADDR1);
			/*if (ADDR % 4 != 0) // TODO: FIX alignement in kompassr
				return (7);*/
			break;
		}
	}

	return 0;
}
//...........................................................................

int FXX(void)
{
    int i, j;
    
    //PRINT("FXX - change addr - %0lX\n", ADDR1);
    
    for (i = 0; i < NOP; i++)
    {
        if (INST[0] == T_MOP[i].CODOP)
        {
            waddstr(wgreen, "  ");
            for (j = 0; j < 5; j++)
                waddch(wgreen, T_MOP[i].MNCOP[j]);
            waddstr(wgreen, " ");
            
            // Вычисляем L
            j = INST[1];
            LENGTH = j + 1;
            
            // Вычисляем B1 (первые 4 бита)
            j = INST[2] >> 4;
            B = j;
            
            // Вычисляем D1 (вторые 12 бит)
            j = INST[2] % 16;
            j = j * 256 + INST[3];
            D = j;
            
            // Вывод первого операнда
            wprintw(wgreen, "X'%.3X'(%.1d,%1d), ", D, LENGTH, B);
            
            // Вычисляем B2 (первые 4 бита)
            j = INST[4] >> 4;
            B2 = j;
            
            // Вычисляем D2 (вторые 12 бит)
            j = INST[4] % 16;
            j = j * 256 + INST[5];
            D2 = j;
            
            // Вывод второго операнда
            wprintw(wgreen, "X'%.3X'(%1d)", D2, B2);
            
            // ADDR = VR[B1] + D1;
            ADDR1 = VR[B] + VR[X] + D;
            wprintw(wgreen,"  %.06lX   \n", ADDR1);
            if (ADDR1 % 4 != 0)
            {
                // PRINT("FXX - bad addr - %0lX\n", ADDR);
                // return (7);
            }
            
            break;
        }
    }
    
    return 0;
}
//...........................................................................
//---------------------------------------------------------------------------
int wind(void)
{
    int j1, k, temp;

	x = 0;
	y = 16;

	kk = CUR_IND;
	LIGHTPTR = I1;

	for (j1 = 0; j1 < 8; j1++)
	{
		wprintw(wred, "%.06lX: ", I1);
		for (j = 0; j < 4; j++)
		{
			for (k = 0; k < 4; k++)
				wprintw(wred, "%.02X", OBLZ[BAS_IND + kk + j * 4 + k]);
			waddstr(wred, " ");
		}

		waddstr(wred, "/* ");
		for (j = 0; j < 16; j++)
		{
			if (isprint (OBLZ[BAS_IND + kk]) )
			{
				waddch(wred, OBLZ[BAS_IND + kk++]);
				wrefresh(wred);
			}
			else
			{
				waddstr(wred, ".");
				kk++;
			}
		}

		waddstr(wred, " */");
		I1 += 16;
	}
	wrefresh(wred); //вывод на экран
	wclear(wred); //очистка содержимого окна дампа

	return 0;
}
//---------------------------------------------------------------------------
//программа покомандной интерпретпции(отладки)
// загруженной программы
int sys(void)
{
	int res, temp;
	int ch;
	int gr_pos_x, gr_pos_y;
	int ii = 0, jj = 0;
	int gr_y;
	char wstr[80];
	int zizi = 0, tempI;


	I = BAS_ADDR; //установить текущий адрес
	//равный начальному
//нижнее поле
	wmargenta = newwin(1, 80, 24, 0);
	wbkgd(wmargenta, COLOR_PAIR(COLOR_MAGENTA));
	waddstr(wmargenta, "\"PgUp\",\"PgDn\",\"Up\",\"Down\"->scroll dump; \"Enter\"->execute next command");

//строка состояния
	wcyan = newwin(1, 80, 23, 0);
	wbkgd(wcyan, COLOR_PAIR(COLOR_CYAN));

//дамп области загрузки
	wred = newwin(8, 67, 15, 0);
	wbkgd(wred, COLOR_PAIR(COLOR_RED));

//поле регистров
	wblue = newwin(17, 12, 0, 68);
	wbkgd(wblue, COLOR_PAIR(COLOR_BLUE));

//текст
	gr_pos_x = 0;
	gr_pos_y = 14;
	gr_y = 11;
	wgreen = newwin(gr_y, 67, gr_pos_y, gr_pos_x); //создадим новое окно
	wbkgd(wgreen, COLOR_PAIR(COLOR_GREEN)); //выбор цветовой пары


	keypad(wmargenta, TRUE);  //разрешить преобразование кодов клавиатуры

BEGIN:

//все допустимые коды к-нд сравнить с текущей и при
//совпадениизапомнить номер строки таблицы операций
	for (i = 0; i < NOP; i++)
	{
        int currentOperationCode = OBLZ[BAS_IND + CUR_IND];
		if (OBLZ[BAS_IND + CUR_IND] == T_MOP[i].CODOP)
		{
			k = i;
			wprintw(wgreen, "%.06lX: ", I);
//рисуем окно, выводим текст
			for (j = 0; j < 6; j++)   /*                        */
			{                      /*                        */
				if (j < T_MOP[i].DLOP) //TODO: проверить длину операции
				{              /* выдать шестнадцатеричн.*/
					wprintw(wgreen, "%.02X", OBLZ[BAS_IND + CUR_IND + j]);
                    wrefresh(wgreen);
					/* запомнить его же в     */
					INST[j] =   /* переменной INST,       */
					          OBLZ [BAS_IND + CUR_IND + j];
                    
				}
				else INST [j] = '\x00';
			}
			if ((res = T_MOP[i].BXPROG()) != 0) /* уйти в программу отобр.*/
            {
                printf("Ошибка при обработке комманды '%c%c%c' 0x%02x\n",T_MOP[i].MNCOP[0],T_MOP[i].MNCOP[1],T_MOP[i].MNCOP[2],T_MOP[i].CODOP);
				return (res); /* ассемблерного эквивале-*/
            }
			/* нта текущей команды,   */
			/*                        */
			goto l0;            /* перейти к дальнейшей  */
		}
	}
	return (6);

l0:
//сдвиг окна вверх
	wrefresh(wgreen);
	ii++;
	if (gr_pos_y > 14 - gr_y + 1)
		mvwin(wgreen, gr_pos_y--, gr_pos_x);
//при достижении некоторого положения, движение останавливается, и производится
//прокрутка окна
	else
	{
		for (jj = 0; jj < gr_y - 1; jj++)
		{
			temp = mvwinnstr(wgreen, jj + 1, 0, wstr, 67);
			mvwaddnstr(wgreen, jj, 0, wstr, 67);
			wrefresh(wgreen);
		}
	}
	wrefresh(wgreen);
	I += T_MOP[k].DLOP;                      /*коррекция счет-ка.адреса*/
	CUR_IND = ( int ) ( I - BAS_ADDR );      /*уст-ка текущ. индекса   */
	/*в массиве OBLZ          */
	I1 = I;                                  /*установка адреса начала */
	/*области отсветки        */

	for ( i = 0; i < 16; i++)
	{
		if (i < 10)
			waddstr(wblue, "R0");
		else
			waddstr(wblue, "R");
		wprintw(wblue, "%d:", i);
		wprintw(wblue, "%.08lX", VR[i]);
	}
    wprintw(wblue, "CC:%01d\n",CC);
	wrefresh(wblue); //вывод на экран
	wclear(wblue); //очистка окна регистров
	wind();

	waddstr(wcyan, "ready to run command with address ");
	wprintw(wcyan, "%.06lX", I - T_MOP[k].DLOP);
	waddstr(wcyan, "\n");
	wrefresh(wcyan);
	wclear(wcyan);

WAIT:

	CUR_IND = (int)(I - BAS_ADDR);

	ch = wgetch(wmargenta);

	switch (ch)
	{
	case 10:
	{
		goto SKIP;
	}

	case  KEY_UP:
	{
		I1 = LIGHTPTR - 16;
		CUR_IND = (int)(I1 - BAS_ADDR);
		wind();
		goto WAIT;
	}

	case  KEY_DOWN:
	{
		I1 = LIGHTPTR + 16;
		CUR_IND = (int)(I1 - BAS_ADDR);
		wind();
		goto WAIT;
	}

	case  KEY_PPAGE:
	{
		I1 = LIGHTPTR - 128;
		CUR_IND = (int)(I1 - BAS_ADDR);
		wind();
		goto WAIT;
	}

	case  KEY_NPAGE:
	{
		I1 = LIGHTPTR + 128;
		CUR_IND = (int)(I1 - BAS_ADDR);
		wind();
		goto WAIT;
	}
	}
	goto WAIT;

SKIP:

    memcpy(opname, T_MOP[k].MNCOP, 5*sizeof(char));
    opname[5]=0;
	switch (T_MOP[k].CODOP)                   //согласно  коду команды,
	{                                         //селектируемой сч.адреса
	//выбрать подпрогр.интер-
	case '\x05': P_BALR();                    //претации семантики
		break;                //текущей команды
	case '\x07': { i = P_BCR();
		       getch();
		       if (i == 1)
			       return 8; }
		                         break;
	case '\x50': P_ST();
		break;
	case '\x58': P_L();
		break;
	case '\x5A': P_A();
		break;
	case '\x5B': P_S();
        break;
    case '\x48': P_LH();
        break;
    case '\x5C': P_SH();
        break;
    case '\x5D': P_STH();
        break;
    case '\x19': P_CR();
        break;
    case '\x47': P_BC();
            break;
    default:
        return 10;// Обработка инструкции ещё не сделана
        break;
	}

	goto BEGIN;

	delwin(wblue);
	delwin(wred);
	delwin(wgreen);
	delwin(wmargenta);
	return 0;
}
//...........................................................................
//..........................Инициализация curses..............................
int InitCurses(void)
{
	initscr();    //инициализация библиотеки curses
	curs_set(0);
	noecho();   //не показывать ввод
	cbreak();   //читать один символ
	//за раз, не ждать \n
	keypad(stdscr, TRUE); //разрешить преобразование кодов клавиатуры
	start_color();


	if (has_colors())
	{
		init_pair(COLOR_BLUE, COLOR_WHITE, COLOR_BLUE);
		init_pair(COLOR_GREEN, COLOR_BLACK, COLOR_GREEN);
		init_pair(COLOR_RED, COLOR_WHITE, COLOR_RED);
		init_pair(COLOR_CYAN, COLOR_BLACK, COLOR_CYAN);
		init_pair(COLOR_MAGENTA, COLOR_WHITE, COLOR_MAGENTA);
	}


	return 0;
}
//...........................................................................

int main( int argc, char **argv )                /* п р о г р а м м а      */
/*абсолютного загрузчика  */
/*об'ектных файлов        */
{
	int I,K,N,J0,res;                         /*рабочие                 */
	unsigned long J;                          /*переменные              */
	FILE *fp;                                 /*программы               */
	char *ptr;

//main programm


	if ( argc != 2 )
	{
		printf ( "%s\n", "Ошибка в командной строке" );
		return -1;
	}

	ptr = argv[1];
	strcpy ( NFIL, ptr );

	if ( strcmp ( &NFIL [ strlen ( NFIL )-3 ], "mod" ) )
	{
		goto ERR9;
		return -1;
	}

	if ((fp = fopen(NFIL,"rt")) == NULL)
		goto ERR1;                         /*сообщение об ошибке     */
	else
	{
		while ( !feof( fp ) )             /*читать все карты файла  */
		{                                 /*со списком              */
			fgets ( SPISOK [ISPIS++], 80, fp ); /*в массив SPISOK         */
			if ( ISPIS == NSPIS )     /*если этот массив пере-  */
			{                         /*полнен, то:             */
				fclose ( fp ); /*закрыть файл со списком */
				goto ERR4;  /*и выдать сообщение об ош*/
			}
		}
		fclose ( fp );                    /*закрыть файл SPISOK     */

		if ( ISPIS == 0 )                 /*если список пустойб     */
			/*то:                     */
			goto ERR2;                /* сообщение об ошибке    */
		else                              /*иначе:                  */
			goto CONT1;               /* продолжить обработку   */
	}

CONT1:

	for ( I = 0; I < ISPIS; I++ )             /*перебирая все собираемые*/
	{
		/*об'ектные файлы,        */
        if ((fp = fopen(SPISOK[I], "rb" )) ==  NULL) {
            system("pwd");
			goto ERR3;
        }                                   /*                        */
		else                              /* иначе:                 */
		{                               /*                        */
			while ( !feof( fp) )      /*  читать файл до конца, */
			{                         /*  размеcтить записи в   */
				fread ( OBJCARD [IOBJC++], 80, 1, fp );/*  массиве OBJCARD и,если*/
				if ( IOBJC == NOBJ ) /*  считаны не все записи,*/
				{           /*  то:                   */
					fclose ( fp ); /*   выдать сообщ.об ошиб.*/
					goto ERR5; /*                        */
				}
			}                         /*                        */
			fclose ( fp );            /*  закрыть очередной файл*/

			goto CONT2;               /*  и продолжить обработку*/
		}
	}

CONT2:

	POINT.P_OBLZ = OBLZ;                      /*расчитать абсолютный    */
	J = POINT.VAL_P.SEGM;                     /*адрес области загрузки  */
	J = J << 4;                               /*OBLZ в переменной J     */
	J += POINT.VAL_P.SMESH;

	if ( ( J0 = (int) J%8 ) == 0 )            /*выровнять полученное    */
	{
		BAS_ADDR = J;                     /*значение на границу     */
		BAS_IND  = 0;
	}
	else                                      /*двойного слова и запомн.*/
	{
		BAS_ADDR = ( ( J >> 3 ) + 1 ) << 3; /*его в перем.BAS_ADDR,а  */
		BAS_IND = 8 - J0;                 /*соотв.индекс масс.OBLZ-в*/
	}         /*перем.BAS_IND           */

	for ( I = 0; I < IOBJC; I++ )             /*перебирая все считанные */
	{                                         /*карты об'ектных файлов, */
		if ( !memcmp ( &OBJCARD [I][1], "TXT", 3 ) )/*отобрать принадл.к типу */
		{                                 /*TXT и расчитать:        */
			memcpy ( TXT.BUF_TXT, OBJCARD [I], 80 ); /*                        */
			J = TXT.STR_TXT.ADOP [0]; /* в переменной J начальн.*/
			J = (J << 8) + TXT.STR_TXT.ADOP [1]; /*  индекс загрузки в мас-*/
			J = (J << 8) + TXT.STR_TXT.ADOP [2]; /*  сиве OBLZ             */
			J += BAS_IND;             /*и                       */
			/*                        */
			K = TXT.STR_TXT.DLNOP [0]; /* в переменной K длину   */
			K = (K << 8) + TXT.STR_TXT.DLNOP [1]; /* загружаемых данных     */

			for ( N=0; N < K; N++ )   /*загрузить данные с очер.*/
				OBLZ [ (int) J++ ] = TXT.STR_TXT.OPER [N]; /*об'ектной карты         */
		}
	}

    /*printf("IOBJC: %i\n",IOBJC);
    for(int k=0;k<IOBJC;k++)
    {
        for(int kk=0;kk<80;kk++){
            printf("%c",OBJCARD[k][kk]);
        }
    }
    getch();*/
    printf("\nНажмите enter, чтобы продолжить (самое время подключить отладчик)\n");
    char t=0;
    while(1)
    {
        t=getchar();
        if(t == '\n'){break;}
        t=0;
    }
	InitCurses();
	res = sys();
	switch (res)
	{
	case 6:
	{
		endwin();
		goto ERR6;
	}
	case 7:
	{
		endwin();
		goto ERR7;
	}
	case 8:
	{
		endwin();
		goto ERR8;
	}
    case 10:
    {
        endwin();
    	goto ERR10;
    }
	}

	endwin();
END:
    /*for(int II=0;II<DOBLZ;II++)
    {
        printf("%02x ",OBLZ[II]);
    }*/
	printf ("\n%s\n", "завершение обработки");

	return 0;
//Б Л О К  выдачи диагностических сообщений
ERR1:
	printf ("%s%s\n", "ошибка открытия файла со списком собираемых ", "модулей");
	goto END;

ERR2:
	printf ("%s\n", "пустой файл со списком собираемых модулей");
	goto END;

ERR3:
	printf ("%s: %s\n",
	        "ошибка открытия файла", SPISOK [I] );
	goto END;

ERR4:
	printf ("%s\n",
	        "переполнение списка собираемых модулей" );
	goto END;

ERR5:
	printf ("%s\n",
	        "переполнение буфера хранения об'ектных карт");
	goto END;

ERR6:
    printf("opcode: 0x%02x\n",OBLZ[BAS_IND+CUR_IND]);
	printf ("%s\n",
	        "недопустимый код команды");
	goto END;

ERR7:
    printf("ADDR: 0x%lx\n",ADDR1);
	printf("прерывание - ошибка адресации\n");
	goto END;

ERR8:
	goto END;

ERR9:
	printf ( "%s\n", "Неверный тип файла с исходным текстом" );
	goto END;
ERR10:
    printf ( "%s'%s'\n", "Не обработанная инструкция ",opname);
    goto END;
}

void writeMemory(const char * fileName)
{
    FILE * f = fopen(fileName, "wt");
    for(int i = 0; i < DOBLZ; i++)
    {
        fprintf(f, "%d) %d\n", i, (int)(OBLZ[i]));
    }
    fclose(f);
}
