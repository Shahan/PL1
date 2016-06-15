#define DL_ASSTEXT 64
#define DL_OBJTEXT 50                             /*длина об'ектн. текста   */
#define NSYM 128                                   /*размер табл.символов    */
#define NPOP 6                                    /*размер табл.псевдоопер. */
#define NOP  11                                   /*размер табл.операций    */
#include <string.h>                               /*вкл.строковые подпрогр. */
#include <stdlib.h>                               /*вкл.подпрогр.преобр.данн*/
#include <stdio.h>                                /*вкл.подпр.станд.вв/выв  */
#include <ctype.h>                                /*вкл.подпр.классиф.симв. */

/*
 ******* Б Л О К  об'явлений статических рабочих переменных
 */

char NFIL [30] = "\x0";

unsigned char PRNMET = 'N';                       /*индикатор обнаруж.метки */
int I3;                                           /*счетчик цикла           */
                                          /*счетчик цикла           */
void print_pl_value (unsigned char* buf, size_t len)
{
    for ( size_t i = 0; i < len; ++i )
    {
        printf ("[%0X %0X]", (buf[i] & 0xF0) >> 4,
                (buf[i] & 0x0F));
    }
    printf ("\n");
}

/*
 ***** Б Л О К  об'явлений прототипов обращений к подпрограммам 1-го просмотра
 */

/*п р о т о т и п  обращ.к*/
int FDC();                                        /*подпр.обр.пс.опер.DC    */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FDS();                                        /*подпр.обр.пс.опер.DS    */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FEND();                                       /*подпр.обр.пс.опер.END   */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FEQU();                                       /*подпр.обр.пс.опер.EQU   */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FSTART();                                     /*подпр.обр.пс.опер.START */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FUSING();                                     /*подпр.обр.пс.опер.USING */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FRR();                                        /*подпр.обр.опер.RR-форм. */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int FRX();                                        /*подпр.обр.опер.RX-форм. */
/*..........................................................................*/
int FXX();
int FNOP();
/*
 ***** Б Л О К  об'явлений прототипов обращений к подпрограммам 2-го просмотра
 */

/*п р о т о т и п  обращ.к*/
int SDC();                                        /*подпр.обр.пс.опер.DC    */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int SDS();                                        /*подпр.обр.пс.опер.DS    */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int SEND();                                       /*подпр.обр.пс.опер.END   */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int SEQU();                                       /*подпр.обр.пс.опер.EQU   */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int SSTART();                                     /*подпр.обр.пс.опер.START */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int SUSING();                                     /*подпр.обр.пс.опер.USING */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int SRR();                                        /*подпр.обр.опер.RR-форм. */
/*..........................................................................*/
/*п р о т о т и п  обращ.к*/
int SRX();                                        /*подпр.обр.опер.RX-форм. */
int SXX();
/*..........................................................................*/

/*
 ******* Б Л О К  об'явлений таблиц базы данных компилятора
 */

/*
 ******* ОБ'ЯВЛЕНИЕ структуры строки (карты) исходного текста
 */

struct ASSKARTA                                   /*структ.карты АССЕМБЛЕРА */
{
	unsigned char METKA    [ 8];              /*поле метки              */
	unsigned char PROBEL1  [ 1];              /*пробел-разделитель      */
	unsigned char OPERAC   [ 5];              /*поле операции           */
	unsigned char PROBEL2  [ 1];              /*пробел-разделитель      */
	unsigned char OPERAND  [12];              /*поле операнда           */
	unsigned char PROBEL3  [ 1];              /*пробел разделитель      */
	unsigned char COMM     [52];              /*поле комментария        */
};

/*
 ******* НАЛОЖЕНИЕ структуры карты исх. текста на входной буфер
 */

union                                             /*определить об'единение  */
{
	unsigned char BUFCARD [80];               /*буфер карты.исх.текста  */
	struct ASSKARTA STRUCT_BUFCARD;           /*наложение шабл.на буфер */
} TEK_ISX_KARTA;

/*
 ***** СЧЕТЧИК относительного адреса (смещешия относительно базы )
 */

int CHADR;                                        /*счетчик                 */

unsigned char PL3_buf[3];
unsigned char PL8_buf[8];
/*
 ***** ТАБЛИЦА символов
 */

int ITSYM = -1;                                   /*инд.своб.стр. табл.симв.*/
struct TSYM                                       /*структ.строки табл.симв.*/
{
	unsigned char IMSYM [8];                  /*имя символа             */
	int ZNSYM;                                /*значение символа        */
	int DLSYM;                                /*длина символа           */
	char PRPER;                               /*признак перемещения     */
};

struct TSYM T_SYM [NSYM];                         /*определение табл.симв.  */

/*
 ***** ТАБЛИЦА машинных операций
 */

struct TMOP                                       /*структ.стр.табл.маш.опер*/
{
	unsigned char MNCOP [5];                  /*мнемокод операции       */
	unsigned char CODOP;                      /*машинный код операции   */
	unsigned char DLOP;                       /*длина операции в байтах */
	int (*BXPROG)();                          /*указатель на подпр.обраб*/
} T_MOP [NOP]  =                                  /*об'явление табл.маш.опер*/
{
	{{'B','A','L','R',' '}, '\x05', 2, FRR},  /*инициализация           */
	{{'B','C','R',' ',' '}, '\x07', 2, FRR},  /*строк                   */
	{{'S','T',' ',' ',' '}, '\x50', 4, FRX},  /*таблицы                 */
	{{'L',' ',' ',' ',' '}, '\x58', 4, FRX},  /*машинных                */
	{{'A',' ',' ',' ',' '}, '\x5A', 4, FRX},  /*операций                */
	{{'S',' ',' ',' ',' '}, '\x5B', 4, FRX},  /*                        */
    {{'S','H',' ',' ',' '}, '\x5C', 4, FRX},
    {{'S','T','H',' ',' '}, '\x5D', 4, FRX},
    {{'C','R',' ',' ',' '}, '\x19', 2, FRR},
    {{'B','C',' ',' ',' '}, '\x47', 4, FRX},
    {{'L','H',' ',' ',' '}, '\x48', 4, FRX},
};

/*
 ***** ТАБЛИЦА псевдоопераций
 */

struct TPOP                                       /*структ.стр.табл.пс.опeр.*/
{
	unsigned char MNCPOP[5];                  /*мнемокод псевдооперации */
	int (*BXPROG) ();                         /*указатель на подпр.обраб*/
} T_POP [NPOP] =                                  /*об'явление табл.псевдооп*/
{
	{{'D','C',' ',' ',' '}, FDC   },          /*инициализация           */
	{{'D','S',' ',' ',' '}, FDS   },          /*строк                   */
	{{'E','N','D',' ',' '}, FEND  },          /*таблицы                 */
	{{'E','Q','U',' ',' '}, FEQU  },          /*псевдоопераций          */
	{{'S','T','A','R','T'}, FSTART},          /*                        */
	{{'U','S','I','N','G'}, FUSING}           /*                        */
};

/*
 ***** ТАБЛИЦА базовых регистров
 */

struct TBASR                                      /*структ.стр.табл.баз.рег.*/
{
	int SMESH;                                /*                        */
	char PRDOST;                              /*                        */
} T_BASR[15] =                                    /*                        */
{
	{0x00,'N'},                               /*инициализация           */
	{0x00,'N'},                               /*строк                   */
	{0x00,'N'},                               /*таблицы                 */
	{0x00,'N'},                               /*базовых                 */
	{0x00,'N'},                               /*регистров               */
	{0x00,'N'},                               /*                        */
	{0x00,'N'},                               /*                        */
	{0x00,'N'},                               /*                        */
	{0x00,'N'},                               /*                        */
	{0x00,'N'},                               /*                        */
	{0x00,'N'},                               /*                        */
	{0x00,'N'},                               /*                        */
	{0x00,'N'},                               /*                        */
	{0x00,'N'},                               /*                        */
	{0x00,'N'}                                /*                        */
};

