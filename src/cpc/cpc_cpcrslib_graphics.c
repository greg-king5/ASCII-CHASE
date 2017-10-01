/* --------------------------------------------------------------------------------------- */ 
// 
// CROSS CHASE by Fabrizio Caruso
//
// Fabrizio_Caruso@hotmail.com
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.

// Permission is granted to anyone to use this software for non-commercial applications, 
// subject to the following restrictions:

// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in
// a product, an acknowledgment in the product documentation would be
// appreciated but is not required.

// 2. Altered source versions must be plainly marked as such, and must not
// be misrepresented as being the original software.

// 3. This notice may not be removed or altered from any source distribution.
/* --------------------------------------------------------------------------------------- */ 

#include <stdio.h>
#include <conio.h>
#include "cpcrslib.h"

// #define _DRAW 	cputc(image->_imageData);
// #define _DELETE cputc(' '); 

extern unsigned char XSize;

// #define CPC_BLUE 2
// #define CPC_RED 4
// #define CPC_YELLOW 0
// #define CPC_CYAN 3


#define CPC_RED 3
#define CPC_CYAN 4
#define CPC_BLUE 0
#define CPC_YELLOW 2 


#include "../display_macros.h"

extern Image PLAYER_IMAGE;
extern Image GHOST_IMAGE;
extern Image DEAD_GHOST_IMAGE;
extern Image INVINCIBLE_GHOST_IMAGE;
extern Image BOMB_IMAGE;
extern Image POWERUP_IMAGE;
extern Image MISSILE_IMAGE;
extern Image GUN_IMAGE;

#if defined(FULL_GAME)
	extern Image LEFT_ENEMY_MISSILE_IMAGE;
	extern Image RIGHT_ENEMY_MISSILE_IMAGE;

	extern Image BUBBLE_IMAGE;

	extern Image EXTRA_POINTS_IMAGE;
	extern Image EXTRA_LIFE_IMAGE;
	extern Image INVINCIBILITY_IMAGE;	
	
	extern Image BROKEN_WALL_IMAGE;
#endif

Image PLAYER_DOWN;
Image PLAYER_UP;
Image PLAYER_RIGHT;
Image PLAYER_LEFT;

unsigned char full[] =  {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};	
unsigned char empty[] = {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};	
char space_str[2] = {' ', '\0'};

void INIT_GRAPHICS(void)
{
    //cpc_DisableFirmware();          //Now, I don't gonna use any firmware routine so I modify interrupts jump to nothing
    // cpc_ClrScr();                           //fills scr with ink 0
    // cpc_SetMode(1);                         //hardware call to set mode 1	
	
	// WAIT_PRESS();
	// cpc_ClrScr();	
	

	// draw(172,0,172,40);	
	//cpc_DisableFirmware();
	// cpc_SetColour(16,20); //background
	// cpc_SetColour(0,20); //border
	// cpc_SetColour(1,10); //
	
	// cpc_SetMode(0);
	// cpc_SetInkGphStr(0,0);
	// cpc_SetInkGphStr(2,2);
	// cpc_SetInkGphStr(1,8);
	
	// cpc_PrintGphStrXY("GAME;OF;LIFE",50,10);
	// cpc_PrintGphStrXY("ARTABURU;2009",49,19);
	
	// cpc_EnableFirmware();
	
	// WAIT_PRESS();
}

void INIT_IMAGES(void)
{		
	PLAYER_IMAGE._color = CPC_YELLOW;
	INVINCIBLE_GHOST_IMAGE._color = CPC_YELLOW;
	POWERUP_IMAGE._color = CPC_YELLOW;
	GUN_IMAGE._color = CPC_YELLOW;
	BOMB_IMAGE._color = CPC_RED;
	DEAD_GHOST_IMAGE._color = CPC_RED;
		
	GHOST_IMAGE._imageData = 'o'; //0xE1; 
	INVINCIBLE_GHOST_IMAGE._imageData = '+';0xFD; 
	BOMB_IMAGE._imageData = 'X';0xEE;
	
	PLAYER_IMAGE._imageData = '*'; 0xF9;
	PLAYER_DOWN._imageData = '*';0xF8;	
	PLAYER_UP._imageData = '*';0xF8;
	PLAYER_RIGHT._imageData = '*';0xFA;
	PLAYER_LEFT._imageData = '*';0xFB;
	PLAYER_DOWN._color = PLAYER_IMAGE._color;
	PLAYER_UP._color = PLAYER_IMAGE._color;	
	PLAYER_RIGHT._color = PLAYER_IMAGE._color;
	PLAYER_LEFT._color = PLAYER_IMAGE._color;
	
	
	POWERUP_IMAGE._imageData = 'S';//0xE3; 
	GUN_IMAGE._imageData = '!';0xB9; 
	MISSILE_IMAGE._imageData = '.';0x90;
	
	#if defined(CPC_NO_COLOR)
		DEAD_GHOST_IMAGE._imageData = '-';0x9F;
	#else
		DEAD_GHOST_IMAGE._imageData = GHOST_IMAGE._imageData;
	#endif

	GHOST_IMAGE._color = CPC_CYAN;
	MISSILE_IMAGE._color = CPC_CYAN;

	#if defined(FULL_GAME)
		LEFT_ENEMY_MISSILE_IMAGE._imageData = '>';
		LEFT_ENEMY_MISSILE_IMAGE._color = CPC_CYAN;
		RIGHT_ENEMY_MISSILE_IMAGE._imageData = '<';
		RIGHT_ENEMY_MISSILE_IMAGE._color = CPC_CYAN;	
		
		BUBBLE_IMAGE._imageData = 0xEF;//'^';
		BUBBLE_IMAGE._color = CPC_CYAN;
		
		EXTRA_POINTS_IMAGE._imageData = '$';
		
		EXTRA_LIFE_IMAGE._imageData = PLAYER_IMAGE._imageData;
		INVINCIBILITY_IMAGE._imageData = 'V';0x05;//'V';
		
		EXTRA_POINTS_IMAGE._color = CPC_YELLOW;
		EXTRA_LIFE_IMAGE._color = CPC_YELLOW;
		INVINCIBILITY_IMAGE._color = CPC_YELLOW;			
	#endif
}

