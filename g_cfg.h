/// @file g_cfg.h

//int Execute_Readcfg(void);
//int readcfg_readfile(char *rdfile, void *parent);

int Execute_CreateExclude(void);
int excludeRead(char *excludefile_toread);
int excludeReadAll(void);

int Execute_Dir(void);

enum getListOptions{
    D_SORTET    = (1<<1),
    D_DIRONLY   = (1<<2),
    D_FILEONLY  = (1<<3),
    D_MAX       = (1<<15)
};
typedef struct getDirList_s{
    char **d;
    char **f;
    char *fullname;
    int option;
}getDirList_t;
int getDirList(char *dirname, getDirList_t *r);