/*
 ***** Б Л О К   об'явления массива с об'ектным текстом
 */

unsigned char OBJTEXT [DL_OBJTEXT][80];           /*массив об'ектных карт   */
int ITCARD = 0;                                   /*указатель текущ.карты   */

struct OPRR                                       /*структ.буф.опер.форм.RR */
{
	unsigned char OP;                         /*код операции            */
	unsigned char R1R2;                       /*R1 - первый операнд     */
	/*R2 - второй операнд     */
};

union                                             /*определить об'единение  */
{
	unsigned char BUF_OP_RR [2];              /*оределить буфер         */
	struct OPRR OP_RR;                        /*структурировать его     */
    int RR;
} RR;

struct OPRX                                       /*структ.буф.опер.форм.RX */
{
	unsigned char OP;                         /*код операции            */
	unsigned char R1X2;                       /*R1 - первый операнд     */
	short B2D2;                                 /*X2 - второй операнд     */
//    int B2D2;                                     /*X2 - второй операнд     */
	/*B2 - баз.рег.2-го оп-да */
	/*D2 - смещен.относит.базы*/
};

struct OPXX                                       /*структ.буф.опер.форм.XX */
{
    unsigned char OP;                             
    unsigned char L1L2;                           
    short B1D1;
    short B2D2;
};
union
{
    unsigned char BUF_OP_XX [6];
    struct OPXX OP_XX;
} XX;
union                                             /*определить об'единение  */
{
	unsigned char BUF_OP_RX [4];              /*оределить буфер         */
	struct OPRX OP_RX;                        /*структурировать его     */
    int RX;
} RX;


struct STR_BUF_ESD                                /*структ.буфера карты ESD */
{
	unsigned char POLE1;                      /*место для кода 0x02     */
	unsigned char POLE2  [ 3];                /*поле типа об'ектн.карты */
	unsigned char POLE3  [ 6];                /*пробелы                 */
	unsigned char POLE31 [ 2];                /*длина данных на карте   */
	unsigned char POLE32 [ 2];                /*пробелы                 */
	unsigned char POLE4  [ 2];                /*внутр.ид-р имени прогр. */
	unsigned char IMPR   [ 8];                /*имя программы           */
	unsigned char POLE6;                      /*код типа ESD-имени      */
	unsigned char ADPRG  [ 3];                /*относит.адрес программы */
	unsigned char POLE8;                      /*пробелы                 */
	unsigned char DLPRG  [ 3];                /*длина программы         */
	unsigned char POLE10 [40];                /*пробелы                 */
	unsigned char POLE11 [ 8];                /*идентификационное поле  */
};

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
void printTXT(struct STR_BUF_TXT txt)
{
    char type[4];
    memcpy(type,txt.POLE2, 3*sizeof(char));
    type[3]=0;
    int adop=txt.ADOP[2]+txt.ADOP[1]*256+txt.ADOP[0]*256*256;
    int opsize=txt.DLNOP[1]+txt.DLNOP[0]*256;
    char oper[57];
    memcpy(oper,txt.OPER, 56*sizeof(char));
    oper[56]=0;
    char id[9];
    memcpy(id,txt.POLE9, 8*sizeof(char));
    id[8]=0;
    printf("%i\t%s\t%i\t%i\t%c%c\t",txt.POLE1,type,adop,opsize,txt.POLE71[0],txt.POLE71[1]);
    printf("%s\t%s\n",oper,id);
}

struct STR_BUF_END                                /*структ.буфера карты END */
{
	unsigned char POLE1;                      /*место для кода 0x02     */
	unsigned char POLE2  [ 3];                /*поле типа об'ектн.карты */
	unsigned char POLE3  [68];                /*пробелы                 */
	unsigned char POLE9  [ 8];                /*идентификационное поле  */
};

union                                             /*определить об'единение  */
{
	struct STR_BUF_ESD STR_ESD;               /*структура буфера        */
	unsigned char BUF_ESD [80];               /*буфер карты ESD         */
} ESD;


union                                             /*определить об'единение  */
{
	struct STR_BUF_TXT STR_TXT;               /*структура буфера        */
	unsigned char BUF_TXT [80];               /*буфер карты TXT         */
} TXT;

union                                             /*определить об'единение  */
{
	struct STR_BUF_END STR_END;               /*структура буфера        */
	unsigned char BUF_END [80];               /*буфер карты ESD         */
} END;
#define maxMoreSym 8
void genMoreSymbols(char SYM[8])
{
    if(memcmp(SYM, "BUF", 3*sizeof(char))!=0)
    {
        return;
    }
    char newSym[8];
    memcpy(newSym, SYM, 8*sizeof(char));
    int last=0;
    for(last = 0;last<8;last++)
    {
        if(newSym[last] == ' ')
        {
            break;
        }
    }
    if(last > 6){return;}
    int dig=last+1;
    newSym[last]='+';
    for(int i=1;i<maxMoreSym;i++)
    {
        newSym[dig]='0'+i;
        ITSYM += 1;
        PRNMET = 'Y';                     /* устан.призн.налич.метки*/
        memcpy ( T_SYM[ITSYM].IMSYM,      /* запомнить имя символа  */
                newSym, 8 ); /* и                      */
        T_SYM[ITSYM].ZNSYM = CHADR;       /* его значение(отн.адр.) */
        T_SYM[ITSYM].DLSYM = 1;
    }
}
/*
 ******* Б Л О К  об'явлений подпрограмм, используемых при 1-ом просмотре
 */

