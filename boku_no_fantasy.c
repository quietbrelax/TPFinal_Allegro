#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h> 
#include <allegro5/allegro_acodec.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "windows.h"

#define NAVEGACAO 0
#define BATALHA 1
#define FINAL 2
#define SELECAO 3
#define CREDITOS 4
#define TAM_HEROI 15
#define PASSO 1
#define TAM_OBJ 100
#define MAX_DIST 40
#define ATACAR 0
#define ESPECIAL 1
#define FUGIR 2
#define CIMA 0
#define ESQUERDA 1
#define BAIXO 2
#define DIREITA 3


const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

int CEU_H;
int X_OPT;
int Y_OPT;
ALLEGRO_FONT *FONTE;
ALLEGRO_FONT *FONTESELECAO;
int movingUp;
int movingDown;
int movingLeft;
int movingRight;
int indicadorHeroi;
int indicadorFinal;
int contadorSprite;


float dist(int x1, int y1, int x2, int y2){
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

typedef struct Heroi{
	int x, y;
	ALLEGRO_COLOR cor;
	float vida;

	int acao;
	int executar;
	int x_old, y_old;
	int direcao;
	int animacao;
	float vidaMax;
	int ataque;
	int ataqueEspecial;
	float agilidade;
	int defesa;
	int reacaoNPC;
	
} Heroi;

typedef struct Vilao
{
	int x, y;
	float raio;
	float vida;
	int turno;
	float vidaMax;
	int valor;
	int dificuldade;
	int ataque;
	ALLEGRO_COLOR cor;
} Vilao;

void initHeroi(Heroi * h){

	h-> x= TAM_HEROI;
	h-> y= SCREEN_H - 2 * TAM_HEROI;
	h-> acao = ATACAR;
	h->executar = 0;
	h-> x_old = h-> x;
	h-> y_old = h-> y;
	h->direcao = CIMA;
	h-> animacao = 0;
	h-> reacaoNPC = 0;
	if(indicadorHeroi == 0){
		h-> cor = al_map_rgb(132, 0, 255);
		h-> vida = 360;
		h-> vidaMax = 360;
		h-> ataque = 50;
		h-> ataqueEspecial = 360;
		h-> agilidade = 8;
		h-> defesa = 10;
	}
	else if(indicadorHeroi == 1){
		h-> cor =  al_map_rgb(0, 255, 247);
		h-> vida = 400;
		h-> vidaMax = 400;
		h-> ataque = 60;
		h-> ataqueEspecial = 170;
		h-> agilidade = 4;
		h-> defesa = 20;
	}
	else if(indicadorHeroi == 2){
		h-> cor = al_map_rgb(31, 102, 107);
		h-> vida = 440;
		h-> vidaMax = 440;
		h-> ataque = 70;
		h-> ataqueEspecial = 120;
		h-> agilidade = 2;
		h-> defesa = 40;
	}
	else if(indicadorHeroi == 3){
		h-> cor = al_map_rgb(207, 214, 0);
		h-> vida = 500;
		h-> vidaMax = 500;
		h-> ataque = 40;
		h-> ataqueEspecial = 80;
		h-> agilidade = 6;
		h-> defesa = 30;
	}
}

int muitoPerto(Vilao v1, Vilao v2){
	if(dist(v1.x, v1.y, v2.x, v2.y) < 3*v1.raio)
		return 1;
	else if(dist(v1.x, v2.x, 480, 0) <=200)
		return 1;
	else
		return 0;
}

void initVilao(Vilao * v){
	v -> x = rand() % 900 + 30;
	v -> y = rand() % 500 + 20;
	v-> dificuldade = rand() % 3;
	v -> turno = 0;
	if(v->dificuldade == 0){
	v -> raio = 20;
	v -> vida = 90;
	v -> vidaMax = 90;
	v -> valor = 50;
	v->cor = al_map_rgb(0, 255, 0);
	v-> ataque = 1;
	}
	else if(v->dificuldade == 1){
	v -> raio = 15;
	v -> vida = 150;
	v -> vidaMax = 150;
	v -> valor = 100;
	v->cor = al_map_rgb(0, 0, 255);
	v->ataque = 20;
	}
	else if(v->dificuldade == 2){
	v -> raio = 10;
	v -> vida = 300;
	v -> vidaMax = 300;
	v -> valor = 150;
	v->cor = al_map_rgb(255, 0, 0);
	v-> ataque = 50;
	}
	printf("(%d, %d)\n", v->x, v->y);
}

int colisaoDeViloes(Vilao v, Vilao viloes[], int n){
	for (int i = 0; i < n; ++i)
	{
		if(muitoPerto(v, viloes[i]))
			return 1;
	}
	if(v.x >= SCREEN_W - TAM_OBJ && v.y + TAM_HEROI <=TAM_OBJ){
		return 1;
	}
	return 0;
}

void initGlobais(){
	CEU_H = SCREEN_H/4;
	X_OPT = 3*SCREEN_W/4;
	Y_OPT = 3*SCREEN_H/4;

	int tam_fonte = 32;
	if(SCREEN_W < 300){
		tam_fonte = 20;
	}
	int tam_fonteSelecao = 20;
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    FONTE = al_load_font("arial.ttf", tam_fonte, 1);   
	if(FONTE == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	FONTESELECAO = al_load_font("arial.ttf", tam_fonteSelecao, 1);   
	if(FONTESELECAO == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	movingUp = 0;
	movingDown = 0;
	movingLeft = 0;
	movingRight = 0;
	indicadorHeroi = 0;
	indicadorFinal = 0;
	contadorSprite = 0;
}

void animacaoSprite(Heroi h, ALLEGRO_BITMAP *um, ALLEGRO_BITMAP *dois, ALLEGRO_BITMAP *tres){
	if(contadorSprite < 30){
			al_draw_bitmap(um , h.x, h.y, 0);
		}
		if(contadorSprite >= 30 && contadorSprite < 60){
			al_draw_bitmap(dois, h.x, h.y, 0);
		}
		if(contadorSprite >= 60 && contadorSprite < 90){
			al_draw_bitmap(um , h.x, h.y, 0);
		}
		if(contadorSprite >= 90 && contadorSprite < 120){
			al_draw_bitmap(tres, h.x, h.y, 0);
		}
		contadorSprite++;
		if(contadorSprite >= 120){
			contadorSprite = 0;
		}
}

void animacaoSelecao(ALLEGRO_BITMAP *um, ALLEGRO_BITMAP *dois, ALLEGRO_BITMAP *tres, int x){
	if(contadorSprite < 30){
			al_draw_bitmap(um , x, 70, 0);
		}
		if(contadorSprite >= 30 && contadorSprite < 60){
			al_draw_bitmap(dois, x, 70, 0);
		}
		if(contadorSprite >= 60 && contadorSprite < 90){
			al_draw_bitmap(um , x, 70, 0);
		}
		if(contadorSprite >= 90 && contadorSprite < 120){
			al_draw_bitmap(tres, x, 70, 0);
		}
		contadorSprite++;
		if(contadorSprite >= 120){
			contadorSprite = 0;
		}
}

void desenhaHeroiNaveg(Heroi h, ALLEGRO_BITMAP *frenteUm, ALLEGRO_BITMAP *frenteDois, ALLEGRO_BITMAP *frenteTres, ALLEGRO_BITMAP *direitaUm, ALLEGRO_BITMAP *direitaDois, ALLEGRO_BITMAP *direitaTres, ALLEGRO_BITMAP *costasUm, ALLEGRO_BITMAP *costasDois, ALLEGRO_BITMAP *costasTres, ALLEGRO_BITMAP *esquerdaUm, ALLEGRO_BITMAP *esquerdaDois, ALLEGRO_BITMAP *esquerdaTres){
	if(h.direcao == CIMA && movingUp == 0){
		al_draw_bitmap(costasUm , h.x, h.y, 0);
		contadorSprite = 0;
	}
	if(movingUp){
		animacaoSprite(h, costasUm, costasDois, costasTres);
	}
	else if(h.direcao == ESQUERDA && movingLeft == 0){
		al_draw_bitmap(esquerdaUm, h.x, h.y, 0);
		contadorSprite = 0;
	}
	if(movingLeft == 1 && movingUp == 0 && movingDown == 0 && movingRight == 0){
		animacaoSprite(h, esquerdaUm, esquerdaDois, esquerdaTres);
	}
	else if(h.direcao == BAIXO && movingDown == 0){
		al_draw_bitmap(frenteUm, h.x, h.y, 0);
		contadorSprite = 0;
	}
	if(movingDown == 1 && movingUp == 0){
		animacaoSprite(h, frenteUm, frenteDois, frenteTres);
	}
	else if(h.direcao == DIREITA && movingRight == 0){
		al_draw_bitmap(direitaUm, h.x, h.y, 0);
		contadorSprite = 0;
	}
	else if(movingRight == 1 && movingLeft == 1){
		al_draw_bitmap(esquerdaUm, h.x, h.y, 0);
	}
	if(movingRight == 1 && movingUp == 0 && movingDown == 0 && movingLeft == 0){
		animacaoSprite(h, direitaUm, direitaDois, direitaTres);
	}
}

void desenhaVilaoNaveg(Vilao v){
	if(v.vida > 0){
		al_draw_filled_circle(v.x, v.y, v.raio, v.cor);
	}
}

void desenhaNavegacao(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *casa, Heroi h, ALLEGRO_BITMAP *kanaFrente, ALLEGRO_BITMAP *kanaEsquerda, ALLEGRO_BITMAP *kanaDireita){
	al_draw_bitmap(background, 0, 0, 0);
	al_draw_bitmap(casa, SCREEN_W - 80, 0, 0);
	if(h.reacaoNPC == 0 || h.y >= 37){
		al_draw_bitmap(kanaFrente, 455, 0, 0);
	}
	if(h.reacaoNPC == 1 && h.y < 37 && h.x < 455){
		al_draw_bitmap(kanaEsquerda, 455, 0, 0);
	}
	if(h.reacaoNPC == 1 && h.y < 37 && h.x > 455){
		al_draw_bitmap(kanaDireita, 455, 0, 0);
	}
}
int desenhaFalaNpc(Heroi h){
	if(h.reacaoNPC == 1){
		al_draw_filled_rectangle(0, SCREEN_H - 100, SCREEN_W, SCREEN_H, al_map_rgb(2, 30, 99));
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, SCREEN_H - 100, 0, "Kana");
		if(h.vida == h.vidaMax){	
			al_draw_text(FONTE, al_map_rgb(255, 255, 255), 30, SCREEN_H - 50, 0, "Se tiver algum problema, eu posso te curar");
			return 0;
		}
		else{
			al_draw_text(FONTE, al_map_rgb(255, 255, 255), 30, SCREEN_H - 50, 0, "Tenha mais cuidado");
			return 1;
		}
	}
}

void pressionaTeclaNaveg(int tecla, Heroi * h, Heroi heroi){

	switch(tecla){
		case ALLEGRO_KEY_UP:
			movingUp = 1;
			break;
		case ALLEGRO_KEY_DOWN:
			movingDown = 1;
			break;
		case ALLEGRO_KEY_LEFT:
			movingLeft = 1;
			break;
		case ALLEGRO_KEY_RIGHT:
			movingRight = 1;
			break;
		case ALLEGRO_KEY_ENTER:
			if(dist(h-> x, h-> y, 480, 0) <= 80 && h-> reacaoNPC == 0){
				h-> reacaoNPC = 1;
				printf("NPC\n");
			}
			else if(dist(h->x, h->y, 480, 0) <= 80 && h->reacaoNPC == 1){
				if(desenhaFalaNpc(heroi) == 1){
					h->vida = h->vidaMax;
				}
				h->reacaoNPC = 0;
				printf("NAO NPC\n");
			}
			break;
	}
}

void despressionaTeclaNaveg(int tecla){
	
	switch(tecla){
		case ALLEGRO_KEY_UP:
			movingUp = 0;
			break;
		case ALLEGRO_KEY_DOWN:
			movingDown = 0;
			break;
		case ALLEGRO_KEY_LEFT:
			movingLeft = 0;
			break;
		case ALLEGRO_KEY_RIGHT:
			movingRight = 0;
			break;
	}
}

void processaTeclaNaveg(Heroi *h){
	
	h-> x_old = h->x;
	h-> y_old = h->y;

	if(h->reacaoNPC == 0){
		if(movingUp){
				if(h->y - PASSO >= 0){
					h-> y -= PASSO;
					h -> direcao = CIMA;
				}
			}
		if(movingDown){
				if(h->y + 40 + PASSO  <= SCREEN_H){
					h-> y += PASSO;
					h -> direcao = BAIXO;
				}
			}
		if(movingLeft){
				if(h->x - PASSO >= 0){
					h-> x -= PASSO;
					h-> direcao = ESQUERDA;
				}
			}
		if(movingRight){
			if(h->x + 40 + PASSO  <= SCREEN_W){
				h-> x += PASSO;
				h -> direcao = DIREITA;
			}
		}
	}
}

int chegouObjetivo(Heroi h){
	if(h.x >= SCREEN_W - TAM_OBJ && h.y + TAM_HEROI <=TAM_OBJ){
		return 1;
	}
	return 0;
}

void desenhaBatalha(Heroi h, int contadorDeTurno, Vilao v, ALLEGRO_BITMAP *backgroundBatalha, ALLEGRO_BITMAP *aranha, ALLEGRO_BITMAP *twice, ALLEGRO_BITMAP *dabi, ALLEGRO_BITMAP *dabiAtaque, ALLEGRO_BITMAP *heroiBatalha){
	al_draw_bitmap(backgroundBatalha, 0, 0, 0);
	if(v.dificuldade == 0){
		al_draw_bitmap(aranha, 100, 0, 0);
	}
	if(v.dificuldade == 1){
		al_draw_bitmap(twice, 150, 150, 0);
	}
	if(v.dificuldade == 2 && v.turno == 0){
		al_draw_bitmap(dabi, 130, 70, 0);
	}
	if(v.dificuldade == 2 && v.turno == 1){
		al_draw_bitmap(dabiAtaque, 130, 70, 0);
	}
	al_draw_bitmap(heroiBatalha, X_OPT - 50, Y_OPT - 200, 0);
	al_draw_filled_rectangle(X_OPT, Y_OPT, SCREEN_W, SCREEN_H, al_map_rgb(20, 16, 77));
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT + 10, Y_OPT + 10, 0, "Ataque");
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT + 10, Y_OPT + 50, 0, "Especial");
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT + 10, Y_OPT + 90, 0, "Fugir");
	al_draw_filled_rounded_rectangle(195, Y_OPT - 215, 305, Y_OPT - 190, 2, 2, al_map_rgb(255, 255, 255));
	al_draw_filled_rounded_rectangle(X_OPT + 90, Y_OPT - 215, X_OPT + 200, Y_OPT - 190, 2, 2, al_map_rgb(255, 255, 255));
	if(h.acao == ATACAR){
		al_draw_filled_circle( X_OPT + 150, Y_OPT + 20, 10, al_map_rgb(255, 0, 0));
	}
	else if(h.acao == ESPECIAL && contadorDeTurno > 5){
		al_draw_filled_circle( X_OPT + 150, Y_OPT + 60, 10, al_map_rgb(255, 0, 0));
	}
	else if(h.acao == FUGIR){
		al_draw_filled_circle( X_OPT + 150, Y_OPT + 100, 10, al_map_rgb(255, 0, 0));
	}
	
}

