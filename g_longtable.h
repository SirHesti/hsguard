/// @file g_longtable.h

void    long_table_free(void);
int     long_table_bsearch(sqlid_t sqnum);
int     long_table_memory(int sizeme);
int     long_table_insert(sqlid_t sqnum);
sqlid_t long_table_value(int pos);
int     long_table_count(void);

#ifdef HS_DEBUG
int long_table_tester(void);
#endif
