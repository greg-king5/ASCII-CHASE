/*****************************************************************************/
/*                                                                           */
/*                                		                                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2017      Fabrizio Caruso                                  		     */
/*                					                                         */
/*              				                                             */
/* EMail:        Fabrizio_Caruso@hotmail.com                                 */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/
 
#include <stdlib.h>



#include "character.h"
#include "settings.h"

#include "display_macros.h"
#include "sound_macros.h"
#include "text.h"

#if defined(__ATMOS__)
	#include<peekpoke.h>
#endif

extern unsigned long points;
extern unsigned char innerVerticalWallX;
extern unsigned char innerVerticalWallY;
extern unsigned char innerVerticalWallLength;

extern unsigned char XSize;
extern unsigned char YSize;

extern unsigned char ghostCount;
extern unsigned short loop;

extern Image DEAD_GHOST_IMAGE;
extern Image GHOST_IMAGE;
extern Image BOMB_IMAGE;

extern Character ghosts[GHOSTS_NUMBER];
extern Character bombs[BOMBS_NUMBER];

void initializeCharacter(Character* characterPtr, unsigned char x, unsigned char y, unsigned char status, Image * imagePtr)
{
	characterPtr->_x = x;
	characterPtr->_y = y;
	characterPtr->_status = status;
	characterPtr->_imagePtr = imagePtr;
	characterPtr->_moved = 0;
}

void setCharacterPosition(Character* characterPtr, unsigned char x, unsigned char y)
{
	characterPtr->_x = x;
	characterPtr->_y = y;
}

unsigned char isCharacterAtLocation(unsigned char x, unsigned char y, Character * characterPtr)
{
	return(characterPtr->_x==x) && (characterPtr->_y==y);
}

unsigned char areCharctersAtSamePosition(Character* lhs, Character* rhs)
{
	return (lhs->_x==rhs->_x)&&(lhs->_y==rhs->_y);
}


unsigned char wallReached(Character *characterPtr)
{
	return (characterPtr->_x==0)||(characterPtr->_x==XSize-1) || 
		   (characterPtr->_y==0)||(characterPtr->_y==YSize-1);
}

void die(Character * playerPtr)
{
	playerPtr->_status = (unsigned char ) 0;
	playerPtr->_moved = (unsigned char ) 0;
}

unsigned char playerReached(Character* preyPtr)
{
	unsigned char i=0;
	for(;i<GHOSTS_NUMBER;++i)
	{
		if(areCharctersAtSamePosition(&ghosts[i],preyPtr))
			return 1;
	}
	return 0;
}

unsigned char playerReachedBombs(Character* preyPtr)
{
	unsigned char i=0;
	for(;i<BOMBS_NUMBER;++i)
	{
		if(areCharctersAtSamePosition(&bombs[i],preyPtr))
			return 1;
	}
	return 0;
}


void ghostDies(Character * ghostPtr)
{
	EXPLOSION_SOUND();
	ghostPtr->_imagePtr = &DEAD_GHOST_IMAGE;
	die(ghostPtr);
	displayStats();
	--ghostCount;
	printGhostCountStats();
}

void checkBombsVsGhost(Character * ghostPtr)
{
	
	if(ghostPtr->_status && playerReachedBombs(ghostPtr))
	{
		points+=GHOST_VS_BOMBS_BONUS;		
		ghostDies(ghostPtr);
	}
	
}

// void checkBombsVsGhost(Character * ghostPtr)
// {
	
	// if(ghostPtr->_status && playerReachedBombs(ghostPtr))
	// {
		// EXPLOSION_SOUND();
		// ghostPtr->_imagePtr = &DEAD_GHOST_IMAGE;
		// die(ghostPtr);
		// points+=GHOST_VS_BOMBS_BONUS;
		// displayStats();
		// --ghostCount;
		// printGhostCountStats();
	// }
// }
						

void checkBombsVsGhosts(void)
{
	unsigned char i;
	for(i=0;i<GHOSTS_NUMBER;++i)
	  {
		 checkBombsVsGhost(&ghosts[i]);
	  }
}

// TODO: To be replaced with something cleaner
// also used with things different from global bombs
unsigned char safeLocation(unsigned char x, unsigned char y, Character *dangerPtr, unsigned char dangerSize)
{
	char i = 0;
	for(;i<GHOSTS_NUMBER;++i)
	{
		if(isCharacterAtLocation(x,y,&ghosts[i]))
			return 0;
	}
	for(i=0;i<dangerSize;++i)
	{
		if(isCharacterAtLocation(x,y,&dangerPtr[i]))
			return 0;
	}
	return 1;
}


void relocateCharacter(Character * characterPtr, Character *dangerPtr, unsigned char dangerSize)
{
	unsigned char x; // = 0; 
	unsigned char y; // = 0; 
	unsigned char x_offset; 
	unsigned char y_offset;
	unsigned char safe = 0;
	do
	{
		// TODO: This should be separated (at least partially) and moved into display_macros
		x_offset = rand() % 5;
		y_offset = rand() % 5;
		if((x_offset==0) && (y_offset==0))
			continue;
		x = characterPtr->_x -2 + x_offset; 
		y = characterPtr->_y -2 + y_offset;
		
		// TODO: This check should be separated and moved into display_macros
		if((x<2) || (x>XSize-2) || (y<=2) || (y>YSize-2))
			continue;
		
		safe = safeLocation(x,y,dangerPtr, dangerSize);
	} while(!safe);
	characterPtr->_x = x;
	characterPtr->_y = y;
}


unsigned char innerWallReached(Character *characterPtr)
{
	return (characterPtr->_x==innerVerticalWallX) && (characterPtr->_y >= innerVerticalWallY) && (characterPtr->_y<= (innerVerticalWallY + innerVerticalWallLength-1));
}

