#ifndef _VIC_SOUNDS
#define _VIC_SOUNDS

	void _ping_sound(unsigned char freq);
	#define PING_SOUND() _ping_sound(0xF0);	
	#define TOCK_SOUND() _ping_sound(0x08);
	#define TICK_SOUND() _ping_sound(0x60);
	
	void _explosion_sound(unsigned char freq, unsigned char vol);
	#define SHOOT_SOUND() _explosion_sound(0xF0, 0x0A);
	#define EXPLOSION_SOUND() _explosion_sound(0x80, 0x08);
	
	void ZAP_SOUND();	

#endif // _VIC_SOUNDS