int FDC()                                         /*подпр.обр.пс.опер.DC    */
{
	if ( PRNMET == 'Y' )                      /*если псевдооп.DC помеч.,*/
	{                                         /*то:                     */
		if                                /* если псевдооперация DC */
		(                                 /* определяет константу   */
		        TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0]=='F'/* типа F, то выполнить   */
		)                                 /* следующее:             */
		{
			T_SYM[ITSYM].DLSYM = 4;   /*  уст.длину симв. =  4, */
			T_SYM[ITSYM].PRPER = 'R'; /*  а,призн.перемест.='R' */
			if ( CHADR % 4 )          /*  и, если CHADR не указ.*/
			{                         /*  на границу слова, то: */
				CHADR = (CHADR /4 + 1) * 4; /*   уст.CHADR на гр.сл. и*/
				T_SYM[ITSYM].ZNSYM = CHADR; /*   запомн. в табл.симв. */
			}
			PRNMET = 'N';             /*  занулить PRNMET зн.'N'*/
		}
        else{
            if                                /* если псевдооперация DC */
                (                                 /* определяет константу   */
                 TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0]=='H'/* типа H, то выполнить   */
                 )                                 /* следующее:             */
            {
                T_SYM[ITSYM].DLSYM = 2;   /*  уст.длину симв. =  2, */
                T_SYM[ITSYM].PRPER = 'R'; /*  а,призн.перемест.='R' */
                if ( CHADR % 2 )          /*  и, если CHADR не указ.*/
                {                         /*  на границу слова, то: */
                    CHADR = (CHADR /2 + 1) * 2; /*   уст.CHADR на гр.сл. и*/
                    T_SYM[ITSYM].ZNSYM = CHADR; /*   запомн. в табл.симв. */
                }
                PRNMET = 'N';             /*  занулить PRNMET зн.'N'*/
            }
            else{
                printf("%c %s\n",PRNMET,TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND);
                return (1);               /* иначе выход по ошибке  */
            }
        }
	}
	else                                      /*если же псевдооп.непомеч*/
        if ( CHADR % 4 )  {                        /*и CHADR не кратен 4,то: */
            CHADR = (CHADR /4 + 1) * 4;       /* установ.CHADR на гр.сл.*/
        }

	CHADR = CHADR + T_SYM[ITSYM].DLSYM;                        /*увелич.CHADR на 4 и     */
	return (0);                               /*успешно завершить подпр.*/
}
/*..........................................................................*/
int FDS()                                         /*подпр.обр.пс.опер.DS    */
{
	if ( PRNMET == 'Y' )                      /*если псевдооп.DC помеч.,*/
	{                                         /*то:                     */
		if                                /* если псевдооперация DC */
		(                                 /* определяет константу   */
		        TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0]=='F'/* типа F, то выполнить   */
		)                                 /* следующее:             */
		{
			T_SYM[ITSYM].DLSYM = 4;   /*  уст.длину симв. =  4, */
			T_SYM[ITSYM].PRPER = 'R'; /*  а,призн.перемест.='R' */
			if ( CHADR % 4 )          /*  и, если CHADR не указ.*/
			{                         /*  на границу слова, то: */
				CHADR = (CHADR /4 + 1) * 4; /*   уст.CHADR на гр.сл. и*/
				T_SYM[ITSYM].ZNSYM = CHADR; /*   запомн. в табл.симв. */
			}
			PRNMET = 'N';             /*  занулить PRNMET зн.'N'*/
		}
		else
			return (1);               /* иначе выход по ошибке  */
        CHADR = CHADR + 4;                        /*увелич.CHADR на 4 и     */
	}
    else{
        //DS 0H, DS 0F
        if (TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[1] == 'H') {
            if ( CHADR % 2 ) {
                CHADR = (CHADR /2 + 1) * 2;
            }
        } else if (TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[1] == 'F') {
            if ( CHADR % 4 ) {
                CHADR = (CHADR /4 + 1) * 4;
            }
        }
    
    }
        
        /*если же псевдооп.непомеч*/
//	if ( CHADR % 4 )                          /*и CHADR не кратен 4,то: */
//		CHADR = (CHADR /4 + 1) * 4;       /* установ.CHADR на гр.сл.*/

	
	return (0);                               /*успешно завершить подпр.*/
}
/*..........................................................................*/
int FEND()                                        /*подпр.обр.пс.опер.END   */
{
	return (100);                             /*выход с призн.конца 1-го*/
	/*просмотра               */
}
/*..........................................................................*/
int FEQU()                                        /*подпр.обр.пс.опер.EQU   */
{
	if                                        /*если в операнде         */
	(                                         /*псевдооперации DC       */
	        TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0] =='*' /*использован симв. '*',  */
	)                                         /*то                      */
	{                                         /* запомнить в табл.симв.:*/
		T_SYM[ITSYM].ZNSYM = CHADR;       /*  CHADR в поле ZNSYM,   */
		T_SYM[ITSYM].DLSYM = 1;           /*  1 в поле DLSYM,       */
		T_SYM[ITSYM].PRPER = 'R';         /*  'R' в пооле PRPER     */
	}
	else                                      /*иначе запомн.в табл.симв*/
	{                                         /* значение оп-нда пс.оп. */
		T_SYM[ITSYM].ZNSYM = atoi (       /* DC в поле ZNSYM,       */
		        (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND
		        );
		T_SYM[ITSYM].DLSYM = 1;           /* 1 в поле DLSYM,        */
		T_SYM[ITSYM].PRPER = 'A';         /* 'A' в поле PRPER       */
	}
	PRNMET = 'N';                             /*занул.PRNMET значен.'N' */
	return (0);                               /*успешное заверш.подпр.  */
}
/*..........................................................................*/
int FSTART()                                      /*подпр.обр.пс.опер.START */
{                                                 /*CHADR установить равным */
	CHADR =                                   /*значению операнда       */
	        atoi(
	        (char*)TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND/*псевдооперации START и, */
	        );
	if ( CHADR % 8 )                          /*если это значение не    */
	{                                         /*кратно 8, то сделать его*/
		CHADR = ( CHADR + ( 8 - CHADR % 8 ) ); /*кратным                 */
	}                                         /*запомнить в табл.симв.: */
	T_SYM[ITSYM].ZNSYM = CHADR;               /* CHADR в поле ZNSYM,    */
	T_SYM[ITSYM].DLSYM = 1;                   /* 1 в поле DLSYM,        */
	T_SYM[ITSYM].PRPER = 'R';                 /* 'R' в поле PRPER       */
	PRNMET = 'N';                             /*занул.PRNMET значен.'N' */
	return (0);                               /*успешное заверш.подпрогр*/
}
/*..........................................................................*/
int FUSING()                                      /*подпр.обр.пс.опер.USING */
{
	return (0);                               /*успешное заверш.подпрогр*/
}
/*..........................................................................*/
int FRR()                                         /*подпр.обр.опер.RR-форм. */
{
	CHADR = CHADR + 2;                        /*увеличить сч.адр. на 2  */
	if ( PRNMET == 'Y' )                      /*если ранее обнар.метка, */
	{                                         /*то в табл. символов:    */
		T_SYM[ITSYM].DLSYM = 2;           /*запомнить длину маш.опер*/
		T_SYM[ITSYM].PRPER = 'R';         /*и установить призн.перем*/
	}
	return(0);                                /*выйти из подпрограммы   */
}
/*..........................................................................*/
int FRX()                                         /*подпр.обр.опер.RX-форм. */
{
	CHADR = CHADR + 4;                        /*увеличить сч.адр. на 4  */
	if ( PRNMET == 'Y' )                      /*если ранее обнар.метка, */
	{                                         /*то в табл. символов:    */
		T_SYM[ITSYM].DLSYM = 4;           /*запомнить длину маш.опер*/
		T_SYM[ITSYM].PRPER = 'R';         /*и установить призн.перем*/
	}
	return(0);                                /*выйти из подпрограммы   */
}
int FXX()                                         /*подпр.обр.опер.XX-форм. */
{
    CHADR = CHADR + 6;                        /*увеличить сч.адр. на 4  */
    if ( PRNMET == 'Y' )                      /*если ранее обнар.метка, */
    {                                         /*то в табл. символов:    */
        T_SYM[ITSYM].DLSYM = 6;           /*запомнить длину маш.опер*/
        T_SYM[ITSYM].PRPER = 'R';         /*и установить призн.перем*/
    }
    return(0);                                /*выйти из подпрограммы   */
}
int FNOP()
{
    return(0);
}
/*..........................................................................*/

/*
 ******* Б Л О К  об'явлений подпрограмм, используемых при 2-ом просмотре
 */