#if defined(FULL_GAME)
	void DRAW_BROKEN_WALL(unsigned char x, unsigned char y)
	{
		gotoxy((x+1+X_OFFSET),(y+Y_OFFSET)); 
		#if defined(CPC_NO_COLOR)
		#else
			SET_TEXT_COLOR(CPC_CYAN);
		#endif
		cputc('X');
	}
#endif
	
void _draw(unsigned char x, unsigned char y, Image * image) 
{
	char str[2];
	str[0] = image->_imageData;
	str[1] = '\0';
	cpc_PrintGphStrStdXY(image->_color,str,(x+X_OFFSET)*2,(y+Y_OFFSET)*8);	
    // cpc_PrintGphStrXY("A",(x+X_OFFSET)*2,(y+Y_OFFSET)*8);
	//cpc_PutSpXOR(full,8,8,cpc_GetScrAddress(8*(x+X_OFFSET),8*(y+Y_OFFSET)));	

	/*
	gotoxy((x+1+X_OFFSET),(y+Y_OFFSET)); 
	#if defined(CPC_NO_COLOR)
	#else
		SET_TEXT_COLOR(image->_color);
	#endif
	_DRAW
	*/
}

void _delete(unsigned char x, unsigned char y)
{
    cpc_PrintGphStrStdXY(1,space_str,(x+X_OFFSET)*2,(y+Y_OFFSET)*8);	
    // cpc_PrintGphStrXY(" ",(x+X_OFFSET)*2,(y+Y_OFFSET)*8);
//	cpc_PutSpXOR(full,8,8,cpc_GetScrAddress(8*(x+X_OFFSET),8*(y+Y_OFFSET)));	
	
	/*
	gotoxy(x+1+X_OFFSET,y+Y_OFFSET);
	_DELETE
	*/
}

void _blink_draw(unsigned char x, unsigned char y, Image * image, unsigned char *blinkCounter) 
{
	char str[2];	
	// gotoxy((x+1+X_OFFSET),(y+Y_OFFSET)); 
	if(*blinkCounter) 
	{
		#if defined(CPC_NO_COLOR)
		#else
			SET_TEXT_COLOR(image->_color);
		#endif
		str[0] = image->_imageData;
		str[1] = '\0';
		cpc_PrintGphStrStdXY(image->_color,str,(x+X_OFFSET)*2,(y+Y_OFFSET)*8);	
		*blinkCounter=0;
	} 
	else 
	{
		cpc_PrintGphStrStdXY(1,space_str,(x+X_OFFSET)*2,(y+Y_OFFSET)*8);
		*blinkCounter=1;
	}	
}

void DRAW_VERTICAL_LINE(unsigned char x,unsigned char y, unsigned char length)
{ 
	unsigned char i;
	#if defined(CPC_NO_COLOR)
	#else
		SET_TEXT_COLOR(CPC_CYAN);
	#endif	
	for(i=0;i<length;++i)
	{
		gotoxy(x+1+X_OFFSET,y+i+Y_OFFSET);
		cputc('|');
	}
}

void DRAW_HORIZONTAL_LINE(unsigned char x,unsigned char y, unsigned char length)
{
	unsigned char i;
	#if defined(CPC_NO_COLOR)
	#else
		SET_TEXT_COLOR(CPC_CYAN);
	#endif	
	gotoxy(X_OFFSET+1+x,Y_OFFSET+y); 
	for(i=0;i<length;++i)
	{
		cputc('-');
	}
}



