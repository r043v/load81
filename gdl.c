#include "gdl.h"

struct gdl_data gdl;

void resetGdl(void)
{
    // scroll way define.
    setNumber("SCROLL_LEFT",4);
    setNumber("SCROLL_RIGHT",8);
    setNumber("SCROLL_UP",1);
    setNumber("SCROLL_DOWN",2);
    setNumber("SCROLL_UPLEFT",1|4);
    setNumber("SCROLL_UPRIGHT",1|8);
    setNumber("SCROLL_DOWNLEFT",2|4);
    setNumber("SCROLL_DOWNRIGHT",2|8);
  
    // add get tick routine
    lua_pushcfunction(l81.L,getTickBinding);
    lua_setglobal(l81.L,"getTick");
    
    // add image blit/load
    lua_pushcfunction(l81.L,loadImageBinding);
    lua_setglobal(l81.L,"loadImage");
    lua_pushcfunction(l81.L,drawImageBinding);
    lua_setglobal(l81.L,"drawImage");
    lua_pushcfunction(l81.L,drawImagePartBinding);
    lua_setglobal(l81.L,"drawImagePart");
    lua_pushcfunction(l81.L,drawImageTileBinding);
    lua_setglobal(l81.L,"drawImageTile");
    lua_pushcfunction(l81.L,getImageSizeBinding);
    lua_setglobal(l81.L,"getImageSize");    
    
    // add anims routine
    lua_pushcfunction(l81.L,newAnimBinding);
    lua_setglobal(l81.L,"newAnim");
    lua_pushcfunction(l81.L,playAnimBinding);
    lua_setglobal(l81.L,"playAnim");    
    lua_pushcfunction(l81.L,drawAnimBinding);
    lua_setglobal(l81.L,"drawAnim"); 
    
    // screen clip zone
    lua_pushcfunction(l81.L,setScreenClipBinding);
    lua_setglobal(l81.L,"setScreenClip"); 
    lua_pushcfunction(l81.L,fullScreenClipBinding);
    lua_setglobal(l81.L,"fullScreenClip"); 

    // maps
    lua_pushcfunction(l81.L,newMapBinding);
    lua_setglobal(l81.L,"newMap");
    lua_pushcfunction(l81.L,mapDrawBinding);
    lua_setglobal(l81.L,"drawMap");
    lua_pushcfunction(l81.L,setMapOutZoneBinding);
    lua_setglobal(l81.L,"setMapOutZone");
    lua_pushcfunction(l81.L,mapScrollBinding);
    lua_setglobal(l81.L,"mapScroll");
    lua_pushcfunction(l81.L,setMapScrollBinding);
    lua_setglobal(l81.L,"setMapScroll");    
    lua_pushcfunction(l81.L,mapScrollGetWayBinding);
    lua_setglobal(l81.L,"mapScrollGetWay");
    lua_pushcfunction(l81.L,setMapAnimatedTileBinding);
    lua_setglobal(l81.L,"setMapAnimatedTile");
}

void flipGdl(void)
{    // fill tick var.
    gdl.tick = mstime();
}

void initGdl(void)
{
    gdl.currentClipLimit.x = gdl.currentClipLimit.y = 0;
    gdl.currentClipLimit.w = l81.width-1;
    gdl.currentClipLimit.h = l81.height-1;
    
    gdl.loadedSurfaces = (SDL_Surface**)malloc(MAX_LOADED_SURFACE * sizeof(SDL_Surface*));
    gdl.loadedSurfacesNb = 0;
    gdl.loadedAnims = (struct anim**)malloc(MAX_LOADED_ANIM * sizeof(struct anim*));
    gdl.loadedAnimsNb = 0;
    gdl.loadedMaps = (struct map**)malloc(MAX_LOADED_MAP * sizeof(struct map*));
    gdl.loadedMapsNb = 0;   
}

