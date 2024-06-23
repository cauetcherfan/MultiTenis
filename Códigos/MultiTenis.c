#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>

const float FPS = 60;

const int SCREEN_W = 640;
const int SCREEN_H = 768;

const int limiteBola = 12;
const int pontuacaoMaxima = 10;

int rebate1 = 0;
int rebate2 = 0;

int placarvermelho = 0;
int placarazul = 0;

ALLEGRO_SAMPLE* som_gol = NULL;

//Canhão
typedef struct canhao
{
	int x, y;
	int id;
	bool cima;
	bool direita;
} canhao;

void iniciaCanhao1(canhao *c, int i)
{
	c->x = 0;
	c->y = 0;
	c->id = i;
	c->cima = false;
	c->direita = true;
}

void iniciaCanhao2(canhao *c, int i)
{
	c->x = SCREEN_W;
	c->y = 0;
	c->id = i;
	c->cima = false;
	c->direita = false;
}

void iniciaCanhao3(canhao *c, int i)
{
	c->x = SCREEN_W;
	c->y = SCREEN_H;
	c->id = i;
	c->cima = true;
	c->direita = false;
}

void iniciaCanhao4(canhao *c, int i)
{
	c->x = 0;
	c->y = SCREEN_H;
	c->id = i;
	c->cima = true;
	c->direita = true;
}

//JOGADOR:
int DIST_FUNDO = 50;
int ALTURA_JOGADOR = 30;
int LARGURA_JOGADOR = 100;
int VEL_JOGADOR = 3.5;

ALLEGRO_FONT *FONT_32;

typedef struct Jogador
{
	float x, y, h, w, vel;
	int direita, esquerda, cima, baixo;
	ALLEGRO_COLOR cor;
	int id;
} Jogador;

void desenhaJogador(Jogador p)
{
	al_draw_filled_rectangle(p.x, p.y, p.x+p.w, p.y+p.h, p.cor);
}

void desenhaQuadra()
{
	al_clear_to_color(al_map_rgb(0,155,0));
	al_draw_line(0, SCREEN_H/2, SCREEN_W, SCREEN_H/2, al_map_rgb(255,255,255), 3);
}

//Bola
typedef struct Bola 
{
	float x, y;
	float raio;
	float dx;
	float dy;
	int bolavalida;
	int id;
	int posicaoCanhao;
	int c;
	bool emJogo;
	bool cima;
	bool direita;
	ALLEGRO_COLOR cor;
} Bola;

void bolavalida(Bola bola[])
{
	int i = 0;
	for(i=0; i<limiteBola; i++)
		bola[i].emJogo = 0;
}

void iniciaBola(Bola *b, int id)
{
	b->id = id;
	b->raio = 2 + rand() % 3;
	b->emJogo = false;
}

void desenhaBola(Bola bolas[])
{
	int i=0;
	for(i=0; i<limiteBola; i++)
	{
		if(bolas[i].emJogo==true)
			al_draw_filled_circle(bolas[i].x, bolas[i].y, bolas[i].raio, bolas[i].cor);
	}
}

Bola* pegaBolaDisponivel(Bola bolas[])
{
	int i=0;
	for(i=0; i<limiteBola; i++)
	{
		if(bolas[i].emJogo==false)
			return &bolas[i];
	}
	return NULL;
}

void lancaBola(Bola *b, canhao canhoes[], int *canhaoAtual)
{
	printf(" Bola liberada");
	b->posicaoCanhao = *canhaoAtual;
	b->x = canhoes[*canhaoAtual].x;
	b->y = canhoes[*canhaoAtual].y;
	b->dx = 1 + rand() % 4;
	b->dy = 1 + rand() % 4;
	b->cima = canhoes[*canhaoAtual].cima;
	b->direita = canhoes[*canhaoAtual].direita;
	b->cor = al_map_rgb(255, 255, 0);
	b->emJogo = 1;

	if(*canhaoAtual==0)
		b->c=0;

	if(*canhaoAtual==1)
		b->c=1;

	if(*canhaoAtual==2)
	{
		//b->raio=10;
		b->dx = -(1 + rand() % 4);
		b->dy = -(1 + rand() % 4);
		b->c=2;
	}

	if(*canhaoAtual==3)
	{
		b->dx = (1 + rand() % 4);
		b->dy = -( 1 + rand() % 4);
		b->c=3;
	}

	if(*canhaoAtual==3)
		*canhaoAtual = 0;

	else
		*canhaoAtual = *canhaoAtual + 1;
}

