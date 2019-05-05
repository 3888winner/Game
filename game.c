/*
 *	Name: Ahmed Abbas
 *	Date: 24/4/19
 *	Program: game.c
 *	Aim: A fun platformer with a customizable map and one enemy.
 *	     Using the SDL2 development library to load and render game data.
 *	     All game physics such as gravity and wall collisions were programmed by me.
 *	     All game sprites were found online and are free for use.
 *
 */

/*Header Files*/
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

/*Constants*/
#define S_W 1280 //640
#define S_H 800 //480
#define G   0.3
#define BS  20

/*struct declarations*/
typedef struct{

	int x, y, health;
	int w,h;
	float dy;
	int walking,flipped,visible,frames,grounded,attacking,invincible;
	int collision;
	int alive;
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


typedef struct{

	int x,y;

}Camera;

/* Function Declarations  */
void render(Stick *man,Stick *enemy, Bullet *bullets[],SDL_Renderer *gRend,SDL_Texture *back[],SDL_Texture *ground,SDL_Texture *bullet,SDL_Texture *map,int w, int h,Tile tiles[w][h],Camera cam,int *time);
void events(Stick *man);
void logic(Stick *man,Stick *enemy, Bullet *bullets[],Camera cam, int *walk, int *attack, int *death,int *time);
void  mapload(FILE *fptr, int w, int h, Tile tiles[w][h],Stick *enemy);
void addbullet(Bullet *bullets[],int x, int y, int dx);
void removebullet(Bullet *bullets[],int i);
void collide(Stick *man, Stick *enemy);
void onground(Stick *man, int w, int h, Tile tiles[w][h]);
void wall(Stick *man, int w, int h, Tile tiles[w][h]);
void check();
void checkfile(const char *filename);
SDL_Rect load_text(SDL_Renderer *renderer,SDL_Texture **texture,TTF_Font *font);


int main(){

	check();

	int time = 0;
	
	// Main character
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
	man.alive = 1;
	man.w = 30;
	man.h = 30;

	// Enemy
	Stick enemy;
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
	enemy.w = 40;
	enemy.h = 34;

	// Bullets
	Bullet *bullets[BS] = {NULL};

	//Camera
	Camera cam = {man.x,man.y};
	
	// Loading map data
	int w,h;
	FILE *fptr = fopen("map","r");
	fscanf(fptr,"%d",&h);
	fscanf(fptr,"%d",&w);
	Tile tiles[w][h];
	mapload(fptr,w,h,tiles,&enemy);
	
	// Creating the Game window
	SDL_Window *window;
	window = SDL_CreateWindow("SDL",0,0,S_W,S_H,SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1 , SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	// loading all images
	SDL_Surface *manrun = IMG_Load("sheet.png");
	SDL_Surface *enemrun = IMG_Load("enemy1.png");
	SDL_Surface *bullsurf = IMG_Load("bullet.png");
	SDL_Surface *bg[2] = {IMG_Load("clouds.png"), IMG_Load("far-grounds.png")};
	SDL_Surface *grnd = IMG_Load("ground.png");
	SDL_Surface *mapsurf = IMG_Load("tileset.png");

	// Converting images to texture objects
	SDL_Texture *map = SDL_CreateTextureFromSurface(renderer,mapsurf);
	man.sheet = SDL_CreateTextureFromSurface(renderer,manrun);
	enemy.sheet = SDL_CreateTextureFromSurface(renderer,enemrun);
	SDL_Texture *bulltext = SDL_CreateTextureFromSurface(renderer,bullsurf);
	SDL_Texture *ground = SDL_CreateTextureFromSurface(renderer,grnd);
	SDL_Texture* BG[2] = {NULL,NULL};
	BG[0] = SDL_CreateTextureFromSurface(renderer,bg[0]);
	BG[1] = SDL_CreateTextureFromSurface(renderer,bg[1]);

	// loading font data
	SDL_Texture *message;
	TTF_Init();
    	TTF_Font *font = TTF_OpenFont("BLOOD.ttf", 24);
	SDL_Rect messagerect = load_text(renderer, &message,font);

	// Character sprite delays
	int walkcount=0, attcount=0, deathcount = 0;
	
	// User input
	SDL_Event e;

	// Main loop
	while(e.key.keysym.sym != SDLK_ESCAPE){
		
		
		render(&man,&enemy,bullets,renderer,BG,ground,bulltext,map,w, h,tiles,cam,&time);
		
		logic(&man,&enemy,bullets,cam,&walkcount,&attcount,&deathcount,&time);
		
		// character and enemy collisions with ground and wall
		onground(&man,w,h,tiles);
		wall(&man,w,h,tiles);
		onground(&enemy,w,h,tiles);
		wall(&enemy,w,h,tiles);

		// user input
		events(&man);

		// camera movement
		if(cam.x <= man.x-240)
			cam.x++;
		if(cam.x > man.x+20){
			cam.x--;
		}
		if(cam.y <= man.y-120)
			cam.y++;
		if(cam.y > man.y-60)
			cam.y--;

		
		SDL_PollEvent(&e); // user input
		SDL_Delay(10); // Rendering too quickly can cause overheating

		while(man.alive == 0 && e.key.keysym.sym != SDLK_ESCAPE){
			// Dead loop

			SDL_RenderCopy(renderer,message,NULL,&messagerect);
			SDL_RenderPresent(renderer);
			SDL_PollEvent(&e);
			SDL_Delay(10);

			if(e.key.keysym.sym == SDLK_RETURN){
				//reset
				man.health = 3;
				man.x = 0;
				man.y = 60;
				man.alive = 1;
			       	man.visible = 1;

				enemy.health = 3;
				enemy.visible = 1;
				enemy.walking = 1;
			}
		}
	}


}


void render(Stick *man,Stick *enemy, Bullet *bullets[],SDL_Renderer *gRend,SDL_Texture *back[],SDL_Texture *ground,SDL_Texture *bulltext,SDL_Texture *map,int w,int h,Tile tiles[w][h],Camera cam,int *time){
	/* Function render renders all images onto the screen */

	if(*time%5 == 0){

		SDL_SetRenderDrawColor(gRend, 0xCF,0xFF,0xCF,0xFF);
		SDL_RenderClear(gRend);

		SDL_RenderSetLogicalSize(gRend,S_W/3,S_H/3);
		
		// Rects make sure the correct images are taken
		SDL_Rect frect = {-150,S_H/5,S_W/1.5,S_H/5};
		SDL_Rect srect = {0,S_H*0.3,S_W,S_H};

		SDL_RenderCopy(gRend,back[0],NULL,&srect);
		SDL_RenderCopy(gRend,back[1],NULL,&frect);
		
		SDL_Rect rect = {74*man->frames,0,74,74};
		SDL_Rect drect = {man->x-cam.x,man->y-cam.y,74,74};

		SDL_Rect erect = {100*enemy->frames,0,100,100};
		SDL_Rect edrect = {enemy->x-cam.x, enemy->y-cam.y,100, 100};

		SDL_RenderCopyEx(gRend,man->sheet,&rect,&drect,0,NULL,man->flipped);
		
		// only render if visible/alive
		if(enemy->visible == 1)
			SDL_RenderCopyEx(gRend,enemy->sheet,&erect,&edrect,0,NULL,enemy->flipped);

		// bullets rendering
		for(int i = 0; i <BS; i++){

			if(bullets[i]){
				SDL_Rect brect = {bullets[i]->x-cam.x,bullets[i]->y-cam.y,16,16};
				SDL_RenderCopy(gRend,bulltext,NULL,&brect);
			}
		}

		// map rendering
		for(int i = 0; i < w; i ++){
			for(int j = 0; j  < h; j ++){

				SDL_Rect rect = {j*16-cam.x,7*h+i*16-cam.y,16,16};
				tiles[i][j].x = j*16;
				tiles[i][j].y = i*16+7*h;

				SDL_RenderCopyEx(gRend,map,&tiles[i][j].rect,&rect,0,NULL,0);

			}
		}

		// present the renderer 
		SDL_RenderPresent(gRend);
	}

	*time = *time + 1;
}


void events(Stick *man){

	/* Function events takes user inputs and translates them into character movements  */

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

void logic(Stick* man,Stick* enemy, Bullet *bullets[],Camera cam, int *walk, int *attack,int *death, int *time){
	
	/* Function logic handles all game physics  */


	if(man->visible == 0){
		if(man->frames <= 28){
			man->frames = 29;
		}
		if(*time%20 == 0)
			man->frames++;
		if(man->frames >= 36){
			man->frames = 36;
			man->alive = 0;
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

		man->dy = 0;
		man->grounded = 1;
		man->visible = 0;
	}

	// Bullet physics
	for(int i = 0; i < BS; i++){
		if(bullets[i]){
			bullets[i]->x += bullets[i]->dx;
			if(bullets[i]->x-cam.x > +500 || bullets[i]->x-cam.x < -500){
				removebullet(bullets,i);
				break;
			}
			if(bullets[i]->x > enemy->x+40 && bullets[i]->x < enemy->x+50 &&
			   bullets[i]->y > enemy->y && bullets[i]->y < enemy->y+40 && enemy->visible == 1){

				enemy->health--;
				removebullet(bullets,i);
				if(enemy->health <= 0){enemy->visible = 0;enemy->walking = 0;}
			}
		}
	}

	if(enemy->walking == 1){

		if(enemy->x <= 20)
			enemy->flipped = 0;

		if(enemy->flipped == 1){
			enemy->x -= 1;
		}
		else
			enemy->x += 1;
	}
	if(enemy->visible)
		collide(man,enemy);


}



void mapload(FILE *fptr,int w, int h, Tile tiles[w][h], Stick *enemy){

	/* Function mapload reads map data from the map file */

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
			else if(t == -1){
				enemy->x=j*15;
				enemy->y=i*15;
				tiles[i][j].collision = 0;
			}
			else
				tiles[i][j].collision = 1;
			printf("%d\t\t",t);

		}
	printf("\n");
	}

}


void addbullet(Bullet *bullets[],int x, int y, int dx){
	
	/* Function addbullet adds a bullet  */

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
	
	/* Function remove bullet removes a bullet and clears the memory */

	if(bullets[i]){
		free(bullets[i]);
		bullets[i] = NULL;

	}
}


void collide(Stick *man, Stick *enemy){
	
	/* Function collide handles enemy and main character collisions */

	if(man->x+30 > enemy->x+40 && man->x+30 < enemy->x+50 &&
	   man->y > enemy->y && man->y+74 < enemy->y+100){

		if(man->invincible == 0)
			man->health--;

		if(man->health <= 0){man->visible = 0;}

		man->invincible = 1;
	}
}


void onground(Stick *man, int w, int h, Tile tiles[w][h]){

	/* Function onground checks collisions between a character and the ground and makes sure they remain on the ground */

	for(int i =0; i<w;i++){
		for(int j = 0; j<h;j++){
			if(man->y+20+man->h > tiles[i][j].y && man->y+20 < tiles[i][j].y+15 &&  tiles[i][j].collision == 1 && man->x+30 < tiles[i][j].x+15 && man->x+40 > tiles[i][j].x){
				man->grounded = 1;
				man->y-=7;// tiles[i][j].y-75;
				return;
			}
		}


	}
	man->grounded = 0;
}

void wall(Stick *man, int w, int h, Tile tiles[w][h]){

	/* Function wall is similar to function onground, however for wall collisions instead */

	for(int i =0; i<w;i++){
		for(int j = 0; j<h;j++){
			if(man->y+50 > tiles[i][j].y && man->y+20 < tiles[i][j].y+15 &&  tiles[i][j].collision == 1 && man->x+30 < tiles[i][j].x+15 && man->x+10+man->w > tiles[i][j].x){
				if(man->flipped == 1)
					man->x += 3;
				else
					man->x -= 3;
				// tiles[i][j].y-75;
				man->flipped = 1;
				return;
			}
		}


	}
}

void check(){

	/* Function check goes through all files and checks if they exist in the current directory */
	
	for(int i =0; i<8;i++){

		switch(i){

			case 0:
				checkfile("map");
				break;

			case 1:
				checkfile("sheet.png");
				break;

			case 2:
				checkfile("enemy1.png");
				break;

			case 3:
				checkfile("bullet.png");
				break;

			case 4:
				checkfile("clouds.png");
				checkfile("far-grounds.png");
				break;

			case 5:
				checkfile("ground.png");
				break;

			case 6:
				checkfile("tileset.png");
				break;

			case 7:
				checkfile("BLOOD.ttf");
				break;

		}


	}

}

void checkfile(const char *filename){

	/* Function checkfile checks if the file with given filename exists in the current directory */

	FILE *file = fopen(filename,"r");

	if(file == NULL){
		printf("%s not found.\nEnding program",filename);
		exit(1);
	}

	fclose(file);

}

SDL_Rect load_text(SDL_Renderer *renderer,SDL_Texture **texture,TTF_Font *font){
	
	/* function load_text loads text and font data into a format SDL can use */

	SDL_Color Red = {255,0,0,0};
	SDL_Surface* surfmessage = TTF_RenderText_Solid(font,"You Died Press Enter to restart",Red);
	 *texture = SDL_CreateTextureFromSurface(renderer,surfmessage);
	SDL_Rect messagerect = {40,40,surfmessage->w,surfmessage->h};
	SDL_FreeSurface(surfmessage);
	return messagerect;
}
