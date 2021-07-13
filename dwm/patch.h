static int width_awesomebar(Bar *bar, BarArg *a);
static int draw_awesomebar(Bar *bar, BarArg *a);
static int click_awesomebar(Bar *bar, Arg *arg, BarArg *a);

static int getstatusbarpid();
static void sigstatusbar(const Arg *arg);

enum {
	INDICATOR_NONE,
	INDICATOR_TOP_LEFT_SQUARE,
	INDICATOR_TOP_LEFT_LARGER_SQUARE,
	INDICATOR_TOP_BAR,
	INDICATOR_TOP_BAR_SLIM,
	INDICATOR_BOTTOM_BAR,
	INDICATOR_BOTTOM_BAR_SLIM,
	INDICATOR_BOX,
	INDICATOR_BOX_WIDER,
	INDICATOR_BOX_FULL,
	INDICATOR_CLIENT_DOTS,
	INDICATOR_RIGHT_TAGS,
	INDICATOR_PLUS,
	INDICATOR_PLUS_AND_SQUARE,
	INDICATOR_PLUS_AND_LARGER_SQUARE,
};

static void drawindicator(Monitor *m, Client *c, unsigned int occ, int x, int y, int w, int h, unsigned int tag, int filled, int invert, int type);
static void drawstateindicator(Monitor *m, Client *c, unsigned int occ, int x, int y, int w, int h, unsigned int tag, int filled, int invert);

static int width_ltsymbol(Bar *bar, BarArg *a);
static int draw_ltsymbol(Bar *bar, BarArg *a);
static int click_ltsymbol(Bar *bar, Arg *arg, BarArg *a);

static int width_status(Bar *bar, BarArg *a);
static int draw_status(Bar *bar, BarArg *a);
static int click_status(Bar *bar, Arg *arg, BarArg *a);

static int width_status2d(Bar *bar, BarArg *a);
static int draw_status2d(Bar *bar, BarArg *a);
static int drawstatusbar(BarArg *a, char *text);
static int status2dtextlength(char *stext);

static int width_stbutton(Bar *bar, BarArg *a);
static int draw_stbutton(Bar *bar, BarArg *a);
static int click_stbutton(Bar *bar, Arg *arg, BarArg *a);

static int click_statuscmd(Bar *bar, Arg *arg, BarArg *a);
static int click_statuscmd_text(Arg *arg, int rel_x, char *text);
static void copyvalidchars(char *text, char *rawtext);

typedef struct {
	const char *cmd;
	int id;
} StatusCmd;

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0

/* XEMBED messages */
#define XEMBED_EMBEDDED_NOTIFY      0
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_FOCUS_IN             4
#define XEMBED_MODALITY_ON         10

#define XEMBED_MAPPED              (1 << 0)
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_WINDOW_DEACTIVATE    2

#define VERSION_MAJOR               0
#define VERSION_MINOR               0
#define XEMBED_EMBEDDED_VERSION (VERSION_MAJOR << 16) | VERSION_MINOR

/* enums */
enum { Manager, Xembed, XembedInfo, XLast }; /* Xembed atoms */

typedef struct Systray Systray;
struct Systray {
	Window win;
	Client *icons;
	Bar *bar;
	int h;
};

/* bar integration */
static int width_systray(Bar *bar, BarArg *a);
static int draw_systray(Bar *bar, BarArg *a);
static int click_systray(Bar *bar, Arg *arg, BarArg *a);

/* function declarations */
static Atom getatomprop(Client *c, Atom prop);
static void removesystrayicon(Client *i);
static void resizerequest(XEvent *e);
static void updatesystrayicongeom(Client *i, int w, int h);
static void updatesystrayiconstate(Client *i, XPropertyEvent *ev);
static Client *wintosystrayicon(Window w);


enum {
	DEFAULT_TAGS,
	ALTERNATIVE_TAGS,
	ALT_TAGS_DECORATION,
};

static char * tagicon(Monitor *m, int tag);

static int width_tags(Bar *bar, BarArg *a);
static int draw_tags(Bar *bar, BarArg *a);
static int click_tags(Bar *bar, Arg *arg, BarArg *a);

static int width_wintitle(Bar *bar, BarArg *a);
static int draw_wintitle(Bar *bar, BarArg *a);
static int click_wintitle(Bar *bar, Arg *arg, BarArg *a);

static void hide(Client *c);
static void show(Client *c);
static void togglewin(const Arg *arg);
static Client * prevtiled(Client *c);
static void showhideclient(const Arg *arg);

static void keyrelease(XEvent *e);
static void combotag(const Arg *arg);
static void comboview(const Arg *arg);

static void autostart_exec(void);

static void cyclelayout(const Arg *arg);
static void col(Monitor *);

static void deck(Monitor *m);

static void flextile(Monitor *m);
static void getfactsforrange(Monitor *m, int an, int ai, int size, int *rest, float *fact);
static void mirrorlayout(const Arg *arg);
static void rotatelayoutaxis(const Arg *arg);
static void incnstack(const Arg *arg);

