#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define S_W 640
#define S_H 480

#define L_W S_W*2
#define L_H S_H*2

typedef struct{

	int x,y;


}Camera;


typedef struct{

	int x,y;
	
	int collision;

	SDL_Rect rect;

}Tile;


int main(){
	
	Camera cam;
	cam.x = 0;
	cam.y = 0;

	SDL_Window *window = SDL_CreateWindow("G",0,0,S_W,S_H,SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Surface *mapsurf = IMG_Load("tileset.png");	

	if(mapsurf == NULL){
		printf("SURF");
		exit(1);
	}

	SDL_Texture *map = SDL_CreateTextureFromSurface(renderer, mapsurf);
	
	//55,38

	FILE *fptr = fopen("map","r");
		
	int w, h, t;
	
	fscanf(fptr,"%d",&h);	
	fscanf(fptr,"%d",&w);

	printf("%d    %d\n",w,h);

	Tile tiles[w][h];
	
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
	
	SDL_Event e;
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,0);
	while(e.key.keysym.sym != SDLK_ESCAPE){
		SDL_SetRenderDrawColor(renderer,0xCF,0xFF,0xCF,0xFF);
		SDL_RenderClear(renderer);	
		//SDL_RenderSetLogicalSize(renderer,L_W/w,L_H/h);
		for(int i = 0; i < w; i ++){
			for(int j = 0; j < h; j ++){
			
				SDL_Rect rect = {j*16-cam.x,i*16-cam.y,16,16};
				SDL_RenderCopyEx(renderer,map,&tiles[i][j].rect,&rect,0,NULL,0);
			
			}
		}
		
		SDL_PollEvent(&e);
		if(e.type == SDL_KEYDOWN){
			if(e.key.keysym.sym == SDLK_RIGHT)
				cam.x++;	
			
			else if(e.key.keysym.sym == SDLK_LEFT)
				cam.x--;
			
			else if(e.key.keysym.sym == SDLK_UP)
				cam.y--;

			else if(e.key.keysym.sym == SDLK_DOWN)
				cam.y++;
		
		}

	SDL_RenderPresent(renderer);
	}
	/*SDL_Delay(5000);*/



}