void desenhaHabilidade(int Xinicial, ALLEGRO_COLOR cor){
	al_draw_filled_rectangle(Xinicial + 50, 255, Xinicial + 70, 275, cor);
	al_draw_filled_rectangle(Xinicial + 80, 255, Xinicial + 100, 275, cor);
	al_draw_filled_rectangle(Xinicial + 110, 255, Xinicial + 130, 275, cor);
	al_draw_filled_rectangle(Xinicial + 140, 255, Xinicial + 160, 275, cor);
	al_draw_filled_rectangle(Xinicial + 170, 255, Xinicial + 190, 275, cor);
	al_draw_filled_rectangle(Xinicial + 50, 290, Xinicial + 70, 310, cor);
	al_draw_filled_rectangle(Xinicial + 80, 290, Xinicial + 100, 310, cor);
	al_draw_filled_rectangle(Xinicial + 110, 290, Xinicial + 130, 310, cor);
	al_draw_filled_rectangle(Xinicial + 140, 290, Xinicial + 160, 310, cor);
	al_draw_filled_rectangle(Xinicial + 170, 290, Xinicial + 190, 310, cor);
	al_draw_filled_rectangle(Xinicial + 50, 325, Xinicial + 70, 345, cor);
	al_draw_filled_rectangle(Xinicial + 80, 325, Xinicial + 100, 345, cor);
	al_draw_filled_rectangle(Xinicial + 110, 325, Xinicial + 130, 345, cor);
	al_draw_filled_rectangle(Xinicial + 140, 325, Xinicial + 160, 345, cor);
	al_draw_filled_rectangle(Xinicial + 170, 325, Xinicial + 190, 345, cor);
	al_draw_filled_rectangle(Xinicial + 50, 360, Xinicial + 70, 380, cor);
	al_draw_filled_rectangle(Xinicial + 80, 360, Xinicial + 100, 380, cor);
	al_draw_filled_rectangle(Xinicial + 110, 360, Xinicial + 130, 380, cor);
	al_draw_filled_rectangle(Xinicial + 140, 360, Xinicial + 160, 380, cor);
	al_draw_filled_rectangle(Xinicial + 170, 360, Xinicial + 190, 380, cor);
	al_draw_filled_rectangle(Xinicial + 50, 395, Xinicial + 70, 415, cor);
	al_draw_filled_rectangle(Xinicial + 80, 395, Xinicial + 100, 415, cor);
	al_draw_filled_rectangle(Xinicial + 110, 395, Xinicial + 130, 415, cor);
	al_draw_filled_rectangle(Xinicial + 140, 395, Xinicial + 160, 415, cor);
	al_draw_filled_rectangle(Xinicial + 170, 395, Xinicial + 190, 415, cor);
}

