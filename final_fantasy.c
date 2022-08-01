#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define NAVEGACAO 0
#define BATALHA 1
#define TAM_TERRA 20
#define TAM_OBJ 100
#define VIDA_W 200
#define VIDA_H 20
#define PASSO 20
#define MAX_DIST 20
#define MAX_MONSTROS 20
#define ATACAR 0
#define ESPECIAL 1
#define FUGIR 2
#define BANDIDO 1 //inimigo com espada
#define DRAGAO 2 //inimigo com poder de fogo
#define CACHORRO 3 //inimigo com poder elétrico

const float FPS = 100;  
const int SCREEN_W = 960;
const int SCREEN_H = 540;

int X_OPT;
int Y_OPT;
ALLEGRO_FONT *FONTE;
//--------------------funcoes gerais------------
//aleatorio entre 0 e n-1
int random(int n){
	return rand()%n;
}
//aleatorio entre min e max (inclusive numeros negativos)
int randInt(int min, int max){ //EX: min = 5, max = 10
	return min + random(max - min + 1);
}

//------------------estruturas------------------
typedef struct Terra {
	int x, y;
	int vida;
	int ataque;
	//batalha
	int acao;
	int executar;
	int x_old, y_old;
} Terra;

typedef struct Monstro {
	int x, y;
	int vida;
	int maxvida;
	int ataque;
	int tipo;
} Monstro;

typedef struct Projetil {
	int x, y;
	int live;
	int speed;
} Projetil;