void atualizaBola(Bola bola[])
{
	int i=0;
	for(i=0;i<limiteBola;i++)
	{
		if(bola[i].y < 0 && bola[i].emJogo == 1)
		{
			placarvermelho += 1;
			al_play_sample(som_gol, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
        	
        if(bola[i].y > SCREEN_H && bola[i].emJogo == 1)
		{
        	placarazul += 1;
			al_play_sample(som_gol, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}

    	if(bola[i].x > SCREEN_W || bola[i].x < 0)
    		bola[i].dx = -bola[i].dx;

         if(bola[i].y > SCREEN_H || bola[i].y < 0)
         	bola[i].emJogo = 0;

	    bola[i].x += bola[i].dx;
    	bola[i].y += bola[i].dy;
    }
}

int rebatep1(int rebate, Bola b[], Jogador j){
	int i = 0;
	for(i=0; i<limiteBola; i++){
		if(b[i].x + b[i].raio >= j.x && b[i].x - b[i].raio <= j.x + j.w && b[i].y + b[i].raio >= j.y && b[i].y - b[i].raio <= j.y + j.h){
			if(b[i].c==0||b[i].c==1){
				if(rebate2 == 1){
					b[i].y = j.y - b[i].raio - 2;
					b[i].dy = 1.5*(-b[i].dy);
					b[i].c = 2;
				}
			}
		}
	}
}

int rebatep2(int rebate, Bola b[], Jogador j){
	int i = 0;
	for(i=0; i<limiteBola; i++){
		if(b[i].x + b[i].raio >= j.x && b[i].x - b[i].raio <= j.x + j.w && b[i].y - b[i].raio <= j.y + j.h && b[i].y + b[i].raio >= j.y){
			if(b[i].c==2||b[i].c==3){
				if(rebate1 == 1){
					b[i].y = j.y + j.h + b[i].raio + 2;
					b[i].dy = 1.5*(-b[i].dy);
					b[i].c = 0;
				}
			}
		}
	}
}

void initJogador(Jogador *p)
{
	p->h = ALTURA_JOGADOR;
	p->w = LARGURA_JOGADOR;
	p->x = SCREEN_W/2 - p->w/2;
	p->direita = 0;
	p-> esquerda = 0;
	p->cima = 0;
	p-> baixo = 0;
	p->vel = VEL_JOGADOR;
}

void initJogador1(Jogador *p1)
{
	initJogador(p1);

	p1->y = p1->h;
	p1->cor = al_map_rgb(0, 0, 155);
	p1->id = 1;
}

void initJogador2(Jogador *p2)
{
	initJogador(p2);

	p2->y = SCREEN_H - DIST_FUNDO - p2->h;
	p2->cor = al_map_rgb(155, 0, 0);
	p2->id = 2;
}

void extremoJogador1(Jogador *p)
{
	if(p->y >= SCREEN_H/2-ALTURA_JOGADOR)
		p->y = SCREEN_H/2-ALTURA_JOGADOR - 1.5;
}

void extremoJogador2(Jogador *p)
{
	if(p->y <= SCREEN_H/2)
		p->y = SCREEN_H/2 + 1.5;	
}

void atualizaJogador(Jogador *p)
{
	if (p->x <=0)
		p->esquerda = 0;

	if(p->x >= SCREEN_W-LARGURA_JOGADOR)
		p->direita = 0;

	if(p->y <= 0)
		p->cima = 0;

	if(p->y >= SCREEN_H-ALTURA_JOGADOR)
		p->baixo = 0;

	p->x = p->x + p->direita*p->vel - p->esquerda*p->vel;
	p->y = p->y + p-> baixo*p->vel - p->cima*p->vel;
}
	
int main(int argc, char **argv)
{
	int i, j;
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
   
	//----------------------- Rotinas de inicialização ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init())
	{
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon())
    {
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon())
	{
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer)
    {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display)
	{
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard())
	{
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon())
	{
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL)
	{
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue)
	{
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

// Inicializa o Allegro e o sistema de áudio
    if (!al_init() || !al_install_audio() || !al_init_acodec_addon()) {
        fprintf(stderr, "Falha ao inicializar o Allegro e o sistema de áudio.\n");
        return -1;
    }

    // Reserva samples
    if (!al_reserve_samples(1)) {
        fprintf(stderr, "Falha ao reservar samples.\n");
        return -1;
    }

    // Carrega o som do gol
    som_gol = al_load_sample("goal.wav");
    if (!som_gol) {
        fprintf(stderr, "Falha ao carregar o som do gol.\n");
        al_uninstall_audio();
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
	
	/*------------------------ JOGADORES ----------------------*/

	Jogador p1;
	initJogador1(&p1);

	FONT_32 = al_load_font("arial.ttf", 32, 1);

	Jogador p2;
	initJogador2(&p2);

	FONT_32 = al_load_font("arial.ttf", 32, 1);

//Canhão 
	canhao canhoes[4];
	iniciaCanhao1(&canhoes[0],0);
	iniciaCanhao2(&canhoes[1],1);
	iniciaCanhao3(&canhoes[2],2);
	iniciaCanhao4(&canhoes[3],3);

	int canhaoAtual = 0;

//Bolinhas
	Bola bolas[limiteBola];
	int b=0;
	for(b=0; b<limiteBola; b++)
		iniciaBola(&bolas[b],b);

	bolavalida(bolas);

	int VJ1=0, DJ1=0, VJ2=0, DJ2=0;
	FILE *historico = fopen("historicoJogo.txt","r");

	fscanf(historico, "%d %d %d %d", &VJ1, &DJ1, &VJ2, &DJ2);
	fclose(historico);

	int playing = 1;
	int fim = 0;

	int tempo;

	while(playing)
	{
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			playing = 0;
		}
		
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			desenhaQuadra();

			//Criando o placar
			al_draw_textf(size_32, al_map_rgb(255, 0, 0), 125, 20, 1, "%d", placarvermelho);
 			al_draw_textf(size_32, al_map_rgb(0, 0, 255), 525, 20, 1, "%d", placarazul);

			atualizaJogador(&p1);
			extremoJogador1(&p1);
			desenhaJogador(p1);

			atualizaJogador(&p2);
			extremoJogador2(&p2);
			desenhaJogador(p2);

 			//contador de 5 segundos para organizar o lançamento de bolinha
			if(al_get_timer_count(timer)%(int)FPS == 0 && (int)(al_get_timer_count(timer)/FPS) % 5 == 0)
			{
				Bola* b = pegaBolaDisponivel(bolas);
				if(b != NULL)
					lancaBola(b, canhoes, &canhaoAtual);
				
			}
			atualizaBola(bolas);
			desenhaBola(bolas);

			rebatep1(rebate1, bolas, p2);
			rebatep2(rebate2, bolas, p1);
			al_flip_display();
			
			if(al_get_timer_count(timer)%(int)FPS == 0)
			{
				tempo = (al_get_timer_count(timer)/FPS);
				if(tempo == 1)
					printf("\n%d segundo se passou...", tempo);
				else
					printf("\n%d segundos se passaram...", tempo);
			}
		}

		if(placarazul == pontuacaoMaxima || placarvermelho == pontuacaoMaxima)
		{
			playing = 0;
			if(placarvermelho==pontuacaoMaxima)
			{
				VJ1++;
				DJ2++;
			}
			else if(placarazul==pontuacaoMaxima)
			{
				VJ2++;
				DJ1++;
			}
		FILE *historico = fopen("historicoJogo.txt","w");
		fprintf(historico, "%d %d %d %d", VJ1, DJ1, VJ2, DJ2);
		fclose(historico);
		} 
		
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			playing = 0;
		}
		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
					playing = 0;
				break;

				case ALLEGRO_KEY_W:
					p2.cima = 1;
				break;
			
				case ALLEGRO_KEY_S:
					p2.baixo = 1;
				break;
			
				case ALLEGRO_KEY_A:
					p2.esquerda = 1;
				break;
			
				case ALLEGRO_KEY_D:
					p2.direita = 1;
				break;

				case ALLEGRO_KEY_UP:
					p1.cima = 1;
				break;
			
				case ALLEGRO_KEY_DOWN:
					p1.baixo = 1;
				break;
			
				case ALLEGRO_KEY_LEFT:
					p1.esquerda = 1;
				break;
			
				case ALLEGRO_KEY_RIGHT:
					p1.direita = 1;
				break;

				case ALLEGRO_KEY_ENTER:
					rebate1 = 1;
				break;

				case ALLEGRO_KEY_SPACE:
					rebate2 = 1;
				break;
			}
		}

		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			//imprime qual tecla foi
			//printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_W:
					p2.cima = 0;
				break;
				
				case ALLEGRO_KEY_S:
					p2.baixo = 0;
				break;
				
				case ALLEGRO_KEY_A:
					p2.esquerda = 0;
				break;
				
				case ALLEGRO_KEY_D:
					 p2.direita = 0;
				break;

				case ALLEGRO_KEY_UP:
					p1.cima = 0;
				break;
				
				case ALLEGRO_KEY_DOWN:
					p1.baixo = 0;
				break;
				
				case ALLEGRO_KEY_LEFT:
					p1.esquerda = 0;
				break;
				
				case ALLEGRO_KEY_RIGHT:
					 p1.direita = 0;
				break;

				case ALLEGRO_KEY_ENTER:
					rebate1 = 0;
				break;

				case ALLEGRO_KEY_SPACE:
					rebate2 = 0;
				break;
			}
		}
	}
	//fim do while
		al_clear_to_color(al_map_rgb(230,232,250));
			al_draw_textf(size_32, al_map_rgb(0,0,0),SCREEN_W/2,SCREEN_H/2-300, 1, "Placar final");  
			al_draw_textf(size_32, al_map_rgb(255,0,0),SCREEN_W/2,SCREEN_H/2-250, 1, "Vermelho: %d", placarvermelho);  
			al_draw_textf(size_32, al_map_rgb(0,0,255), SCREEN_W/2, SCREEN_H/2-200, 1, "Azul: %d", placarazul);
			al_draw_textf(size_32, al_map_rgb(0,0,0),SCREEN_W/2,SCREEN_H/2, 1, "Histórico:");  
			al_draw_textf(size_32, al_map_rgb(255,0,0),SCREEN_W/2,SCREEN_H/2+50, 1, "Vermelho -  %d vitórias", VJ1); 
			al_draw_textf(size_32, al_map_rgb(0,0,255),SCREEN_W/2,SCREEN_H/2+100, 1, "Azul -  %d vitórias", VJ2); 
			al_flip_display();
     		al_rest(5);

	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
    al_destroy_sample(som_gol);
    al_uninstall_audio();

	return 0;
}