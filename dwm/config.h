/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx = 1; /* border pixel of windows */
static const unsigned int snap = 32; /* snap pixel */
static const unsigned int gappih = 20; /* horiz inner gap between windows */
static const unsigned int gappiv = 10; /* vert inner gap between windows */
static const unsigned int gappoh =
    10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov =
    30; /* vert outer gap between windows and screen edge */
static const int smartgaps_fact =
    1; /* gap factor when there is only one client; 0 = no gaps, 3 = 3x outer
          gaps */
static const int showbar = 1;           /* 0 means no bar */
static const int topbar = 1; /* 0 means bottom bar */
static const char buttonbar[] = "Gentoo";
static const unsigned int systrayspacing = 2; /* systray spacing */
static const int showsystray = 1;             /* 0 means no systray */
/* Indicators: see patch/bar_indicators.h for options */
static int tagindicatortype = INDICATOR_TOP_BAR;

static int tiledindicatortype = INDICATOR_NONE;
static int floatindicatortype = INDICATOR_TOP_LEFT_SQUARE;
static const char *fonts[] = {"monospace:size=10"};
static const char dmenufont[] = "monospace:size=10";

static char c000000[] = "#000000"; // placeholder value

static char normfgcolor[] = "#bbbbbb";
static char normbgcolor[] = "#2e3440";
static char normbordercolor[] = "#444444";
static char normfloatcolor[] = "#ffffff";

static char selfgcolor[] = "#eeeeee";
static char selbgcolor[] = "#5e81ac";
static char selbordercolor[] = "#5e81ac";
static char selfloatcolor[] = "#5e81ac";

static char titlenormfgcolor[] = "#bbbbbb";
static char titlenormbgcolor[] = "#2e3440";
static char titlenormbordercolor[] = "#444444";
static char titlenormfloatcolor[] = "#db8fd9";

static char titleselfgcolor[] = "#eeeeee";
static char titleselbgcolor[] = "#5e81ac";
static char titleselbordercolor[] = "#5e81ac";
static char titleselfloatcolor[] = "#5e81ac";

static char tagsnormfgcolor[] = "#bbbbbb";
static char tagsnormbgcolor[] = "#2e3440";
static char tagsnormbordercolor[] = "#444444";
static char tagsnormfloatcolor[] = "#db8fd9";

static char tagsselfgcolor[] = "#5e81ac";
static char tagsselbgcolor[] = "#2e3440";
static char tagsselbordercolor[] = "#3b4252";
static char tagsselfloatcolor[] = "#5e81ac";

static char hidnormfgcolor[] = "#5e81ac";
static char hidselfgcolor[] = "#227799";
static char hidnormbgcolor[] = "#2e3440";
static char hidselbgcolor[] = "#2e3440";

static char urgfgcolor[] = "#bbbbbb";
static char urgbgcolor[] = "#2e3440";
static char urgbordercolor[] = "#ff0000";
static char urgfloatcolor[] = "#db8fd9";



static char *colors[][ColCount] = {
    /*                       fg                bg                border float
     */
    [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor, normfloatcolor},
    [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor, selfloatcolor},
    [SchemeTitleNorm] = {titlenormfgcolor, titlenormbgcolor,
                         titlenormbordercolor, titlenormfloatcolor},
    [SchemeTitleSel] = {titleselfgcolor, titleselbgcolor, titleselbordercolor,
                        titleselfloatcolor},
    [SchemeTagsNorm] = {tagsnormfgcolor, tagsnormbgcolor, tagsnormbordercolor,
                        tagsnormfloatcolor},
    [SchemeTagsSel] = {tagsselfgcolor, tagsselbgcolor, tagsselbordercolor,
                       tagsselfloatcolor},
    [SchemeHidNorm] = {hidnormfgcolor, hidnormbgcolor, c000000, c000000},
    [SchemeHidSel] = {hidselfgcolor, hidselbgcolor, c000000, c000000},
    [SchemeUrg] = {urgfgcolor, urgbgcolor, urgbordercolor, urgfloatcolor},
};



static const char *const autostart[] = {
    "dwm-autostart", NULL, NULL /* terminate */
};

const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL};
static Sp scratchpads[] = {
    /* name          cmd  */
    {"spterm", spcmd1},
};