//------------------incializacoes------------------
void initGlobais(){
	X_OPT = 3*SCREEN_W/4;
	Y_OPT = 3*SCREEN_H/4;

	int tam_fonte = 32;
	if(SCREEN_H < 300)
		tam_fonte = 20;

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    FONTE = al_load_font("arial.ttf", tam_fonte, 1);   
	if(FONTE == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
}

void initTerra(Terra *terra){
	terra->x = 0;
	terra->y = SCREEN_H-TAM_TERRA-20;
	terra->vida = 200;
	terra->ataque = 50;
	terra->acao = ATACAR;
	terra->executar = 0;
	terra->x_old = terra->x;
	terra->y_old = terra->y;
}

void initProjetilMonstro(Projetil *projetil){
	projetil->x = SCREEN_W/4;
	projetil->y = SCREEN_H/3;
	projetil->speed = 5;
	projetil->live = 0;
}

void initProjetil(Projetil *projetil){
	projetil->x = SCREEN_W/4*3;
	projetil->y = SCREEN_H/3*2 - 20;
	projetil->speed = 5;
	projetil->live = 0;
}

//------------------processos do projetil do monstro------------------
void desenhaProjetilMonstro(Projetil projetil, Monstro monstro, int *hitAnimation, int frameAnimation, ALLEGRO_BITMAP *hitEffect1, ALLEGRO_BITMAP *hitEffect2, ALLEGRO_BITMAP *hitEffect3, ALLEGRO_BITMAP *hitEffect4, ALLEGRO_BITMAP *electricball1, ALLEGRO_BITMAP *electricball2, ALLEGRO_BITMAP *electricball3, ALLEGRO_BITMAP *electricball4, ALLEGRO_BITMAP *fireBall1, ALLEGRO_BITMAP *fireBall2, ALLEGRO_BITMAP *fireBall3, ALLEGRO_BITMAP *fireBall4, ALLEGRO_BITMAP *fireBall5, ALLEGRO_BITMAP *fireBall6, ALLEGRO_BITMAP *fireBall7, ALLEGRO_BITMAP *fireBall8){

	if(projetil.live){
		(*hitAnimation) = (*hitAnimation) + 2; //muda o frame da animacao dos ataques
		if(monstro.tipo == BANDIDO){
			int imageWidth = al_get_bitmap_width(hitEffect1);
			int imageHeight = al_get_bitmap_height(hitEffect1);

			//animacao do ataque de espada
			if((*hitAnimation) > 125 && (*hitAnimation) <= 150)
				al_draw_scaled_bitmap(hitEffect1, 0, 0, imageWidth, imageHeight, SCREEN_W/2, SCREEN_H/4, imageWidth, imageHeight, 1);
			if((*hitAnimation) > 150 && (*hitAnimation) <= 175)
				al_draw_scaled_bitmap(hitEffect2, 0, 0, imageWidth, imageHeight, SCREEN_W/2, SCREEN_H/4, imageWidth, imageHeight, 1);
			if((*hitAnimation) > 175 && (*hitAnimation) <= 200)
				al_draw_scaled_bitmap(hitEffect3, 0, 0, imageWidth, imageHeight, SCREEN_W/2, SCREEN_H/4, imageWidth, imageHeight, 1);
			if((*hitAnimation) > 200)
				al_draw_scaled_bitmap(hitEffect4, 0, 0, imageWidth, imageHeight, SCREEN_W/2, SCREEN_H/4, imageWidth, imageHeight, 1);

		} else if (monstro.tipo == CACHORRO){
			int imageWidth = al_get_bitmap_width(electricball1);
			int imageHeight = al_get_bitmap_height(electricball1);

			//animacao da bola eletrica
			if(frameAnimation >= 0 && frameAnimation <= 25)
				al_draw_scaled_bitmap(electricball1, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth, imageHeight, 1);
			if(frameAnimation > 25 && frameAnimation <= 50)
				al_draw_scaled_bitmap(electricball2, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth, imageHeight, 1);
			if(frameAnimation > 50 && frameAnimation <= 75)
				al_draw_scaled_bitmap(electricball3, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth, imageHeight, 1);
			if(frameAnimation > 75)
				al_draw_scaled_bitmap(electricball4, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth, imageHeight, 1);

		} else if (monstro.tipo == DRAGAO){
			int imageWidth = al_get_bitmap_width(fireBall1);
			int imageHeight = al_get_bitmap_height(fireBall1);

			//animacao da bola de fogo
			if(frameAnimation >= 0 && frameAnimation <= 12)
				al_draw_scaled_bitmap(fireBall1, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth/6, imageHeight/6, 1);
			if(frameAnimation > 12 && frameAnimation <= 24)
				al_draw_scaled_bitmap(fireBall2, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth/6, imageHeight/6, 1);
			if(frameAnimation > 24 && frameAnimation <= 36)
				al_draw_scaled_bitmap(fireBall3, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth/6, imageHeight/6, 1);
			if(frameAnimation > 36 && frameAnimation <= 48)
				al_draw_scaled_bitmap(fireBall4, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth/6, imageHeight/6, 1);
			if(frameAnimation > 48 && frameAnimation <= 60)
				al_draw_scaled_bitmap(fireBall5, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth/6, imageHeight/6, 1);
			if(frameAnimation > 60 && frameAnimation <= 72)
				al_draw_scaled_bitmap(fireBall6, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth/6, imageHeight/6, 1);
			if(frameAnimation > 72 && frameAnimation <= 84)
				al_draw_scaled_bitmap(fireBall7, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth/6, imageHeight/6, 1);
			if(frameAnimation > 84)
				al_draw_scaled_bitmap(fireBall8, 0, 0, imageWidth, imageHeight, projetil.x -30, projetil.y + 100, imageWidth/6, imageHeight/6, 1);
		}
	}
}
//-------------------funcao que eh ativada quando o monstro incia o ataque----------------------
void atiraProjetilMonstro(Projetil **projetil){
	(*projetil)->live = 1;
}
//--------------------------funcao que muda a posicao do projetil--------------------------------
int atualizaProjetilMonstro(Projetil *projetil, Monstro *monstro, Terra *terra, int *hitAnimation){
	if(projetil->live){
		projetil->x += projetil->speed;
		if(projetil->x > SCREEN_W/4*3){
			projetil->live = 0;
			projetil->x = SCREEN_W/4;
			terra->vida -= monstro->ataque;
			(*hitAnimation) = 0;
		}
		if(terra->vida <= 0){
			return 1;
		}
		return 0;
	}
}

//------------------processos do projetil do heroi------------------
void desenhaProjetil(Projetil projetil, Terra terra, ALLEGRO_BITMAP *iceBall1, ALLEGRO_BITMAP *iceBall2, ALLEGRO_BITMAP *iceBall3, ALLEGRO_BITMAP *iceBall4, ALLEGRO_BITMAP *iceBall5, ALLEGRO_BITMAP *iceBall6, ALLEGRO_BITMAP *iceBall7, ALLEGRO_BITMAP *iceBall8, int frameAnimation, ALLEGRO_BITMAP *especial1, ALLEGRO_BITMAP *especial2, ALLEGRO_BITMAP *especial3, ALLEGRO_BITMAP *especial4, ALLEGRO_BITMAP *especial5, ALLEGRO_BITMAP *especial6, ALLEGRO_BITMAP *especial7, int *hitAnimation){
	if(projetil.live){
		if(terra.acao == ESPECIAL){
			(*hitAnimation) = (*hitAnimation) + 2; //muda o frame da animacao dos ataques
			int imageWidth = al_get_bitmap_width(especial1);
			int imageHeight = al_get_bitmap_height(especial1);
			float scale = 1.5;
			
			//animacao do ataque especial do heroi
			if((*hitAnimation) >= 0 && (*hitAnimation) <= 25)
				al_draw_scaled_bitmap(especial1, 0, 0, imageWidth, imageHeight, SCREEN_W/3 - 150, SCREEN_H/4 - 50, imageWidth*scale + 50, imageHeight*scale, 0);
			if((*hitAnimation) > 25 && (*hitAnimation) <= 50)
				al_draw_scaled_bitmap(especial2, 0, 0, imageWidth, imageHeight, SCREEN_W/3 - 150, SCREEN_H/4 - 50, imageWidth*scale + 50, imageHeight*scale, 0);
			if((*hitAnimation) > 50 && (*hitAnimation) <= 75)
				al_draw_scaled_bitmap(especial3, 0, 0, imageWidth, imageHeight, SCREEN_W/3 - 150, SCREEN_H/4 - 50, imageWidth*scale + 50, imageHeight*scale, 0);
			if((*hitAnimation) > 75 && (*hitAnimation) <= 100)
				al_draw_scaled_bitmap(especial4, 0, 0, imageWidth, imageHeight, SCREEN_W/3 - 150, SCREEN_H/4 - 50, imageWidth*scale + 50, imageHeight*scale, 0);
			if((*hitAnimation) > 100 && (*hitAnimation) <= 125)
				al_draw_scaled_bitmap(especial5, 0, 0, imageWidth, imageHeight, SCREEN_W/3 - 150, SCREEN_H/4 - 50, imageWidth*scale + 50, imageHeight*scale, 0);
			if((*hitAnimation) > 125 && (*hitAnimation) <= 150)
				al_draw_scaled_bitmap(especial6, 0, 0, imageWidth, imageHeight, SCREEN_W/3 - 150, SCREEN_H/4 - 50, imageWidth*scale + 50, imageHeight*scale, 0);
			if((*hitAnimation) > 150)
				al_draw_scaled_bitmap(especial7, 0, 0, imageWidth, imageHeight, SCREEN_W/3 - 150, SCREEN_H/4 - 50, imageWidth*scale + 50, imageHeight*scale, 0);	
		}

		if(terra.acao == ATACAR){
			int imageWidth = al_get_bitmap_width(iceBall1);
			int imageHeight = al_get_bitmap_height(iceBall1);
			float scale = 1.5;
			
			//animacao do ataque normal do heroi
			if(frameAnimation >= 0 && frameAnimation <= 12)
				al_draw_scaled_bitmap(iceBall1, 0, 0, imageWidth, imageHeight, projetil.x - 75, projetil.y - 100, imageWidth/scale, imageHeight/scale, 0);
			if(frameAnimation > 12 && frameAnimation <= 24)
				al_draw_scaled_bitmap(iceBall2, 0, 0, imageWidth, imageHeight, projetil.x - 75, projetil.y - 100, imageWidth/scale, imageHeight/scale, 0);
			if(frameAnimation > 24 && frameAnimation <= 36)
				al_draw_scaled_bitmap(iceBall3, 0, 0, imageWidth, imageHeight, projetil.x - 75, projetil.y - 100, imageWidth/scale, imageHeight/scale, 0);
			if(frameAnimation > 36 && frameAnimation <= 48)
				al_draw_scaled_bitmap(iceBall4, 0, 0, imageWidth, imageHeight, projetil.x - 75, projetil.y - 100, imageWidth/scale, imageHeight/scale, 0);
			if(frameAnimation > 48 && frameAnimation <= 60)
				al_draw_scaled_bitmap(iceBall5, 0, 0, imageWidth, imageHeight, projetil.x - 75, projetil.y - 100, imageWidth/scale, imageHeight/scale, 0);
			if(frameAnimation > 60 && frameAnimation <= 72)
				al_draw_scaled_bitmap(iceBall6, 0, 0, imageWidth, imageHeight, projetil.x - 75, projetil.y - 100, imageWidth/scale, imageHeight/scale, 0);
			if(frameAnimation > 72 && frameAnimation <= 84)
				al_draw_scaled_bitmap(iceBall7, 0, 0, imageWidth, imageHeight, projetil.x - 75, projetil.y - 100, imageWidth/scale, imageHeight/scale, 0);
			if(frameAnimation > 84)
				al_draw_scaled_bitmap(iceBall8, 0, 0, imageWidth, imageHeight, projetil.x - 75, projetil.y - 100, imageWidth/scale, imageHeight/scale, 0);
		}
	}
}
//-------------------funcao que eh ativada quando o heroi incia o ataque----------------------
void atiraProjetil(Projetil **projetil){
	(*projetil)->live = 1;
}
//--------------------------funcao que muda a posicao do projetil--------------------------------
int atualizaProjetil(Projetil *projetil, Monstro *monstro, Terra *terra, Projetil *projetilMonstro, int *score){
	if(projetil->live){
		projetil->x -= projetil->speed;
		if(projetil->x < SCREEN_W/4){
			projetil->live = 0;
			projetil->x = SCREEN_W/4*3;
			monstro->vida -= terra->ataque;
			if(monstro->vida <= 0){
				(*score) += monstro->maxvida;
				return NAVEGACAO;
			} else {
				atiraProjetilMonstro(&projetilMonstro);
			}
		}
	}
	return BATALHA;
}
//-------------------funcao que eh ativada quando o heroi incia o especial----------------------
int atualizaProjetilEspecial(Projetil *projetil, Monstro *monstro, Terra *terra, Projetil *projetilMonstro, int *hitAnimation, int *score){
	if(projetil->live){
		projetil->x -= projetil->speed;
		if(projetil->x < SCREEN_W/4){
			projetil->live = 0;
			projetil->x = SCREEN_W/4*3;
			monstro->vida -= terra->ataque*3;
			(*hitAnimation) = 0;
			if(monstro->vida <= 0){ //se o monstro morrer o jogo retorna para o modo navegacao
				(*score) += monstro->maxvida;
				return NAVEGACAO;
			} else {
				atiraProjetilMonstro(&projetilMonstro); //se o monstro nao morrer o monstro ataca
			}
		}
	}
	return BATALHA;
}

//----------------------------CRIACAO MONSTROS----------------------------
//calcula a distancia entre dois pontos
float dist(int x1, int y1, int x2, int y2){ 
	return sqrt( pow(x1 - x2,2) + pow(y1 - y2,2) );
}
//verifica se duas coordenadas estão muito proximas
int colisaoCoordenadas(Monstro m1, Monstro m2){
	if(dist(m1.x, m1.y, m2.x, m2.y) <= MAX_DIST*5)
		return 1;
	return 0;
}
//verifica se existe colisao entre as coordenadas dos monstros
int colisaoMonstros(Monstro monstro, Monstro monstros[], int n){
	int i=0;
	for(i=0; i<n; i++){
		if(colisaoCoordenadas(monstro, monstros[i]))
			return 1;
	}
	return 0;
}
//cria um monstro
void criaMonstro(Monstro *m){
	m->tipo = randInt(1, 3);
	m->x = randInt(MAX_DIST, SCREEN_W - MAX_DIST);
	m->y = randInt(MAX_DIST, SCREEN_H - MAX_DIST);
	
	if(m->tipo == BANDIDO){
		m->vida = randInt(50, 100);
		m->maxvida = m->vida;
		m->ataque = randInt(15, 25);
	} else if (m->tipo == DRAGAO){
		m->vida = randInt(300, 400);
		m->maxvida = m->vida;
		m->ataque = randInt(10, 20);
	} else if (m->tipo == CACHORRO){
		m->vida = randInt(150, 200);
		m->maxvida = m->vida;
		m->ataque = randInt(30, 40);
	}
}

//=================================NAVEGACAO===================================

//desenha o cenario
void desenhaCenarioNaveg(ALLEGRO_BITMAP *image, ALLEGRO_BITMAP *cave, int score){
	
	//desenha o fundo do cenario
	int imageWidth = al_get_bitmap_width(image);
	int imageHeight = al_get_bitmap_height(image);
	float scale = SCREEN_W/imageWidth;
	al_draw_scaled_bitmap(image, 0, 0, imageWidth, imageHeight, 0, 0, imageWidth*scale, imageHeight*scale, 0);

	//desenha a caverna
	float imageWidthCave = al_get_bitmap_width(cave);
	float imageHeightCave = al_get_bitmap_height(cave);
	float scaleCave = 1.5;
	al_draw_scaled_bitmap(cave, 0, 0, imageWidthCave, imageHeightCave, SCREEN_W - imageWidthCave*scaleCave + 20, -30, imageWidthCave*scaleCave, imageHeightCave*scaleCave, 0);

	//escreve a pontuacao do jogador
	char my_text[20];
	sprintf(my_text, "Pontos: %d", score);
	al_draw_text(FONTE, al_map_rgb(0, 0, 0), 10, 5, 0, my_text);
}
//desenha o heroi no cenario
void desenhaTerraNaveg(Terra terra, ALLEGRO_BITMAP *image, int direita){
	int imageWidth = al_get_bitmap_width(image);
	int imageHeight = al_get_bitmap_height(image);
	float scale = SCREEN_W/960;
	if(direita){
		al_draw_scaled_bitmap(image, 0, 0, imageWidth, imageHeight, terra.x, terra.y, imageWidth*scale, imageHeight*scale, 1);
	} else {
		al_draw_scaled_bitmap(image, 0, 0, imageWidth, imageHeight, terra.x, terra.y, imageWidth*scale, imageHeight*scale, 0);
	}
}

//processa as teclas pressionadas no teclado
int processaTeclaNaveg(Terra *terra, int tecla){
	//salva as posicoes do heroi antes de encontrar um monstro
	terra->x_old = terra->x;
	terra->y_old = terra->y;

	switch(tecla){
		case ALLEGRO_KEY_W:
			if(terra->y - PASSO >=0 )terra->y -= PASSO;
			return 1;

		case ALLEGRO_KEY_S:
			if(terra->y + TAM_TERRA + PASSO + 10 <= SCREEN_H)terra->y += PASSO;
			return 2;

		case ALLEGRO_KEY_A:
			if(terra->x - PASSO >= 0)terra->x -=PASSO;
			return 3;

		case ALLEGRO_KEY_D:
			if(terra->x + TAM_TERRA + PASSO  <= SCREEN_W)terra->x += PASSO;
			return 4;
	}
}
//verifica se o heroi encontrou um monstro
int detectouMonstro(Terra terra, Monstro monstro){
	if (dist(terra.x, terra.y, monstro.x, monstro.y) <= MAX_DIST && monstro.vida > 0){
		printf("\nInimigo encontrado!");
		return 1;
	}
	return 0;
}
//verifica se o heroi chegou ao objetivo final
int chegouObjetivo(Terra terra){
	if(terra.x >= SCREEN_W - TAM_OBJ - 30 && terra.y + TAM_TERRA <= TAM_OBJ*0.75)
		return 1;
	return 0;
}

//=================================BATALHA===================================
//desenha o cenario
void desenhaCenarioBatalha(ALLEGRO_BITMAP *image, Terra terra, int score){
	int imageWidth = al_get_bitmap_width(image);
	int imageHeight = al_get_bitmap_height(image);
	float scale = SCREEN_W/imageWidth;
	//desenha o fundo
	al_draw_scaled_bitmap(image, 0, 0, imageWidth, imageHeight, 0, 0, imageWidth*scale, imageHeight*scale, 0);

	//cria as opcoes do modo de batalha
	al_draw_filled_rounded_rectangle(X_OPT-5, Y_OPT-5, SCREEN_W -5, SCREEN_H-5,
   									10, 10, al_map_rgb(255, 255, 255));
	al_draw_filled_rounded_rectangle(X_OPT, Y_OPT, SCREEN_W - 10, SCREEN_H - 10,
   									10, 10, al_map_rgb(20, 16, 77));
    al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT + 10, Y_OPT + 5, 0, "Atacar");
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT + 10, Y_OPT + 5 + 40, 0, "Especial");
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT + 10, Y_OPT + 5 + 40*2, 0, "Fugir");

	//desenha o cursor do modo de opcoes
	if(terra.acao == ATACAR)
		al_draw_filled_triangle( X_OPT + 160, Y_OPT + 25, X_OPT + 190, Y_OPT+15, X_OPT + 190, Y_OPT+35, al_map_rgb(255, 255, 255));
	if(terra.acao == ESPECIAL)
		al_draw_filled_triangle( X_OPT + 160, Y_OPT + 25 + 40, X_OPT + 190, Y_OPT+15+40, X_OPT + 190, Y_OPT+35+40, al_map_rgb(255, 255, 255));
	if(terra.acao == FUGIR)
		al_draw_filled_triangle( X_OPT + 160, Y_OPT + 25+40*2, X_OPT + 190, Y_OPT+15+40*2, X_OPT + 190, Y_OPT+35+40*2, al_map_rgb(255, 255, 255));

	//escreve a pontuacao
	char my_text[20];
	sprintf(my_text, "Pontos: %d", score);
	al_draw_text(FONTE, al_map_rgb(0, 0, 0), 10, 5, 0, my_text);
}
//desenha o heroi
void desenhaTerraBatalha(ALLEGRO_BITMAP *image){
	int imageWidth = al_get_bitmap_width(image);
	int imageHeight = al_get_bitmap_height(image);
	float scale = SCREEN_W/480;
	al_draw_scaled_bitmap(image, 0, 0, imageWidth, imageHeight, SCREEN_W/4*3 - imageWidth/2, SCREEN_H/3 + imageHeight*2, imageWidth*scale, imageHeight*scale, 0);
}
//desenha a barra de vida do heroi
void desenhaVidaTerra(Terra t){
	al_draw_filled_rounded_rectangle(SCREEN_W*21/32, SCREEN_H/4, SCREEN_W*21/32 + VIDA_W, SCREEN_H/4 + VIDA_H, 2, 2, al_map_rgb(0, 0, 0));
	if(t.vida >= 135)								   
		al_draw_filled_rounded_rectangle(SCREEN_W*21/32+4, SCREEN_H/4+4, SCREEN_W*21/32 + t.vida - 4, SCREEN_H/4 + VIDA_H-4, 2, 2, al_map_rgb(0, 176, 24));
	if(t.vida < 135 && t.vida > 65)								   
		al_draw_filled_rounded_rectangle(SCREEN_W*21/32+4, SCREEN_H/4+4, SCREEN_W*21/32 + t.vida - 4, SCREEN_H/4 + VIDA_H-4, 2, 2, al_map_rgb(255, 237, 0));
	if(t.vida <= 65)								   
		al_draw_filled_rounded_rectangle(SCREEN_W*21/32+4, SCREEN_H/4+4, SCREEN_W*21/32 + t.vida - 4, SCREEN_H/4 + VIDA_H-4, 2, 2, al_map_rgb(202, 0, 42));
}
//desenha o monstro
void desenhaMonstroBatalha(Monstro m, ALLEGRO_BITMAP *enemy1, ALLEGRO_BITMAP *enemy2, ALLEGRO_BITMAP *enemy3){
	if(m.tipo == BANDIDO){
		float scale = SCREEN_W/480;
		int imageWidth = al_get_bitmap_width(enemy1);
		int imageHeight = al_get_bitmap_height(enemy1);
		al_draw_scaled_bitmap(enemy1, 0, 0, imageWidth, imageHeight, SCREEN_W/4 - imageWidth, SCREEN_H/3, imageWidth*scale, imageHeight*scale, 0);
	} else if (m.tipo == DRAGAO){
		float scale = 1.5;
		int imageWidth = al_get_bitmap_width(enemy2);
		int imageHeight = al_get_bitmap_height(enemy2);
		al_draw_scaled_bitmap(enemy2, 0, 0, imageWidth, imageHeight, SCREEN_W/4 - imageWidth*3/4, SCREEN_H/3 - 20, imageWidth*scale, imageHeight*scale, 0);
	} else if (m.tipo == CACHORRO){
		float scale = SCREEN_W/960;
		int imageWidth = al_get_bitmap_width(enemy3);
		int imageHeight = al_get_bitmap_height(enemy3);
		al_draw_scaled_bitmap(enemy3, 0, 0, imageWidth, imageHeight, SCREEN_W/4 - imageWidth/2, SCREEN_H/3, imageWidth*scale, imageHeight*scale, 0);
	}	
}
//desenha a barra de vida do monstro
void desenhaVidaMonstro(Monstro m, int verde, int amarelo, float porcentagemVida){
	al_draw_filled_rounded_rectangle(SCREEN_W/7, SCREEN_H/4, SCREEN_W/7 + VIDA_W, SCREEN_H/4 + VIDA_H, 2, 2, al_map_rgb(0, 0, 0));
	if(m.vida >= verde)								   
		al_draw_filled_rounded_rectangle(SCREEN_W/7+4, SCREEN_H/4+4, SCREEN_W/7 + (float)m.vida/porcentagemVida - 4, SCREEN_H/4 + VIDA_H-4, 2, 2, al_map_rgb(0, 176, 24));
	if(m.vida < verde && m.vida > amarelo)							   
		al_draw_filled_rounded_rectangle(SCREEN_W/7+4, SCREEN_H/4+4, SCREEN_W/7 + (float)m.vida/porcentagemVida - 4, SCREEN_H/4 + VIDA_H-4, 2, 2, al_map_rgb(255, 237, 0));
	if(m.vida <= amarelo)							   
		al_draw_filled_rounded_rectangle(SCREEN_W/7+4, SCREEN_H/4+4, SCREEN_W/7 + (float)m.vida/porcentagemVida - 4, SCREEN_H/4 + VIDA_H-4, 2, 2, al_map_rgb(202, 0, 42));
}
//desenha a barra de mana do heroi
void desenhaManaBar(ALLEGRO_BITMAP *manabar0, ALLEGRO_BITMAP *manabar1, ALLEGRO_BITMAP *manabar2, ALLEGRO_BITMAP *manabar3, int mana){
	int imageWidth = al_get_bitmap_width(manabar0);
	int imageHeight = al_get_bitmap_height(manabar0);
	float scale = SCREEN_W/480;
	
	if(mana == 0)
		al_draw_scaled_bitmap(manabar0, 0, 0, imageWidth, imageHeight, 0, SCREEN_H/4*3, imageWidth/scale, imageHeight/2, 0);
	if(mana == 1)
		al_draw_scaled_bitmap(manabar1, 0, 0, imageWidth, imageHeight, 0, SCREEN_H/4*3, imageWidth/scale, imageHeight/2, 0);
	if(mana == 2)
		al_draw_scaled_bitmap(manabar2, 0, 0, imageWidth, imageHeight, 0, SCREEN_H/4*3, imageWidth/scale, imageHeight/2, 0);
	if(mana == 3)
		al_draw_scaled_bitmap(manabar3, 0, 0, imageWidth, imageHeight, 0, SCREEN_H/4*3, imageWidth/scale, imageHeight/2, 0);
}
//processa a tecla pressionada
void processaTeclaBatalha(Terra *terra, int tecla, Projetil projetil, Projetil projetilMonstro){
	if(!projetil.live && !projetilMonstro.live){
		//movimentos do cursor no menu de opcoes
		if(terra->acao == ESPECIAL){	
			switch(tecla){
			case ALLEGRO_KEY_UP:
				terra->acao = ATACAR;
				break;

			case ALLEGRO_KEY_DOWN:
				terra->acao = FUGIR;
				break;
			}
		}
		if(terra->acao == FUGIR){
			switch(tecla){
			case ALLEGRO_KEY_UP:
				terra->acao = ESPECIAL;
				break;

			case ALLEGRO_KEY_DOWN:
				terra->acao = FUGIR;
				break;
			}
		}
		if(terra->acao == ATACAR){
			switch(tecla){
			case ALLEGRO_KEY_UP:
				terra->acao = ATACAR;
				break;

			case ALLEGRO_KEY_DOWN:
				terra->acao = ESPECIAL;
				break;
			}
		}
		//jogador escolhe alguma acao
		switch(tecla){
			case ALLEGRO_KEY_ENTER:
				terra->executar = 1;
				break;
		}
	}	
}
//processa quando o jogador aperta o botao enter para executar alguma acao
int processaAcaoTerra(Terra *terra, Monstro *monstro, Projetil *projetil, Projetil *projetilMonstro, Projetil *projetilEspecial, int *mana){

	if(terra->executar){
		terra->executar = 0;
		if(terra->acao == ATACAR){
			atiraProjetil(&projetil);
			if((*mana) < 3)
				(*mana) += 1; //a cada ataque o jogador ganha 1 de mana, com 3 de mana ele pode executar o especial
			if(monstro->vida <= 0){
				printf("\nInimigo derrotado!");
				return NAVEGACAO; //quando o inimigo eh derrotado o jogador volta para o modo navegacao
			}
		}

		if(terra->acao == FUGIR){ //cada monstro tem uma diferente chance de deixar o jogador fugir
			if(monstro->tipo == BANDIDO){ //bandido tem grande chance de deixar o jogador fugir
				if(randInt(0, 10) > 3){
					terra->x = terra->x_old;
					terra->y = terra->y_old;
					terra->acao = ATACAR;
					return NAVEGACAO;	
				} else {
					atiraProjetilMonstro(&projetilMonstro); //se o jogador nao consegue executar a fuga, o monstro ataca
				}
			} 
			if (monstro->tipo == DRAGAO){ //dragao tem chance mediana de deixar o jogador fugir
				if(randInt(0, 10) > 5){
					terra->x = terra->x_old;
					terra->y = terra->y_old;
					terra->acao = ATACAR;
					return NAVEGACAO;
				} else {
					atiraProjetilMonstro(&projetilMonstro); //se o jogador nao consegue executar a fuga, o monstro ataca
				}
			} 
			if (monstro->tipo == CACHORRO){ //cachorro tem baixa chance de deixar o jogador fugir
				if(randInt(0, 10) > 7){
					terra->x = terra->x_old;
					terra->y = terra->y_old;
					terra->acao = ATACAR;
					return NAVEGACAO;
				} else {
					atiraProjetilMonstro(&projetilMonstro); //se o jogador nao consegue executar a fuga, o monstro ataca
				}
			}
		}
		
		if(terra->acao == ESPECIAL){
			if((*mana) == 3){ //o jogador so pode usar o especial quando esta com a mana cheia
				atiraProjetil(&projetilEspecial);
				(*mana) = 0;
				if(monstro->vida <= 0){
					printf("\nInimigo derrotado!");
					return NAVEGACAO;
				}
			}
		}
	}
	return BATALHA;
}
 