unsigned char nearInnerWall(Character *characterPtr)
{
	return (characterPtr->_x>=innerVerticalWallX-1) && (characterPtr->_x<=innerVerticalWallX+1) &&
		   (characterPtr->_y >= innerVerticalWallY-1) && (characterPtr->_y<= (innerVerticalWallY + innerVerticalWallLength));
}


// Check whether a moving ghost meets another ghost that is alive
unsigned char ghostsMeetAlive(unsigned char preyIndex)
{
	char i;
	
	for(i=0;i<GHOSTS_NUMBER;++i)
	{
		if((i!=preyIndex) && // not itself
		   ghosts[i]._status && // ghost-ghost collision possible (we assume ghosts[preyindex] is alive and has moved)
		   areCharctersAtSamePosition(&ghosts[i],&ghosts[preyIndex])) 
			return 1;
	}
	return 0;
}

// Check whether an alive ghost meets another dead ghost (transformed into a bomb)
unsigned char ghostsMeetDead(unsigned char preyIndex)
{
	short i;
	
	for(i=0;i<GHOSTS_NUMBER;++i)
	{
		if((i!=preyIndex) && // not itself
		    !(ghosts[i]._status) && // ghost[i] is dead (transformed into a bomb) 
		    areCharctersAtSamePosition(&ghosts[i],&ghosts[preyIndex])) 
			return 1;
	}
	return 0;
}

// void ghostVsGhostCollision(Character * ghostPtr)
// {
	// EXPLOSION_SOUND();
	// ghostPtr->_imagePtr = &DEAD_GHOST_IMAGE;
	// DRAW_GHOST(ghostPtr->_x, ghostPtr->_y, ghostPtr->_imagePtr);
	// die(ghostPtr);
	// points+=GHOST_VS_GHOST_BONUS;
	// displayStats();
	// --ghostCount;
	// printGhostCountStats();
// }


void checkGhostsVsGhosts()
{
	#if defined(FAST_NONEXACT_COLLISION_DECTION) && defined(__ATMOS__)
		unsigned char peek;
		char i; char j;
		if(loop%GHOST_VS_GHOST_COLLISION_LEVEL==0 && loop>GHOST_VS_GHOST_COLLISION_START) // It should only be used for alive vs alive collision
		{
			for(i=0;i<GHOSTS_NUMBER;++i)
			{	
				peek = PEEK(0xBB80+(ghosts[i]._x+2)+(ghosts[i]._y+3)*40);
				if(ghosts[i]._status && ghosts[i]._moved && (peek==GHOST_IMAGE._imageData || peek == DEAD_GHOST_IMAGE._imageData + 128))
				{
					EXPLOSION_SOUND();
					for(j=0;j<GHOSTS_NUMBER;j++)
					{
						if((ghosts[j]._status) && (ghosts[j]._x==ghosts[i]._x) && (ghosts[j]._y==ghosts[i]._y))
						{
							die(&ghosts[j]);
							points+=GHOST_VS_GHOST_BONUS;
							displayStats();
							ghosts[j]._imagePtr = &DEAD_GHOST_IMAGE;
							--ghostCount;
							printGhostCountStats();
						}
					}
				}
			}
		}
		// alive vs dead (ghost transformed into a bomb) collision
		for(i=0;i<GHOSTS_NUMBER;++i)
		{	
			peek = PEEK(0xBB80+(ghosts[i]._x+2)+(ghosts[i]._y+3)*40);
			if(ghosts[i]._status && ghosts[i]._moved && (peek == DEAD_GHOST_IMAGE._imageData + 128))
			{
				// EXPLOSION_SOUND();
				// die(&ghosts[i]);
				// points+=GHOST_VS_GHOST_BONUS;
				// displayStats();
				// ghosts[i]._imagePtr = &DEAD_GHOST_IMAGE;
				// --ghostCount;	
				// printGhostCountStats();
				points+=GHOST_VS_GHOST_BONUS;
				ghostDies(&ghosts[i]);			
			}
		}
	#else
		
	char i;
	
	// Check whether an alive moving ghosts meets another alive ghosts
	if(loop%GHOST_VS_GHOST_COLLISION_LEVEL==0 && loop>GHOST_VS_GHOST_COLLISION_START) // alive vs alive collision possible
	{
		for(i=0;i<GHOSTS_NUMBER;++i)
		{
			if(ghosts[i]._moved)
			{
				if(ghostsMeetAlive(i))
				{
					// EXPLOSION_SOUND();
					// ghosts[i]._imagePtr = &DEAD_GHOST_IMAGE;
					// DRAW_GHOST(ghosts[i]._x, ghosts[i]._y, ghosts[i]._imagePtr);
					// die(&ghosts[i]);
					// points+=GHOST_VS_GHOST_BONUS;
					// displayStats();
					// --ghostCount;
					// printGhostCountStats();
					points+=GHOST_VS_GHOST_BONUS;
					ghostDies(&ghosts[i]);					
				}
			}
		}
	}
	
	// Check whether an alive ghosts meet another dead ghost (including a ghost converted into a bomb in the previous loop)
	for(i=0;i<GHOSTS_NUMBER;++i)
	{
		if(ghosts[i]._status)
		{
			if(ghostsMeetDead(i))
			{
				EXPLOSION_SOUND();
				ghosts[i]._imagePtr = &DEAD_GHOST_IMAGE;
				DRAW_GHOST(ghosts[i]._x, ghosts[i]._y, ghosts[i]._imagePtr);
				die(&ghosts[i]);
				points+=GHOST_VS_GHOST_BONUS;
				displayStats();
				--ghostCount;
				printGhostCountStats();
			}
		}
	}
	#endif
}
