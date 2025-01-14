/*
 * Configuration routines.
 *
 * 19971219 support for mame.cfg by Valerio Verrando
 * 19980402 moved out of msdos.c (N.S.), generalized routines (BW)
 * 19980917 added a "-cheatfile" option (misc) in MAME.CFG      JCK
 */

#include "driver.h"
#include <ctype.h>
#include "allegro.h"

/* from video.c */
extern int frameskip,autoframeskip;
extern int video_sync, wait_vsync;
extern int use_dirty;
extern int skiplines, skipcolumns;
extern float osd_gamma_correction;
extern int gfx_width, gfx_height;

/* from sound.c */
extern int soundcard, usestereo, attenuation;

/* from input.c */
extern int use_mouse, joystick;

/* from cheat.c */
extern char *cheatfile;

/* from datafile.c */
extern char *history_filename,*mameinfo_filename;

/* from fileio.c */
void decompose_rom_sample_path (char *rompath, char *samplepath);
extern char *nvdir, *hidir, *cfgdir, *inpdir, *stadir, *memcarddir;
extern char *artworkdir, *screenshotdir, *alternate_name;

extern char *cheatdir;

extern char *mdir;

/*from video.c flag for 15.75KHz modes (req. for 15.75KHz Arcade Monitor Modes)*/
extern int arcade_mode;

static int mame_argc;
static char **mame_argv;
static int game;
char *rompath, *samplepath;

int underclock_sound=0;
int underclock_cpu=0;
int fast_sound=0;

/* from minimal.c */
extern int rotate_controls;
extern int rotate_buttons;
extern int ror;

static struct { char *name; int id; } joy_table[] =
{
	{ "none",               JOY_TYPE_NONE },
	{ "auto",               JOY_TYPE_AUTODETECT },
	{ "standard",           JOY_TYPE_STANDARD },
	{ "dual",               JOY_TYPE_2PADS },
	{ "4button",            JOY_TYPE_4BUTTON },
	{ "6button",            JOY_TYPE_6BUTTON },
	{ "8button",            JOY_TYPE_8BUTTON },
	{ "fspro",              JOY_TYPE_FSPRO },
	{ "wingex",             JOY_TYPE_WINGEX },
	{ "sidewinder",         JOY_TYPE_SIDEWINDER },
	{ "gamepadpro",         JOY_TYPE_GAMEPAD_PRO },
	{ "grip",               JOY_TYPE_GRIP },
	{ "grip4",              JOY_TYPE_GRIP4 },
	{ "sneslpt1",           JOY_TYPE_SNESPAD_LPT1 },
	{ "sneslpt2",           JOY_TYPE_SNESPAD_LPT2 },
	{ "sneslpt3",           JOY_TYPE_SNESPAD_LPT3 },
	{ "psxlpt1",            JOY_TYPE_PSXPAD_LPT1 },
	{ "psxlpt2",            JOY_TYPE_PSXPAD_LPT2 },
	{ "psxlpt3",            JOY_TYPE_PSXPAD_LPT3 },
	{ "n64lpt1",            JOY_TYPE_N64PAD_LPT1 },
	{ "n64lpt2",            JOY_TYPE_N64PAD_LPT2 },
	{ "n64lpt3",            JOY_TYPE_N64PAD_LPT3 },
	{ "wingwarrior",        JOY_TYPE_WINGWARRIOR },
	{ "segaisa",            JOY_TYPE_IFSEGA_ISA },
	{ "segapci",            JOY_TYPE_IFSEGA_PCI },
	{ 0, 0 }
} ;

/* directory to have correct entries for each dir */
char dir1[512],dir2[512],dir3[512],dir4[512],dir5[512],dir6[512],dir7[512];
	

/*
 * gets some boolean config value.
 * 0 = false, >0 = true, <0 = auto
 * the shortcut can only be used on the commandline
 */
