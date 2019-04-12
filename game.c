#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define S_W 1280 //640
#define S_H 800 //480
#define G   0.3
#define BS  20

typedef struct{

	int x, y, health;
	float dy;
	int walking,flipped,visible,frames,grounded,attacking,invincible;
	int collision;

	SDL_Texture* sheet;
}Stick;


typedef struct{

	int x,y,dx;

}Bullet;


typedef struct{

	int x,y;
	
	int w,h;

	int collision;

	SDL_Rect rect;
	

}Tile;

void render(Stick *man,Stick *enemy, Bullet *bullets[],SDL_Renderer *gRend,SDL_Texture *back[],SDL_Texture *ground,SDL_Texture *bullet,SDL_Texture *map,int w, int h,Tile tiles[w][h],int *time);
void events(Stick *man);
void logic(Stick *man,Stick *enemy, Bullet *bullets[], int *walk, int *attack, int *death,int *time);
void  mapload(FILE *fptr, int w, int h, Tile tiles[w][h]);
void addbullet(Bullet *bullets[],int x, int y, int dx);
void removebullet(Bullet *bullets[],int i);
void collide(Stick *man, Stick *enemy);
void onground(Stick *man, int w, int h, Tile tiles[w][h]);
void wall(Stick *man, int w, int h, Tile tiles[w][h]);