int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
   
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

	//instala o audio
	if(!al_install_audio()) {
		fprintf(stderr, "failed to install audio!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
	//inicializa o modulo que permite carregar imagens no jogo
	al_init_image_addon();
	//inicializa o modulo que permite carregar audios no jogo
	al_init_acodec_addon();
	//reserva 4 espacos para audio
	al_reserve_samples(4); 

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

	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());  	

	//inicia o temporizador
	al_start_timer(timer);
	
	//o jogo comeca no modo NAVEGACAO
	int modo_jogo = NAVEGACAO;
	//criacao das estruturas
	Terra terra;
	Monstro monstros[MAX_MONSTROS];
	Projetil projetil;
	Projetil projetilMonstro;
	Projetil projetilEspecial;

	srand(time(NULL)); //geracao de seed aleatoria para os rands

	//criacao dos monstros juntamente com a verificacao se existe colisao entre eles
	int i = 0;
	for (i = 0; i < MAX_MONSTROS; i++){
		criaMonstro(&monstros[i]);
		if(colisaoMonstros(monstros[i], monstros, i )){
			i--;
		}else{
			printf("\nMonstro -> (%d, %d)", monstros[i].x, monstros[i].y);
		}	
	}

	//algumas variaveis importantes
	int idMonstro = 0; //variavel que armazena o id do monstro encontrado
	int direcao = 0; //variavel que armazena a direcao que o heroi esta olhando
	int mana = 0; //variavel que armazena a quantidade de mana que o heroi possui no momento
	int frameAnimation = 0; //variavel que armazena a mundanca de frames em animacoes no geral
	int hitAnimation = 0; //variavel que armazena a mudanca de frames na animacao de ataque do BANDIDO
	int verde=0, amarelo=0; //variaveis que armazenam o valor para que a vida do monstro se torne amarela ou vermelha
	float porcentagemVida = 0; //variavel que armazena a porcentagem da vida max do monstro
	int score = 0; //variavel que armazena a pontuacao
	int recorde; //variavel que armazena o recorde
	int gameOver = 0; //variavel que armazena se o jogo acabou
	int playing = 1; //variavel que armazena se o jogo esta sendo executado
	int win = 0; //variavel que armazena se o jogador venceu

	//abertura do arquivo que armazena o recorde
	FILE *arq;
	arq = fopen("recorde.txt", "r");
	fscanf(arq, "%d", &recorde);
	printf("\nrecorde: %d", recorde);
	
	//----------------------------SPRITES-------------------------

	//carrega as imagens do modo navegacao
	ALLEGRO_BITMAP *cenario = al_load_bitmap("grass1.png");
	ALLEGRO_BITMAP *cave = al_load_bitmap("cave.png");
	//sprites terra
	ALLEGRO_BITMAP *terra_front = al_load_bitmap("Terra - Walk (Front).gif");
	ALLEGRO_BITMAP *terra_back = al_load_bitmap("Terra - Walk (Back).gif");
	ALLEGRO_BITMAP *terra_left = al_load_bitmap("Terra - Walk (Left).gif");
	//carrega as imagens do modo batalha
	ALLEGRO_BITMAP *cenario_batalha = al_load_bitmap("background_battle.png");
	ALLEGRO_BITMAP *terra_battle = al_load_bitmap("Terra - Action.gif");
	//inimigos
	ALLEGRO_BITMAP *enemy1 = al_load_bitmap("enemy1.png");
	ALLEGRO_BITMAP *enemy2 = al_load_bitmap("enemy2.png");
	ALLEGRO_BITMAP *enemy3 = al_load_bitmap("enemy3.png");
	//bara de mana
	ALLEGRO_BITMAP *manabar0 = al_load_bitmap("manabar0.png");
	ALLEGRO_BITMAP *manabar1 = al_load_bitmap("manabar1.png");
	ALLEGRO_BITMAP *manabar2 = al_load_bitmap("manabar2.png");
	ALLEGRO_BITMAP *manabar3 = al_load_bitmap("manabar3.png");
	//bola de gelo terra
	ALLEGRO_BITMAP *iceBall1 = al_load_bitmap("iceBall1.png");
	ALLEGRO_BITMAP *iceBall2 = al_load_bitmap("iceBall2.png");
	ALLEGRO_BITMAP *iceBall3 = al_load_bitmap("iceBall3.png");
	ALLEGRO_BITMAP *iceBall4 = al_load_bitmap("iceBall4.png");
	ALLEGRO_BITMAP *iceBall5 = al_load_bitmap("iceBall5.png");
	ALLEGRO_BITMAP *iceBall6 = al_load_bitmap("iceBall6.png");
	ALLEGRO_BITMAP *iceBall7 = al_load_bitmap("iceBall7.png");
	ALLEGRO_BITMAP *iceBall8 = al_load_bitmap("iceBall8.png");
	//especial terra
	ALLEGRO_BITMAP *especial1 = al_load_bitmap("especial1.png");
	ALLEGRO_BITMAP *especial2 = al_load_bitmap("especial2.png");
	ALLEGRO_BITMAP *especial3 = al_load_bitmap("especial3.png");
	ALLEGRO_BITMAP *especial4 = al_load_bitmap("especial4.png");
	ALLEGRO_BITMAP *especial5 = al_load_bitmap("especial5.png");
	ALLEGRO_BITMAP *especial6 = al_load_bitmap("especial6.png");
	ALLEGRO_BITMAP *especial7 = al_load_bitmap("especial7.png");
	//animacao de ataque do bandido
	ALLEGRO_BITMAP *hitEffect1 = al_load_bitmap("hitEffect1.png");
	ALLEGRO_BITMAP *hitEffect2 = al_load_bitmap("hitEffect2.png");
	ALLEGRO_BITMAP *hitEffect3 = al_load_bitmap("hitEffect3.png");
	ALLEGRO_BITMAP *hitEffect4 = al_load_bitmap("hitEffect4.png");
	//animacao de atque do dragao
	ALLEGRO_BITMAP *fireBall1 = al_load_bitmap("fireBall1.png");
	ALLEGRO_BITMAP *fireBall2 = al_load_bitmap("fireBall2.png");
	ALLEGRO_BITMAP *fireBall3 = al_load_bitmap("fireBall3.png");
	ALLEGRO_BITMAP *fireBall4 = al_load_bitmap("fireBall4.png");
	ALLEGRO_BITMAP *fireBall5 = al_load_bitmap("fireBall5.png");
	ALLEGRO_BITMAP *fireBall6 = al_load_bitmap("fireBall6.png");
	ALLEGRO_BITMAP *fireBall7 = al_load_bitmap("fireBall7.png");
	ALLEGRO_BITMAP *fireBall8 = al_load_bitmap("fireBall8.png");
	//animacao de ataque do cachorro
	ALLEGRO_BITMAP *electricball1 = al_load_bitmap("eletricball1.png");
	ALLEGRO_BITMAP *electricball2 = al_load_bitmap("eletricball2.png");
	ALLEGRO_BITMAP *electricball3 = al_load_bitmap("eletricball3.png");
	ALLEGRO_BITMAP *electricball4 = al_load_bitmap("eletricball4.png");
	
	//carrega as musicas
	ALLEGRO_SAMPLE* sound = al_load_sample("music.wav");
	ALLEGRO_SAMPLE* soundBattle = al_load_sample("battle.wav");
	ALLEGRO_SAMPLE* soundVictory = al_load_sample("victory.wav");
	ALLEGRO_SAMPLE* soundLose = al_load_sample("lose.wav");

	//inicializacao de estruturas
	initTerra(&terra);
	initProjetil(&projetil);
	initProjetilMonstro(&projetilMonstro);
	initProjetil(&projetilEspecial);
	initGlobais();

	//comeca a tocar a musica do modo NAVEGACAO
	al_play_sample(sound, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

	//======================EXECUCAO DO JOGO============================
	while(playing) {
		
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {

			//muda o frame de animacao de sprites
			frameAnimation+=2; 
			if(frameAnimation > 100)
				frameAnimation=0;

			if(modo_jogo == NAVEGACAO){
				
				desenhaCenarioNaveg(cenario, cave, score);
				if (direcao == 1){
					desenhaTerraNaveg(terra, terra_back, 0);
				} else if (direcao == 3){
					desenhaTerraNaveg(terra, terra_left, 0);
				} else if (direcao == 4){
					desenhaTerraNaveg(terra, terra_left, 1);
				} else {
					desenhaTerraNaveg(terra, terra_front, 0);
				}

				//verifica se o jogador encontrou algum monstro
				for (i = 0; i < MAX_MONSTROS; i++)
				{
					if(detectouMonstro(terra, monstros[i])){
						idMonstro = i; //o id do monstro encontrado eh salvo
						printf(" (%d)", idMonstro);
						verde = (float)monstros[idMonstro].maxvida*(65.0/100.0);
						amarelo = (float)monstros[idMonstro].maxvida*(30.0/100.0);
						porcentagemVida = (float)monstros[idMonstro].maxvida/200.0;
						al_stop_samples();
						al_play_sample(soundBattle, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); //comeca a tocar a musica de BATALHA
						modo_jogo = BATALHA; //se um monstro eh encontrado, o jogador vai para o modo de BATALHA
					}
				}
				
				if(chegouObjetivo(terra)){ //verifica se o jogador chegou no final do jogo
					playing = 0;
					gameOver = 1;
					win = 1;
				}

			} else{
				//modo de jogo de batalha
				desenhaCenarioBatalha(cenario_batalha, terra, score);
				desenhaTerraBatalha(terra_battle);
				desenhaMonstroBatalha(monstros[idMonstro], enemy1, enemy2, enemy3);
				desenhaVidaTerra(terra);
				desenhaVidaMonstro(monstros[idMonstro], verde, amarelo, porcentagemVida);
				desenhaManaBar(manabar0, manabar1, manabar2, manabar3, mana);
				desenhaProjetil(projetil, terra, iceBall1, iceBall2, iceBall3, iceBall4, iceBall5, iceBall6, iceBall7, iceBall8, frameAnimation, especial1, especial2, especial3, especial4, especial5, especial6, especial7, &hitAnimation);
				desenhaProjetil(projetilEspecial, terra, iceBall1, iceBall2, iceBall3, iceBall4, iceBall5, iceBall6, iceBall7, iceBall8, frameAnimation, especial1, especial2, especial3, especial4, especial5, especial6, especial7, &hitAnimation);
				desenhaProjetilMonstro(projetilMonstro, monstros[idMonstro], &hitAnimation, frameAnimation, hitEffect1, hitEffect2, hitEffect3, hitEffect4, electricball1, electricball2, electricball3, electricball4, fireBall1, fireBall2, fireBall3, fireBall4, fireBall5, fireBall6, fireBall7, fireBall8);

				if(processaAcaoTerra(&terra, &monstros[idMonstro], &projetil, &projetilMonstro, &projetilEspecial, &mana) == NAVEGACAO){
					al_stop_samples();
					al_play_sample(sound, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); //comeca a tocar musica de NAVEGACAO
					modo_jogo = NAVEGACAO;
				}
				if(atualizaProjetil(&projetil, &monstros[idMonstro], &terra, &projetilMonstro, &score) == NAVEGACAO){
					al_stop_samples();
					al_play_sample(sound, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); //comeca a tocar musica de NAVEGACAO
					modo_jogo = NAVEGACAO;
				}
				if(atualizaProjetilEspecial(&projetilEspecial, &monstros[idMonstro], &terra, &projetilMonstro, &hitAnimation, &score) == NAVEGACAO){
					al_stop_samples();
					al_play_sample(sound, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); //comeca a tocar musica de NAVEGACAO
					modo_jogo = NAVEGACAO;
				}
				if (atualizaProjetilMonstro(&projetilMonstro, &monstros[idMonstro], &terra, &hitAnimation)){ //verifica se a vida do heroi chegou a zero
					playing = 0;
					gameOver = 1;
					win = 0;
				}
			}
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
			gameOver = 0;
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if(modo_jogo == NAVEGACAO){
				direcao = processaTeclaNaveg(&terra, ev.keyboard.keycode);
			} else{
				processaTeclaBatalha(&terra, ev.keyboard.keycode, projetil, projetilMonstro);
			}
		}
	} //fim do while
	//======================TELA DE FIM DE JOGO============================
    al_stop_samples(); //para as musicas
	int musica = 1; //variavel para iniciar uma musica
	while(gameOver){ 

		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		al_clear_to_color(al_map_rgb(0, 0, 0)); //colore o fundo de preto

		if(win){ //verifica se o jogador venceu
			if(musica == 1)
				al_play_sample(soundVictory, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // toca a musica de vitoria
			musica = 0;
			al_draw_text(FONTE, al_map_rgb(58, 144, 0), SCREEN_W/2 - SCREEN_W/16*3.5, SCREEN_H/2 - SCREEN_H/12, 0, "Parabéns, você venceu!");
			char scoreText[20];
			sprintf(scoreText, "Pontuação: %d", score);
			char recordeText[20];
			sprintf(recordeText, "Seu recorde: %d", recorde);

			//bateu o recorde
			if(score > recorde){
				al_draw_text(FONTE, al_map_rgb(58, 144, 0), SCREEN_W/2 - SCREEN_W/16*3.5, SCREEN_H/2, 0, "Você bateu seu recorde!");
				al_draw_text(FONTE, al_map_rgb(58, 144, 0), SCREEN_W/2 - SCREEN_W/16*3.5, SCREEN_H/2 + 50, 0, scoreText);
				arq = fopen("recorde.txt", "w");
				fprintf(arq, "%d", score); //novo recorde eh armazenado no arquivo de recorde
			} else {
				
				al_draw_text (FONTE, al_map_rgb(58, 144, 0), SCREEN_W/2 - SCREEN_W/16*3.5, SCREEN_H/2, 0, recordeText);
				al_draw_text(FONTE, al_map_rgb(58, 144, 0), SCREEN_W/2 - SCREEN_W/16*3.5, SCREEN_H/2 + 50, 0, scoreText);
			}	
		} else {
			if(musica == 1)
				al_play_sample(soundLose, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); //toca a musica de derrota
			musica = 0;
			al_draw_text(FONTE, al_map_rgb(255, 0, 0), SCREEN_W/2 - SCREEN_W/16*3.5, SCREEN_H/2 - SCREEN_H/12, 0, "GAME OVER!");
		}
		//atualiza a tela (quando houver algo para mostrar)
		al_flip_display();
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			gameOver = 0;
		}
	}

	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	//detroi cenario navegacao
	al_destroy_bitmap(cenario);
	al_destroy_bitmap(cave);
	//destroi sprites terra
	al_destroy_bitmap(terra_battle);
	al_destroy_bitmap(terra_front);
	al_destroy_bitmap(terra_back);
	al_destroy_bitmap(terra_left);
	//destroi cenario batalha
	al_destroy_bitmap(cenario_batalha);
	//destroi inimigos
	al_destroy_bitmap(enemy1);
	al_destroy_bitmap(enemy2);
	al_destroy_bitmap(enemy3);
	//destroi barra de mana
	al_destroy_bitmap(manabar0);
	al_destroy_bitmap(manabar1);
	al_destroy_bitmap(manabar2);
	al_destroy_bitmap(manabar3);
	//destroi ice ball
	al_destroy_bitmap(iceBall1);
	al_destroy_bitmap(iceBall2);
	al_destroy_bitmap(iceBall3);
	al_destroy_bitmap(iceBall4);
	al_destroy_bitmap(iceBall5);
	al_destroy_bitmap(iceBall6);
	al_destroy_bitmap(iceBall7);
	al_destroy_bitmap(iceBall8);
	//destroi especial terra
	al_destroy_bitmap(especial1);
	al_destroy_bitmap(especial2);
	al_destroy_bitmap(especial3);
	al_destroy_bitmap(especial4);
	al_destroy_bitmap(especial5);
	al_destroy_bitmap(especial6);
	al_destroy_bitmap(especial7);
	//destroi ataque de bandido
	al_destroy_bitmap(hitEffect1);
	al_destroy_bitmap(hitEffect2);
	al_destroy_bitmap(hitEffect3);
	al_destroy_bitmap(hitEffect4);
	//destroi fire ball
	al_destroy_bitmap(fireBall1);
	al_destroy_bitmap(fireBall2);
	al_destroy_bitmap(fireBall3);
	al_destroy_bitmap(fireBall4);
	al_destroy_bitmap(fireBall5);
	al_destroy_bitmap(fireBall6);
	al_destroy_bitmap(fireBall7);
	al_destroy_bitmap(fireBall8);
	//destroi electric ball
	al_destroy_bitmap(electricball1);
	al_destroy_bitmap(electricball2);
	al_destroy_bitmap(electricball3);
	al_destroy_bitmap(electricball4);
	//destroi musicas
	al_destroy_sample(sound);
	al_destroy_sample(soundBattle);
	al_destroy_sample(soundVictory);
	al_destroy_sample(soundLose);
	
	return 0;
}