void closeGdl(void)
{	int n;

	for(n=0 ; n < gdl.loadedSurfacesNb ; n++)
		SDL_FreeSurface(gdl.loadedSurfaces[n]);
	free(gdl.loadedSurfaces);

	for(n=0 ; n < gdl.loadedAnimsNb ; n++)
		free(gdl.loadedAnims[n]);
	free(gdl.loadedAnims);

	for(n=0 ; n < gdl.loadedMapsNb ; n++)
	{	free(gdl.loadedMaps[n]->Animate);
		free(gdl.loadedMaps[n]->array);
		free(gdl.loadedMaps[n]);
	}
	free(gdl.loadedMaps);
}

// return current tick
int getTickBinding(lua_State *L){ lua_pushnumber(L,gdl.tick); return 1; }

int loadImageBinding(lua_State *L)
{	const char *s;
	size_t len;
	SDL_Surface * loaded=NULL;
	int error=0;
	s = lua_tolstring(L,-1,&len); if(!s) error=1;
	
	if(!error)
	{	loaded = SDL_LoadBMP(s);
		if(loaded != NULL) gdl.loadedSurfaces[gdl.loadedSurfacesNb] = loaded; else error=1;
	}
	
	if(error) lua_pushnumber(L,-1); else lua_pushnumber(L,gdl.loadedSurfacesNb++); return 1;
}

int drawImageBinding(lua_State *L)
{	int x,y,n;
	x = lua_tonumber(L,-3); y = lua_tonumber(L,-2); n = lua_tonumber(L,-1);
	if(n < 0 || gdl.loadedSurfacesNb <= n) return 0;
	SDL_Rect dest; dest.x = x; dest.y = y;
	SDL_BlitSurface(gdl.loadedSurfaces[n], NULL, l81.fb->screen, &dest);
	return 0;
}

int getImageSizeBinding(lua_State *L)
{	int n = lua_tonumber(L,-1);
	if(n < 0 || gdl.loadedSurfacesNb <= n) { lua_pushnumber(L,-1); lua_pushnumber(L,-1); }
	else {	lua_pushnumber(L,gdl.loadedSurfaces[n]->w);
		lua_pushnumber(L,gdl.loadedSurfaces[n]->h);
	}	return 2;
}

int drawImagePartBinding(lua_State *L)
{	int x,y,ix,iy,w,h,n;
	x  = lua_tonumber(L,-7); // screen x & y
	y  = lua_tonumber(L,-6);
	ix = lua_tonumber(L,-5); // image x & y
	iy = lua_tonumber(L,-4);
	w  = lua_tonumber(L,-3); // in picture blit size
	h  = lua_tonumber(L,-2);
	n  = lua_tonumber(L,-1); // picture nb
	if(n < 0 || gdl.loadedSurfacesNb <= n) return 0;
	SDL_Rect dest; dest.x = x; dest.y = y; // screen
	SDL_Rect dest2; dest2.x = ix; dest2.y = iy; dest2.w = w; dest2.h = h; // image
	SDL_BlitSurface(gdl.loadedSurfaces[n], &dest2, l81.fb->screen, &dest);
	return 0;
}

// will blit a tile from a tileset, tileset must have all picture sequencial, in a horizontal way
int drawImageTileBinding(lua_State *L)
{	int x,y,nb,sx,sy,n;
	x  = lua_tonumber(L,-6); // screen x & y
	y  = lua_tonumber(L,-5);
	nb = lua_tonumber(L,-4); // tile number
	sx = lua_tonumber(L,-3); // tiles size x & y
	sy = lua_tonumber(L,-2);
	n  = lua_tonumber(L,-1); // tileset picture nb
	if(n < 0 || gdl.loadedSurfacesNb <= n) return 0;
	SDL_Rect dest, src;
	src.x = nb*sx; src.y=0;
	src.h = sy; src.w = sx;
	dest.x = x; dest.y = y;
	SDL_BlitSurface(gdl.loadedSurfaces[n], &src, l81.fb->screen, &dest);
	return 0;
}

