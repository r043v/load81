#ifndef GDL_H
#define GDL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define MAX_LOADED_SURFACE 2048
#define MAX_LOADED_ANIM 512
#define MAX_LOADED_MAP 32

#define COLOR_PINK 0
#define COLOR_WHITE 1
#define COLOR_BLACK 2
#define COLOR_UP_LEFT 3

#include "load81.h"

struct anim
{	long long lastTime  ;
	unsigned int frmTime   ;
	unsigned int curentFrm ;
	unsigned int frmNumber ;
	unsigned int animType  ;
	unsigned int frameWidth;
	SDL_Surface * imgs;
};

struct mapOutZone
{	unsigned int height,width,x,y;
};

struct map
{	struct mapOutZone out;
	unsigned int scrollx, scrolly;
	unsigned int tileSizex, tileSizey;
	unsigned int xTileDec, yTileDec;
	unsigned int sizeInTilex, sizeInTiley;
	unsigned int sizeInPixelx, sizeInPixely;
	unsigned int firstTileBlitx, firstTileBlity;
	unsigned int uncutDrawx, uncutDrawy;
	unsigned int pixelMorex, pixelMorey;
	unsigned int currentDecx, currentDecy;
	unsigned int maxScrollx, maxScrolly;
	unsigned int pixelessx, pixelessy;
	unsigned int morex, morey;
	unsigned int tiledrawx ,tiledrawy;
	
	SDL_Surface *tileset;
	struct anim ** Animate;
	unsigned int tileNumber;
	unsigned int *array;
};

struct gdl_data
{   // current SDL clip limit
    SDL_Rect currentClipLimit;
    
    // pictures/anims/maps arrays
    SDL_Surface ** loadedSurfaces;
    int loadedSurfacesNb;
    struct anim ** loadedAnims;
    int loadedAnimsNb;
    struct map ** loadedMaps;
    int loadedMapsNb;
    
    long long tick; // current tick, filled at each frame
};

extern struct gdl_data gdl;

void initGdl(void);
void closeGdl(void);
void flipGdl(void);
void resetGdl(void);

int getTickBinding(lua_State *L);
int loadImageBinding(lua_State *L);
int drawImageBinding(lua_State *L);
int getImageSizeBinding(lua_State *L);
int drawImagePartBinding(lua_State *L);
int drawImageTileBinding(lua_State *L);
void drawImageTile(int x, int y, int nb, int sx, SDL_Surface *img);
int Animate(struct anim **b);
void playAnim(struct anim **b, int x, int y);
void drawAnim(struct anim **b, int x, int y);
void drawFrm(struct anim **b, int x, int y, int frm);
struct anim * setAnim(SDL_Surface *imgs, unsigned int nb, unsigned int sx, unsigned int frmTime, unsigned int animType);
void resetAnim(struct anim **a);
int newAnimBinding(lua_State *L);
int drawAnimBinding(lua_State *L);
int playAnimBinding(lua_State *L);
void setScreenClip(int x, int y, int w, int h);
int setScreenClipBinding(lua_State *L);
void fullScreenClip(void);
int fullScreenClipBinding(lua_State *L);
void setMapOutZone(struct map*m,struct mapOutZone*out);
struct map * setMap(unsigned int*array,SDL_Surface*tileset,unsigned int tileNumber,unsigned int tileSizex,unsigned int tileSizey,unsigned int sizex,unsigned int sizey,unsigned int scrollx,unsigned int scrolly, struct mapOutZone * out, unsigned int copyArray);
int newMapBinding(lua_State *L);
unsigned int mapScroll(struct map * m, unsigned int way, unsigned int pawa);
int mapScrollBinding(lua_State*L);
unsigned int setMapScroll(struct map * m, unsigned int x, unsigned int y);
int setMapScrollBinding(lua_State*L);
int mapScrollGetWayBinding(lua_State*L);
void setMapAnimatedTile(struct map * m, unsigned int tile, struct anim * Anim);
int setMapAnimatedTileBinding(lua_State*L);
void mapDraw(struct map * m);
int mapDrawBinding(lua_State*L);
int setMapOutZoneBinding(lua_State*L);

extern void setNumber(char *name, lua_Number n);
extern long long mstime(void);

#endif