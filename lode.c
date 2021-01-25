//  ____   ___    ___    ____   ___   __ _                               
// |  _ \ ( _ )  / _ \  / ___| / _ \ / _| |___      ____ _ _ __ ___  ___ 
// | | | |/ _ \/\ | | | \___ \| | | | |_| __\ \ /\ / / _` | '__/ _ \/ __|
// | |_| | (_>  < |_| |  ___) | |_| |  _| |_ \ V  V / (_| | | |  __/\__ \ © 2017 Ondøej Sabela
// |____/ \___/\/\___/  |____/ \___/|_|  \__| \_/\_/ \__,_|_|  \___||___/ (A ti co zpívají tu weselou písnièku na konci, a ti co vymysleli nìkolik èástí tohoto kódu, a ti, co dali na internet ten zvuk výbuchu...)
//Vìnováno Ondrovi Koïouskovi :D
#include<stdio.h>
#include<stdlib.h>
#define EASTER 1

//Tento program se bude kompilovat dlouho kvuli easter eggu
//Aby se program zkompiloval musí se nastavit parametry kompilátoru v Nástroje->Nastavení pøekladaèe (Options->Compiler options)
// -lwinmm
//Kompilace s EASTER trvá asi 30 sekund

#include <windows.h>
#include <mmsystem.h>
#if EASTER
#include "lode.h"
#endif
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

#define INIT_STRING(array,index,init,condition,expression) for(index = init;condition;index++) array[index] = expression
#define CYCLE_N_TIMES(index, neco, expression) for(index = 0;index<neco;index++) expression
#define IN_DEBUG(expression) if(DEBUG)expression
#define FOREACH_SHIP_START int i,j;\
	for(i=0; i<vyska; i++) \
	{\
		for(j=0; j<sirka; j++)\
		{
#define FOREACH_SHIP_END \
	}\
	}

#define SHIP_NONE 0
#define SHIP_PRESENT 1 //lod pritomna
#define SHIP_EXPLODING 2 //vybuchujici lod
#define SHIP_EXPLODED 3 //vystreleno a zasazeno
#define SHIP_REVEALED 4 //Vystreleno a promrhano
//vice nez 4 = pocet promrhanych vystrelu 4

#define RESULT_HIT 1
#define RESULT_REVEALED 2
#define RESULT_NOTHING 3

#define COLOR_Black 0
#define COLOR_Gray 8
#define COLOR_Blue 1
#define COLOR_Green 2
#define COLOR_Aqua 3
#define COLOR_Red 4
#define COLOR_Purple 5
#define COLOR_Yellow 6
#define COLOR_White 7
#define COLOR_BrightWhite 15
#define COLOR_LightYellow 14
#define COLOR_LightPurple 13
#define COLOR_LightRed 12
#define COLOR_LightAqua 11
#define COLOR_LightBlue 9
#define COLOR_LightGreen 10

int WIDTH = 100;
int HEIGHT = 40;

int DEBUG = 0; //Pøepínaè módu ladìní, pøi ladìní vypisuje prùbìžný stav moøe a atd.

typedef struct
{
	char Message[100];
	int Result;
} ShootResult;
int more[20][20];

int sirka;
int vyska;

int remaining;
int attemptCount = 0;
int hit = 0;

int restart = 1;
int dobaHry = 0;
time_t startTime;
time_t stopTime;
int refreshCount;

int volume = 0;
//Hlasitost, kterou uzivatel nastavil

//Hraci metody
int getShip(int x,int y);
void setShip(int x,int y,int value);
void nextRound();
void newGame();
void menu();//Vykresli hlavni menu
void settings();//vykresli nastaveòové menu
void saveFile();//zapíše statistiku
void printFile();//Pøeète statistiku