void STXT( int ARG )                              /*подпр.формир.TXT-карты  */
{
	char *PTR;                                /*рабоч.переменная-указат.*/

    printf("CHADR %d\n", CHADR);
    
	PTR = (char *)&CHADR;                     /*формирование поля ADOP  */
	TXT.STR_TXT.ADOP[2]  = *PTR;              /*TXT-карты в формате     */
	TXT.STR_TXT.ADOP[1]  = *(PTR+1);          /*двоичного целого        */
	TXT.STR_TXT.ADOP[0]  = '\x00';            /*в соглашениях ЕС ЭВМ    */

    memset ( TXT.STR_TXT.OPER, 64, 56 );          /*формирование поля OPER  */
    switch (ARG)
    {
        case 2:
            memcpy ( TXT.STR_TXT.OPER,RR.BUF_OP_RR, 2 ); /* для RR-формата         */
            TXT.STR_TXT.DLNOP [1] = 2;
            break;
        case 3:
            memcpy ( TXT.STR_TXT.OPER , PL3_buf , 3);   /* äëÿ PL3         */
            TXT.STR_TXT.DLNOP [1] = 3 ;
            break;
        case 4:
            memcpy ( TXT.STR_TXT.OPER, RX.BUF_OP_RX, 4);/* для RX-формата         */
            TXT.STR_TXT.DLNOP [1] = 4;
            break;
        case 6:
            memcpy ( TXT.STR_TXT.OPER , XX.BUF_OP_XX , 6);/* XX - format         */
            TXT.STR_TXT.DLNOP [1] = 6;
            break;
        case 8:
            memcpy ( TXT.STR_TXT.OPER , PL8_buf , 8);
            TXT.STR_TXT.DLNOP [1] = 8;
            break;
        default:
            printf("Ошибка при форм. TXT-карты: ARG = %i\n",ARG);
            exit(-1);
            break;
    }
	memcpy (TXT.STR_TXT.POLE9,ESD.STR_ESD.POLE11,8);/*формиров.идентифик.поля */
    printTXT(TXT.STR_TXT);
	memcpy ( OBJTEXT[ITCARD], TXT.BUF_TXT, 80 ); /*запись об'ектной карты  */
	ITCARD += 1;                              /*коррекц.инд-са своб.к-ты*/
	CHADR = CHADR + ARG;                      /*коррекц.счетчика адреса */
	return;
}

