/// @file hsguard.h

#include "tools.h"
#ifndef OS_LINUX
    #error "this software is linux only install wsl under windows"
#endif
#include "ctools/c_crc32.h"
#include "sqlite3.h"

/// on Emergency für testdb
enum ermergency_bits{
    emMIN   = (1<<9),
    emSKIP  = (1<<10),
    emPANIC = (1<<11),
    emABORT = (1<<12),
    emUPDATE= (1<<13),
    emUPLCHK= (1<<14),
    emMAX   = (1<<15)
};

/// Die einzelnen Stufen der Lognachrichten
enum{
    mMIN =LMSG_SILENT,
    mROOT=mMIN+1, // 1
    mFAIL, // 2
    mUSER, // 3
    mINFO, // 4
    mDEVL, // 5
    mMAX   // 6
};

// Im Prinzip "int,*char" aufbau
typedef struct int_char_s{
    int  num;
    char *tok;
}int_char_t;

enum {
    FO_MIN=1,
    FO_ONELINE=FO_MIN,  // 1
    FO_LONGLINE,        // 2
    FO_MAXLINE,         // 3
    FO_MINI,            // 4
    FO_BASIC,           // 5
    FO_MEDIUM,          // 6
    FO_FULL,            // 7
    FO_MAX,             // 8
    FO_LIST_DEFAULT     =FO_LONGLINE,
    FO_INFO_DEFAULT     =FO_FULL,
    FO_FINDDUPES_DEFAULT=FO_MEDIUM
};

#ifndef  hsguard_main
extern
#endif
int_char_t format_table[]
#ifdef  hsguard_main
={
    { FO_ONELINE   , "LINE"     },
    { FO_ONELINE   , "ONELINE"  },
    { FO_LONGLINE  , "LONGLINE" },
    { FO_MAXLINE   , "MAXLINE"  },
    { FO_MINI      , "MINI"     },
    { FO_BASIC     , "BASIC"    },
    { FO_MEDIUM    , "MED"      },
    { FO_MEDIUM    , "MEDIUM"   },
//    { FO_FULL      , "FULL"     }, // darf nicht mit f anfangen :( ist sonst -ff = 2
    { FO_FULL      , "ALL"      },
    { FO_MAX       , "MAX"      },
    { 0            , NULL       }
}
#endif // hsguard_main
;

#ifdef  hsguard_main
#define hsguard_xtrn
#define hsguard_dflt_NULL =NULL
#define hsguard_dflt_empty =""
#define hsguard_dflt_0 = 0
#define hsguard_dflt_minus = -1
#define hsguard_dflt_eins = 1
#define hsguard_dflt_emergency = emPANIC
#else
#define hsguard_xtrn extern
#define hsguard_dflt_NULL
#define hsguard_dflt_empty
#define hsguard_dflt_0
#define hsguard_dflt_minus
#define hsguard_dflt_eins
#define hsguard_dflt_emergency
#endif // _hsguard_main

#define hsguard_dflt_1 hsguard_dflt_eins

#include "g_sql.h"
#include "g_helper.h"
#include "g_update.h"
#include "g_list.h"
#include "g_info.h"
#include "g_remove.h"
#include "g_color.h"
#include "g_finddupes.h"
#include "g_longtable.h"
#include "g_testdb.h"
#include "g_cfg.h"

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif // max

#define swap(x,y) do{typeof(x) x1rst=x; x=y; y=x1rst;} while(false)
#define default_wbs (size_t)1024*1024*1024*16

hsguard_xtrn char       HSguard_DB[PATH_MAX];
//hsguard_xtrn char       HSguard_RC[PATH_MAX];
hsguard_xtrn int        cmd_isCreateExc hsguard_dflt_0;
hsguard_xtrn int        cmd_isDump      hsguard_dflt_0;
hsguard_xtrn int        cmd_isDir       hsguard_dflt_0;
hsguard_xtrn int        cmd_isInitDB    hsguard_dflt_0;
hsguard_xtrn int        cmd_isUpdate    hsguard_dflt_0;
hsguard_xtrn int        cmd_isRescanFlg hsguard_dflt_0;
hsguard_xtrn int        cmd_isNoDirTest hsguard_dflt_0;
hsguard_xtrn int        cmd_isYesFlg    hsguard_dflt_0;
hsguard_xtrn int        cmd_isRecursive hsguard_dflt_0;
hsguard_xtrn int        cmd_isFindDupes hsguard_dflt_0;
hsguard_xtrn int        cmd_isListAll   hsguard_dflt_0;
hsguard_xtrn int        cmd_isListDir   hsguard_dflt_0;
hsguard_xtrn int        cmd_isRemove    hsguard_dflt_0;
hsguard_xtrn int        cmd_isTestDB    hsguard_dflt_0;
hsguard_xtrn size_t     cmd_isWarnbigsz hsguard_dflt_0;
//hsguard_xtrn int        cmd_isCaption   hsguard_dflt_0;
hsguard_xtrn int        cmd_isAnsiFlg   hsguard_dflt_0;
hsguard_xtrn int        cmd_isFormOut   hsguard_dflt_minus;

hsguard_xtrn int        update_deep     hsguard_dflt_minus;
hsguard_xtrn char      *update_dir      hsguard_dflt_NULL;
hsguard_xtrn char      *list_dir        hsguard_dflt_NULL;

hsguard_xtrn int        cmd_isNoScreen  hsguard_dflt_0;

hsguard_xtrn int        cmd_isInfo      hsguard_dflt_0;
hsguard_xtrn int        cmd_isInfoDir   hsguard_dflt_0;
hsguard_xtrn char      *info_path       hsguard_dflt_NULL;

hsguard_xtrn int        emergency       hsguard_dflt_emergency;
hsguard_xtrn int        abortlevel      hsguard_dflt_eins;

hsguard_xtrn sqlite3   *sq3db           hsguard_dflt_NULL;
hsguard_xtrn char      *sql_lastdirname hsguard_dflt_NULL;
hsguard_xtrn sqlid_t    sql_lastdirnum  hsguard_dflt_minus;
hsguard_xtrn char      *sql_ErrMsg      hsguard_dflt_NULL;
hsguard_xtrn char     **exlst           hsguard_dflt_NULL;
hsguard_xtrn char     **exfiles         hsguard_dflt_NULL;
hsguard_xtrn char      *basis           hsguard_dflt_NULL;

hsguard_xtrn int        sql_stored      hsguard_dflt_0;
hsguard_xtrn void      *sql_datalst     hsguard_dflt_NULL;
hsguard_xtrn char      *sql_Data        hsguard_dflt_NULL;