/* Tags
 * In a traditional dwm the number of tags in use can be changed simply by
 * changing the number of strings in the tags array. This build does things a
 * bit different which has some added benefits. If you need to change the number
 * of tags here then change the NUMTAGS macro in dwm.c.
 *
 * Examples:
 *
 *  1) static char *tagicons[][NUMTAGS*2] = {
 *         [DEFAULT_TAGS] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "A",
 * "B", "C", "D", "E", "F", "G", "H", "I" },
 *     }
 *
 *  2) static char *tagicons[][1] = {
 *         [DEFAULT_TAGS] = { "•" },
 *     }
 *
 * The first example would result in the tags on the first monitor to be 1
 * through 9, while the tags for the second monitor would be named A through I.
 * A third monitor would start again at 1 through 9 while the tags on a fourth
 * monitor would also be named A through I. Note the tags count of NUMTAGS*2 in
 * the array initialiser which defines how many tag text / icon exists in the
 * array. This can be changed to *3 to add separate icons for a third monitor.
 *
 * For the second example each tag would be represented as a bullet point. Both
 * cases work the same from a technical standpoint - the icon index is derived
 * from the tag index and the monitor index. If the icon index is is greater
 * than the number of tag icons then it will wrap around until it an icon
 * matches. Similarly if there are two tag icons then it would alternate between
 * them. This works seamlessly with alternative tags and alttagsdecoration
 * patches.
 */
static char *tagicons[][NUMTAGS] = {
    [DEFAULT_TAGS] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"},
    [ALTERNATIVE_TAGS] = {"A", "B", "C", "D", "E", "F", "G", "H", "I"},
    [ALT_TAGS_DECORATION] = {"<1>", "<2>", "<3>", "<4>", "<5>", "<6>", "<7>",
                             "<8>", "<9>"},
};


/* There are two options when it comes to per-client rules:
 *  - a typical struct table or
 *  - using the RULE macro
 *
 * A traditional struct table looks like this:
 *    // class      instance  title  wintype  tags mask  isfloating  monitor
 *    { "Gimp",     NULL,     NULL,  NULL,    1 << 4,    0,          -1 },
 *    { "Firefox",  NULL,     NULL,  NULL,    1 << 7,    0,          -1 },
 *
 * The RULE macro has the default values set for each field allowing you to only
 * specify the values that are relevant for your rule, e.g.
 *
 *    RULE(.class = "Gimp", .tags = 1 << 4)
 *    RULE(.class = "Firefox", .tags = 1 << 7)
 *
 * Refer to the Rule struct definition for the list of available fields
 * depending on the patches you enable.
 */
static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     *	WM_WINDOW_ROLE(STRING) = role
     *	_NET_WM_WINDOW_TYPE(ATOM) = wintype
     */
    RULE(.wintype = WTYPE "DIALOG",
         .isfloating = 1) RULE(.wintype = WTYPE "UTILITY", .isfloating = 1)
        RULE(.wintype = WTYPE "TOOLBAR",
             .isfloating = 1) RULE(.wintype = WTYPE "SPLASH", .isfloating = 1)
            RULE(.class = "Gimp", .tags = 1 << 4)
//                        RULE(.class = "Firefox", .tags = 1 << 7)
                RULE(.instance = "spterm", .tags = SPTAG(0), .isfloating = 1)
};



/* Bar rules allow you to configure what is shown where on the bar, as well as
 * introducing your own bar modules.
 *
 *    monitor:
 *      -1  show on all monitors
 *       0  show on monitor 0
 *      'A' show on active monitor (i.e. focused / selected) (or just -1 for
 * active?) bar - bar index, 0 is default, 1 is extrabar alignment - how the
 * module is aligned compared to other modules widthfunc, drawfunc, clickfunc -
 * providing bar module width, draw and click functions name - does nothing,
 * intended for visual clue and for logging / debugging
 */
static const BarRule barrules[] = {
/* monitor  bar    alignment         widthfunc                drawfunc
 * clickfunc                name */
    {-1, 0, BAR_ALIGN_LEFT, width_stbutton, draw_stbutton, click_stbutton,
     "statusbutton"},
    {-1, 0, BAR_ALIGN_LEFT, width_tags, draw_tags, click_tags, "tags"},
    {0, 0, BAR_ALIGN_RIGHT, width_systray, draw_systray, click_systray,
     "systray"},
    {-1, 0, BAR_ALIGN_LEFT, width_ltsymbol, draw_ltsymbol, click_ltsymbol,
     "layout"},
    {'A', 0, BAR_ALIGN_RIGHT, width_status2d, draw_status2d, click_statuscmd,
     "status2d"},
    {-1, 0, BAR_ALIGN_NONE, width_awesomebar, draw_awesomebar, click_awesomebar,
     "awesomebar"},
       // BAR_WINTITLE_PATCH
};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int nstack = 0; /* number of clients in primary stack area */
// display reparieren
static const int resizehints =
    0; /* 1 means respect size hints in tiled resizals */



