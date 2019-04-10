#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define S_W 1280 //640
#define S_H 800 //480
#define G   1
#define BS  20

typedef struct{

	int x, y, dy;
	
	int walking,flipped,visible,frames,grounded,attacking;

	SDL_Texture* sheet;
}Stick;


typedef struct{

	int x,y,dx;

}Bullet;


void render(Stick *man, Bullet *bullets[],SDL_Renderer *gRend,SDL_Texture *back[],SDL_Texture *ground,SDL_Texture *bullet,int time);
void events(Stick *man);
void logic(Stick *man, Bullet *bullets[]);
void mapload(FILE *fptr);
void addbullet(Bullet *bullets[],int x, int y, int dx);
void removebullet(Bullet *bullets[],int i);

int main(){

	int time = 0;

	Stick man;
	man.x = 30;
	man.y = 80;
	man.dy = 0;
	man.walking = 0;
	man.flipped = 1;
	man.grounded = 0;
	man.visible = 1;
	man.attacking = 0;

	Bullet *bullets[BS] = {NULL};

	SDL_Window *window;
	window = SDL_CreateWindow("SDL",0,0,S_W,S_H,SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1 , SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Surface *manrun = IMG_Load("sheet.png");
	SDL_Surface *bullsurf = IMG_Load("bullet.png");
	SDL_Surface *bg[2] = {IMG_Load("clouds.png"), IMG_Load("far-grounds.png")}; 
	SDL_Surface *grnd = IMG_Load("ground.png");
	
	man.sheet = SDL_CreateTextureFromSurface(renderer,manrun);

	SDL_Texture *bulltext = SDL_CreateTextureFromSurface(renderer,bullsurf); 
	SDL_Texture *ground = SDL_CreateTextureFromSurface(renderer,grnd);
	SDL_Texture* BG[2] = {NULL,NULL};
	BG[0] = SDL_CreateTextureFromSurface(renderer,bg[0]);
	BG[1] = SDL_CreateTextureFromSurface(renderer,bg[1]);


	int walkcounter = 0, atcounter = 0;;		
	SDL_Event e;
	while(e.key.keysym.sym != SDLK_ESCAPE){
		render(&man,bullets,renderer,BG,ground,bulltext,time);
		SDL_Delay(10);
		SDL_PollEvent(&e);
		if(man.walking == 1){
			walkcounter++;
			if(walkcounter >= 4){
				man.frames++;
				walkcounter = 0;
			}
			if(man.frames >= 16){
				man.frames = 1;
			}	
		}
		else if(man.attacking == 1){
			atcounter++;
			if(man.frames <= 22)
				man.frames = 23;
			if(atcounter >= 4){
				man.frames++;
				atcounter = 0;
			}			
			if(man.frames >= 28){
				man.attacking = 0;
				man.frames = 0;
			
				if(man.flipped == 1){
					addbullet(bullets,man.x+4,man.y+32,-5);
				}
				else
					addbullet(bullets,man.x+54,man.y+32,5);
			}
		}
		else
			man.frames = 0;

		logic(&man,bullets);
		events(&man);
	}


}


void render(Stick *man, Bullet *bullets[],SDL_Renderer *gRend,SDL_Texture *back[],SDL_Texture *ground,SDL_Texture *bulltext,int time){
	if(time%10 == 0){
//376,94   434,190
//451,94   466,189
//483,95   541,190
		SDL_SetRenderDrawColor(gRend, 0xCF,0xFF,0xCF,0xFF);
		SDL_RenderClear(gRend);
		
		SDL_RenderSetLogicalSize(gRend,S_W,S_H);
	
		SDL_Rect frect = {0,S_H/2,S_W,S_H/2};
		SDL_Rect srect = {0,S_H*0.3,S_W,S_H};
	
		SDL_RenderCopy(gRend,back[0],NULL,&srect);
		SDL_RenderCopy(gRend,back[1],NULL,&frect);	

		SDL_RenderSetLogicalSize(gRend, S_W/2.5,S_H/2.5);
		
		SDL_Rect rect = {74*man->frames,0,74,74};
		SDL_Rect drect = {man->x,man->y,74,74};
		
		SDL_Rect grect = {376,94,434-376,190-94};
		SDL_Rect dgrect = {0,220,434-376,190-94};
		
		SDL_RenderCopyEx(gRend,man->sheet,&rect,&drect,0,NULL,man->flipped);
		SDL_RenderCopyEx(gRend,ground,&grect,&dgrect,0,NULL,0);
	
		SDL_Rect grect1 = {451,94,15,189-94};
		SDL_Rect dgrect1 = {434-376,220,15,189-94};
		SDL_RenderCopyEx(gRend,ground,&grect1,&dgrect1,0,NULL,0);
		
		int i;
		for(i = 0; i<28;i++){

			SDL_Rect dgrect1 = {14*i+434-376,220,15,189-94};
			SDL_RenderCopyEx(gRend,ground,&grect1,&dgrect1,0,NULL,0);
		}
		
		SDL_Rect grect2 = {483,95,541-483,190-95};
		SDL_Rect dgrect2 = {14*i+434-376,220,541-483,190-95};
		SDL_RenderCopyEx(gRend,ground,&grect2,&dgrect2,0,NULL,0);

		for(i = 0; i <BS; i++){

			if(bullets[i]){
				SDL_Rect brect = {bullets[i]->x,bullets[i]->y,16,16};
				SDL_RenderCopy(gRend,bulltext,NULL,&brect);
			}
		}

		SDL_RenderPresent(gRend);
	}
	time++;
}


void events(Stick *man){

	const Uint8* states = SDL_GetKeyboardState(NULL);

	 if(states[SDL_SCANCODE_UP] && man->grounded == 1 ){
		
		man->dy = -10;
		man->y += man->dy;
		man->grounded = 0;;
		
	}
	else if(states[SDL_SCANCODE_SPACE] && man->attacking == 0){
		man->attacking = 1;
	}
	 else if(states[SDL_SCANCODE_RIGHT]){
		man->flipped = 0;
		man->x += 3;
		man->walking = 1;
	}
	else if(states[SDL_SCANCODE_LEFT]){	
		man->flipped = 1;
		man->x -= 3;
		man->walking = 1;
	}
	else
		man->walking = 0;
}

void logic(Stick* man, Bullet *bullets[]){
	
	if(man->grounded == 0){
		man->dy += G;
		if(man->dy > 7)
	       		man->dy = 7;	
	}

	man->y += man->dy;
	
	if(man->y > 170){
		man->y = 170;
		man->dy = 0;
		man->grounded = 1;
	}

	int i;
	for(i = 0; i < BS; i++){
		if(bullets[i]){
			bullets[i]->x += bullets[i]->dx;
			if(bullets[i]->x > 2000 || bullets[i]->x <-100){
				removebullet(bullets,i);
			}
		}
	}
}


void mapload(FILE *fptr){

	fptr = fopen("map.txt","r");
	
	if(fptr == NULL){
		printf("File Not Found");
		exit(1);
	}
	int data[10],i=0;
	while(!feof(fptr)){
		fscanf(fptr,"%d",&data[i]);
		i++;
	}
	

}

void addbullet(Bullet *bullets[],int x, int y, int dx){

	int found=-1,i;
	for(i = 0; i < BS; i++){
		if(bullets[i] == NULL){
			found = i;
			break;
		}
	}

	if(found >= 0){
		bullets[i] = malloc(sizeof(Bullet));
		bullets[i]->x = x;
	        bullets[i]->y = y;
		bullets[i]->dx = dx;	
	}
}

void removebullet(Bullet *bullets[],int i){

	if(bullets[i]){
		free(bullets[i]);
		bullets[i] = NULL;
		
	}


}