void escreveStatus(int Xinicial){
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), Xinicial, 255, 0, "Vida");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), Xinicial, 290, 0, "Atk");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), Xinicial, 325, 0, "Esp");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), Xinicial, 360, 0, "Def");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), Xinicial, 395, 0, "Agil");
}

void desenhaStatus(){
	escreveStatus(40);
	escreveStatus(270);
	escreveStatus(500);
	escreveStatus(730);
	//Kali
	desenhaHabilidade(40, al_map_rgb(140, 13, 214));
	al_draw_filled_rectangle(150, 255, 170, 275, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(180, 255, 200, 275, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(210, 255, 230, 275, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(180, 290, 200, 310, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(210, 290, 230, 310, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(150, 360, 170, 380, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(180, 360, 200, 380, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(210, 360, 230, 380, al_map_rgb(132, 144, 145));
	
	//Tony
	desenhaHabilidade(270, al_map_rgb(2, 227, 219));
	al_draw_filled_rectangle(410, 255, 430, 275, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(440, 255, 460, 275, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(440, 290, 460, 310, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(440, 325, 460, 345, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(410, 360, 430, 380, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(440, 360, 460, 380, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(410, 395, 430, 415, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(440, 395, 460, 415, al_map_rgb(132, 144, 145));
	//Viktor
	desenhaHabilidade(500, al_map_rgb(9, 64, 63));
	al_draw_filled_rectangle(670, 255, 690, 275, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(640, 325, 660, 345, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(670, 325, 690, 345, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(670, 360, 690, 380, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(580, 395, 600, 415, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(610, 395, 630, 415, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(640, 395, 660, 415, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(670, 395, 690, 415, al_map_rgb(132, 144, 145));

	//Jessy
	desenhaHabilidade(730, al_map_rgb(171, 201, 18));
	al_draw_filled_rectangle(840, 290, 860, 310, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(870, 290, 890, 310, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(900, 290, 920, 310, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(840, 325, 860, 345, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(870, 325, 890, 345, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(900, 325, 920, 345, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(900, 360, 920, 380, al_map_rgb(132, 144, 145));
	al_draw_filled_rectangle(900, 395, 920, 415, al_map_rgb(132, 144, 145));
}

void desenhaSelecao(ALLEGRO_BITMAP *kaliUm, ALLEGRO_BITMAP *kaliDois, ALLEGRO_BITMAP *kaliTres, ALLEGRO_BITMAP *tonyUm, ALLEGRO_BITMAP *tonyDois, ALLEGRO_BITMAP *tonyTres, ALLEGRO_BITMAP *viktorUm, ALLEGRO_BITMAP *viktorDois, ALLEGRO_BITMAP *viktorTres, ALLEGRO_BITMAP *jessyUm, ALLEGRO_BITMAP *jessyDois, ALLEGRO_BITMAP *jessyTres){
	al_clear_to_color(al_map_rgb(121, 173, 134));
	al_draw_filled_rectangle(40, 50, 230, 490, al_map_rgb(168, 187, 189));
	al_draw_filled_rectangle(270, 50, 460, 490, al_map_rgb(168, 187, 189));
	al_draw_filled_rectangle(500, 50, 690, 490, al_map_rgb(168, 187, 189));
	al_draw_filled_rectangle(730, 50, 920, 490, al_map_rgb(168, 187, 189));
	al_draw_filled_rectangle(0, 450, SCREEN_W, SCREEN_H, al_map_rgb(36, 0, 66));
	desenhaStatus();
	if(indicadorHeroi == 0){
		al_draw_filled_triangle(135, 80, 155, 30, 115, 30, al_map_rgb(140, 13, 214));
		animacaoSelecao(kaliUm, kaliDois, kaliTres, 60);
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, 460, 0, "Kali");
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, 500 , 0, "Vamos fazer nosso melhor");
		al_draw_bitmap(tonyUm, 290, 70, 0);
		al_draw_bitmap(viktorUm, 520, 70, 0);
		al_draw_bitmap(jessyUm, 750, 70, 0);
	}
	else if(indicadorHeroi == 1){
		al_draw_filled_triangle(365, 80, 345, 30, 385, 30, al_map_rgb(2, 227, 219));
		animacaoSelecao(tonyUm, tonyDois, tonyTres, 290);
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, 460, 0, "Tony");
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, 500 , 0, "Quer dar um role?");
		al_draw_bitmap(kaliUm, 60, 70, 0);
		al_draw_bitmap(viktorUm, 520, 70, 0);
		al_draw_bitmap(jessyUm, 750, 70, 0);
	}
	else if(indicadorHeroi == 2){
		al_draw_filled_triangle(595, 80, 575, 30, 615, 30, al_map_rgb(9, 64, 63));
		animacaoSelecao(viktorUm, viktorDois, viktorTres, 520);
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, 460, 0, "Viktor");
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, 500 , 0, "Apenas baixas inimigas");
		al_draw_bitmap(kaliUm, 60, 70, 0);
		al_draw_bitmap(tonyUm, 290, 70, 0);
		al_draw_bitmap(jessyUm, 750, 70, 0);
	}
	else if(indicadorHeroi == 3){
		al_draw_filled_triangle(825, 80, 805, 30, 845, 30, al_map_rgb(171, 201, 18));
		animacaoSelecao(jessyUm, jessyDois, jessyTres, 750);
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, 460, 0, "Jessy");
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 20, 500 , 0, "Its hunting time");
		al_draw_bitmap(kaliUm, 60, 70, 0);
		al_draw_bitmap(tonyUm, 290, 70, 0);
		al_draw_bitmap(viktorUm, 520, 70, 0);
	}
}	

void processaTeclaSelecao(Heroi *h, int tecla, int * modo_jogo){
	switch(tecla){
		case ALLEGRO_KEY_LEFT:
			indicadorHeroi --;
			if(indicadorHeroi < 0){
				indicadorHeroi = 3;
			}
			break;
		case ALLEGRO_KEY_RIGHT:
			indicadorHeroi ++;
			if(indicadorHeroi > 3){
				indicadorHeroi = 0;
			}
			break;
		case ALLEGRO_KEY_ENTER:
			*modo_jogo = NAVEGACAO;
			break;
	}
}

void processaTeclaFinal(int tecla){
	switch(tecla){
		case ALLEGRO_KEY_LEFT:
			indicadorFinal --;
			if(indicadorFinal < 0){
				indicadorFinal = 1;
			}
			break;
		case ALLEGRO_KEY_RIGHT:
			indicadorFinal ++;
			if(indicadorFinal > 1){
				indicadorFinal = 0;
			}
			break;
	}
}
 
int detectouMonstro(Heroi h, Vilao v){
	if(dist(h.x, h.y, v.x, v.y) <= MAX_DIST && v.vida > 0){
		printf("Detectou monstro\n");
		return 1;
	}
	return 0;
}

void processaTeclaBatalha(Heroi *h, int tecla, int contadorDeTurno){
	if(h->animacao == 0){
		switch(tecla){
			case ALLEGRO_KEY_UP:
				h-> acao --;
				if(h->acao < 0){
					h->acao = 2;
				}
				if(h->acao == ESPECIAL && contadorDeTurno <= 9){
					h-> acao--;
				}
				break;
			case ALLEGRO_KEY_DOWN:
				h ->acao ++;
				if(h->acao > 2){
					h->acao = 0;
				}
				if(h->acao == ESPECIAL && contadorDeTurno <= 9){
					h-> acao++;
				}
				printf("%d\n", h->acao);
				break;
			case ALLEGRO_KEY_ENTER:
				h-> executar = 1;
				break;
		}
	}
}

int processaAcaoHeroi(Heroi *h, int * contadorDeTurno, Vilao * v){

	if(h-> executar){

		h-> executar = 0;
		if(h-> acao == FUGIR){
			if(rand() % 10 + 1 > 10 - h -> agilidade){
				h-> x = h->x_old;
				h -> y = h->y_old;
				h-> acao = ATACAR;
				return NAVEGACAO;
		 	}
		 	else{
		 		h-> acao = ATACAR;
		 		printf("Nao fugiu\n");
		 		printf("\a\n");
		 		v -> turno = 1;
		 		return BATALHA;
		 	}
		}
		else if(h-> acao == ATACAR){
			h->animacao = 1;
		}

		else if(h-> acao == ESPECIAL){
			h->animacao = 1;
			if(indicadorHeroi == 0){
				system("kali.mp4");
			}
			if(indicadorHeroi == 1){
				system("tony.mp4");
			}
			if(indicadorHeroi == 2){
				system("viktor.mp4");
			}
			if(indicadorHeroi == 3){
				system("jessy.mp4");
			}
			al_rest(10);
			*contadorDeTurno = 0;
		}
	}
	return BATALHA;
}

void desenhaVida(Heroi h, Vilao v){
	al_draw_filled_rounded_rectangle(200, Y_OPT - 210, 200 + 100.0*(v.vida/v.vidaMax), Y_OPT - 195, 2, 2, al_map_rgb(0, 255, 0));
	al_draw_filled_rounded_rectangle(X_OPT + 95, Y_OPT - 210, X_OPT + 95 + 100.0*(h.vida/h.vidaMax), Y_OPT - 195, 2, 2, al_map_rgb(255*(1 - (h.vida/h.vidaMax)), 255*(h.vida/h.vidaMax), 0));
}

void desenhaCreditos(){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(FONTESELECAO, al_map_rgb(6, 184, 83), 40, 40, 0, "Criacao de Personagens");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 40, 60, 0, "Alice Melgaco: Jessy, Tony");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 40, 80, 0, "Clara Ruas: Aranha, Jessy, Kali, Kana");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 40, 100, 0, "Gabriela Barroso: Kana");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 40, 120, 0, "Kohei Horikoshi: Twice, Dabi");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 40, 140, 0, "Vitor Linhares: Jessy, Viktor");
	al_draw_text(FONTESELECAO, al_map_rgb(6, 184, 83), 400, 40, 0, "Artes");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 400, 60, 0, "Clara Ruas: Aranha, Background Naveg, Jessy,");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 420, 80, 0, "Kali, Kana, Tony, Twice, Viktor");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 400, 100, 0, "LEANBOOX(devianart): Dabi");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 400, 120, 0, "Vernisan(reddit): casa");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 400, 140, 0, "PixelChoice: Background Batalha");
	al_draw_text(FONTESELECAO, al_map_rgb(6, 184, 83), 40, 200, 0, "Animacoes Especial");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 40, 220, 0, "Clara Ruas: Jessy, Kali, Tony, Viktor");
	al_draw_text(FONTESELECAO, al_map_rgb(6, 184, 83), 40, 280, 0, "Musica");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 40, 300, 0, "CRASPORE - Flashbacks");
	al_draw_text(FONTESELECAO, al_map_rgb(6, 184, 83), 400, 200, 0, "Programacao");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 400, 220, 0, "Clara Ruas");
	al_draw_text(FONTESELECAO, al_map_rgb(50, 230, 128), 400, 260, 0, "Orientadores");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 400, 280, 0, "Pedro Olmo");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 400, 300, 0, "Vitor Rezende");
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 300, 360, 0, "Obrigada por jogar!");
	al_draw_text(FONTESELECAO, al_map_rgb(255, 255, 255), 300, 400, 0, "Pressione ENTER pra sair");
}

void desenhaTelaFinal(Heroi h, char my_text[], int * pontuacaoMax, int recorde, FILE *maiorPontuacao, char my_record[]){
	al_clear_to_color(al_map_rgb(0, 73, 77));
	if(h.vida <= 0){
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 350, 150, 0, "Voce perdeu ");
	}
	else{
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), 350, 150, 0, "Voce ganhou");
	}
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 350, 220, 0, "Sua pontuacao foi: ");
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 640, 220, 0, my_text);
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 350, 260, 0, "Seu recorde eh:");
	if(*pontuacaoMax <= recorde){
		maiorPontuacao = fopen("Pontuacao.txt", "w");
		fprintf(maiorPontuacao, "%d", recorde);
		*pontuacaoMax = recorde;
		fclose(maiorPontuacao);
	}
	sprintf(my_record, "%d", *pontuacaoMax);
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 580, 260, 0, my_record);
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 350, 300, 0, "Jogar novamente");
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 650, 300, 0, "Creditos/Sair");
	if(indicadorFinal == 0){
		al_draw_filled_circle(330, 320, 15, al_map_rgb(255, 0, 0));
	}
	else if(indicadorFinal == 1){
		al_draw_filled_circle(630, 320, 15, al_map_rgb(255, 0, 0));
	}

}