static const Layout layouts[] = {
    /* symbol     arrange function, { nmaster, nstack, layout, master axis,
       stack axis, secondary stack axis, symbol func } */
    {"[]=",
     flextile,
     {-1, -1, SPLIT_VERTICAL, TOP_TO_BOTTOM, TOP_TO_BOTTOM, 0,
      NULL}},           // default tile layout
    {"><>", NULL, {0}}, /* no layout function means floating behavior */
    {"[M]", flextile, {-1, -1, NO_SPLIT, MONOCLE, MONOCLE, 0, NULL}}, // monocle
    {"|||",
     flextile,
     {-1, -1, SPLIT_VERTICAL, LEFT_TO_RIGHT, TOP_TO_BOTTOM, 0,
      NULL}}, // columns (col) layout
    {">M>",
     flextile,
     {-1, -1, FLOATING_MASTER, LEFT_TO_RIGHT, LEFT_TO_RIGHT, 0,
      NULL}}, // floating master
    {"[D]",
     flextile,
     {-1, -1, SPLIT_VERTICAL, TOP_TO_BOTTOM, MONOCLE, 0, NULL}}, // deck
    {"TTT",
     flextile,
     {-1, -1, SPLIT_HORIZONTAL, LEFT_TO_RIGHT, LEFT_TO_RIGHT, 0,
      NULL}}, // bstack
    {"===",
     flextile,
     {-1, -1, SPLIT_HORIZONTAL, LEFT_TO_RIGHT, TOP_TO_BOTTOM, 0,
      NULL}}, // bstackhoriz
    {"|M|",
     flextile,
     {-1, -1, SPLIT_CENTERED_VERTICAL, LEFT_TO_RIGHT, TOP_TO_BOTTOM,
      TOP_TO_BOTTOM, NULL}}, // centeredmaster
    {"-M-",
     flextile,
     {-1, -1, SPLIT_CENTERED_HORIZONTAL, TOP_TO_BOTTOM, LEFT_TO_RIGHT,
      LEFT_TO_RIGHT, NULL}}, // centeredmaster horiz
    {":::",
     flextile,
     {-1, -1, NO_SPLIT, GAPPLESSGRID, GAPPLESSGRID, 0, NULL}}, // gappless grid
    {"[\\]",
     flextile,
     {-1, -1, NO_SPLIT, DWINDLE, DWINDLE, 0, NULL}}, // fibonacci dwindle
    {"(@)",
     flextile,
     {-1, -1, NO_SPLIT, SPIRAL, SPIRAL, 0, NULL}}, // fibonacci spiral
    {"[T]",
     flextile,
     {-1, -1, SPLIT_VERTICAL, LEFT_TO_RIGHT, TATAMI, 0, NULL}}, // tatami mats
    {"[]=", tile, {0}},
    {"[M]", monocle, {0}},
    {"|||", col, {0}},
    {"[D]", deck, {0}},
    {NULL, NULL, {0}},
};


/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, comboview, {.ui = 1 << TAG}},                                  \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, combotag, {.ui = 1 << TAG}},                   \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},



/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"dmenu_run",
                                 "-m",
                                 dmenumon,
                                 "-fn",
                                 dmenufont,
                                 "-nb",
                                 normbgcolor,
                                 "-nf",
                                 normfgcolor,
                                 "-sb",
                                 selbgcolor,
                                 "-sf",
                                 selfgcolor,
                                 NULL};
static const char *termcmd[] = {"kitty", NULL};

/* This defines the name of the executable that handles the bar (used for
 * signalling purposes) */
#define STATUSBAR "dwmblocks"

static const char *firefoxcmd[] = {"firefox", NULL};
static const char *nitrogencmd[] = {"nitrogen", NULL};
static const char *nm_applet[] = {"nm-applet", NULL};
static Key on_empty_keys[] = {
    /* modifier key            function                argument */
    {0, XK_f, spawn, {.v = firefoxcmd}},
    {0, XK_p, spawn, {.v = dmenucmd}},
    {0, XK_n, spawn, {.v = nm_applet}},
    {0, XK_w, spawn, {.v = nitrogencmd}},
};

