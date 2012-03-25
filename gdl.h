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

#define GET_TILE_FROM_MAP 0
#define GET_TILE_FROM_ZONE 1
#define GET_TILE_FROM_SCREEN 2

#include "load81.h"

#define u32 unsigned int

struct anim
{	long long lastTime  ;
	u32 frmTime   ;
	u32 curentFrm ;
	u32 frmNumber ;
	u32 animType  ;
	u32 frameWidth;
	SDL_Surface * imgs;
};

struct mapOutZone
{	u32 height,width,x,y;
};

struct map
{	struct mapOutZone out;
	u32 scrollx, scrolly;
	u32 tileSizex, tileSizey;
	u32 xTileDec, yTileDec;
	u32 sizeInTilex, sizeInTiley;
	u32 sizeInPixelx, sizeInPixely;
	u32 firstTileBlitx, firstTileBlity;
	u32 uncutDrawx, uncutDrawy;
	u32 pixelMorex, pixelMorey;
	u32 currentDecx, currentDecy;
	u32 maxScrollx, maxScrolly;
	u32 pixelessx, pixelessy;
	u32 morex, morey;
	u32 tiledrawx ,tiledrawy;
	
	SDL_Surface *tileset;
	struct anim ** Animate;
	u32 tileNumber;
	u32 *array;
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
struct anim * setAnim(SDL_Surface *imgs, u32 nb, u32 sx, u32 frmTime, u32 animType);
void resetAnim(struct anim **a);
int newAnimBinding(lua_State *L);
int drawAnimBinding(lua_State *L);
int playAnimBinding(lua_State *L);
void setScreenClip(int x, int y, int w, int h);
int setScreenClipBinding(lua_State *L);
void fullScreenClip(void);
int fullScreenClipBinding(lua_State *L);
void setMapOutZone(struct map*m,struct mapOutZone*out);
struct map * setMap(u32*array,SDL_Surface*tileset,u32 tileNumber,u32 tileSizex,u32 tileSizey,u32 sizex,u32 sizey,u32 scrollx,u32 scrolly, struct mapOutZone * out, u32 copyArray);
int newMapBinding(lua_State *L);
u32 mapScroll(struct map * m, u32 way, u32 pawa);
int mapScrollBinding(lua_State*L);
u32 setMapScroll(struct map * m, u32 x, u32 y);
int setMapScrollBinding(lua_State*L);
int mapScrollGetWayBinding(lua_State*L);
void setMapAnimatedTile(struct map * m, u32 tile, struct anim * Anim);
int setMapAnimatedTileBinding(lua_State*L);
void mapDraw(struct map * m);
int mapDrawBinding(lua_State*L);
int setMapOutZoneBinding(lua_State*L);
int getTileBinding(lua_State*L);
int setTileBinding(lua_State*L);
void luaPush(const char*, void*);

extern void setNumber(char *name, lua_Number n);
extern long long mstime(void);

#endif