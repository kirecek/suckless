/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 10;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 1;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int horizpadbar        = 0;        /* horizontal padding for statusbar */
static const int vertpadbar         = 15;        /* vertical padding for statusbar */
static const char *fonts[]          = { "Ubuntu Mono:size=12" };
static const char dmenufont[]       = "Ubuntu Mono:size=12";
static const char col_bg_sel[]      = "#9A9A9A";
static const char col_border_sel[]  = "#414141";
static const char col_fg_sel[]      = "#70ea9b";
static const char col_bg[]          = "#282828";
static const char col_fg[]          = "#ffffff";
static const char *colors[][3]      = {
    /*               fg         bg         border   */
    [SchemeNorm] = { col_fg, "#171819", col_bg },
    [SchemeSel]  = { col_fg_sel, col_bg_sel,  "#458588" },
    [SchemeStatus] = { "#FFFFFF", "#171819", col_bg },
    [SchemeTagsSel] = { "#FB4934", col_bg, col_bg },
    [SchemeTagsNorm] = { "#FFFFFF", "#212121", col_bg },
    [SchemeTagsInd] = { "#919191", col_bg, col_bg },
    [SchemeInfoSel] = { "#000000", "#458588", col_bg },
    [SchemeInfoNorm] = { col_fg, "#171819", col_bg },
    [SchemeLayoutInfo] = { "#494949", "#171717", col_bg },
};
static const char sel_chars[2] = {']', '['};

/* tagging */
// static const char *tags[] = { "", "", "", "", "", "", "", "", "" };
static const char *tags[] = { "www", "term", "dev", "chat", "sys", "media", "gfx", "games", "crap" };

static const Rule rules[] = {
    /* xprop(1):
     *  WM_CLASS(STRING) = instance, class
     *  WM_NAME(STRING) = title
     */
    /* class            instance    title       tags mask     isfloating   monitor */
    { "Steam",          NULL,       NULL,       1 << 7,       1,           -1 },
    { "Gimp",           NULL,       NULL,       1 << 6,       1,           -1 },
    { "krita",          NULL,       NULL,       1 << 6,       0,           -1 },
    { "firefox",        NULL,       NULL,       1,            0,           -1 },
    { "Slack",          NULL,       NULL,       1 << 3,       0,           -1 },
    { "Emacs",          NULL,       NULL,       1 << 2,       0,           -1 },
    { "code",           NULL,       NULL,       1 << 2,       0,           -1 },
    { "Arandr",         NULL,       NULL,       1 << 4,       0,           -1 },
    { "Pavucontrol",    NULL,       NULL,       1 << 4,       0,           -1 },
    { "zoom",           NULL,       NULL,       1 << 5,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[T]",      tile },    /* first entry is default */
    { "[F]",      NULL },    /* no layout function means floating behavior */
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
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *roficmd[] = { "rofi", "-show", "drun", "-show-icons", NULL };
static const char *termcmd[]  = { "alacritty", NULL };
static const char *clipmenucmd[]  = { "clipmenu", NULL };

/* multimedia commands */
static const char *upvol[]       = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[]     = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[]     = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };
static const char *brightup[]    = { "/usr/bin/xbacklight", "-inc", "10", NULL};
static const char *brightdown[]  = { "/usr/bin/xbacklight", "-dec", "10", NULL};

/* commands spawned when clicking statusbar, the mouse button pressed is exported as BUTTON */
static char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };

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