static int get_bool (char *section, char *option, char *shortcut, int def)
{
	int res, i;

	res = def;

	/* check the commandline */
	for (i = 1; i < mame_argc; i++)
	{
		if (mame_argv[i][0] != '-') continue;
		/* look for "-option" */
		if (strcasecmp(&mame_argv[i][1], option) == 0)
			res = 1;
		/* look for "-shortcut" */
		if (shortcut && (strcasecmp(&mame_argv[i][1], shortcut) == 0))
			res = 1;
		/* look for "-nooption" */
		if (strncasecmp(&mame_argv[i][1], "no", 2) == 0)
		{
			if (strcasecmp(&mame_argv[i][3], option) == 0)
				res = 0;
			if (shortcut && (strcasecmp(&mame_argv[i][3], shortcut) == 0))
				res = 0;
		}
		/* look for "-autooption" */
		if (strncasecmp(&mame_argv[i][1], "auto", 4) == 0)
		{
			if (strcasecmp(&mame_argv[i][5], option) == 0)
				res = -1;
			if (shortcut && (strcasecmp(&mame_argv[i][5], shortcut) == 0))
				res = -1;
		}
	}
	return res;
}

static int get_int (char *section, char *option, char *shortcut, int def)
{
	int res,i;

	res = def;

	/* get it from the commandline */
	for (i = 1; i < mame_argc; i++)
	{
		if (mame_argv[i][0] != '-')
			continue;
		if ((strcasecmp(&mame_argv[i][1], option) == 0) ||
			(shortcut && (strcasecmp(&mame_argv[i][1], shortcut ) == 0)))
		{
			i++;
			if (i < mame_argc) res = atoi (mame_argv[i]);
		}
	}
	return res;
}

static float get_float (char *section, char *option, char *shortcut, float def)
{
	int i;
	float res;

	res = def;

	/* get it from the commandline */
	for (i = 1; i < mame_argc; i++)
	{
		if (mame_argv[i][0] != '-')
			continue;
		if ((strcasecmp(&mame_argv[i][1], option) == 0) ||
			(shortcut && (strcasecmp(&mame_argv[i][1], shortcut ) == 0)))
		{
			i++;
			if (i < mame_argc) res = atof (mame_argv[i]);
		}
	}
	return res;
}

static char *get_string (char *section, char *option, char *shortcut, char *def)
{
	char *res;
	int i;

	res = def;

	/* get it from the commandline */
	for (i = 1; i < mame_argc; i++)
	{
		if (mame_argv[i][0] != '-')
			continue;

		if ((strcasecmp(&mame_argv[i][1], option) == 0) ||
			(shortcut && (strcasecmp(&mame_argv[i][1], shortcut)  == 0)))
		{
			i++;
			if (i < mame_argc) res = mame_argv[i];
#ifdef _GCW0_
			if (res[0] == '-') res[0] = '/'; // Stupid bug, don't know how to fix it
#endif
		}
	}
	return res;
}

void get_rom_sample_path (int argc, char **argv, int game_index)
{
	int i;

	alternate_name = 0;
	mame_argc = argc;
	mame_argv = argv;
	game = game_index;

	rompath    = get_string ("directory", "rompath",    NULL, ".;roms");
	samplepath = get_string ("directory", "samplepath", NULL, ".;samples");

	/* handle '-romdir' hack. We should get rid of this BW */
	alternate_name = 0;
	for (i = 1; i < argc; i++)
	{
		if (strcasecmp (argv[i], "-romdir") == 0)
		{
			i++;
			if (i < argc) alternate_name = argv[i];
		}
	}

	/* decompose paths into components (handled by fileio.c) */
	decompose_rom_sample_path (rompath, samplepath);
}

/* for playback of .inp files */
void init_inpdir(void)
{
    inpdir = get_string ("directory", "inp",     NULL, "inp");
}

