#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 3;
static const unsigned int gappx    = 10;
static const unsigned int snap     = 32;

/* bar settings */
static const int showbar = 1;
static const int topbar  = 1;

/* statusbar padding */
static const int horizpadbar = 0;
static const int vertpadbar  = 15;

/* font settings */
static const char *fonts[]    = { "Ubuntu Mono:size=12" };
static const char dmenufont[] = "Ubuntu Mono:size=12";

/* color variables */
static const char col_bg[]        = "#171819";
static const char col_fg[]        = "#ffffff";
static const char col_fg_dark[]   = "#000000";
static const char col_highlight[] = "#458588";

/* color schemes */
static const char *colors[][3] = {
    /*                      fg              bg              border   */
    [SchemeNorm]        = { col_fg,         col_bg,         col_bg },
    [SchemeSel]         = { col_highlight,  col_bg,         col_highlight },
    [SchemeStatus]      = { col_fg,         col_bg,         col_bg },
    [SchemeTagsSel]     = { col_fg_dark,    col_highlight,  col_bg },
    [SchemeTagsNorm]    = { col_fg,         "#212121",      col_bg },
    [SchemeTagsInd]     = { col_highlight,  col_bg,         col_bg },
    [SchemeInfoSel]     = { col_fg_dark,    col_highlight,  col_bg },
    [SchemeInfoNorm]    = { col_fg,         "#171819",      col_bg },
    [SchemeLayoutInfo]  = { col_highlight,  col_bg,         col_bg },
};

// changing these chars require also modification of dwm.c!!!
// TODO: remove from header file
static const char sel_chars[2] = {']', '['};

/* tagging */
static const char *tags[] = { "www", "term", "dev", "chat", "sys", "media", "gfx", "games", "crap" };

static const Rule rules[] = {
    /* xprop(1):
     *  WM_CLASS(STRING) = instance, class
     *  WM_NAME(STRING) = title
     */
    /* class            instance    title           tags mask     isfloating    monitor scratch key*/
    { "Steam",          NULL,       NULL,           1 << 7,       1,           -1,      0 },
    { "Gimp",           NULL,       NULL,           1 << 6,       1,           -1,      0 },
    { "krita",          NULL,       NULL,           1 << 6,       0,           -1,      0 },
    { "firefox",        NULL,       NULL,           1,            0,           -1,      0 },
    { "Slack",          NULL,       NULL,           1 << 3,       0,           -1,      0 },
    { "Emacs",          NULL,       NULL,           1 << 2,       0,           -1,      0 },
    { "code",           NULL,       NULL,           1 << 2,       0,           -1,      0 },
    { "Arandr",         NULL,       NULL,           1 << 4,       0,           -1,      0 },
    { "Pavucontrol",    NULL,       NULL,           1 << 4,       0,           -1,      0 },
    { "zoom",           NULL,       NULL,           1 << 5,       0,           -1,      0 },
	{ NULL,             NULL,       "scratchpad",   0,            1,           -1,       's' },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[T]",      tile },
    { "[F]",      NULL },
    { "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
    { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2]             = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]       = { "dmenu_run", "-m", dmenumon, NULL };
static const char *roficmd[]        = { "rofi", "-show", "drun", "-show-icons", NULL };
static const char *termcmd[]        = { "alacritty", NULL };
static const char *clipmenucmd[]    = { "clipmenu", NULL };

/* multimedia commands */
static const char *upvol[]       = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[]     = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[]     = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };
static const char *brightup[]    = { "/usr/bin/xbacklight", "-inc", "10", NULL};
static const char *brightdown[]  = { "/usr/bin/xbacklight", "-dec", "10", NULL};

/* commands spawned when clicking statusbar, the mouse button pressed is exported as BUTTON */
static char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };

static const char *scratchpadcmd[] = {"s", "alacritty", "-t", "scratchpad", NULL}; 

static Key keys[] = {
    /* modifier                     key        function        argument */
    { MODKEY,                       XK_p,      spawn,          {.v = roficmd} },
    { MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,                       XK_b,      togglebar,      {0} },
    { MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
    { MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
    { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
    { MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
    { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
    { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
    { MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
    { MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
    { MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
    { MODKEY,                       XK_Return, zoom,           {0} },
    { MODKEY,                       XK_Tab,    view,           {0} },
    { MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
    { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
    { MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
    { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
    { MODKEY,                       XK_space,  setlayout,      {0} },
    { MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
    { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
    { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
    { MODKEY|ControlMask,           XK_1,      toggletag,      {.ui = 0 } },
    { MODKEY|ControlMask,           XK_2,      toggletag,      {.ui = 1 } },
    { MODKEY|ControlMask,           XK_3,      toggletag,      {.ui = 2 } },
    { MODKEY|ControlMask,           XK_4,      toggletag,      {.ui = 3 } },
    { MODKEY|ControlMask,           XK_5,      toggletag,      {.ui = 4 } },
    { MODKEY|ControlMask,           XK_6,      toggletag,      {.ui = 5 } },
    { MODKEY|ControlMask,           XK_7,      toggletag,      {.ui = 6 } },
    { MODKEY|ControlMask,           XK_8,      toggletag,      {.ui = 7 } },
    { MODKEY|ControlMask,           XK_9,      toggletag,      {.ui = 8 } },
    { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
    TAGKEYS(                        XK_1,                      0)
    TAGKEYS(                        XK_2,                      1)
    TAGKEYS(                        XK_3,                      2)
    TAGKEYS(                        XK_4,                      3)
    TAGKEYS(                        XK_5,                      4)
    TAGKEYS(                        XK_6,                      5)
    TAGKEYS(                        XK_7,                      6)
    TAGKEYS(                        XK_8,                      7)
    TAGKEYS(                        XK_9,                      8)
    { MODKEY|ShiftMask,             XK_q,      quit,           {0} },
    { MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 
    { MODKEY,                       XK_c,      spawn,          {.v = clipmenucmd} },
    // FN keys
    { 0,                            XF86XK_AudioLowerVolume,   spawn, {.v = downvol } },
    { 0,                            XF86XK_AudioMute,          spawn, {.v = mutevol } },
    { 0,                            XF86XK_AudioRaiseVolume,   spawn, {.v = upvol   } },
    { 0,                            XF86XK_MonBrightnessUp,    spawn, {.v = brightup } },
    { 0,                            XF86XK_MonBrightnessDown,  spawn, {.v = brightdown } },

	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkStatusText,        0,              Button1,        spawn,          {.v = statuscmd } },
    { ClkStatusText,        0,              Button2,        spawn,          {.v = statuscmd } },
    { ClkStatusText,        0,              Button3,        spawn,          {.v = statuscmd } },
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