/* Symbol handlers */
static void setflexsymbols(Monitor *m, unsigned int n);
static void monoclesymbols(Monitor *m, unsigned int n);
static void decksymbols(Monitor *m, unsigned int n);

/* Layout split */
static void layout_no_split(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_vertical(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_horizontal(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_vertical_dual_stack(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_horizontal_dual_stack(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_centered_vertical(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_centered_horizontal(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_floating_master(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_vertical_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_horizontal_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_vertical_dual_stack_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_horizontal_dual_stack_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_centered_vertical_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_split_centered_horizontal_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);
static void layout_floating_master_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n);

/* Layout tile arrangements */
static void arrange_left_to_right(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_top_to_bottom(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_monocle(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_gapplessgrid(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_gapplessgrid_alt1(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_gapplessgrid_alt2(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_gridmode(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_horizgrid(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_dwindle(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_spiral(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);
static void arrange_tatami(Monitor *m, int ax, int ay, int ah, int aw, int ih, int iv, int n, int an, int ai);

/* Named flextile constants */
enum {
	LAYOUT,       // controls overall layout arrangement / split
	MASTER,       // indicates the tile arrangement for the master area
	STACK,        // indicates the tile arrangement for the stack area
	STACK2,       // indicates the tile arrangement for the secondary stack area
	LTAXIS_LAST,
};

/* Layout arrangements */
enum {
	NO_SPLIT,
	SPLIT_VERTICAL,                    // master stack vertical split
	SPLIT_HORIZONTAL,                  // master stack horizontal split
	SPLIT_CENTERED_VERTICAL,           // centered master vertical split
	SPLIT_CENTERED_HORIZONTAL,         // centered master horizontal split
	SPLIT_VERTICAL_DUAL_STACK,         // master stack vertical split with dual stack
	SPLIT_HORIZONTAL_DUAL_STACK,       // master stack vertical split with dual stack
	FLOATING_MASTER,                   // (fake) floating master
	SPLIT_VERTICAL_FIXED,              // master stack vertical fixed split
	SPLIT_HORIZONTAL_FIXED,            // master stack horizontal fixed split
	SPLIT_CENTERED_VERTICAL_FIXED,     // centered master vertical fixed split
	SPLIT_CENTERED_HORIZONTAL_FIXED,   // centered master horizontal fixed split
	SPLIT_VERTICAL_DUAL_STACK_FIXED,   // master stack vertical split with fixed dual stack
	SPLIT_HORIZONTAL_DUAL_STACK_FIXED, // master stack vertical split with fixed dual stack
	FLOATING_MASTER_FIXED,             // (fake) fixed floating master
	LAYOUT_LAST,
};

static char layoutsymb[] = {
	32,  // " ",
	124, // "|",
	61,  // "=",
	94,  // "^",
	126, // "~",
	58,  // ":",
	59,  // ";",
	43,  // "+",
	124, // "￤",
	61,  // "=",
	94,  // "^",
	126, // "~",
	58,  // ":",
	59,  // ";",
	43,  // "+",
};

/* Tile arrangements */
enum {
	TOP_TO_BOTTOM,     // clients are arranged vertically
	LEFT_TO_RIGHT,     // clients are arranged horizontally
	MONOCLE,           // clients are arranged in deck / monocle mode
	GAPPLESSGRID,      // clients are arranged in a gappless grid (original formula)
	GAPPLESSGRID_ALT1, // clients are arranged in a gappless grid (alt. 1, fills rows first)
	GAPPLESSGRID_ALT2, // clients are arranged in a gappless grid (alt. 2, fills columns first)
	GRIDMODE,          // clients are arranged in a grid
	HORIZGRID,         // clients are arranged in a horizontal grid
	DWINDLE,           // clients are arranged in fibonacci dwindle mode
	SPIRAL,            // clients are arranged in fibonacci spiral mode
	TATAMI,            // clients are arranged as tatami mats
	AXIS_LAST,
};

static char tilesymb[] = {
	61,  // "=",
	124, // "|",
	68,  // "D",
	71,  // "G",
	49,  // "1",
	50,  // "2"
	35,  // "#",
	126, // "~",
	92,  // "\\",
	64,  // "@",
	84,  // "T",
};

static void monocle(Monitor *m);

static void tile(Monitor *);

static void pertagview(const Arg *arg);

static void sighup(int unused);
static void sigterm(int unused);

typedef struct {
       const char *name;
       const void *cmd;
} Sp;

static void removescratch(const Arg *arg);
static void setscratch(const Arg *arg);
static void togglescratch(const Arg *arg);

/* Key binding functions */
static void defaultgaps(const Arg *arg);
static void incrgaps(const Arg *arg);
static void incrigaps(const Arg *arg);
static void incrogaps(const Arg *arg);
static void incrohgaps(const Arg *arg);
static void incrovgaps(const Arg *arg);
static void incrihgaps(const Arg *arg);
static void incrivgaps(const Arg *arg);
static void togglegaps(const Arg *arg);

/* Internals */
static void getgaps(Monitor *m, int *oh, int *ov, int *ih, int *iv, unsigned int *nc);
static void setgaps(int oh, int ov, int ih, int iv);