void parse_cmdline (int argc, char **argv, int game_index)
{
	static float f_beam, f_flicker;
	char *resolution;
	char *joyname;
	char tmpres[10];
	int i;
	char *tmpstr;
	
	mame_argc = argc;
	mame_argv = argv;
	game = game_index;

	/* read graphic configuration */
	options.use_artwork = get_bool   ("config", "artwork",	NULL,  1);
	options.use_samples = get_bool   ("config", "samples",	NULL,  1);
	video_sync  = get_bool   ("config", "vsync",        NULL,  0);
	wait_vsync  = get_bool   ("config", "waitvsync",    NULL,  0);
	use_dirty	= get_bool	 ("config", "dirty",	NULL,	-1);
	options.antialias   = get_bool   ("config", "antialias",    NULL,  1);
	options.translucency = get_bool    ("config", "translucency", NULL, 1);

	tmpstr             = get_string ("config", "depth", NULL, "auto");
	options.color_depth = atoi(tmpstr);
	if (options.color_depth != 8 && options.color_depth != 16) options.color_depth = 0;	/* auto */

	skiplines   = get_int    ("config", "skiplines",    NULL, 0);
	skipcolumns = get_int    ("config", "skipcolumns",  NULL, 0);
	f_beam      = get_float  ("config", "beam",         NULL, 1.0);
	if (f_beam < 1.0) f_beam = 1.0;
	if (f_beam > 16.0) f_beam = 16.0;
	f_flicker   = get_float  ("config", "flicker",      NULL, 0.0);
	if (f_flicker < 0.0) f_flicker = 0.0;
	if (f_flicker > 100.0) f_flicker = 100.0;
	osd_gamma_correction = get_float ("config", "gamma",   NULL, 1.0);
	if (osd_gamma_correction < 0.5) osd_gamma_correction = 0.5;
	if (osd_gamma_correction > 2.0) osd_gamma_correction = 2.0;

	tmpstr = get_string ("config", "frameskip", "fs", "auto");
	if (!strcasecmp(tmpstr,"auto"))
	{
		frameskip = 0;
		autoframeskip = 1;
	}
	else
	{
		frameskip = atoi(tmpstr);
		autoframeskip = 0;
	}
	options.norotate  = get_bool ("config", "norotate",  NULL, 0);
	options.ror       = get_bool ("config", "ror",       NULL, 0);
	options.rol       = get_bool ("config", "rol",       NULL, 0);
	options.flipx     = get_bool ("config", "flipx",     NULL, 0);
	options.flipy     = get_bool ("config", "flipy",     NULL, 0);

	/* read sound configuration */
	soundcard           = get_int  ("config", "soundcard",  NULL, -1);
	options.use_emulated_ym3812 = !get_bool ("config", "ym3812opl",  NULL,  0);
	options.samplerate = get_int  ("config", "samplerate", "sr", 22050);
	if (options.samplerate < 5000) options.samplerate = 5000;
	if (options.samplerate > 44100) options.samplerate = 44100;
	usestereo           = get_bool ("config", "stereo",  NULL,  0);
	attenuation         = get_int  ("config", "volume",  NULL,  0);
	if (attenuation < -32) attenuation = -32;
	if (attenuation > 0) attenuation = 0;

	/* read input configuration */
	use_mouse = get_bool   ("config", "mouse",   NULL,  1);
	joyname   = get_string ("config", "joystick", "joy", "standard");

	/* misc configuration */
	options.cheat      = get_bool ("config", "cheat", NULL, 0);
	options.mame_debug = get_bool ("config", "debug", NULL, 0);
	options.ui_restart = get_bool ("config", "restart", NULL, 0);

	cheatfile  = get_string ("config", "cheatfile", "cf", "cheat.dat");

 	history_filename  = get_string ("config", "historyfile", NULL, "history.dat");    /* JCK 980917 */

	mameinfo_filename  = get_string ("config", "mameinfofile", NULL, "mameinfo.dat");    /* JCK 980917 */

	/* get resolution */
	resolution  = get_string ("config", "resolution", NULL, "auto");

	/* set default subdirectories */
	mdir       = get_string ("directory", "mamepath",   NULL, ".");
	
	sprintf(dir1,"%s/nvram",mdir);   nvdir      = get_string ("directory", "nvram",   NULL, dir1);
	sprintf(dir2,"%s/hi",mdir);      hidir      = get_string ("directory", "hi",      NULL, dir2);
	sprintf(dir3,"%s/cfg",mdir);     cfgdir     = get_string ("directory", "cfg",     NULL, dir3);
	sprintf(dir4,"%s/snap",mdir);    screenshotdir = get_string ("directory", "snap",     NULL, dir4);
	sprintf(dir5,"%s/memcard",mdir); memcarddir = get_string ("directory", "memcard", NULL, dir5);
	sprintf(dir6,"%s/sta",mdir);     stadir     = get_string ("directory", "sta",     NULL, dir6);
	sprintf(dir7,"%s/artwork",mdir); artworkdir = get_string ("directory", "artwork", NULL, dir7);

	cheatdir = get_string ("directory", "cheat", NULL, ".");

	logerror("cheatfile = %s - cheatdir = %s\n",cheatfile,cheatdir);

	tmpstr = get_string ("config", "language", NULL, "english");
	options.language_file = osd_fopen(0,tmpstr,OSD_FILETYPE_LANGUAGE,0);

	/* this is handled externally cause the audit stuff needs it, too */
	get_rom_sample_path (argc, argv, game_index);

	/* process some parameters */
	options.beam = (int)(f_beam * 0x00010000);
	if (options.beam < 0x00010000)
		options.beam = 0x00010000;
	if (options.beam > 0x00100000)
		options.beam = 0x00100000;

	options.flicker = (int)(f_flicker * 2.55);
	if (options.flicker < 0)
		options.flicker = 0;
	if (options.flicker > 255)
		options.flicker = 255;

	/* any option that starts with a digit is taken as a resolution option */
	/* this is to handle the old "-wxh" commandline option. */
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && isdigit(argv[i][1]) &&
				(strstr(argv[i],"x") || strstr(argv[i],"X")))
			resolution = &argv[i][1];
	}

	/* break up resolution into gfx_width and gfx_height */