int main(){

	int time = 0;

	Stick man;
	man.x = 30;
	man.y = 40;
	man.dy = 0;
	man.health = 3;
	man.walking = 0;
	man.flipped = 1;
	man.grounded = 0;
	man.visible = 1;
	man.attacking = 0;
	man.invincible = 0;	
	man.collision = 0;

	Stick enemy;
	enemy.x = 40;
	enemy.y = 162;
	enemy.dy = 0;
	enemy.health = 3;
	enemy.walking = 1;
	enemy.flipped = 0;
	enemy.grounded = 0;
	enemy.visible = 1;
	enemy.attacking = 0;
	enemy.frames = 0;
	enemy.invincible = 0;
	enemy.collision = 0;

	Bullet *bullets[BS] = {NULL};
	int w,h;
	FILE *fptr = fopen("map","r");
	fscanf(fptr,"%d",&h);	
	fscanf(fptr,"%d",&w);
	Tile tiles[w][h];
	mapload(fptr,w,h,tiles);
	
	printf("\n%d\n",(**tiles).w);
	SDL_Window *window;
	window = SDL_CreateWindow("SDL",0,0,S_W,S_H,SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1 , SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Surface *manrun = IMG_Load("sheet.png");
	SDL_Surface *enemrun = IMG_Load("enemy1.png");
	SDL_Surface *bullsurf = IMG_Load("bullet.png");
	SDL_Surface *bg[2] = {IMG_Load("clouds.png"), IMG_Load("far-grounds.png")}; 
	SDL_Surface *grnd = IMG_Load("ground.png");
	
	SDL_Surface *mapsurf = IMG_Load("tileset.png");
	SDL_Texture *map = SDL_CreateTextureFromSurface(renderer,mapsurf);

	man.sheet = SDL_CreateTextureFromSurface(renderer,manrun);
	enemy.sheet = SDL_CreateTextureFromSurface(renderer,enemrun);;

	SDL_Texture *bulltext = SDL_CreateTextureFromSurface(renderer,bullsurf); 
	SDL_Texture *ground = SDL_CreateTextureFromSurface(renderer,grnd);
	SDL_Texture* BG[2] = {NULL,NULL};
	BG[0] = SDL_CreateTextureFromSurface(renderer,bg[0]);
	BG[1] = SDL_CreateTextureFromSurface(renderer,bg[1]);

	int walkcount=0, attcount=0, deathcount = 0;
	SDL_Event e;
	while(e.key.keysym.sym != SDLK_ESCAPE){
		render(&man,&enemy,bullets,renderer,BG,ground,bulltext,map,w, h,tiles,&time);
		SDL_Delay(10);
		SDL_PollEvent(&e);
		logic(&man,&enemy,bullets,&walkcount,&attcount,&deathcount,&time);
		onground(&man,w,h,tiles);
		wall(&man,w,h,tiles);
	
		onground(&enemy,w,h,tiles);
		wall(&man,w,h,tiles);
	
		events(&man);
	}


}


void render(Stick *man,Stick *enemy, Bullet *bullets[],SDL_Renderer *gRend,SDL_Texture *back[],SDL_Texture *ground,SDL_Texture *bulltext,SDL_Texture *map,int w,int h,Tile tiles[w][h],int *time){
	if(*time%5 == 0){
//376,94   434,190
//451,94   466,189
//483,95   541,190
		SDL_SetRenderDrawColor(gRend, 0xCF,0xFF,0xCF,0xFF);
		SDL_RenderClear(gRend);
		
		SDL_RenderSetLogicalSize(gRend,S_W/3,S_H/3);
	
		SDL_Rect frect = {0,S_H/2,S_W,S_H/2};
		SDL_Rect srect = {0,S_H*0.3,S_W,S_H};
	
		SDL_RenderCopy(gRend,back[0],NULL,&srect);
		SDL_RenderCopy(gRend,back[1],NULL,&frect);	

		
		SDL_Rect rect = {74*man->frames,0,74,74};
		SDL_Rect drect = {man->x,man->y,74,74};
		
		SDL_Rect erect = {100*enemy->frames,0,100,100};
		SDL_Rect edrect = {enemy->x, enemy->y,100, 100};

		SDL_Rect grect = {376,94,434-376,190-94};
		SDL_Rect dgrect = {0,220,434-376,190-94};
		
		/*SDL_RenderSetLogicalSize(gRend, S_W/2.5,S_H/2.5);*/
		SDL_RenderCopyEx(gRend,man->sheet,&rect,&drect,0,NULL,man->flipped);
		if(enemy->visible == 1)	
			SDL_RenderCopyEx(gRend,enemy->sheet,&erect,&edrect,0,NULL,enemy->flipped);

		/*SDL_RenderSetLogicalSize(gRend, S_W/2.5,S_H/2.5);*/
	/*	SDL_RenderCopyEx(gRend,ground,&grect,&dgrect,0,NULL,0);
	
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
		*/
		for(int i = 0; i <BS; i++){

			if(bullets[i]){
				SDL_Rect brect = {bullets[i]->x,bullets[i]->y,16,16};
				SDL_RenderCopy(gRend,bulltext,NULL,&brect);
			}
		}
		
		for(int i = 0; i < w; i ++){
			for(int j = 0; j  < h; j ++){
			
				SDL_Rect rect = {j*16,7*h+i*16,16,16};
				tiles[i][j].x = j*16;
				tiles[i][j].y = i*16+7*h;

				SDL_RenderCopyEx(gRend,map,&tiles[i][j].rect,&rect,0,NULL,0);
			
			}
		}

		SDL_RenderPresent(gRend);
	}
	*time = *time + 1;
}


void events(Stick *man){

	const Uint8* states = SDL_GetKeyboardState(NULL);

	 if(states[SDL_SCANCODE_UP] && man->grounded == 1 ){
		
		man->dy = -5;
		man->y += man->dy;
		man->grounded = 0;
		
	}
	else if(states[SDL_SCANCODE_SPACE] && man->attacking == 0){
		man->attacking = 1;
	}
	 else if(states[SDL_SCANCODE_RIGHT]){
		man->flipped = 0;
		man->x += 1;
		man->walking = 1;
	}
	else if(states[SDL_SCANCODE_LEFT]){	
		man->flipped = 1;
		man->x -= 1;
		man->walking = 1;
	}
	else
		man->walking = 0;
}

void logic(Stick* man,Stick* enemy, Bullet *bullets[], int *walk, int *attack,int *death, int *time){



	if(man->visible == 0){
		if(man->frames <= 28){
			man->frames = 29;
		}
		if(*time%20 == 0)
			man->frames++;
		if(man->frames >= 36){
			man->frames = 36;
		}
	}
	else if(man->walking == 1){
		
		if(*time%5 == 0){
			man->frames++;
			man->invincible = 0;
		}
		

		if(man->frames >= 16){
			man->frames = 1;
		}	
		//*walk = wc;
		}
	else if(man->attacking == 1){
		int ac = *attack;
		ac++;
		if(man->frames <= 22)
			man->frames = 23;
		if(ac >= 4){
			man->frames++;
			ac = 0;
		}			
		if(man->frames >= 28){
			man->attacking = 0;
			man->frames = 0;
		
			if(man->flipped == 1){
				addbullet(bullets,man->x+4,man->y+32,-5);
			}
			else
				addbullet(bullets,man->x+54,man->y+32,5);
		}
		*attack = ac;
	}
	else
		man->frames = 0;
	
	
	if(enemy->walking == 1){
			enemy->frames++;	
		if(enemy->frames >= 6)
			enemy->frames = 0;
	}
 
	else if(enemy->visible == 0){
	
		enemy->frames = 6;	

	}
	else
		enemy->frames = 0;

	
	if(man->grounded == 0){
		man->dy += G;
		if(man->dy > 7)
	       		man->dy = 7;	
	}

	if(enemy->grounded == 0){
		enemy->dy += G;
		if(enemy->dy > 7)
	       		enemy->dy = 7;	
	}

	man->y += man->dy;
	enemy->y += enemy->dy;
	
	if(man->y > S_H){
		man->y = 0;
		man->dy = 0;
		man->grounded = 1;
	}

	int i;
	for(i = 0; i < BS; i++){
		if(bullets[i]){
			bullets[i]->x += bullets[i]->dx;
			if(bullets[i]->x > 500 || bullets[i]->x <-100){
				removebullet(bullets,i);
				break;
			}
			if(bullets[i]->x > enemy->x+40 && bullets[i]->x < enemy->x+50 &&
			   bullets[i]->y > enemy->y && bullets[i]->y < enemy->y+100){
				
				enemy->health--;
				removebullet(bullets,i);
				if(enemy->health <= 0){enemy->visible = 0;enemy->walking = 0;}
			}
		}
	}

	if(enemy->walking == 1){
	
		
		if(enemy->flipped == 1){
			enemy->x -= 1;
		}
		else
			enemy->x += 1;
	}
	
	collide(man,enemy);


}



void mapload(FILE *fptr,int w, int h, Tile tiles[w][h]){

	
	int t;
	

	printf("%d    %d\n",w,h);
	

	
	for(int i = 0; i < w; i++){
		for(int j = 0; j < h; j++){
			
			fscanf(fptr,"%d",&t);
			tiles[i][j].rect.x = 55 + t*16;
			tiles[i][j].rect.y = 41;
			tiles[i][j].rect.w = 15;
			tiles[i][j].rect.h = 15;
			
			if(t == 0)
				tiles[i][j].collision = 0;
			else
				tiles[i][j].collision = 1;
			printf("%d\t\t",t);
			
		}
	printf("\n");
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

void collide(Stick *man, Stick *enemy){

	if(man->x > enemy->x+40 && man->x < enemy->x+50 &&
	   man->y > enemy->y && man->y+74 < enemy->y+100){
		
		if(man->invincible == 0)	
			man->health--;

		if(man->health <= 0){man->visible = 0;}

		man->invincible = 1;
	}
}


void onground(Stick *man, int w, int h, Tile tiles[w][h]){

	for(int i =0; i<w;i++){
		for(int j = 0; j<h;j++){
			printf("%d     %d\n",man->x,tiles[i][j].x);
			if(man->y+50 > tiles[i][j].y && man->y < tiles[i][j].y+15 &&  tiles[i][j].collision == 1 && man->x+23 < tiles[i][j].x+15 && man->x+40 > tiles[i][j].x){ 
				man->grounded = 1;
				man->y-=7;// tiles[i][j].y-75;
				printf("broke!");
				return;
			}
		}

			
	}
	man->grounded = 0;
}

void wall(Stick *man, int w, int h, Tile tiles[w][h]){

	for(int i =0; i<w;i++){
		for(int j = 0; j<h;j++){
			printf("%d     %d\n",man->x,tiles[i][j].x);
			if(man->y+50 > tiles[i][j].y && man->y < tiles[i][j].y+20 &&  tiles[i][j].collision == 1 && man->x+23 < tiles[i][j].x+15 && man->x+40 > tiles[i][j].x){ 
				if(man->flipped == 1)
					man->x += 3;
				else
					man->x -= 3;
				// tiles[i][j].y-75;
				printf("broke!");
				return;
			}
		}

			
	}
}