void desenhaAtaque(int *velocidade, Heroi * h, Vilao * v, int * recorde, int * modo_jogo, int contadorDeTurno){
	al_draw_filled_circle(X_OPT - 100 - *velocidade, Y_OPT - 90, 10, al_map_rgb(255, 255, 255));
	*velocidade += 5;
	if(X_OPT - 100 - *velocidade < 200){
		h->animacao = 0;
		*velocidade = 0;
		v-> turno = 1;
		if(indicadorHeroi == 0){
			if(v -> dificuldade == 0){
				v-> vida -= (h-> ataque - 40);
			}
			else{
				v-> vida -= (h-> ataque + 50 *(1 - (h->vida/h->vidaMax)));
			}
		}
		else{
			v-> vida -= h-> ataque;
		}
		printf("%d\n", contadorDeTurno);
		if(v-> vida <= 0){
			*recorde += v-> valor;
			*modo_jogo = NAVEGACAO;
		}
	}
}

void desenhaAtaqueVilao(int *valorAtaqueVilao, int *fatorAleatorio, Vilao *v, int *velocidadeVilao, Heroi * h){
	*valorAtaqueVilao = *fatorAleatorio + v->ataque;
	if(v-> dificuldade == 2){
		al_draw_filled_circle( 200 + *velocidadeVilao, Y_OPT - 90, *valorAtaqueVilao/5, al_map_rgb(24, 193, 245));
	}
	else{
		al_draw_filled_circle( 200 + *velocidadeVilao, Y_OPT - 90, *valorAtaqueVilao/5, al_map_rgb(255, 255, 255));
	}
	*velocidadeVilao += 5;
	if(200 + *velocidadeVilao > X_OPT - 100){
		v->turno = 0;
		*velocidadeVilao = 0;
		h->vida -= *valorAtaqueVilao + h-> defesa;
		printf("%d\n", *valorAtaqueVilao);
		*fatorAleatorio = rand() % 50;
	}
}

