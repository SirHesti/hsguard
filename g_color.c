#include "hsguard.h"

// Im Prinzip "int,*char" aufbau
//typedef struct{
//    int   num;
//    char *tok;
//}COLOR_NUM_STRING_t;

// um aus string-farbe einen int-wert zu generieren, in ausnahmefällen geht es auch anderesrum
// verwendet von
// - color_getcolByName
// - color_getmap_colorname
int_char_t COLOR_NAME[]={
    { COLOR_BLACK   , "BLACK"   },
    { COLOR_RED     , "RED"     },
    { COLOR_GREEN   , "GREEN"   },
    { COLOR_YELLOW  , "YELLOW"  },
    { COLOR_BLUE    , "BLUE"    },
    { COLOR_PURPLE  , "PURPLE"  },
    { COLOR_CYAN    , "CYAN"    },
    { COLOR_WHITE   , "WHITE"   },

    { COLOR_BBLACK  , "BBLACK"  },
    { COLOR_BRED    , "BRED"    },
    { COLOR_BGREEN  , "BGREEN"  },
    { COLOR_BYELLOW , "BYELLOW" },
    { COLOR_BBLUE   , "BBLUE"   },
    { COLOR_BPURPLE , "BPURPLE" },
    { COLOR_BCYAN   , "BCYAN"   },
    { COLOR_BWHITE  , "BWHITE"  },
// was sonst noch so verwendet wird, aber obengenanntes meint
    { COLOR_BRED    , "LIGHTRED"    },
    { COLOR_BGREEN  , "LIGHTGREEN"  },
    { COLOR_BYELLOW , "LIGHTYELLOW" },
    { COLOR_BBLUE   , "LIGHTBLUE"   },
    { COLOR_BPURPLE , "LIGHTPURPLE" },
    { COLOR_BCYAN   , "LIGHTCYAN"   },
    { COLOR_BYELLOW , "BROWN"       },
    { COLOR_PURPLE  , "MAGENTA"     },
    { COLOR_BPURPLE , "LIGHTMAGENTA"},

    { COLOR_BBLACK  , "DARKGRAY"    },
    { COLOR_BWHITE  , "LIGHTGRAY"   }
};

// record eines columnMAPPER
typedef struct{
    int column_id;
    int color_id;
    char *column_name;
}COLS_SEE_t;

// Diese Tabelle wird hinsichtlich der Farbe "COLOR_*" manipuliert
// Die angegebenen Farben sind nur die Vorgaben
COLS_SEE_t columnMAPPER[]={
    { col_ID         , COLOR_BBLUE   , "col_id" },
    { col_Fname      , COLOR_BGREEN  , "col_fname" },
    { col_Fsize      , COLOR_BWHITE  , "col_fsize" },
    { col_Fmtime     , COLOR_BPURPLE , "col_fmtime" },
    { col_CRC32      , COLOR_BYELLOW , "col_crc32" },
    { col_LastChk    , COLOR_BPURPLE , "col_lastchk" },
    { col_CheckCount , COLOR_BPURPLE , "col_checkcnt" },
    { col_Founded    , COLOR_PURPLE  , "col_founded" },

    { col_locID      , COLOR_CYAN    , "col_locid" },
    { col_Location   , COLOR_CYAN    , "col_location" },
    { col_locFounded , COLOR_PURPLE  , "col_locfounded" },
};

// das hier ist die Tablle, die die AnsiFarben ausliest
int_char_t ansiCOLOR[]={
    { COLOR_RESET   , "\033[00m"    },  //  0 Reset
    // Basics
    { COLOR_BLACK   , "\033[00;30m" },  //  1 Black
	{ COLOR_RED     , "\033[00;31m" },  //  2 Red
	{ COLOR_GREEN   , "\033[00;32m" },  //  3 Green
	{ COLOR_YELLOW  , "\033[00;33m" },  //  4 Yellow
	{ COLOR_BLUE    , "\033[00;34m" },  //  5 Blue
	{ COLOR_PURPLE  , "\033[00;35m" },  //  6 Purple
	{ COLOR_CYAN    , "\033[00;36m" },  //  7 Cyan
	{ COLOR_WHITE   , "\033[00;37m" },  //  8 White
 	// Bold
	{ COLOR_BBLACK  , "\033[01;30m" },  //  9 Black
	{ COLOR_BRED    , "\033[01;31m" },  // 10 Red
	{ COLOR_BGREEN  , "\033[01;32m" },  // 11 Green
	{ COLOR_BYELLOW , "\033[01;33m" },  // 12 Yellow
	{ COLOR_BBLUE   , "\033[01;34m" },  // 13 Blue
	{ COLOR_BPURPLE , "\033[01;35m" },  // 14 Purple
	{ COLOR_BCYAN   , "\033[01;36m" },  // 15 Cyan
	{ COLOR_BWHITE  , "\033[01;37m" },  // 16 White
}; // total 17

