//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	{"", "~/.dwm/scripts/date.sh",	30,		0},
	{"", "~/.dwm/scripts/mem.sh",	10,		0},
	{"", "~/.dwm/scripts/cpu.sh",	10,		0},
	{"", "~/.dwm/scripts/wifi.sh",	60,		0},
	{"", "~/.dwm/scripts/power.sh",	50,		0},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim[] = "  ";
static unsigned int delimLen = 10;