// internal use, for anim
void drawImageTile(int x, int y, int nb, int sx, SDL_Surface *img)
{	SDL_Rect dest, src;
	src.x = nb*sx ; src.y = 0 ;
	src.h = img->h ; src.w = sx ;
	dest.x = x; dest.y = y;
	SDL_BlitSurface(img, &src, l81.fb->screen, &dest);
}

int Animate(struct anim **b) // check for time and animate if need, not draw
{   struct anim *a = *b ;
	if(a->lastTime + a->frmTime < gdl.tick)
	{	if(++(a->curentFrm) >= a->frmNumber)
		{	switch(a->animType)
			{ case 0 : a->curentFrm = 0 ; break ; /* loop anim */
			  case 1 : a->curentFrm = a->frmNumber-1 ; break ; /* stop at last frame */
			};
		}
		a->lastTime = gdl.tick; return 1;
	}	return 0;
}

void playAnim(struct anim **b, int x, int y)
{	struct anim *a = *b ;    Animate(b) ;
	drawImageTile(x,y,a->curentFrm,a->frameWidth,a->imgs);
}

void drawAnim(struct anim **b, int x, int y)
{	struct anim *a = *b ;
	drawImageTile(x,y,a->curentFrm,a->frameWidth,a->imgs);
}

void drawFrm(struct anim **b, int x, int y, int frm)
{   struct anim *a = *b ;
    drawImageTile(x,y,frm,a->frameWidth,a->imgs);
}

struct anim * setAnim(SDL_Surface *imgs, unsigned int nb, unsigned int sx, unsigned int frmTime, unsigned int animType)
{	struct anim *a = (struct anim *)malloc(sizeof(struct anim)) ;
	a->imgs = imgs ; a->frmNumber = nb; a->curentFrm = 0; a->lastTime = gdl.tick;
	a->frmTime = frmTime ; a->animType = animType; a->frameWidth = sx;
	return a ;
}

void resetAnim(struct anim **a){ (*a)->curentFrm=0 ; (*a)->lastTime = gdl.tick ; }

int newAnimBinding(lua_State *L)
{	int n,sx,nb,atime,atype, error=0;
	n     = lua_tonumber(L,-5);
	sx    = lua_tonumber(L,-4);
	nb    = lua_tonumber(L,-3);
	atime = lua_tonumber(L,-2);
	atype = lua_tonumber(L,-1);
  
	if(n < 0 || gdl.loadedSurfacesNb <= n) error=1;
	else gdl.loadedAnims[gdl.loadedAnimsNb] = setAnim(gdl.loadedSurfaces[n],nb,sx,atime,atype);
	if(error) lua_pushnumber(L,-1); else lua_pushnumber(L,gdl.loadedAnimsNb++); return 1;
}

int drawAnimBinding(lua_State *L)
{	int x = lua_tonumber(L,-3);
	int y = lua_tonumber(L,-2);
	int n = lua_tonumber(L,-1);
	if(n < 0 || gdl.loadedAnimsNb <= n) return 0;
	drawAnim(&gdl.loadedAnims[n],x,y);
	return 0;
}

int playAnimBinding(lua_State *L)
{	int x = lua_tonumber(L,-3);
	int y = lua_tonumber(L,-2);
	int n = lua_tonumber(L,-1);
	if(n < 0 || gdl.loadedAnimsNb <= n) return 0;
	playAnim(&gdl.loadedAnims[n],x,y);
	return 0;
}

// screen clipping

void setScreenClip(int x, int y, int w, int h)
{	SDL_Rect limit; limit.x = x; limit.y = y; limit.w = w; limit.h = h;
	SDL_SetClipRect(l81.fb->screen,&limit);
}

int setScreenClipBinding(lua_State *L){ setScreenClip(lua_tonumber(L,-4),lua_tonumber(L,-3),lua_tonumber(L,-2),lua_tonumber(L,-1)); return 0; }