int SDC()                                         /*подпр.обр.пс.опер.DC    */
{
	char *RAB;                                /*рабочая переменная      */

	RX.OP_RX.OP   = 0;                        /*занулим два старших     */
	RX.OP_RX.R1X2 = 0;                        /*байта RX.OP_RX          */
	if
	(                                         /* если операнд начинается*/
	        !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND,/* с комбинации           */
	                "F'", 2)            /* F',                    */
	)                                         /* то                     */
	{
		RAB=strtok                        /*в перем. c указат.RAB   */
		             (              /*выбираем первую лексему */
		             (char*)TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND+2,/*операнда текущей карты  */
		             "'"            /*исх.текста АССЕМБЛЕРА   */
		             );

		RX.OP_RX.B2D2 = atoi ( RAB );     /*перевод ASCII-> int     */
		RAB = (char *) &RX.OP_RX.B2D2;    /*приведение к соглашениям*/
		swab ( RAB, RAB, 2 );             /* ЕС ЭВМ                 */
        STXT (4);                                 /*формирование TXT-карты  */
	}
    else{                                      /*иначе                   */
        
        if
            (                                             
             !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND,
                     "H'", 2)
             )                                            
        {
            RAB=strtok                                    
            (
             (char*)TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND+2,
             "'"
             );
            RR.RR = atoi ( RAB );                         
            RAB = (char *) &RR.RR;                        
            swab ( RAB , RAB , 2 );                       
            STXT (2);
        }
        else{
            printf("ОШИБКА: SDC(): неизвестный тип операнда %s\n",TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND);
            return (1);                       /*сообщение об ошибке     */
        }
    }

	return (0);                               /*успешн.завершение подпр.*/
}
/*..........................................................................*/
int SDS()                                         /*подпр.обр.пс.опер.DS    */
{
	if
	(                                         /* если операнд начинается*/
	        TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0]=='F' /* с комбинации F'        */
     ) {
        /* то:                    */
        RX.OP_RX.OP   = 0;                        /*занулим два старших     */
        RX.OP_RX.R1X2 = 0;                        /*байта RX.OP_RX          */
        RX.OP_RX.B2D2 = 0;                /*занулим RX.OP_RX.B2D2   */
        STXT (4);
    } else if
        (                                         /* если операнд начинается*/
         TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0]=='H' /* с комбинации F'        */
         )
    {
        /* то:                    */
        RR.OP_RR.R1R2 = 0;                /*занулим RX.OP_RX.B2D2   */
        STXT (2);
    }  else if
        (                                         /* если операнд начинается*/
            TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[1]=='H' /* с комбинации F'        */
         ){                                        /* то:                    */
        //RR.OP_RR.OP = 0;
        //RR.OP_RR.R1R2 = 0;
        if (CHADR % 2) {
            CHADR = (CHADR / 2 + 1) * 2;
        }
        //STXT (2);                                 /*формирование TXT-карты  */
    } else if
        (                                         /* если операнд начинается*/
         TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[1]=='F' /* с комбинации F'        */
        ){                                         /* то:                    */
        //RX.OP_RX.R1X2 = 0;                /*занулим RX.OP_RX.B2D2   */
        //RX.OP_RX.B2D2 = 0;                /*занулим RX.OP_RX.B2D2   */
        if (CHADR % 4) {
            CHADR = (CHADR / 4 + 1) * 4;
        }
    } else {
        printf("ОШИБКА: SDS(): неизвестный тип операнда %s\n",TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND);
        return (1);                       /*сообщение об ошибке     */
    }

	return (0);                               /*успешно завершить подпр.*/
}
/*..........................................................................*/
int SEND()                                        /*подпр.обр.пс.опер.END   */
{
	/*формирование            */
	memcpy (                                  /*идентификационнго поля  */
	        END.STR_END.POLE9,          /* END - карты            */
	        ESD.STR_ESD.POLE11,         /*                        */
	        8                           /*                        */
	        );                          /*                        */
	memcpy (                                  /*запись об'ектной карты  */
	        OBJTEXT[ITCARD],            /* в                      */
	        END.BUF_END,                /* массив                 */
	        80                          /* об'ектных              */
	        );                          /* карт                   */
	ITCARD += 1;                              /*коррекц.инд-са своб.к-ты*/
	return (100);                             /*выход с призн.конца 2-го*/
	/*просмотра               */
}
/*..........................................................................*/
int SEQU()                                        /*подпр.обр.пс.опер.EQU   */
{
	return (0);                               /*успешное заверш.подпр.  */
}
/*..........................................................................*/
int SSTART()                                      /*подпр.обр.пс.опер.START */
{
	char *PTR;                                /*набор                   */
	char *METKA;                              /*рабочих                 */
	char *METKA1;                             /*переменных              */
	int J=0;                                  /*подпрограммы            */
	int RAB;                                  /*                        */

	METKA1 = strtok                           /*в перем. c указат.METKA1*/
	                 (                  /*выбираем первую лексему */
	                 (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.METKA, /*операнда текущей карты  */
	                 " "                /*исх.текста АССЕМБЛЕРА   */
	                 );
	for ( J=0; J<=ITSYM; J++ )                /* все метки исх.текста в */
	{                                         /* табл. T_SYM сравниваем */
		/* со знач.перем. *METKA1 */
		METKA = strtok (
		        (char*) T_SYM[J].IMSYM, " "
		        );
		if( !strcmp ( METKA, METKA1 ) )   /* и при совпадении:      */
		{                                 /*  берем разность сч.адр.*/
			RAB = CHADR - T_SYM[J].ZNSYM; /*  знач.этой метки, обра-*/
			PTR = (char *)&RAB;       /*  зуя длину программы в */
			swab ( PTR, PTR, 2 );     /*  соглашениях ЕС ЭБМ, и */
			ESD.STR_ESD.DLPRG [0] = 0; /*  записыв.ее в ESD-карту*/
			ESD.STR_ESD.DLPRG [1] = *PTR; /*  побайтно              */
			ESD.STR_ESD.DLPRG [2] = *(PTR + 1); /*                        */
			CHADR = T_SYM[J].ZNSYM;   /*устанавл.CHADR, равным  */
			/*операнду операт.START   */
			/*исходного текста        */
			PTR = (char *)&CHADR;     /*формирование поля ADPRG */
			ESD.STR_ESD.ADPRG[2]  = *PTR;/*ESD-карты в формате     */
			ESD.STR_ESD.ADPRG[1]  = *(PTR+1);/*двоичного целого        */
			ESD.STR_ESD.ADPRG[0]  = '\x00';/*в соглашениях ЕС ЭВМ    */
			memcpy (                  /*формирование            */
			        ESD.STR_ESD.IMPR, /* имени программы        */
			        METKA,      /*  и                     */
			        strlen ( METKA ) /*                        */
			        );          /*                        */
			memcpy (                  /*идентификационнго поля  */
			        ESD.STR_ESD.POLE11, /* ESD - карты            */
			        METKA,      /*                        */
			        strlen ( METKA ) /*                        */
			        );          /*                        */
			memcpy (                  /*запись об'ектной карты  */
			        OBJTEXT[ITCARD], /* в                      */
			        ESD.BUF_ESD, /* массив                 */
			        80          /* об'ектных              */
			        );          /* карт                   */
			ITCARD += 1;              /*коррекц.инд-са своб.к-ты*/
			return (0);               /*успешное заверш.подпрогр*/
		}
	}
	return (2);                               /*ошибочное заверш.прогр. */
}
/*..........................................................................*/
int SUSING()                                      /*подпр.обр.пс.опер.USING */
{
	/*набор                   */
	char *METKA;                              /*рабочих                 */
	char *METKA1;                             /*переменных              */
	char *METKA2;                             /*                        */
	int J;                                    /*                        */
	int NBASRG;                               /*                        */
	METKA1 = strtok                           /*в перем. c указат.METKA1*/
	                 (                  /*выбираем первую лексему */
	                 (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND, /*операнда текущей карты  */
	                 ","                /*исх.текста АССЕМБЛЕРА   */
	                 );
	METKA2 = strtok                           /*в перем. c указат.METKA2*/
	                 (                  /*выбираем вторую лексему */
	                 NULL,              /*операнда текущей карты  */
	                 " "                /*исх.текста АССЕМБЛЕРА   */
	                 );
	if ( isalpha ( (int) *METKA2 ) )          /*если лексема начинается */
	{                                         /*с буквы, то:            */

		for ( J=0; J<=ITSYM; J++ )        /* все метки исх.текста в */
		{                                 /* табл. T_SYM сравниваем */
			/* со знач.перем. *METKA2 */
			METKA = strtok (
			        (char*) T_SYM[J].IMSYM, " "
			        );
			if( !strcmp ( METKA, METKA2 ) ) /* и при совпадении:      */
			{                         /*  запоминаем значение   */
				if ( (NBASRG = T_SYM[J].ZNSYM) <= 0x0f ) /*  метки в NBASRG и в сл.*/
					goto SUSING1; /*  NBASRG <= 0x0f идем на*/
				/*  устан.регистра базы   */
				else        /* иначе:                 */
					return (6); /*  сообщение об ошибке   */

			}
		}
		return (2);                       /*заверш.подпр.по ошибке  */
	}
	else                                      /*иначе, если второй опер.*/
	{                                         /*начинается с цифры, то: */
		NBASRG = atoi ( METKA2 );         /* запомним его в NBASRG  */
		if ( (NBASRG = T_SYM[J].ZNSYM) <= 0x0f ) /* и,если он <= 0x0f,то:  */
			goto SUSING1;             /* идем на устан.рег.базы */
		else                              /*иначе:                  */
			return (6);               /* сообщение об ошибке    */
	}

SUSING1:                                          /*установить базовый рег. */

	T_BASR [NBASRG - 1].PRDOST = 'Y';         /* взвести призн.активн.  */
	if ( *METKA1 == '*' )                     /* если перв.оп-нд == '*',*/
	{
		T_BASR[NBASRG-1].SMESH = CHADR;   /* выбир.знач.базы ==CHADR*/
	}
	else                                      /*иначе:                  */
	{
		for ( J=0; J<=ITSYM; J++ )        /* все метки исх.текста в */
		{                                 /* табл. T_SYM сравниваем */
			/* со знач.перем. *METKA1 */
			METKA = strtok (
			        (char*) T_SYM[J].IMSYM, " "
			        );
			if( !strcmp ( METKA, METKA1 ) ) /* и при совпадении:      */
			{                         /*  берем значение этой   */
				T_BASR[NBASRG-1].SMESH = T_SYM[J].ZNSYM; /*  этой метки как базу   */
			}
		}
		return (2);                       /*завершение прогр.по ошиб*/
	}
	return (0);                               /*успешное заверш.подпрогр*/
}
/*..........................................................................*/
int SRR()                                         /*подпр.обр.опер.RR-форм. */
{
	char *METKA;                              /*набор                   */
	char *METKA1;                             /*рабочих                 */
	char *METKA2;                             /*переменных              */
	unsigned char R1R2;                       /*                        */
	int J;                                    /*                        */
	RR.OP_RR.OP = T_MOP[I3].CODOP;            /*формирование кода операц*/

	METKA1 = strtok                           /*в перем. c указат.METKA1*/
	                 (                  /*выбираем первую лексему */
	                 (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND, /*операнда текущей карты  */
	                 ","                /*исх.текста АССЕМБЛЕРА   */
	                 );

	METKA2 = strtok                           /*в перем. c указат.METKA2*/
	                 (                  /*выбираем вторую лексему */
	                 NULL,              /*операнда текущей карты  */
	                 " "                /*исх.текста АССЕМБЛЕРА   */
	                 );

	if ( isalpha ( (int) *METKA1 ) )          /*если лексема начинается */
	{                                         /*с буквы, то:            */
		for ( J=0; J<=ITSYM; J++ )        /* все метки исх.текста в */
		{                                 /* табл. T_SYM сравниваем */
			/* со знач.перем. *METKA1 */
			METKA = strtok (
			        (char*) T_SYM[J].IMSYM, " "
			        );
			if( !strcmp ( METKA, METKA1 ) ) /* и при совпадении:      */
			{                         /*  берем значение этой   */
				R1R2 = T_SYM[J].ZNSYM << 4; /*  метки в качестве перв.*/
				goto SRR1;
			}                         /*  опреранда машинной ком*/
		}
		return(2);                        /*сообщ."необ'явл.идентиф"*/
	}
	else                                      /*иначе, берем в качестве */
	{                                         /*перв.операнда машинн.ком*/
		R1R2 = atoi ( METKA1 ) << 4;      /*значен.выбр.   лексемы  */
	}


SRR1:


	if ( isalpha ( (int) *METKA2 ) )          /*если лексема начинается */
	{                                         /*с буквы, то:            */
		for ( J=0; J<=ITSYM; J++ )        /* все метки исх.текста в */
		{                                 /* табл. T_SYM сравниваем */
			/* со знач.перем. *МЕТКА2 */
			METKA = strtok (
			        (char*) T_SYM[J].IMSYM, " "
			        );
			if( !strcmp ( METKA, METKA2 ) ) /* и при совпадении:      */
			{                         /*  берем значение этой   */
				R1R2 = R1R2 + T_SYM[J].ZNSYM; /*  метки в качестве втор.*/
				goto SRR2;  /*                        */
			}                         /*  опреранда машинной ком*/
		}                                 /*                        */
		return(2);                        /*сообщ."необ'явл.идентиф"*/
	}
	else                                      /*иначе, берем в качестве */
	{                                         /*втор.операнда машинн.ком*/
		R1R2 = R1R2 + atoi ( METKA2 );    /*значен.выбр.   лексемы  */
	}

SRR2:

	RR.OP_RR.R1R2 = R1R2;                     /*формируем опер-ды маш-ой*/
	/*команды                 */

	STXT(2);
	return(0);                                /*выйти из подпрограммы   */
}

/*..........................................................................*/
int SRX()                                         /*подпр.обр.опер.RX-форм. */
{
	char *METKA;                              /*набор                   */
	char *METKA1;                             /*рабочих                 */
	char *METKA2;                             /*переменных              */
	char *PTR;                                /*                        */
	int DELTA;                                /*                        */
	int ZNSYM;                                /*                        */
	int NBASRG;                               /*                        */
	int J;                                    /*                        */
	int I;                                    /*                        */
	unsigned char R1X2;                       /*                        */
	int B2D2;                                 /*                        */
	RX.OP_RX.OP = T_MOP[I3].CODOP;            /*формирование кода операц*/
	METKA1 = strtok                           /*в перем. c указат.METKA1*/
	                 (                  /*выбираем первую лексему */
	                 (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND, /*операнда текущей карты  */
	                 ","                /*исх.текста АССЕМБЛЕРА   */
	                 );

	METKA2 = strtok                           /*в перем. c указат.METKA2*/
	                 (                  /*выбираем вторую лексему */
	                 NULL,              /*операнда текущей карты  */
	                 " "                /*исх.текста АССЕМБЛЕРА   */
	                 );

	if ( isalpha ( (int) *METKA1 ) )          /*если лексема начинается */
	{                                         /*с буквы, то:            */
		for ( J=0; J<=ITSYM; J++ )        /* все метки исх.текста в */
		{                                 /* табл. T_SYM сравниваем */
			/* со знач.перем. *METKA  */
			METKA = strtok (
			        (char*) T_SYM[J].IMSYM, " "
			        );
			if( !strcmp ( METKA, METKA1 ) ) /* и при совпадении:      */

			{                         /*  берем значение этой   */
				R1X2 = T_SYM[J].ZNSYM << 4; /*  метки в качестве перв.*/
				goto SRX1;
			}                         /*  опреранда машинной ком*/
		}
        printf("Ошибка: %s\n",METKA1);
		return(2);                        /*сообщ."необ'явл.идентиф"*/
	}
	else                                      /*иначе, берем в качестве */
	{                                         /*перв.операнда машинн.ком*/
		R1X2 = atoi ( METKA1 ) << 4;      /*значен.выбр.   лексемы  */
	}


SRX1:


	if ( isalpha ( (int) *METKA2 ) )          /*если лексема начинается */
	{                                         /*с буквы, то:            */
		for ( J=0; J<=ITSYM; J++ )        /* все метки исх.текста в */
		{                                 /* табл. T_SYM сравниваем */
			/* со знач.перем. *МЕТКА  */
			METKA = strtok (
			        (char*) T_SYM[J].IMSYM, " "
			        );
			if( !strcmp ( METKA, METKA2 ) ) /* и при совпадении:      */
			{                         /*  установить нач.знач.: */
				NBASRG = 0; /*   номера базов.регистра*/
				DELTA  = 0xfff - 1;/*   и его значен.,а также*/
				ZNSYM  = T_SYM[J].ZNSYM;/*   смещен.втор.операнда */
				for ( I=0; I<15; I++ ) /*далее в цикле из всех   */
				{           /*рег-ров выберем базовым */
					if ( /*тот, который имеет:     */
					        T_BASR[I].PRDOST == 'Y' /* призн.активности,      */
					        && /*  и                     */
					        ZNSYM - T_BASR[I].SMESH >= 0 /* значенение, меньшее по */
					        && /* величине,но наиболее   */
					        ZNSYM - T_BASR[I].SMESH < DELTA /* близкое к смещению вто-*/
					        ) /* рого операнда          */
					{
						NBASRG = I + 1;
						DELTA  = ZNSYM - T_BASR[I].SMESH;
					}
				}
				if ( NBASRG == 0 || DELTA > 0xfff ) /*если баз.рег.не выбр.,то*/
					return(5); /* заверш.подпр.по ошибке */
				else        /*иначе                   */
				{           /* сформировыать машинное */
					B2D2 = NBASRG << 12; /* представление второго  */
					B2D2 = B2D2 + DELTA; /* операнда в виде B2D2   */
					PTR = (char *)&B2D2; /* и в соглашениях ЕС ЭВМ */
					swab ( PTR, PTR, 2 ); /* с записью в тело ком-ды*/
					RX.OP_RX.B2D2 = B2D2;
				}
				goto SRX2;  /*перех.на форм.первого   */
			}                         /*  опреранда машинной ком*/
		}
        printf("Ошибка: %s\n",METKA2);
		return(2);                        /*сообщ."необ'явл.идентиф"*/
	}
	else                                      /*иначе, берем в качестве */
	{                                         /*втор.операнда машинн.ком*/
        printf("Ошибка: %s\n",METKA2);
		return(4);                        /*значен.выбр.   лексемы  */
	}

SRX2:

	RX.OP_RX.R1X2 = R1X2;                     /*дозапись перв.операнда  */

	STXT(4);                                  /*формирование TXT-карты  */
	return(0);                                /*выйти из подпрограммы   */
}
//int SXX()                                         /*подпр.обр.опер.XX-форм. */
//{
//    XX.OP_XX.OP = T_MOP[I3].CODOP;
//    
//    printf ("%s\n", (char*)TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND);
//    char op1[8], op2[8];
//    int offset, length, B1D1, B2D2;
//    
//    if (4 == sscanf ((char*)TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND, "%8[^+]+%d(%d),%8s", op1, &offset, &length, op2)) {
//        printf ("%s %s %d %d\n", op1, op2, offset, length);
//        
//        int ide1, ide2;
//        if ((ide1 = get_symbol_index(op1)) < 0 || (ide2 = get_symbol_index(op2)) < 0) {
//            return 2;
//        }
//        
//        B1D1 = get_full_addr(T_SYM[ide1].ZNSYM) + offset;
//        if (-1 == B1D1) { printf ("bad addr of first operand\n"); return 2; }
//        swab ( &B1D1 , &B1D1 , 2 );
//        
//        B2D2 = get_full_addr(T_SYM[ide2].ZNSYM);
//        if (-1 == B2D2) { printf ("bad addr of second operand\n"); return 2; }
//        swab ( &B2D2 , &B2D2 , 2 );
//        
//        XX.OP_XX.L1L2 = length - 1;
//        XX.OP_XX.B1D1 = B1D1;
//        XX.OP_XX.B2D2 = B2D2;
//        
//        printf ("%d(%0X) %d(%0X)\n", ide1, B1D1, ide2, B2D2);
//    }
//    else
//    {
//        printf ("bad command format");
//        return 2; // another error code?
//    }
//    
//    STXT(6); 
//    return 0;
//}
/*..........................................................................*/
int SOBJFILE()                                    /*подпрогр.формир.об'екн. */
{                                                 /*файла                   */
	FILE *fp;                                 /*набор рабочих           */
	int RAB2;                                 /*переменных              */
	/*формирование пути и име-*/
	strcat ( NFIL, "tex" );                   /*ни об'ектного файла     */
	if ( (fp = fopen ( NFIL, "wb" )) == NULL ) /*при неудачн.открыт.ф-ла */
		return (-7);                      /* сообщение об ошибке    */
	else                                      /*иначе:                  */
		RAB2 =fwrite (OBJTEXT, 80, ITCARD, fp); /* формируем тело об.файла*/
	fclose ( fp );                            /*закрываем об'ектный файл*/
	return ( RAB2 );                          /*завершаем  подпрограмму */

}
/*..........................................................................*/
void INITUNION ()
{

/*
 ***** и н и ц и а л и з а ц и я   полей буфера формирования записей ESD-типа
 *****                             для выходного объектного файла
 */

	ESD.STR_ESD.POLE1      =  0x02;
	memcpy ( ESD.STR_ESD.POLE2, "ESD", 3 );
	memset ( ESD.STR_ESD.POLE3,  0x40, 6 );
	ESD.STR_ESD.POLE31 [0] = 0x00;
	ESD.STR_ESD.POLE31 [1] = 0x10;
	memset ( ESD.STR_ESD.POLE32, 0x40, 2 );
	ESD.STR_ESD.POLE4  [0] = 0x00;
	ESD.STR_ESD.POLE4  [1] = 0x01;
	memset ( ESD.STR_ESD.IMPR,   0x40, 8 );
	ESD.STR_ESD.POLE6      = 0x00;
	memset ( ESD.STR_ESD.ADPRG,  0x00, 3 );
	ESD.STR_ESD.POLE8      = 0x40;
	memset ( ESD.STR_ESD.DLPRG,  0x00, 3 );
	memset ( ESD.STR_ESD.POLE10, 0x40,40 );
	memset ( ESD.STR_ESD.POLE11, 0x40, 8 );

/*
 ***** и н и ц и а л и з а ц и я   полей буфера формирования записей TXT-типа
 *****                             для выходного объектного файла
 */

	TXT.STR_TXT.POLE1      =  0x02;
	memcpy ( TXT.STR_TXT.POLE2, "TXT", 3 );
	TXT.STR_TXT.POLE3      =  0x40;
	memset ( TXT.STR_TXT.ADOP,   0x00, 3 );
	memset ( TXT.STR_TXT.POLE5,  0x40, 2 );
	memset ( TXT.STR_TXT.DLNOP,  0X00, 2 );
	memset ( TXT.STR_TXT.POLE7,  0x40, 2 );
	TXT.STR_TXT.POLE71 [0] = 0x00;
	TXT.STR_TXT.POLE71 [1] = 0x01;
	memset ( TXT.STR_TXT.OPER,   0x40,56 );
	memset ( TXT.STR_TXT.POLE9,  0x40, 8 );

/*
 ***** и н и ц и а л и з а ц и я   полей буфера формирования записей END-типа
 *****                             для выходного объектного файла
 */

	END.STR_END.POLE1      =  0x02;
	memcpy ( END.STR_END.POLE2, "END", 3 );
	memset ( END.STR_END.POLE3,  0x40,68 );
	memset ( END.STR_END.POLE9,  0x40, 8 );

}

/*..........................................................................*/

int main( int argc, char **argv )                /*главная программа       */
{
	FILE *fp;
	char *ptr = argv [1];
	unsigned char ASSTEXT [DL_ASSTEXT][80];

/*
 ******* Б Л О К  об'явлений рабочих переменных
 */

	int I1, I2, RAB;                          /* переменные цикла      */

	INITUNION ();                             /* начальное заполнение  */
	/* буферов формирования  */
	/* записей выходного объ-*/
	/* ектного  файла        */

/*
 ******       Н А Ч А Л О   П Е Р В О Г О  П Р О С М О Т Р А      *****
 */
/*
 ***** Б Л О К  инициализации массива ASSTEXT, заменяющий иниц-ю в об'явлении
 *****          (введен как реакция на требования BORLANDC++ 2.0)
 */

	strcpy ( NFIL, ptr );

	if ( argc != 2 )

	{
		printf ( "%s\n", "Ошибка в командной строке" );
		return 1;
	}


	if ( strcmp ( &NFIL [ strlen ( NFIL )-3 ], "ass" ) )

	{
		printf ( "%s\n", "Неверный тип файла с исходным текстом" );
		return 1;
	}


	else

	{
		if ( (fp = fopen ( NFIL, "rb" )) == NULL ) /*при неудачн.открыт.ф-ла */
		/* сообщение об ошибке    */
		{
			printf ( "%s\n", "Не найден файл с исходным текстом" );
			return 1;
		}

		else

		{
			for ( I1 = 0; I1 <= DL_ASSTEXT; I1++ )

			{
                if ( fgets(ASSTEXT [I1], 80, fp) == NULL )
				//if ( !fread ( ASSTEXT [I1], 80, 1, fp ) )
				{
					if ( feof ( fp ) )
						goto main1;
					else
					{
						printf ( "%s\n", "Ошибка при чтении фыйла с исх.текстом" );
						return 1;
					}
				}
                for (int i=0; i<strlen(ASSTEXT [I1]); i++)
                {
                    if(ASSTEXT [I1][i]=='\n')
                        ASSTEXT [I1][i]=0;
                }
                //printf("%s\n",ASSTEXT [I1]);
			}

			printf ( "%s\n", "Переполнение буфера чтения исх.текста" );
			return 1;
		}

	}

main1:

	fclose ( fp );
	NFIL [ strlen ( NFIL )-3 ] = '\x0';

/*
 ***** К О Н Е Ц блока инициализации
 */

	for ( I1=0; I1 < DL_ASSTEXT; I1++ )       /*для карт с 1 по конечную*/
	{                                         /*                        */
		memcpy ( TEK_ISX_KARTA.BUFCARD, ASSTEXT[I1], /*ч-ть очередн.карту в буф*/
		         80 );/*                        */
		if (TEK_ISX_KARTA.STRUCT_BUFCARD.METKA [0] == /*переход при отсутствии  */
		    ' ') /*метки                   */
			goto CONT1;               /*на CONT1,               */
		/*иначе:                  */
		ITSYM += 1;                       /* переход к след.стр.TSYM*/
		PRNMET = 'Y';                     /* устан.призн.налич.метки*/
		memcpy ( T_SYM[ITSYM].IMSYM,      /* запомнить имя символа  */
		         TEK_ISX_KARTA.STRUCT_BUFCARD.METKA, 8 ); /* и                      */
		T_SYM[ITSYM].ZNSYM = CHADR;       /* его значение(отн.адр.) */
        genMoreSymbols(TEK_ISX_KARTA.STRUCT_BUFCARD.METKA);

/*
 ***** Б Л О К  поиска текущей операции среди псевдоопераций
 */

CONT1:
        printf("%s\n",TEK_ISX_KARTA.BUFCARD);
		for ( I2=0; I2 < NPOP; I2++ )     /*для всех стр.таб.пс.опер*/
		{                                 /*выполнить следущее:     */
			if(                       /* если                   */
			        !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAC,/* псевдооперация         */
			                T_POP[I2].MNCPOP, 5) /* распознана,            */
			        )           /* то:                    */
			/*                        */
			{ switch ( T_POP[I2].BXPROG () ) /* уйти в подпр.обработки */
			  {
			  case 0:
				  goto CONT2; /* и завершить цикл       */
			  case 1:
				  goto ERR1;
			  case 100:
				  goto CONT3;
			  }   }                   /*                        */
		}                                 /*                        */

		for ( I3=0; I3 < NOP; I3++ )      /*для всех стр.таб.м.опер.*/
		{                                 /*выполнить следующее:    */
			if(                       /* если                   */
			        !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAC,/* машинная операция      */
			                T_MOP[I3].MNCOP, 5) /* распознана,            */
			        )           /* то:                    */
			{
				T_MOP[I3].BXPROG (); /* уйти в подпр.обработки */
				PRNMET = 'N'; /* снять призн.налич.метки*/
				goto CONT2; /* и завершить цикл       */
			}
		}
		goto ERR3;                        /*сообщ.'мнемокод нерасп.'*/

CONT2:
		continue;                         /*конец цикла обработки   */
	}                                         /*карт исх.текста         */

/*
 ******       Н А Ч А Л О   В Т О Р О Г О  П Р О С М О Т Р А      *****
 */

CONT3:
        printf("\n\n\n\n");

	T_MOP[0].BXPROG = SRR; // BALR            /*установить указатели    */
	T_MOP[1].BXPROG = SRR; // BCR             /*на подпрограммы обраб-ки*/
	T_MOP[2].BXPROG = SRX; // ST              /*команд АССЕМБЛЕРА при   */
	T_MOP[3].BXPROG = SRX; // L               /*втором просмотре        */
	T_MOP[4].BXPROG = SRX; // A
	T_MOP[5].BXPROG = SRX; // S
    
    T_MOP[6].BXPROG = SRX; // SH
    T_MOP[7].BXPROG = SRX; // STH
    T_MOP[8].BXPROG = SRR; // CR
    T_MOP[9].BXPROG = SRX; // BC
    T_MOP[10].BXPROG = SRX; // LH

    
	T_POP[0].BXPROG = SDC;                    /*установить указатели    */
	T_POP[1].BXPROG = SDS;                    /*на подпрограммы обраб-ки*/
	T_POP[2].BXPROG = SEND;                   /*псевдокоманд АССЕМБЛЕРА */
	T_POP[3].BXPROG = SEQU;                   /*при втором просмотре    */
	T_POP[4].BXPROG = SSTART;
	T_POP[5].BXPROG = SUSING;

	for ( I1=0; I1 < DL_ASSTEXT; I1++ )       /*для карт с 1 по конечную*/
	{           /*                        */
		memcpy ( TEK_ISX_KARTA.BUFCARD, ASSTEXT [I1],/*ч-ть очередн.карту в буф*/
		         80 );/*                        */
/*
 ***** Б Л О К  поиска текущей операции среди псевдоопераций
 */

		for ( I2=0; I2 < NPOP; I2++ )     /*для всех стр.таб.пс.опер*/
		{                                 /*выполнить следущее:     */
			if(                       /* если                   */
			        !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAC,/* псевдооперация         */
			                T_POP[I2].MNCPOP, 5) /* распознана,            */
			        )           /* то                     */
			/*                        */
			{ switch ( T_POP[I2].BXPROG () ) /* уйти в подпр.обработки */
			  {
			  case 0:
				  goto CONT4; /* и завершить цикл       */
			  case 100:         /*уйти на формирование    */
				  goto CONT5; /*об'ектного файла        */
			  }   }                   /*                        */
		}                                 /*                        */

		for ( I3=0; I3 < NOP; I3++ )      /*для всех стр.таб.м.опер.*/
		{                                 /*выполнить следующее:    */
			if(                       /* если                   */
			        !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAC,/* машинная операция      */
			                T_MOP[I3].MNCOP, 5) /* распознана             */
			        )           /* то                     */
			/*                        */
			{
				switch ( T_MOP[I3].BXPROG () ) /* уйти в подпр.обработки */
				{
				case 0:
					goto CONT4; /* и завершить цикл       */

				case 2:     /*выдать диагностическое  */
					goto ERR2; /*сообщение               */

				case 4:     /*выдать диагностическое  */
					goto ERR4; /*сообщение               */

				case 5:     /*выдать диагностическое  */
					goto ERR5; /*сообщение               */

				case 6:     /*выдать диагностическое  */
					goto ERR6; /*сообщение               */

				case 7:     /*выдать диагностическое  */
					goto ERR6; /*сообщение               */

				}
			}

		}

CONT4:
		continue;                         /*конец цикла обработки   */
	}                                         /*карт исх.текста         */

CONT5:

	if ( ITCARD == (RAB = SOBJFILE ()) )      /*если в об'ектный файл   */
		/*выведены все карты, то: */
		printf                            /* сообшение об успешном  */
		        (                         /* завершении             */
		        "%s\n",
		        "успешое завершение трансляции"
		        );
	else                                      /*иначе:                  */
	{
		if ( RAB == -7 )
			goto ERR7;
		else
			printf                    /* сообшение о неудачном  */
			        (                 /* фрмировании об'ектного */
			        "%s\n",           /* файла                  */
			        "ошибка при формировании об'ектного файла"
			        );
	}
	return 1;                                   /*завершить main-прогр.   */

ERR1:
	printf ("%s\n","ошибка формата данных");  /*выдать диагностич.сообщ.*/
	goto CONT6;

ERR2:
	printf ("%s\n","необ'явленный идентификатор"); /*выдать диагностич.сообщ.*/
	goto CONT6;

ERR3:
	printf ("%s\n","ошибка кода операции");   /*выдать диагностич.сообщ.*/
	goto CONT6;

ERR4:
	printf ("%s\n","ошибка второго операнда"); /*выдать диагностич.сообщ.*/
	goto CONT6;

ERR5:
	printf ("%s\n","ошибка базирования");     /*выдать диагностич.сообщ.*/
	goto CONT6;

ERR6:
	printf ("%s\n","недопустимый номер регистра"); /*выдать диагностич.сообщ.*/
	goto CONT6;

ERR7:
	printf ("%s\n","ошибка открытия об'ектн.файла");/*выдать диагностич.сообщ.*/
	goto CONT6;


CONT6:
	printf ("%s%d\n","ошибка в карте N ",I1+1); /*выдать диагностич.сообщ.*/
    printf("%s\n",TEK_ISX_KARTA.BUFCARD);
	return 0;
}                                                 /*конец main-программы    */
/* return index in identifier's table or -1 if value doesn't exist */

int get_symbol_index (const char* METKA)
{
    for (int i = 0; i<=ITSYM; i++ )
    {
        if(!strncmp ( METKA , (char*) T_SYM[i].IMSYM, 8 ))
        {
            return i;
        }
    }
    
    return -1;
}

/* return absolute addr from relative or -1 on error */

int get_full_addr (int relative_addr)
{
    int NBASRG = 0;
    int DELTA  = 0xFFF - 1;
    
    for (int I=0; I<15; I++)
    {
        if (
            T_BASR[I].PRDOST == 'Y'                 &&
            relative_addr - T_BASR[I].SMESH >= 0    &&
            relative_addr - T_BASR[I].SMESH < DELTA
            )
        {
            NBASRG = I + 1;
            DELTA  = relative_addr - T_BASR[I].SMESH;
        }
    }
    
    if ( NBASRG == 0 || DELTA > 0xFFF )
    {
        return -1;
    }
    else
    {
        return (NBASRG << 12) + DELTA;
    }
}
