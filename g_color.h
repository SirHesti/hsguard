/// @file g_color.h

enum col_tabelle{
    col_MIN=0,
    col_ID=col_MIN,

    col_Fname,
    col_Fsize,
    col_Fmtime,
    col_CRC32,
    col_LastChk,
    col_CheckCount,
    col_Founded,

    col_locID,
    col_Location,
    col_locFounded,

    col_MAX
};
enum COLOR_nums {
    COLOR_RESET = -1,
    COLOR_MIN = 0,
    COLOR_BLACK=COLOR_MIN,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_PURPLE,
    COLOR_CYAN,
    COLOR_WHITE,

    COLOR_BBLACK,   //    COLOR_DARKGRAY,
    COLOR_BRED,
    COLOR_BGREEN,
    COLOR_BYELLOW,  //    COLOR_BROWN,
    COLOR_BBLUE,
    COLOR_BPURPLE,
    COLOR_BCYAN,
    COLOR_BWHITE,   //COLOR_LIGHTGRAY,

    COLOR_MAX
};

char *color_get_ansi(int col_val);
int color_get_mapnum(int col_num);
char *color_getmap_name(int mapnum);
int color_getcolByName(char *colname);
char *color_ansi_str(int color_num);
char *color_getmap_colorname(int mapnum);
char *color_getcolor(int colnum);
int color_setmap_color(int mapnum, int colornum);