#ifdef _GCW0_
	extern int odx_device_scale(bool determine);
#endif
	gfx_height = gfx_width = 0;
	if (strcasecmp (resolution, "auto") != 0)
	{
		char *tmp;
		strncpy (tmpres, resolution, 10);
		tmp = strtok (tmpres, "xX");
		gfx_width = atoi (tmp);
		tmp = strtok (0, "xX");
		if (tmp)
			gfx_height = atoi (tmp);

		options.vector_width = gfx_width;
		options.vector_height = gfx_height;
	}
#ifdef _GCW0_
	else if (odx_device_scale(true) == 2) {
		options.vector_width = 640;
		options.vector_height = 480;	    
	}
#endif

	/* convert joystick name into an Allegro-compliant joystick signature */
	joystick = -2; /* default to invalid value */

	for (i = 0; joy_table[i].name != NULL; i++)
	{
		if (strcasecmp (joy_table[i].name, joyname) == 0)
		{
			joystick = joy_table[i].id;
			logerror("using joystick %s = %08x\n",
						joyname,joy_table[i].id);
			break;
		}
	}

	if (joystick == -2)
	{
		logerror("%s is not a valid entry for a joystick\n",
					joyname);
		joystick = JOY_TYPE_NONE;
	}

	/* Underclock settings */
	underclock_sound = get_int ("config", "uclocks",   NULL, 0);
	underclock_cpu   = get_int ("config", "uclock",    NULL, 0);

	/* Fast sound setting */
	fast_sound       = get_bool("config", "fastsound", NULL, 0);

	/* Rotate controls */
	rotate_controls       = get_bool("config", "rotatecontrols", NULL, 0);
	ror = options.ror;
	
	/* Rotate buttons */
	rotate_buttons        = get_bool("config", "rotatebuttons", NULL, 0);
}