/**
 @brief  Gibt den AnsiString (unter Beachtung des cmd_isAnsiFlg) zurück
 @return String ggf. auch einen Leerstring
 @param  color_num numerische Farbe für den String ( COLOR_* )

___[ Revision ]______________________________________________________________

 ** 09.05.25 HS Initial
___________________________________________________________________________*/

char *color_ansi_str(int color_num)
{
    int i;
    if (!cmd_isAnsiFlg) return "";
    for (i=0;i<_countof(ansiCOLOR);i++)
    {
        if (color_num==ansiCOLOR[i].num) return ansiCOLOR[i].tok;
    }
    return ansiCOLOR[0].tok;
}

/**
 @brief  aus der location eine Farbe (Ansi zusammenstellen)
 @return String ggf. auch einen Leerstring
 @param  column die einfärbt werden soll @ref col_tabelle

___[ Revision ]______________________________________________________________

 ** 09.05.25 HS Initial
___________________________________________________________________________*/

char *color_get_ansi(int col_val)
{
    int mapnum;
    int color_num;
    mapnum=color_get_mapnum(col_val);
    if (mapnum<0) color_num=-1;
    else          color_num=columnMAPPER[mapnum].color_id;
    return color_ansi_str(color_num);
}

/**
 @brief  start the the recode
 @return mapnum oder -1
 @param  Im Mapper den internen IDX für die column suchen

___[ Revision ]______________________________________________________________

 ** 09.05.25 HS Initial
___________________________________________________________________________*/

int color_get_mapnum(int col_num)
{
    int i;
    for (i=0;i<_countof(columnMAPPER);i++)
    {
        if (col_num==columnMAPPER[i].column_id) return i;
    }
    return -1;
}

/**
 @brief  Names der internen IDX(mapnum) zurückgeben
 @return name wie "col_Fsize"
 @param  mapnum IDX

___[ Revision ]______________________________________________________________

 ** 09.05.25 HS Initial
___________________________________________________________________________*/

char *color_getmap_name(int mapnum)
{
    if (mapnum<0) return NULL;
    if (mapnum>=_countof(columnMAPPER)) return NULL;
    return columnMAPPER[mapnum].column_name;
}

/**
 @brief  Farbnummer(int) anhand der FarbString("BLUE") finden
 @return int-farbe (COLOR_*) oder -1
 @param  colname string die zur Farbe passt

___[ Revision ]______________________________________________________________

 ** 09.05.25 HS Initial
___________________________________________________________________________*/

int color_getcolByName(char *colname)
{
    int i;
    if (!colname) return -1;
    for (i=0;i<_countof(COLOR_NAME);i++)
    {
        if (!strcasecmp(colname,COLOR_NAME[i].tok)) return COLOR_NAME[i].num;
    }
    return -1;
}

/**
 @brief  Farbname wie "BLUE" vom columnMAPPER ableiten
 @return String von einer Farbe
 @param  mapnum IDX

 Stellt sicher, dass man da ist

___[ Revision ]______________________________________________________________

 ** 09.05.25 HS Initial
___________________________________________________________________________*/

char *color_getmap_colorname(int mapnum)
{
    int i;
    if (mapnum<0) return "";
    if (mapnum>=_countof(columnMAPPER)) return "";
    for (i=0;i<_countof(COLOR_NAME);i++)
    {
        if (columnMAPPER[mapnum].color_id==COLOR_NAME[i].num) return COLOR_NAME[i].tok;
    }
    return "";
}

/**
 @brief  Namen der Farbe nummer X
 @return String von einer Farbe
 @param  colnum nummer der Farbe COLOR_MIN-COLOR_MAX (0-15)

 Stellt sicher, dass man da ist

___[ Revision ]______________________________________________________________

 ** 10.05.25 HS Initial
___________________________________________________________________________*/

char *color_getcolor(int colnum)
{
    int i;
    for (i=0;i<_countof(COLOR_NAME);i++)
    {
        if (colnum==COLOR_NAME[i].num) return COLOR_NAME[i].tok;
    }
    return "";
}

/**
 @brief  Setze in der columnMAPPER farbe colornum
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  mapnum IDX
 @param  colornum farbnummer aus dem COLOR_*

___[ Revision ]______________________________________________________________

 ** 09.05.25 HS Initial
___________________________________________________________________________*/

int color_setmap_color(int mapnum, int colornum)
{
    if (mapnum<0) return EXIT_FAILURE;
    if (mapnum>=_countof(columnMAPPER)) return EXIT_FAILURE;
    if (colornum<COLOR_MIN) return EXIT_FAILURE;
    if (colornum>=COLOR_MAX) return EXIT_FAILURE;
    columnMAPPER[mapnum].color_id=colornum;
    return EXIT_SUCCESS;
}