void refresh();//Dìlá estetické mìnìní lodíze stavu 2 na stav 3 , který znamená to samé, akorát, že má jinou ikonku :)
void printMessage(const char* mes);//Vypíše zprávu do kolonky Hlášení: v pravém horním rohu
void printStats();//Vypíše staty jako poèet pokusù, poèet lodí atd.
void printSea();//Vykresli cele pole lodi doprostred obrazovky
void printSelectedCell(int x, int y);//Vykresli zamerovac
ShootResult shoot(int x,int y);//Vystreli
ShootResult forceShoot(int x,int y);//Vystreli a kasle na vsechny podminky, jestli je to moc daleko atd.

void SetColor(int color);//https://www.computerhope.com/color.htm Nastavi barvu popredi a pozadi zaroven Kod je pozadi * 16 + popredi
void ResetColor();//nastaví default barvu

char getInput();//Získá zmáèknutou klávesu a zároveò neblokuje UI
void GetMouseAndKeyboard(int *x, int *y, int *keyCode, void (*whatToDoWhileWaiting)());
void prepareForMouseInput();//Nastaví atributy konzole rpo zadávání myší
void hideCursor();
void showCursor();

void start();//Zapoène startovací sekvenci
void afterGame();//Zapoène konèící sekvenci
void progress();//Ukáže barevný prùbìh naèítání

COORD GetPosition();//Získá pozici kurzoru
void SetPosition(int x,int y);//Nastaví =||=
void shiftLineBack(int n);//Pøesune kurzor pro výpis na zaèátek pøedchozího øádku

void printIntoCenter(const char* str,int line,int borderWidth,char border);
void printIntoCenterArray(const char** str,int line,int borderWidth,int lineCount,char border);
void printBoxWithText(const char** strn,int lineCount,int borderWidth, int startLine);

void setVolume();  //vyzve k nastaveni hlastiosti

int Color(int BackGround,int Foreground)
{
	return BackGround*16 + Foreground; //Rychla metoda pro prevedeni barvy popredi a pozadi do jednoho kodu
}

int main()
{
	char resize[20];
	while(restart)//Program se bude donekoneèna restartovat, dokud nezvolí uživatel konec
	{
		attemptCount = 0;
		hit = 0;
		
		sprintf(resize,"mode con:cols=%d lines=%d",WIDTH,HEIGHT);//Nastaveni velikosti okna
		system ( resize );
		
		srand (time(NULL));//Inicializace generatoru nahodnych cisel
		start();
	}
	system("pause");
	return 0;
}