int confereImagem(ALLEGRO_BITMAP *imagem, ALLEGRO_TIMER *timer, ALLEGRO_DISPLAY *display, int i){
 	if(!imagem) {
    	fprintf(stderr, "Falha ao criar um bitmap! %d\n", i);
    	al_destroy_display(display);
    	al_destroy_timer(timer);
		return -1;
  	}
}

int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	//---------- DECLARAÇÃO DE VARIAVEIS -------------
	//Navegacao
	 ALLEGRO_BITMAP *background = NULL;
	 ALLEGRO_BITMAP *casa = NULL;
	 ALLEGRO_BITMAP *kanaFrente = NULL;
	 ALLEGRO_BITMAP *kanaEsquerda = NULL;
	 ALLEGRO_BITMAP *kanaDireita = NULL;
	 //Batalha
	 ALLEGRO_BITMAP *backgroundBatalha = NULL;
	 ALLEGRO_BITMAP *aranha = NULL;
	 ALLEGRO_BITMAP *twice = NULL;
	 ALLEGRO_BITMAP *dabi = NULL;
	 ALLEGRO_BITMAP *dabiAtaque = NULL;
	 //Sons
	 ALLEGRO_SAMPLE *musicaNaveg = NULL;
	 ALLEGRO_SAMPLE *musicaBatalha = NULL;
	 //Personagens
	 //Navegacao
	 //Kali
	 ALLEGRO_BITMAP *kaliFrenteUm = NULL;
	 ALLEGRO_BITMAP *kaliFrenteDois = NULL;
	 ALLEGRO_BITMAP *kaliFrenteTres = NULL;
	 ALLEGRO_BITMAP *kaliCostasUm = NULL;
	 ALLEGRO_BITMAP *kaliCostasDois = NULL;
	 ALLEGRO_BITMAP *kaliCostasTres = NULL;
	 ALLEGRO_BITMAP *kaliDireitaUm = NULL;
	 ALLEGRO_BITMAP *kaliDireitaDois = NULL;
	 ALLEGRO_BITMAP *kaliDireitaTres = NULL;
	 ALLEGRO_BITMAP *kaliEsquerdaUm = NULL;
	 ALLEGRO_BITMAP *kaliEsquerdaDois = NULL;
	 ALLEGRO_BITMAP *kaliEsquerdaTres = NULL;
	 //Tony
	 ALLEGRO_BITMAP *tonyFrenteUm = NULL;
	 ALLEGRO_BITMAP *tonyFrenteDois = NULL;
	 ALLEGRO_BITMAP *tonyFrenteTres = NULL;
	 ALLEGRO_BITMAP *tonyCostasUm = NULL;
	 ALLEGRO_BITMAP *tonyCostasDois = NULL;
	 ALLEGRO_BITMAP *tonyCostasTres = NULL;
	 ALLEGRO_BITMAP *tonyDireitaUm = NULL;
	 ALLEGRO_BITMAP *tonyDireitaDois = NULL;
	 ALLEGRO_BITMAP *tonyDireitaTres = NULL;
	 ALLEGRO_BITMAP *tonyEsquerdaUm = NULL;
	 ALLEGRO_BITMAP *tonyEsquerdaDois = NULL;
	 ALLEGRO_BITMAP *tonyEsquerdaTres = NULL;
	 //Viktor
	 ALLEGRO_BITMAP *viktorFrenteUm = NULL;
	 ALLEGRO_BITMAP *viktorFrenteDois = NULL;
	 ALLEGRO_BITMAP *viktorFrenteTres = NULL;
	 ALLEGRO_BITMAP *viktorCostasUm = NULL;
	 ALLEGRO_BITMAP *viktorCostasDois = NULL;
	 ALLEGRO_BITMAP *viktorCostasTres = NULL;
	 ALLEGRO_BITMAP *viktorDireitaUm = NULL;
	 ALLEGRO_BITMAP *viktorDireitaDois = NULL;
	 ALLEGRO_BITMAP *viktorDireitaTres = NULL;
	 ALLEGRO_BITMAP *viktorEsquerdaUm = NULL;
	 ALLEGRO_BITMAP *viktorEsquerdaDois = NULL;
	 ALLEGRO_BITMAP *viktorEsquerdaTres = NULL;
	 //Jessy
	 ALLEGRO_BITMAP *jessyFrenteUm = NULL;
	 ALLEGRO_BITMAP *jessyFrenteDois = NULL;
	 ALLEGRO_BITMAP *jessyFrenteTres = NULL;
	 ALLEGRO_BITMAP *jessyCostasUm = NULL;
	 ALLEGRO_BITMAP *jessyCostasDois = NULL;
	 ALLEGRO_BITMAP *jessyCostasTres = NULL;
	 ALLEGRO_BITMAP *jessyDireitaUm = NULL;
	 ALLEGRO_BITMAP *jessyDireitaDois = NULL;
	 ALLEGRO_BITMAP *jessyDireitaTres = NULL;
	 ALLEGRO_BITMAP *jessyEsquerdaUm = NULL;
	 ALLEGRO_BITMAP *jessyEsquerdaDois = NULL;
	 ALLEGRO_BITMAP *jessyEsquerdaTres = NULL;
	 //Batalha
	 //Ataque Normal
	 ALLEGRO_BITMAP *kaliBatalha = NULL;
	 ALLEGRO_BITMAP *tonyBatalha = NULL;
	 ALLEGRO_BITMAP *viktorBatalha = NULL;
	 ALLEGRO_BITMAP *jessyBatalha = NULL;
	 //Selecao
	 //Kali
	 ALLEGRO_BITMAP *kaliSelecaoUm = NULL;
	 ALLEGRO_BITMAP *kaliSelecaoDois = NULL;
	 ALLEGRO_BITMAP *kaliSelecaoTres = NULL;
	 //tony
	 ALLEGRO_BITMAP *tonySelecaoUm = NULL;
	 ALLEGRO_BITMAP *tonySelecaoDois = NULL;
	 ALLEGRO_BITMAP *tonySelecaoTres = NULL;
	 //viktor
	 ALLEGRO_BITMAP *viktorSelecaoUm = NULL;
	 ALLEGRO_BITMAP *viktorSelecaoDois = NULL;
	 ALLEGRO_BITMAP *viktorSelecaoTres = NULL;
	 //jessy
	 ALLEGRO_BITMAP *jessySelecaoUm = NULL;
	 ALLEGRO_BITMAP *jessySelecaoDois = NULL;
	 ALLEGRO_BITMAP *jessySelecaoTres = NULL;

   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

	//BITMAPS
	//Navegacao
	background = al_load_bitmap("Bitmaps\\background.png");
 	confereImagem(background, timer, display, 0);
	casa = al_load_bitmap("Bitmaps\\casa.png");
	confereImagem(casa, timer, display, 0);
  //sprite kana
	kanaFrente = al_load_bitmap("Bitmaps\\kana frente.png");
	confereImagem(kanaFrente, timer, display, 0);
	kanaDireita = al_load_bitmap("Bitmaps\\kana direita.png");
	confereImagem(kanaDireita, timer, display, 0);
	kanaEsquerda = al_load_bitmap("Bitmaps\\kana esquerda.png");
	confereImagem(kanaEsquerda, timer, display, 0);
  //Batalha
	backgroundBatalha = al_load_bitmap("Bitmaps\\background batalha.png");
	confereImagem(backgroundBatalha, timer, display, 0);
	aranha = al_load_bitmap("Bitmaps\\aranha.png");
	confereImagem(aranha, timer, display, 0);
	twice = al_load_bitmap("Bitmaps\\twice.png");
	confereImagem(twice, timer, display, 0);
	dabi = al_load_bitmap("Bitmaps\\dabi.png");
	confereImagem(dabi, timer, display, 0);
  dabiAtaque = al_load_bitmap("Bitmaps\\dabi ataque.png");
	confereImagem(dabiAtaque, timer, display, 0);
  //SPRITE DOS PERSONAGENS, NAVEGAÇÃO
  //Kali
	kaliFrenteUm = al_load_bitmap("Bitmaps\\kali frente um.png");
 	confereImagem(kaliFrenteUm, timer, display, 0);
	kaliFrenteDois = al_load_bitmap("Bitmaps\\kali frente dois.png");
 	confereImagem(kaliFrenteDois, timer, display, 0);
	kaliFrenteTres = al_load_bitmap("Bitmaps\\kali frente tres.png");
 	confereImagem(kaliFrenteTres, timer, display, 0);
	kaliCostasUm = al_load_bitmap("Bitmaps\\kali costas um.png");
 	confereImagem(kaliCostasUm, timer, display, 0);
	kaliCostasDois = al_load_bitmap("Bitmaps\\kali costas dois.png");
 	confereImagem(kaliCostasDois, timer, display, 0);
	kaliCostasTres = al_load_bitmap("Bitmaps\\kali costas tres.png");
 	confereImagem(kaliCostasTres, timer, display, 0);
	kaliDireitaUm = al_load_bitmap("Bitmaps\\kali direita um.png");
 	confereImagem(kaliDireitaUm, timer, display, 0);
	kaliDireitaDois = al_load_bitmap("Bitmaps\\kali direita dois.png");
 	confereImagem(kaliDireitaDois, timer, display, 0);
	kaliDireitaTres = al_load_bitmap("Bitmaps\\kali direita tres.png");
 	confereImagem(kaliDireitaTres, timer, display, 0);
	kaliEsquerdaUm = al_load_bitmap("Bitmaps\\kali esquerda um.png");
 	confereImagem(kaliEsquerdaUm, timer, display, 0);
	kaliEsquerdaDois = al_load_bitmap("Bitmaps\\kali esquerda dois.png");
 	confereImagem(kaliEsquerdaDois, timer, display, 0);
	kaliEsquerdaTres = al_load_bitmap("Bitmaps\\kali esquerda tres.png");
 	confereImagem(kaliEsquerdaTres, timer, display, 0);
  //Tony
 	tonyFrenteUm = al_load_bitmap("Bitmaps\\tony frente um.png");
 	confereImagem(tonyFrenteUm, timer, display, 0);
	tonyFrenteDois = al_load_bitmap("Bitmaps\\tony frente dois.png");
 	confereImagem(tonyFrenteDois, timer, display, 0);
	tonyFrenteTres = al_load_bitmap("Bitmaps\\tony frente tres.png");
 	confereImagem(tonyFrenteTres, timer, display, 0);
	tonyCostasUm = al_load_bitmap("Bitmaps\\tony costas um.png");
 	confereImagem(tonyCostasUm, timer, display, 0);
	tonyCostasDois = al_load_bitmap("Bitmaps\\tony costas dois.png");
 	confereImagem(tonyCostasDois, timer, display, 0);
	tonyCostasTres = al_load_bitmap("Bitmaps\\tony costas tres.png");
 	confereImagem(tonyCostasTres, timer, display, 0);
	tonyDireitaUm = al_load_bitmap("Bitmaps\\tony direita um.png");
 	confereImagem(tonyDireitaUm, timer, display, 0);
	tonyDireitaDois = al_load_bitmap("Bitmaps\\tony direita dois.png");
 	confereImagem(tonyDireitaDois, timer, display, 0);
	tonyDireitaTres = al_load_bitmap("Bitmaps\\tony direita tres.png");
 	confereImagem(tonyDireitaTres, timer, display, 0);
	tonyEsquerdaUm = al_load_bitmap("Bitmaps\\tony esquerda um.png");
 	confereImagem(tonyEsquerdaUm, timer, display, 0);
	tonyEsquerdaDois = al_load_bitmap("Bitmaps\\tony esquerda dois.png");
 	confereImagem(tonyEsquerdaDois, timer, display, 0);
	tonyEsquerdaTres = al_load_bitmap("Bitmaps\\tony esquerda tres.png");
 	confereImagem(tonyEsquerdaTres, timer, display, 0);
  //Viktor
 	viktorFrenteUm = al_load_bitmap("Bitmaps\\viktor frente um.png");
 	confereImagem(viktorFrenteUm, timer, display, 0);
	viktorFrenteDois = al_load_bitmap("Bitmaps\\viktor frente dois.png");
 	confereImagem(viktorFrenteDois, timer, display, 0);
	viktorFrenteTres = al_load_bitmap("Bitmaps\\viktor frente tres.png");
 	confereImagem(viktorFrenteTres, timer, display, 0);
	viktorCostasUm = al_load_bitmap("Bitmaps\\viktor costas um.png");
 	confereImagem(viktorCostasUm, timer, display, 0);
	viktorCostasDois = al_load_bitmap("Bitmaps\\viktor costas dois.png");
 	confereImagem(viktorCostasDois, timer, display, 0);
	viktorCostasTres = al_load_bitmap("Bitmaps\\viktor costas tres.png");
 	confereImagem(viktorCostasTres, timer, display, 0);
	viktorDireitaUm = al_load_bitmap("Bitmaps\\viktor direita um.png");
 	confereImagem(viktorDireitaUm, timer, display, 0);
	viktorDireitaDois = al_load_bitmap("Bitmaps\\viktor direita dois.png");
 	confereImagem(viktorDireitaDois, timer, display, 0);
	viktorDireitaTres = al_load_bitmap("Bitmaps\\viktor direita tres.png");
 	confereImagem(viktorDireitaTres, timer, display, 0);
	viktorEsquerdaUm = al_load_bitmap("Bitmaps\\viktor esquerda um.png");
 	confereImagem(viktorEsquerdaUm, timer, display, 0);
	viktorEsquerdaDois = al_load_bitmap("Bitmaps\\viktor esquerda dois.png");
 	confereImagem(viktorEsquerdaDois, timer, display, 0);
	viktorEsquerdaTres = al_load_bitmap("Bitmaps\\viktor esquerda tres.png");
 	confereImagem(viktorEsquerdaTres, timer, display, 0);
  //Jessy
 	jessyFrenteUm = al_load_bitmap("Bitmaps\\jessy frente um.png");
 	confereImagem(jessyFrenteUm, timer, display, 0);
	jessyFrenteDois = al_load_bitmap("Bitmaps\\jessy frente dois.png");
 	confereImagem(jessyFrenteDois, timer, display, 0);
	jessyFrenteTres = al_load_bitmap("Bitmaps\\jessy frente tres.png");
 	confereImagem(jessyFrenteTres, timer, display, 0);
	jessyCostasUm = al_load_bitmap("Bitmaps\\jessy costas um.png");
 	confereImagem(jessyCostasUm, timer, display, 0);
	jessyCostasDois = al_load_bitmap("Bitmaps\\jessy costas dois.png");
 	confereImagem(jessyCostasDois, timer, display, 0);
	jessyCostasTres = al_load_bitmap("Bitmaps\\jessy costas tres.png");
 	confereImagem(jessyCostasTres, timer, display, 0);
	jessyDireitaUm = al_load_bitmap("Bitmaps\\jessy direita um.png");
 	confereImagem(jessyDireitaUm, timer, display, 0);
	jessyDireitaDois = al_load_bitmap("Bitmaps\\jessy direita dois.png");
 	confereImagem(jessyDireitaDois, timer, display, 0);
	jessyDireitaTres = al_load_bitmap("Bitmaps\\jessy direita tres.png");
 	confereImagem(jessyDireitaTres, timer, display, 0);
	jessyEsquerdaUm = al_load_bitmap("Bitmaps\\jessy esquerda um.png");
 	confereImagem(jessyEsquerdaUm, timer, display, 0);
	jessyEsquerdaDois = al_load_bitmap("Bitmaps\\jessy esquerda dois.png");
 	confereImagem(jessyEsquerdaDois, timer, display, 0);
	jessyEsquerdaTres = al_load_bitmap("Bitmaps\\jessy esquerda tres.png");
 	confereImagem(jessyEsquerdaTres, timer, display, 0);
  //SPRITE DOS PERSONAGENS, BATALHA
  //Kali
 	kaliBatalha = al_load_bitmap("Bitmaps\\kali batalha.png");
 	confereImagem(kaliBatalha, timer, display, 0);
  //Tony
 	tonyBatalha = al_load_bitmap("Bitmaps\\tony batalha.png");
 	confereImagem(kaliBatalha, timer, display, 0);
  //Viktor
 	viktorBatalha = al_load_bitmap("Bitmaps\\viktor batalha.png");
 	confereImagem(viktorBatalha, timer, display, 0);
  //Jessy
 	jessyBatalha = al_load_bitmap("Bitmaps\\jessy batalha.png");
 	confereImagem(jessyBatalha, timer, display, 0);
  //SPRITE DOS PERSONAGENS, SELECAO
  //Kali
 	kaliSelecaoUm = al_load_bitmap("Bitmaps\\kali selecao um.png");
 	confereImagem(kaliSelecaoUm, timer, display, 0);
 	kaliSelecaoDois = al_load_bitmap("Bitmaps\\kali selecao dois.png");
 	confereImagem(kaliSelecaoDois, timer, display, 0);
 	kaliSelecaoTres = al_load_bitmap("Bitmaps\\kali selecao tres.png");
 	confereImagem(kaliSelecaoTres, timer, display, 0);
  //Tony
 	tonySelecaoUm = al_load_bitmap("Bitmaps\\tony selecao um.png");
 	confereImagem(tonySelecaoUm, timer, display, 0);
 	tonySelecaoDois = al_load_bitmap("Bitmaps\\tony selecao dois.png");
 	confereImagem(tonySelecaoDois, timer, display, 0);
 	tonySelecaoTres = al_load_bitmap("Bitmaps\\tony selecao tres.png");
 	confereImagem(tonySelecaoTres, timer, display, 0);
  //Viktor
 	viktorSelecaoUm = al_load_bitmap("Bitmaps\\viktor selecao um.png");
 	confereImagem(viktorSelecaoUm, timer, display, 0);
 	viktorSelecaoDois = al_load_bitmap("Bitmaps\\viktor selecao dois.png");
 	confereImagem(viktorSelecaoDois, timer, display, 0);
 	viktorSelecaoTres = al_load_bitmap("Bitmaps\\viktor selecao tres.png");
 	confereImagem(viktorSelecaoTres, timer, display, 0);
  //Jessy
 	jessySelecaoUm = al_load_bitmap("Bitmaps\\jessy selecao um.png");
 	confereImagem(jessySelecaoUm, timer, display, 0);
 	jessySelecaoDois = al_load_bitmap("Bitmaps\\jessy selecao dois.png");
 	confereImagem(jessySelecaoDois, timer, display, 0);
 	jessySelecaoTres = al_load_bitmap("Bitmaps\\jessy selecao tres.png");
 	confereImagem(jessySelecaoTres, timer, display, 0);
 
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	//inicia o temporizador
	al_start_timer(timer);
	
	int playing = 1;
	int modo_jogo = SELECAO;
	int k;
	srand((unsigned)time(NULL));
	k = rand() % 12 + 19;
	
	initGlobais();

	Heroi heroi;
	//initHeroi(&heroi);
	Vilao viloes[30];
	for (int i = 0; i < k; ++i)
	{
		initVilao(&viloes[i]);
		if(colisaoDeViloes(viloes[i], viloes, i) == 1){
			i--;
		}
	}
	
	int recorde;
	recorde = 0;
	char my_text[20];
	int velocidade = 0;
	int velocidadeVilao = 0;
	int contadorDeTurno = 0;
	int pontuacaoMax = 0;
	char my_record[20];
	FILE * maiorPontuacao;
	maiorPontuacao = fopen("Pontuacao.txt", "r");
	fscanf(maiorPontuacao, "%d", &pontuacaoMax);
	fclose(maiorPontuacao);
	int indicadorVilao;
	int valorAtaqueVilao;
	int fatorAleatorio;
	fatorAleatorio = rand() % 50;

	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			if(modo_jogo == SELECAO){
				desenhaSelecao(kaliSelecaoUm, kaliSelecaoDois, kaliSelecaoTres, tonySelecaoUm, tonySelecaoDois, tonySelecaoTres, viktorSelecaoUm, 
					viktorSelecaoDois, viktorSelecaoTres, jessySelecaoUm, jessySelecaoDois, jessySelecaoTres);
			}
			else if(modo_jogo == NAVEGACAO){
				sprintf(my_text, "%d", recorde);
				desenhaNavegacao(background, casa, heroi, kanaFrente, kanaEsquerda, kanaDireita);
				processaTeclaNaveg(&heroi);
				if(indicadorHeroi == 0){
					desenhaHeroiNaveg(heroi, kaliFrenteUm, kaliFrenteDois, kaliFrenteTres, kaliDireitaUm, kaliDireitaDois, kaliDireitaTres, kaliCostasUm, kaliCostasDois, kaliCostasTres, kaliEsquerdaUm, kaliEsquerdaDois, kaliEsquerdaTres);
				}
				if(indicadorHeroi == 1){
					desenhaHeroiNaveg(heroi, tonyFrenteUm, tonyFrenteDois, tonyFrenteTres, tonyDireitaUm, tonyDireitaDois, tonyDireitaTres, tonyCostasUm, tonyCostasDois, tonyCostasTres, tonyEsquerdaUm, tonyEsquerdaDois, tonyEsquerdaTres);
				}
				if(indicadorHeroi == 2){
					desenhaHeroiNaveg(heroi, viktorFrenteUm, viktorFrenteDois, viktorFrenteTres, viktorDireitaUm, viktorDireitaDois, viktorDireitaTres, viktorCostasUm, viktorCostasDois, viktorCostasTres, viktorEsquerdaUm, viktorEsquerdaDois, viktorEsquerdaTres);
				}
				if(indicadorHeroi == 3){
					desenhaHeroiNaveg(heroi, jessyFrenteUm, jessyFrenteDois, jessyFrenteTres, jessyDireitaUm, jessyDireitaDois, jessyDireitaTres, jessyCostasUm, jessyCostasDois, jessyCostasTres, jessyEsquerdaUm, jessyEsquerdaDois, jessyEsquerdaTres);
				}
				desenhaFalaNpc(heroi);
				al_draw_text(FONTE, al_map_rgb(255, 0, 0), 10, 10, 0, my_text);
				for (int i = 0; i < 18; ++i){
					if(detectouMonstro(heroi, viloes[i])){
						modo_jogo = BATALHA;
						indicadorVilao = i;
					}
				}
				if(chegouObjetivo(heroi)){
					recorde += 500;
					sprintf(my_text, "%d", recorde);
					modo_jogo = FINAL;
				}
			}
			else if(modo_jogo == BATALHA){
				sprintf(my_text, "%d", recorde);
				if(indicadorHeroi == 0){
					desenhaBatalha(heroi, contadorDeTurno, viloes[indicadorVilao], backgroundBatalha, aranha, twice, dabi, dabiAtaque, kaliBatalha);
				}
				if(indicadorHeroi == 1){
					desenhaBatalha(heroi, contadorDeTurno, viloes[indicadorVilao], backgroundBatalha, aranha, twice, dabi, dabiAtaque, tonyBatalha);
				}
				if(indicadorHeroi == 2){
					desenhaBatalha(heroi, contadorDeTurno, viloes[indicadorVilao], backgroundBatalha, aranha, twice, dabi, dabiAtaque, viktorBatalha);
				}
				if(indicadorHeroi == 3){
					desenhaBatalha(heroi, contadorDeTurno, viloes[indicadorVilao], backgroundBatalha, aranha, twice, dabi, dabiAtaque, jessyBatalha);
				}
				desenhaVida(heroi, viloes[indicadorVilao]);
				if(heroi.acao == ATACAR && heroi.animacao == 1){
					desenhaAtaque(&velocidade, &heroi, &viloes[indicadorVilao], &recorde, &modo_jogo, contadorDeTurno);
				}
				if(heroi.acao == ESPECIAL && heroi.animacao == 1){
					contadorDeTurno = 0;
					heroi.acao = ATACAR;
					heroi.animacao = 0;
					if(viloes[indicadorVilao].vida <= 0){
						recorde += viloes[indicadorVilao].valor;
						modo_jogo = NAVEGACAO;
					}
					else{
						viloes[indicadorVilao].turno = 1;
						viloes[indicadorVilao].vida -= heroi.ataqueEspecial;
					}
				}
				if(viloes[indicadorVilao].turno == 1){
					desenhaAtaqueVilao(&valorAtaqueVilao, &fatorAleatorio, &viloes[indicadorVilao], &velocidadeVilao, &heroi);
				}
				al_draw_text(FONTE, al_map_rgb(128, 200, 30), 10, 10, 0, my_text);
				modo_jogo = processaAcaoHeroi(&heroi, &contadorDeTurno, &viloes[indicadorVilao]);
				if(viloes[indicadorVilao].vida <= 0){
					modo_jogo = NAVEGACAO;
				}
				if(heroi.vida <= 0){
					modo_jogo = FINAL;
				}
			}
			else if(modo_jogo == FINAL){
				desenhaTelaFinal(heroi, my_text, &pontuacaoMax, recorde, maiorPontuacao, my_record);
			}
			else if(modo_jogo == CREDITOS){
				desenhaCreditos();
			}
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			modo_jogo = CREDITOS;
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if(modo_jogo == SELECAO){
				processaTeclaSelecao(&heroi, ev.keyboard.keycode, &modo_jogo);
				initHeroi(&heroi);
			}
			else if(modo_jogo == NAVEGACAO){
				pressionaTeclaNaveg(ev.keyboard.keycode, &heroi, heroi);
			}
			else if(modo_jogo == BATALHA){
				processaTeclaBatalha(&heroi, ev.keyboard.keycode, contadorDeTurno);
				if(heroi.executar){
					contadorDeTurno++;
					printf("%d\n", contadorDeTurno);
				}
			}
			else if(modo_jogo == FINAL){
				processaTeclaFinal(ev.keyboard.keycode);
				if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER){
					if(indicadorFinal == 0){
						modo_jogo = SELECAO;
						k = rand() % 12 + 19;
						for (int i = 0; i < k; ++i){
							initVilao(&viloes[i]);
							if(colisaoDeViloes(viloes[i], viloes, i) == 1){
								i--;
							}
						}
						if(heroi.vida <= 0){
							recorde = 0;
						}
						contadorDeTurno = 0;
						indicadorHeroi = 0;
					}
					else if(indicadorFinal == 1){
					modo_jogo = CREDITOS;
					indicadorFinal = 0;
					}
				}
			}
			else if(modo_jogo == CREDITOS){
				if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER){
					playing = 0;
				}
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			despressionaTeclaNaveg(ev.keyboard.keycode);
		}
	} //fim do while
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	return 0;
}