void fullScreenClip(void)
{	SDL_Rect limit; limit.x = limit.y = 0; limit.h = l81.fb->height; limit.w = l81.fb->width;
	SDL_SetClipRect(l81.fb->screen,&limit);
}

int fullScreenClipBinding(lua_State *L){ NOTUSED(L); fullScreenClip(); return 0; }

// map.

unsigned int mapComputeDec(unsigned int value)
{	unsigned int v=1, dec=0; while(v<value) { v<<=1; dec++; } return dec;
}

void setMapOutZone(struct map*m,struct mapOutZone*out)
{	m->out.x = out->x;
	m->out.y = out->y;
	m->out.height = out->height;
	m->out.width = out->width;
	m->uncutDrawx = (out->width)>>m->xTileDec;
	m->pixelMorex = (out->width)-(m->uncutDrawx<<m->xTileDec);
	m->uncutDrawy = (out->height)>>m->yTileDec;
	m->pixelMorey = (out->height)-(m->uncutDrawy<<m->yTileDec);
	m->maxScrollx = (m->tileSizex*m->sizeInTilex) - out->width  ;
	m->maxScrolly = (m->tileSizey*m->sizeInTiley) - out->height ;
}

struct map * setMap(unsigned int*array,SDL_Surface*tileset,unsigned int tileNumber,unsigned int tileSizex,unsigned int tileSizey,unsigned int sizex,unsigned int sizey,unsigned int scrollx,unsigned int scrolly, struct mapOutZone * out, unsigned int copyArray)
{	struct map * m = (struct map*)malloc(sizeof(struct map));
	unsigned int size = sizex*sizey ;
	if(!array || !size) return 0 ;

	// copy the array (so we'll can modify it)
	if(copyArray)
	{	m->array = (unsigned int*)malloc(size*sizeof(unsigned int));
		memcpy(m->array,array,size*sizeof(unsigned int));
	} else m->array = array;

	m->tileset = tileset ;
	m->tileNumber = tileNumber ;
	m->tileSizex = tileSizex ;
	m->tileSizey = tileSizey ;
	m->scrollx = scrollx ;
	m->scrolly = scrolly ;
	m->sizeInTilex = sizex;
	m->sizeInTiley = sizey;
//printf("1\n");
	// precompute some usefull value
	m->xTileDec = mapComputeDec(tileSizex); // tile size is multiple of 2, so can use >> or << instead of / or *
	m->yTileDec = mapComputeDec(tileSizey);
//printf("2\n");
	m->sizeInPixelx = sizex<<m->xTileDec;
	m->sizeInPixely = sizey<<m->yTileDec;
	m->firstTileBlitx = (m->scrollx>>m->xTileDec);
	m->firstTileBlity = (m->scrolly>>m->yTileDec);
	m->currentDecx = m->scrollx-(m->firstTileBlitx<<m->xTileDec);
	m->currentDecy = m->scrolly-(m->firstTileBlity<<m->yTileDec);
//printf("3\n");
	if(!out)
	{	struct mapOutZone o;
		o.x = o.y = 0;
		o.height = l81.height-1;
		o.width = l81.width-1;
		setMapOutZone(m,&o);
	} else	setMapOutZone(m,out);
	  
	m->pixelessx = m->pixelMorex+m->currentDecx;
	m->pixelessy = m->pixelMorey+m->currentDecy;
	m->morex = m->pixelessx >= m->tileSizex;
	m->morey = m->pixelessy >= m->tileSizey;

	if(m->morex) m->pixelessx -= m->tileSizex;
	if(m->morey) m->pixelessy -= m->tileSizey;

	m->tiledrawx = m->uncutDrawx+m->morex+(m->pixelessx!=0);
	m->tiledrawy = m->uncutDrawy+m->morey+(m->pixelessy!=0);
//printf("4\n");
	m->Animate = (struct anim**)malloc(tileNumber*sizeof(struct anim*));
	memset(m->Animate,0,tileNumber*sizeof(struct anim*));
//printf("5\n");
	return m;
}

