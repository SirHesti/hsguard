/// @file g_helper.h

void die(char *msg);
int GetConfig (void);
int GetConfigFrom (void *cfg, char *section);
int showDump(void);
int showHelp(void);

int Execute_Create(void);
int Execute_InitDB(void);

int cleanup(int exitcode);
int GetCmdlineSwitches(void);
int getMinMax(int value, int minimum, int maximum);
int getLevel(char *cmd, int def, int minimum, int maximum, char extension, int extstart, int(*func)(char*));
int check_basics(void);
int getPath  (char **value, char *inputpath);
int getPathID(char **value, char *inputpath);
int getPlaceFrom_info_path(struct place_s *pla);
int getEmergency(char *option);
int ensureDir(char **origin, char*base);
void *get_sql_datalst(void);

int getFlagFromString(char *cmd, int behavior, int def);
int lmsgGuard(int log_screen);

int getNumFromTable(char *name);
int getNumForVerbose(char *name);
