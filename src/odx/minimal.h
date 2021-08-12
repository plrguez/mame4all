/*

  GP2X minimal library v0.A by rlyeh, (c) 2005. emulnation.info@rlyeh (swap it!)

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)

  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.

*/
#ifndef __MINIMAL_H__
#define __MINIMAL_H__

#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>

#include <SDL/SDL.h>

extern bool determine_device_scale;
extern int device_scale;
#define ODX_SCREEN_WIDTH (320 * device_scale)
#define ODX_SCREEN_HEIGHT (240 * device_scale)

#define odx_video_color8(C,R,G,B)  (odx_palette_rgb[C] = ((((R)&0xF8)<<8)|(((G)&0xFC)<<3)|(((B)&0xF8)>>3)))
#define odx_video_color16(R,G,B,A) ((((R)&0xF8)<<8)|(((G)&0xFC)<<3)|(((B)&0xF8)>>3))
#define odx_video_getr16(C) (((C)>>8)&0xF8)
#define odx_video_getg16(C) (((C)>>3)&0xFC)
#define odx_video_getb16(C) (((C)<<3)&0xF8)

enum  { OD_UP=1<<0,         OD_LEFT=1<<1,       OD_DOWN=1<<2,  OD_RIGHT=1<<3,
        OD_START=1<<4,  OD_SELECT=1<<5,    OD_L=1<<6,    OD_R=1<<7,
        OD_A=1<<8,       OD_B=1<<9,        OD_X=1<<10,    OD_Y=1<<11,

	/* Virtual keys * SELECT + 'button' */
        OD_SEL_START=1<<12,  OD_SEL_SELECT=1<<13,    OD_SEL_L=1<<14,    OD_SEL_R=1<<15,
        OD_SEL_A=1<<16,       OD_SEL_B=1<<17,        OD_SEL_X=1<<18,    OD_SEL_Y=1<<19
#ifdef _GCW0_
  /* Power button */
        ,OD_POWER=1<<20,OD_L2=1<<21,OD_R2=1<<22,OD_L3=1<<23,OD_R3=1<<24
#endif
};

#define OD_KEY_MAX 16

enum e_video_scaling {
    SCALE_INIT=0,
    SCALE_NORMAL=0,
    SCALE_HORIZONTAL,
    SCALE_BEST,
    SCALE_FAST,
    SCALE_HALFSIZE,
    ROTATE,
    ROTATE_SCALE_HORIZONTAL,
    ROTATE_SCALE_BEST,
    ROTATE_SCALE_FAST,
    ROTATE_SCALE_HALFSIZE,
    ROTATE_SCALE_HARDWARE,
    SCALE_HARDWARE,
    SCALE_END=SCALE_HARDWARE,
};

enum e_rotate_directions {
    ROTATE_LEFT,
    ROTATE_RIGHT,
};

enum e_flip_xy {
    FLIP_OFF,
    FLIP_ON,
};

enum e_flip_xy_screen {
    FLIP_NONE,
    FLIP_XY,
    FLIP_X,
    FLIP_Y,
};

enum e_keep_aspect {
    VIDEO_FULLSCREEN,
    VIDEO_KEEP_ASPECT,
    VIDEO_INTEGER_SCALING,
};

enum e_video_filter {
    VIDEO_FILTER_BICUBIC,
    VIDEO_FILTER_BILINEAR,
    VIDEO_FILTER_NEAREST,
};

enum e_game_options {
    GO_VIDEO_DEPTH,
    GO_VIDEO_ASPECT,
    GO_KEEP_ASPECT,
    GO_VIDEO_FILTER,
    GO_BICUBIC_LEVEL,
    GO_VIDEO_ROTATION,
    GO_BUTTONS_ROTATION,
    GO_FLIP_XY,
    GO_VIDEO_SYNC,
    GO_FRAME_SKIP,
    GO_SOUND,
    GO_CPU_CLOCK,
    GO_AUDIO_CLOCK,
    GO_FAST_CORES,
    GO_CHEATS,
    GO_LAST,
};

extern SDL_Surface 				*layer,*video;
//extern SDL_Surface 				*video;
extern unsigned char			*od_screen8;
extern unsigned short			*od_screen16;

extern volatile unsigned short	odx_palette[512];
extern unsigned short			odx_palette_rgb[256];

extern int						odx_clock;

extern unsigned int				odx_sound_rate;
extern int						odx_sound_stereo;

extern int						rotate_controls;
extern int						rotate_buttons;

#ifdef _GCW0_
extern SDL_Joystick				*odx_joyanalog;
#endif

extern void odx_video_flip(void);
extern void odx_video_flip_single(void);
extern void odx_video_wait_vsync(void);
extern void odx_video_setpalette(void);

extern unsigned int odx_joystick_read();
extern unsigned int odx_joystick_press();

extern void odx_sound_volume(int vol);

extern void odx_timer_delay(unsigned int ticks);
extern unsigned long odx_timer_read(void);

extern void odx_sound_play(void *buff, int len);
extern void odx_sound_thread_start(void);
extern void odx_sound_thread_stop(void);

#ifdef _GCW0_
extern int odx_is_kmsdrm_640480(void);
extern int odx_device_scale(bool determine);
#endif

extern void odx_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int Hz);
extern void odx_deinit(void);

extern void odx_set_clock(int mhz);
#ifdef _GCW0_
extern void odx_set_video_mode_for_layer(int bpp,int width,int height);
#endif
extern void odx_set_video_mode(int bpp,int width,int height);
extern void odx_clear_video();
extern void odx_clear_video_multibuf();

extern void odx_printf(char* fmt, ...);
extern void odx_printf_init(void);
extern void odx_gamelist_text_out(int x, int y, char *eltexto);
extern void odx_gamelist_text_out_fmt(int x, int y, char* fmt, ...);

extern int abs_x, abs_y, abs_z;

#define odx_video_wait_vsync()  { }
#define odx_video_setpalette()  { }

#endif