int newMapBinding(lua_State *L)
{	unsigned int tileNumber,tileSizex,tileSizey,sizex,sizey; int tileset;
	tileset    = lua_tonumber(L,2);
	tileNumber = lua_tonumber(L,3);
	tileSizex  = lua_tonumber(L,4);
	tileSizey  = lua_tonumber(L,5);
	sizex      = lua_tonumber(L,6);
	sizey      = lua_tonumber(L,7);
	unsigned int * array = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
	memset(array,0,sizex * sizey * sizeof(unsigned int));
	
	unsigned int * ptr = array;
	lua_pushnil(L); while(lua_next(L, 1) != 0) { *ptr++ = lua_tonumber(L,-1); lua_pop(L, 1); }

	if(tileset < 0 || gdl.loadedSurfacesNb <= tileset) lua_pushnumber(L,-1);
	else
	{	gdl.loadedMaps[gdl.loadedMapsNb] = setMap(array,gdl.loadedSurfaces[tileset],tileNumber,tileSizex,tileSizey,sizex,sizey,0,0,0,0);
		lua_pushnumber(L,gdl.loadedMapsNb++);
	}	return 1;
}

unsigned int mapScroll(struct map * m, unsigned int way, unsigned int pawa)
{	unsigned int cpt, rtn=0;
	if(way&1)	// up
		for(cpt=0;cpt<pawa;cpt++)
		{	if(m->scrolly) m->scrolly--; else rtn |= 1;
		};

	if(way&2)	// down
		for(cpt=0;cpt<pawa;cpt++)
		{	if(m->scrolly < m->maxScrolly) m->scrolly++; else rtn |= 2;
		};

	if(way&4)	// left
		for(cpt=0;cpt<pawa;cpt++)
		{	if(m->scrollx) m->scrollx--; else rtn |= 4;
		};

	if(way&8)	// right
		for(cpt=0;cpt<pawa;cpt++)
		{	if(m->scrollx < m->maxScrollx) m->scrollx++; else rtn |= 8;
		};

	m->firstTileBlitx = (m->scrollx>>m->xTileDec);
	m->firstTileBlity = (m->scrolly>>m->yTileDec);
	m->currentDecx = m->scrollx-(m->firstTileBlitx<<m->xTileDec);
	m->currentDecy = m->scrolly-(m->firstTileBlity<<m->yTileDec);
	
	m->pixelessx = m->pixelMorex+m->currentDecx;
	m->pixelessy = m->pixelMorey+m->currentDecy;
	m->morex = m->pixelessx >= m->tileSizex;
	m->morey = m->pixelessy >= m->tileSizey;

	if(m->morex) m->pixelessx -= m->tileSizex;
	if(m->morey) m->pixelessy -= m->tileSizey;

	m->tiledrawx = m->uncutDrawx+m->morex+(m->pixelessx!=0);
	m->tiledrawy = m->uncutDrawy+m->morey+(m->pixelessy!=0);

	return rtn;
}

int mapScrollBinding(lua_State*L)
{	int mapid,way,pawa;
	mapid = lua_tonumber(L,-3);
	way   = lua_tonumber(L,-2);
	pawa  = lua_tonumber(L,-1);
	if(mapid < 0 || gdl.loadedMapsNb <= mapid) return 0;
	unsigned int result = mapScroll(gdl.loadedMaps[mapid],way,pawa);
	lua_pushnumber(L,((result&1) != 0)); // up
	lua_pushnumber(L,((result&2) != 0)); // down
	lua_pushnumber(L,((result&4) != 0)); // left
	lua_pushnumber(L,((result&8) != 0)); // right
	return 4;
}

unsigned int setMapScroll(struct map * m, unsigned int x, unsigned int y)
{	m->scrollx = x; m->scrolly = y; return mapScroll(m,0,0);
}