void start()
{
	CONSOLE_FONT_INFOEX cfi;

	SetConsoleTitleA("Lod\330 v C");

	system("cls");

	cfi.cbSize = sizeof cfi;
	cfi.nFont = 0;
	cfi.dwFontSize.X = 8;
	cfi.dwFontSize.Y = 16;
	cfi.FontFamily = FF_DONTCARE;             //Nastaveni fontu konzole na takovy, ktery je optimalni pro tuto hru
	cfi.FontWeight = FW_NORMAL;               //https://stackoverflow.com/questions/33975912/how-to-set-console-font-to-raster-font-programmatically
	wcscpy(cfi.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
#if EASTER
	printIntoCenterArray(art,0,WIDTH-2,27,' ');
#endif

	progress();
	menu();
}
void menu()
{
	int selected;
	const char *menu[] = {"Nova hra - 1","Archiv vysledku - 2","Nastaveni - 3","Konec - 4"};
	printBoxWithText(menu,4,24,10);//Zobrazi menu v kraasném ètvereèku
	printf("Zadejte cislo pozadovane moznosti\n");
	scanf("%d",&selected);
#if EASTER
	PlaySoundA(NULL,NULL,0);//Zastavi jakoukoli hudbu
#endif
	switch(selected)
	{
		case 1:
			newGame();
			time(&startTime);//Startne stopky hry(ulozi do startTime aktualni cas)
			while(remaining>0)
			{
				nextRound();
				if(restart)
				{
					break;
				}
			}
			if(restart)
			{
				break; //Pokud uživatel zvolil pøedèasné ukonèení hry, ukoèí cyklus kol hry
			}
			time(&stopTime);//Ukonci stopky
			dobaHry = difftime(stopTime, startTime);//Vypocita dobu behu v sekundach
			restart = 1;
			afterGame();
			break;
		case 2:
			printFile();
			system("pause");
			break;
		case 3:
			settings();//Zobrazi obrazovku nastaveni
			break;
		case 4:
			restart = 0;//Tady má promìnná restart jinou funkci, nastaví, že po skonèení této metody se už program nerestartuje
			break;
	}
}
void newGame()
{
	int pocetLodi,actualCount = 0,x,y,areYouShure;

	FOREACH_SHIP_START
	more[j][i]=0; //Projede celé pole a nastaví na nìm 0 protože tato metoda mohla být zavolána po pøedchozí høe
	FOREACH_SHIP_END
	
	if(volume == 0)//Aby to furt neotravovalo pri kazdem spusteni nove hry
	{
		printf("Pro zacatek zadejet hlasitost vasi posadky. Je to pro vase dobro.\n");
		setVolume();
	}

	do
	{
		printf("\nZadejte sirku more\n");
		fflush(stdin);
		scanf("%d",&sirka);
		printf("Zadejte vysku more\n");
		scanf("%d",&vyska);
		if(sirka<2||sirka>10||vyska<2||vyska>10)
		{
			printf("Spatna odpoved na velikost hraciho more\nChcete to tak opravdu nechat? (Posadka neruci za chyby)\n (0/1) ");
			scanf("%d",&areYouShure);
			if(areYouShure)
			{
				goto pokracovani;
			}
		}
	}
	while(sirka<2||sirka>10||vyska<2||vyska>10);
pokracovani:
	printf("Zadejte pocet lodi, ktere se vygeneruji, 0 pro nahodny pocet\n");
	scanf("%d",&pocetLodi);
	if(pocetLodi>sirka*vyska)
	{
		printf("Musi byt mensi nez sirka * vyska\n");
		goto pokracovani;
	}
	if(pocetLodi==0)
	{
		pocetLodi= (rand() % sirka*vyska) + 1;
	}
	remaining= pocetLodi;
	srand(time(NULL));//Inicializace generatoru nahodnych cisel
	while(actualCount<pocetLodi)
	{
		x = rand()% sirka;
		y = rand()% vyska;

		if(getShip(x,y)==SHIP_PRESENT)
			continue;

		setShip(x,y,SHIP_PRESENT);

		actualCount++;
	}
	IN_DEBUG(printf("%d vygenerovano\n",actualCount));
	system("cls");
}
void rePrintUI()
{
	printStats();
	printSea();
}
void nextRound()
{
	int sloupec;
	int radek;
	int znovu;
	char presto;
	int MouseX ,MouseY;
	int keyCode;
	ShootResult result;
	hideCursor();
	rePrintUI();
	do
	{
input :
		MouseX = -1;//Aby se dalo detekovat udalost na myši
		GetMouseAndKeyboard(&MouseX,&MouseY,&keyCode,rePrintUI);//Èeká na vstup a zároveò vykresluje hrací prostøedí

		//Korekce pozice myši v poli (kvùli knížeèce, která je zobrazena nad moøem)
		radek = MouseY -18;
		sloupec = (MouseX-(WIDTH/2-sirka-2))/2;
		sloupec--;

		IN_DEBUG(printf("Strilite na: X: %d, Y: %d\n",sloupec,radek));

		if(keyCode == VK_ESCAPE) //Escape ukoèí hru
		{
			restart = 1;
			return;
		}
		else if(MouseX == -1)//Pokud *náhodou* uživatel zmáèknul nìco jiného než ESC a nekliknul myší
		{
			goto input; //Pokud uživatel zmáèkl nìco jiného, tak musíme poèkat, dokud nezmáèkne ESC nebo myš
			restart = 0;
		}
		else//Pokud uživatel zmáèkl nìco jiného, nebo kliknul myší (nebyl ESC + byla myš)
		{
			restart = 0;
		}

		if(sloupec<0||sloupec>=sirka||radek<0||radek>=vyska)
		{
			SetColor(COLOR_LightRed);
			showCursor();
			printf("Tyto souradnice jsou od nas moc daleko. Chcete presto vystrelit?\nPosadka neruci za to co to udela. (0/1) ");
			ResetColor();
			fflush(stdin);
			do
			{
				presto = getInput();
			}
			while(presto==0);
			printf("%c\n",presto);
			if(presto=='1')
				break;
			else
			{
				attemptCount--;//Aby se nepocital tento pokus
				system("cls");//Jinak by zustal cerveny napis videt
				fflush(stdin);//Jinak by funkce GetMouseAndKeyboard na øádku 257(zaèátku této metody)
			}
			shiftLineBack(2);
			CYCLE_N_TIMES(znovu,WIDTH*2,printf(" "));//Prekresli napis vystrelit znovu?
		}
	}
	while(sloupec<0||sloupec>=sirka||radek<0||radek>=vyska);
	result = shoot(sloupec, radek);

	printMessage(result.Message);
	switch(result.Result)
	{
		case RESULT_REVEALED:
			printf("Vystrelit znovu? (0/1) ");
			showCursor();
			do
			{
				znovu = getInput();
			}
			while(znovu==0);

			if(znovu=='1')
			{
				result = forceShoot(sloupec,radek);
				printMessage(result.Message);
			}
			else
			{
				attemptCount--; //Aby se nepocitalo kdyz na neco dvakrat vystrelite ale podruhe si to rozmyslite
			}
			shiftLineBack(2);
			CYCLE_N_TIMES(znovu,WIDTH*2,printf(" "));//Prekresli napis vystrelit znovu?
			break;
		case RESULT_HIT:
#if EASTER
			PlaySound((LPCSTR)boom_wav, NULL, SND_MEMORY | SND_ASYNC );
#endif
			break;
	}
	refresh();
	showCursor();
}
void afterGame()
{
	char inp;
	hideCursor();
	system("cls");
	//Hudba
#if EASTER
	waveOutSetVolume(0,(0,0xffff & ((65535*volume*2)/100))+( 0xffff0000 & ((65535*volume*2)/100)));//Zdvojnasobi hlasitost danou uzivatelem
	PlaySound((LPCSTR)sailor_wav, NULL, SND_MEMORY | SND_ASYNC );
#endif
	do
	{
		inp = getInput();
		SetColor(rand()%16);//Náhodná barva
		SetPosition(rand()%(WIDTH-1),(rand()%HEIGHT-1) + 8);//Na náhodné pozici, ale aby nepøekreslil nápisy
		printf("%c",rand()%(256-33) + 33);//vypíše náhodný znak :), ale musí být vìtší, než 7, protože 7 znamená pípnutí konzole

		printIntoCenter("Porazili jsme je!",1,1,' ');
		SetPosition(0,3);
		SetColor(15);//Svetle bila
		printf("Chcete ulozit do statistiky? (0/1) \n");

	}
	while(inp==0);
	showCursor();
	if(inp=='1')
	{
		saveFile();
	}
	system("cls");
	showCursor();
}
void refresh()
{
	if(refreshCount%2 == 1)//Každý sudý refresh
	{
		FOREACH_SHIP_START
		if(getShip(j,i)==SHIP_EXPLODING)
		{
			setShip(j,i,SHIP_EXPLODED);
		}
		FOREACH_SHIP_END
	}
	refreshCount++;
}
void printSea()
{
	int i,j,k,n,actualShip;

	SetColor(Color(0,COLOR_BrightWhite));
	printf("   ");//Odsazeni pred radkem popisku ABCD atd.
	CYCLE_N_TIMES(n,WIDTH/2-sirka-3,printf(" "));//Zarovnani na stred pred radkem popisku ABCD atd.

	CYCLE_N_TIMES(k,sirka,printf("%c ",'A'+k));
	printf("\n");
	for(i=0; i<vyska; i++)
	{
		CYCLE_N_TIMES(n,WIDTH/2-sirka-3,printf(" "));//Zarovnani na stred

		//Popis sloupcu 1, 2 3...
		SetColor(Color(0,COLOR_BrightWhite));
		printf("%2d",i+1);

		//Odsazeni moøe aby bylo pìkné
		SetColor(Color(COLOR_LightBlue,COLOR_LightAqua));
		printf("%c",176);

		for(j=0; j<sirka; j++)
		{
			actualShip = getShip(j,i);
			switch(actualShip)
			{
				case SHIP_EXPLODING:
					SetColor(Color(COLOR_LightBlue,COLOR_LightRed));
					IN_DEBUG(printf("%d ",getShip(j,i)));
					else
						printf("X ");
					break;
				case SHIP_EXPLODED:
					SetColor(Color(COLOR_LightYellow,COLOR_Red));
					IN_DEBUG(printf("%d ",getShip(j,i)));
					else
						printf("%c ",207);
					break;
				case SHIP_REVEALED:
					SetColor(Color(COLOR_LightBlue,COLOR_Purple));
					IN_DEBUG(printf("%d ",getShip(j,i)));
					else
						printf("_ ");
					break;
				default:
					SetColor(Color(COLOR_LightBlue,COLOR_LightAqua));
					IN_DEBUG(printf("%d ",getShip(j,i)));
					else
						printf("%c%c",176,176);
					break;
			}
			ResetColor();
		}
		printf("\n");
	}
}
void printStats()
{
	char cas[20];
	time_t prubeznyCas;
	time_t prubeznaDoba;
	SetPosition(0,0);
	SetColor(Color(0,COLOR_LightYellow));
	puts("    ___________________   ___________________\n"
	     ".-/|       ~~**~~      \\ /      ~~**~~       |\\-.\n"
	     "||||      La Plan       :                    ||||\n"
	     "||||                    :                    ||||\n"
	     "||||                    :                    ||||\n"
	     "||||                    :                    ||||\n"
	     "||||                    :                    ||||\n"
	     "||||                    :     __/\\__    \177    ||||\n"
	     "||||                    :  ~~~\\____/~~~~~~   ||||\n"
	     "||||  ESC = hlavni menu :    ~  ~~~   ~.     ||||\n"
	     "||||___________________ : ___________________||||\n"
	     "||/====================\\:/====================\\||\n"
	     "`---------------------~___~--------------------''");
	SetColor(Color(0,COLOR_LightAqua));
	SetPosition(8,4);
	printf("Zbyvajici lode: ");
	SetPosition(35,4);
	printf("%d",remaining);
	SetColor(Color(0,COLOR_LightGreen));
	SetPosition(7,5);
	printf("Sestrelene lode:");
	SetPosition(35,5);
	printf("%d",hit);
	SetColor(Color(0,COLOR_LightRed));
	SetPosition(8,6);
	printf("Pocet pokusu: ");
	SetPosition(35,6);
	printf("%d",attemptCount);

	SetColor(COLOR_Aqua);
	SetPosition(WIDTH-30,3);
	time(&prubeznyCas);//Ukonci stopky
	prubeznaDoba = difftime(prubeznyCas, startTime);//Vypocita dobu behu v sekundach
	sprintf(cas,"Cas plavby: %d min. %d sek.",prubeznaDoba/60,prubeznaDoba%60);
	printf("%*s", 30, cas);
	SetPosition(WIDTH-30,4);
	printf("%*s", 30, "Hlaseni: ");

	SetPosition(0,15);
	SetColor(15);//Jasnì bílá
	printIntoCenter("SCH\220MA BITEVN\222HO POLE\n\n",15,0,'=');
}
void settings()
{
	const char *menu[] = {"Hlasitost - 1","Ladeni - 2","Veliksot okna - 3","Zpet - 4"};
	char resize[20];
	int input;
	while(input!=4)
	{
		system("cls");
		printBoxWithText(menu,4,24,10);
		printIntoCenter("Vyberte cislo pozadovane moznosti\n",17,0,' ');
		fflush(stdin);
		scanf("%d",&input);
		switch(input)
		{
			case 1:
				setVolume();
				break;
			case 2:
				printf("Pri zapnutem ladeni se zobrazuje odkryte more a dalsi informace.\nZapnout ladeni?(0/1)\n");
				scanf("%d",&DEBUG);
				break;
			case 3:
				printf("Zadejte sirku\n");
				scanf("%d",&WIDTH);
				printf("Zadejte vysku\n");
				scanf("%d",&HEIGHT);
				sprintf(resize,"mode con:cols=%d lines=%d",WIDTH,HEIGHT);
				system ( resize );
				break;
			case 4:
				return;
		}
	}
}
void setVolume()
{
	DWORD left;
	DWORD right;

	printf("Zadejte hlasitost: (0-100)\n");
	fflush(stdin);
	scanf("%d",&volume);

	left = 0xffff & ((65535*volume)/100) ;//Vypocitani hlasitosti kvuli logarytmickemu meritku
	right = 0xffff0000 & ((65535*volume)/100);//https://stackoverflow.com/questions/22785199/change-left-and-right-channels-volume-seperatly-waveoutsetvolume-c
	waveOutSetVolume(0,left+right);
}
void saveFile()
{
	char jmeno[50];
	int x,y;
	FILE *f;
	printf("Zadejte svoje jmeno pro zapis do statistiky\n");
	fflush(stdin);
	gets(jmeno);

	//otevreni souboru
	f=fopen("stats.txt","a");//pro postupne pridavani zaznamu
	if(f==NULL)
	{
		printf("\nSoubor se statistikou se nepovedlo otevrit\n");
		getchar();
	}
	fprintf(f,"%s\n",jmeno);
	fprintf(f,"%d\n",attemptCount);
	fprintf(f,"%d min %d sek\n",dobaHry/60,dobaHry%60);
	fprintf(f,"%d\n",hit);
	fprintf(f,"%d\n",sirka);
	fprintf(f,"%d\n",vyska);

	if (fclose(f)==EOF) printf("Soubor se nepodarilo zavrit\n");
}
void printFile()
{
	FILE *f;
	int w,h,pokus,zasah,sekundy,minuty;
	char name[40];

	f=fopen("stats.txt","r");       //otevreni souboru pro cteni
	if(f==NULL)
	{
		printf("\nSoubor se statistikou se nepovedlo otevrit\n");
		system("pause");
	}

	while( fgets(name,40,f) != NULL) //cteni ze souboru dokud nedojdou jmena k precteni
	{
		fscanf(f,"%d\n",&pokus);
		fscanf(f,"%d min %d sek\n",&minuty,&sekundy);
		fscanf(f,"%d\n",&zasah);
		fscanf(f,"%d\n",&w);
		fscanf(f,"%d\n",&h);
		printf("Jmeno: %s\nDoba hrani: \%d min %d sek\nPocet pokusu: %d\nCelkovy pocet lodi: %d"
		       "\nSirka more: %d\nVyska pole: %d\n\n",name,minuty,sekundy,pokus,zasah,w,h);
	}
	if (fclose(f)==EOF) printf("Soubor se nepodarilo zavrit\n");
}
void printMessage(const char* mes)
{
	//http://ideone.com/7wIBAB - Zarovnani doprava
	COORD StartPos;
	StartPos = GetPosition();
	SetPosition(WIDTH-40,5);
	SetColor(COLOR_LightGreen);
	printf("%*s", 40, mes);
	SetPosition(StartPos.X,StartPos.Y);
}

int getShip(int x,int y)
{
	return more[x][y];
}
void setShip(int x,int y,int value)
{
	more[x][y]=value;
}

ShootResult forceShoot(int x,int y)
{
	ShootResult result;
	int actualShipState = getShip(x,  y);
	if(actualShipState>SHIP_NONE)
	{
		if(actualShipState>=SHIP_REVEALED)
		{
			strcpy(result.Message, "Na tuto pozici promrhan dalsi vystrel.");
			setShip(x,y,actualShipState+1);
			result.Result =RESULT_NOTHING;

		}
		else
		{
			strcpy(result.Message, "Nepratelska lod zasazena!");
			setShip(x,y,SHIP_EXPLODING);
			result.Result = RESULT_HIT;
			hit++;
			remaining--;
		}
	}
	else
	{
		result.Result =RESULT_NOTHING;
	}
	return result;
}
ShootResult shoot(int x,int y)
{
	ShootResult result;
	int actualShipState = getShip(x,  y);
	if(actualShipState==SHIP_REVEALED)
	{
		strcpy(result.Message, "Na tuto pozici uz vystreleno.");
		result.Result = RESULT_REVEALED;
	}
	else if(actualShipState>SHIP_REVEALED)
	{
		sprintf(result.Message,"Na toto misto promrhano uz %d vystrelu.",actualShipState-SHIP_REVEALED + 1);
		result.Result = RESULT_REVEALED;
	}
	else if(actualShipState==SHIP_PRESENT)
	{
		strcpy(result.Message, "Nepratelska lod zasazena!");
		setShip(x,y,SHIP_EXPLODING);
		result.Result = RESULT_HIT;
		hit++;
		remaining--;
	}
	else
	{
		strcpy(result.Message,"Nic jsme netrefili.");
		setShip(x,y,SHIP_REVEALED);
		result.Result =RESULT_NOTHING;
	}
	attemptCount++;
	return result;
}

//
//Esteticke metody
//

void progress()//Vykresli prubehove pole s weselyma barvickama. Uííííííííííííí
{
	int prirustek = (rand() % 10) + 5;
	int i,j;
	for(i=0; i<WIDTH; i=i+prirustek)
	{
		if(prirustek>=WIDTH-i)
			prirustek = WIDTH-i;
		CYCLE_N_TIMES(j,prirustek, {SetColor((i+prirustek)%256); printf("=");})
		Sleep(100);
		prirustek = (rand() % 10) + 5;
	}
	shiftLineBack(2);
	Sleep(200);
	ResetColor();
	CYCLE_N_TIMES(j,WIDTH*2,printf(" "));
	printf("\n");
}
void shiftLineBack(int n)//Pøesune kurzor pro výpis na zaèátek pøedchozího øádku
{
	int i;
	CYCLE_N_TIMES(i,n,printf("\b\r"));//Vleze zpìt o jeden znak(na predchozi radek) a pak zase na  toho radku
}
void SetPosition(int X, int Y)
{
	HANDLE Screen;
	Screen = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD Position= {X, Y};

	SetConsoleCursorPosition(Screen, Position);
}
COORD GetPosition()
{
	HANDLE Screen;
	Screen = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	GetConsoleScreenBufferInfo(Screen, &SBInfo);
	return SBInfo.dwCursorPosition;
}
void SetColor(int color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
	SetConsoleTextAttribute(hConsole, color);
}
void ResetColor()
{
	SetColor(7);
}
void printIntoCenter(const char* str,int line,int borderWidth,char border)
{
	int i;
	SetPosition(WIDTH/2-borderWidth/2,line);
	printf("%c",border);

	CYCLE_N_TIMES(i,borderWidth-2,printf(" "));

	printf("%c",border);
	SetPosition(WIDTH/2-strlen(str)/2,line);
	printf(str);
}
void printIntoCenterArray(const char** str,int line,int borderWidth,int lineCount,char border)
{
	int i;
	CYCLE_N_TIMES(i,lineCount,printIntoCenter(str[i],i+line,borderWidth,border));
}
void printBoxWithText(const char** strn,int lineCount,int borderWidth, int startLine)
{
	int line;
	char okraj[WIDTH];
	COORD originalPos;
	originalPos = GetPosition();
	INIT_STRING(okraj,line,0,line<borderWidth,196);
	okraj[line] = 0;//pøipsání nulového znaku na konec

	//startLine == -1 = zaèít od støedu

	printIntoCenter(okraj,startLine,borderWidth,179);

	for(line=startLine+1; line<lineCount+startLine+1; line++)
	{
		printIntoCenter(strn[line-startLine-1],line,borderWidth,179);
	}

	printIntoCenter(okraj,line,borderWidth,179);
	SetPosition(originalPos.X,originalPos.Y);
}
char getInput()//Získá zmáèknutou klávesu a zároveò neblokuje UI
{
	char c = 0;
	if(kbhit())
		c = _getch();
	return c;
}
void prepareForMouseInput()//Nastaví atributy konzole rpo zadávání myší
{
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);//Získá handle , který je potøebná pro práci s atributy té konzole
	SetConsoleMode(hConsole,ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT |ENABLE_WINDOW_INPUT);//nastaví atributy konzole na to,
}																												   //aby se v ní nedalo vybírat oznaèením myší a aby pøijímala události o kliku myši a zmeny veliksoti okna
void GetMouseAndKeyboard(int *x, int *y, int *keyCode, void (*whatToDoWhileWaiting)())
{
	HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD InputRecord;
	DWORD Events;
	int processed = 0;
	prepareForMouseInput();
	FlushConsoleInputBuffer(hin);
	while(1)
	{
		ReadConsoleInput(hin, &InputRecord, 1, &Events);//Získá poslední vstupní událost z konzole. Pokud žádná nebyla, tak na ni èeká
		(*whatToDoWhileWaiting)();//Aby nebylo zablokovane UI
		switch ( InputRecord.EventType )
		{
			case KEY_EVENT: // keyboard input
				*keyCode = InputRecord.Event.KeyEvent.wVirtualKeyCode;
				processed = 1;
				break;

			case MOUSE_EVENT: // mouse input
				printSelectedCell(InputRecord.Event.MouseEvent.dwMousePosition.X,InputRecord.Event.MouseEvent.dwMousePosition.Y);
				if(InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
				{
					*x = InputRecord.Event.MouseEvent.dwMousePosition.X;
					*y = InputRecord.Event.MouseEvent.dwMousePosition.Y;
					processed = 1;
				}// mouse
				break;

			case WINDOW_BUFFER_SIZE_EVENT: // Resizovani okna - zajisti
				WIDTH = InputRecord.Event.WindowBufferSizeEvent.dwSize.X;
				HEIGHT = InputRecord.Event.WindowBufferSizeEvent.dwSize.Y;
				system("cls");
				printStats();
				printSea();
				break;

			case FOCUS_EVENT:  // disregard focus events

			case MENU_EVENT:   // disregard menu events

				break;
			default:
				printf("Neznamy typ vstupni udalosti\n");
				break;
		}
		FlushConsoleInputBuffer(hin);
		if(processed)
		{
			return;
		}
	}
}

void hideCursor()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}
void showCursor()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 10;
	info.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &info);
}
void printSelectedCell(int x, int y)//Vykresli zamerovac
{
	int radek = y -18;
	COORD origPos = GetPosition();
	int sloupec = (x-(WIDTH/2-sirka-2))/2;//Korekce pozice mysi v poli lodi
	sloupec--;
	if(sloupec>=0&&sloupec<sirka&&radek>=0&&radek<vyska)//Aby se zamerovac nevykresloval tam, kde lodì nejsou
	{
		SetPosition(x,y);
		SetColor(Color(COLOR_LightBlue,COLOR_LightRed));
		printf("#");
		ResetColor();
		SetPosition(origPos.X,origPos.Y);
	}
}
//Došli jste až sem? Gratuluji, vaše SPŠE. (Protože kulaté èíslo øádku by vypadalo že to psal nìkdo pøíèetný)