static Key keys[] = {
/* modifier                     key            function                argument
 */
    {MODKEY, XK_p, spawn, {.v = dmenucmd}},
    {MODKEY | ShiftMask, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY, XK_d, incnmaster, {.i = -1}},
    {MODKEY | ControlMask, XK_i, incnstack, {.i = +1}},
    {MODKEY | ControlMask, XK_u, incnstack, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY, XK_Return, zoom, {0}},
    {MODKEY, XK_u, incrgaps, {.i = +1}},
    {MODKEY | ShiftMask, XK_u, incrgaps, {.i = -1}},
    {MODKEY, XK_i, incrigaps, {.i = +1}},
    {MODKEY | ShiftMask, XK_i, incrigaps, {.i = -1}},
    {MODKEY, XK_o, incrogaps, {.i = +1}},
    {MODKEY | ShiftMask, XK_o, incrogaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_6, incrihgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_6, incrihgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_7, incrivgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_7, incrivgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_8, incrohgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_8, incrohgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_9, incrovgaps, {.i = +1}},
    {MODKEY | Mod1Mask | ShiftMask, XK_9, incrovgaps, {.i = -1}},
    {MODKEY | Mod1Mask, XK_0, togglegaps, {0}},
    {MODKEY | Mod1Mask | ShiftMask, XK_0, defaultgaps, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY | ControlMask, XK_z, showhideclient, {0}},
    {MODKEY | ShiftMask, XK_q, killclient, {0}},
    {MODKEY | ShiftMask, XK_x, quit, {0}},
    {MODKEY | ShiftMask, XK_r, quit, {1}},
//    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
//    {MODKEY, XK_f, setlayout, {.v = &layouts[1]}},
//    {MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
//    {MODKEY, XK_c, setlayout, {.v = &layouts[3]}},
    {MODKEY | ControlMask,
     XK_t,
     rotatelayoutaxis,
     {.i = +1}}, /* flextile, 1 = layout axis */
    {MODKEY | ControlMask,
     XK_Tab,
     rotatelayoutaxis,
     {.i = +2}}, /* flextile, 2 = master axis */
    {MODKEY | ControlMask | ShiftMask,
     XK_Tab,
     rotatelayoutaxis,
     {.i = +3}}, /* flextile, 3 = stack axis */
    {MODKEY | ControlMask | Mod1Mask,
     XK_Tab,
     rotatelayoutaxis,
     {.i = +4}}, /* flextile, 4 = secondary stack axis */
    {MODKEY | Mod5Mask,
     XK_t,
     rotatelayoutaxis,
     {.i = -1}}, /* flextile, 1 = layout axis */
    {MODKEY | Mod5Mask,
     XK_Tab,
     rotatelayoutaxis,
     {.i = -2}}, /* flextile, 2 = master axis */
    {MODKEY | Mod5Mask | ShiftMask,
     XK_Tab,
     rotatelayoutaxis,
     {.i = -3}}, /* flextile, 3 = stack axis */
    {MODKEY | Mod5Mask | Mod1Mask,
     XK_Tab,
     rotatelayoutaxis,
     {.i = -4}}, /* flextile, 4 = secondary stack axis */
    {MODKEY | ControlMask,
     XK_Return,
     mirrorlayout,
     {0}}, /* flextile, flip master and stack areas */
    //    {MODKEY, XK_space, setlayout, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_grave, togglescratch, {.ui = 0}},
    {MODKEY | ControlMask, XK_grave, setscratch, {.ui = 0}},
    {MODKEY | ShiftMask, XK_grave, removescratch, {.ui = 0}},
    {MODKEY, XK_0, view, {.ui = ~SPTAGMASK}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~SPTAGMASK}},
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    {MODKEY, XK_space, cyclelayout, {.i = -1}},
    {MODKEY | ShiftMask, XK_space, cyclelayout, {.i = +1}},
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8)};


/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
/* click                event mask           button          function argument
 */
    {ClkButton, 0, Button1, spawn, {.v = dmenucmd}},
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button1, togglewin, {0}},
    {ClkWinTitle, 0, Button3, showhideclient, {0}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button1, sigstatusbar, {.i = 1}},
    {ClkStatusText, 0, Button2, sigstatusbar, {.i = 2}},
    {ClkStatusText, 0, Button3, sigstatusbar, {.i = 3}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};