int setMapScrollBinding(lua_State*L)
{	int mapid,x,y;
	mapid = lua_tonumber(L,-3);
	x     = lua_tonumber(L,-2);
	y     = lua_tonumber(L,-1);
	if(mapid < 0 || gdl.loadedMapsNb <= mapid) return 0;
	unsigned int result = setMapScroll(gdl.loadedMaps[mapid],x,y);
	lua_pushnumber(L,((result&1) != 0)); // up
	lua_pushnumber(L,((result&2) != 0)); // down
	lua_pushnumber(L,((result&4) != 0)); // left
	lua_pushnumber(L,((result&8) != 0)); // right
	return 4;
}

int mapScrollGetWayBinding(lua_State*L)
{	int v,h;
	//printf("set way %d %d => %d\n",v,h,v|h);
	v = lua_tonumber(L,-2);
	h = lua_tonumber(L,-1);
	lua_pushnumber(L,v|h); // han Oo
	return 1;
}

void setMapAnimatedTile(struct map * m, unsigned int tile, struct anim * Anim)
{	if(!tile || !Anim || tile >= m->tileNumber) return ;
	m->Animate[tile] = Anim;
}

int setMapAnimatedTileBinding(lua_State*L)
{	int map,tile,anim;
	map  = lua_tonumber(L,-3);
	tile = lua_tonumber(L,-2);
	anim = lua_tonumber(L,-1);
	if(map  < 0 || gdl.loadedMapsNb <= map)   return 0;
	if(anim < 0 || gdl.loadedAnimsNb <= anim) return 0;
	setMapAnimatedTile(gdl.loadedMaps[map],tile,gdl.loadedAnims[anim]);
	return 0;
}

// bad draw function, sdl is too limited.

void mapDraw(struct map * m)
{	//SDL_Rect savedClipLimit; memcpy(&savedClipLimit,&l81.currentClipLimit,sizeof(SDL_Rect));
	setScreenClip(m->out.x,m->out.y,m->out.width,m->out.height);
	unsigned int cptx,cpty,blitPosx,blitPosy,tile;

	blitPosx = (m->out.x) - m->currentDecx;
	blitPosy = (m->out.y) - m->currentDecy;
	unsigned int startblitxpos = blitPosx;
	
	for(cpty=0;cpty<m->tiledrawy;cpty++)
	{	for(cptx=0;cptx<m->tiledrawx;cptx++)
		{	tile = m->array[m->firstTileBlitx+cptx+m->sizeInTilex*(m->firstTileBlity+cpty)];
			if(tile && tile < m->tileNumber)
			{	if(!m->Animate[tile])	drawImageTile(blitPosx,blitPosy,tile,m->tileSizex,m->tileset);
				else			playAnim(&m->Animate[tile],blitPosx,blitPosy);
			}
			
			blitPosx += m->tileSizex;
		};
		blitPosy += m->tileSizey;
		blitPosx = startblitxpos;
	};

	fullScreenClip();
	
	//memcpy(&l81.currentClipLimit,&savedClipLimit,sizeof(SDL_Rect));
}

int mapDrawBinding(lua_State*L)
{	//printf("enter draw map !\n");
	int mapid = lua_tonumber(L,-1);
	if(mapid < 0 || gdl.loadedMapsNb <= mapid)
	{	printf("bad map : %d\n",mapid);
		return 0;
	
	}
	//printf("draw map.\n");
	mapDraw(gdl.loadedMaps[mapid]);

	return 0;
}

int setMapOutZoneBinding(lua_State*L)
{	int mapid,x,y,w,h;
	mapid = lua_tonumber(L,-5);
	x = lua_tonumber(L,-4);
	y = lua_tonumber(L,-3);
	w = lua_tonumber(L,-2);
	h = lua_tonumber(L,-1);

	struct mapOutZone out;
	if(mapid < 0 || gdl.loadedMapsNb <= mapid) return 0;
	struct map * m = gdl.loadedMaps[mapid];
	out.x=x; out.y=y; out.height=h; out.width=w;
	setMapOutZone(m,&out);
	return 0;
}