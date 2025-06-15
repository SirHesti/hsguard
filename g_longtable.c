#include "hsguard.h"

#ifdef HS_DEBUG
#define step_idtable 2
#else
#define step_idtable 16
#endif

// no public
int     base_bsearch=0;
int     count_idtable=-1;
int     size_idtable=-1;
sqlid_t *idtable=NULL;

/**
 @brief  Alles freigeben / Standard herstellen

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS Initial
___________________________________________________________________________*/

void long_table_free(void)
{
    base_bsearch=0;
    count_idtable=-1;
    size_idtable=-1;
    idtable=free0(idtable);
}

/**
 @brief  bsearch - Spezialfall for idtable
 @return -1 error 0-x found here
 @param  sqnum welche ID gefunden werden soll

 Überbleibsel @ref base_bsearch stellt, wenn nichts gefunden einen
 Einfügepunkt dar.

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS Initial
___________________________________________________________________________*/

int long_table_bsearch (sqlid_t sqnum)
{
    int lim, rim;
    base_bsearch=0;
    if (!idtable)           return -1;
    if (count_idtable<1)    return -1;
    for (lim = count_idtable; lim != 0; lim >>= 1)
    {
        rim=base_bsearch+(lim >> 1);
        if(sqnum==idtable[rim]) return rim;
        if (sqnum>idtable[rim])
        {
            base_bsearch=rim+1;;
            lim--;
        }
    }
    return -1;
}

/**
 @brief  Stellt Speicher zur Verfügung, wie in sizeme angegeben
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  sizeme

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS Initial
 ** 14.05.25 HS Beim erstaufruf sizeme berücksichtigen
___________________________________________________________________________*/

int long_table_memory(int sizeme)
{
    sqlid_t *newptr;
    if (!idtable)
    {
        count_idtable=0;
        for (size_idtable=step_idtable*4;sizeme>=size_idtable;size_idtable=size_idtable+step_idtable);
        idtable=malloc0(size_idtable * sizeof(sqlid_t));
        if (idtable) return EXIT_SUCCESS;
        return EXIT_FAILURE;
    }
    if (sizeme<size_idtable) return EXIT_SUCCESS;
    for (;sizeme>=size_idtable;size_idtable=size_idtable+step_idtable);
    newptr=realloc(idtable, size_idtable * sizeof(sqlid_t));
    if (!newptr) EXIT_FAILURE;
    idtable=newptr;
    return EXIT_SUCCESS;
}

/**
 @brief  Eine ID in die Tabelle einpflegen (sortiert)
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  sqnum

 Es wird zum Suchen der bsearch-algorithmus (Quicksort) verwendet.

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS Initial
___________________________________________________________________________*/

int long_table_insert(sqlid_t sqnum)
{
    int i;
    if (idtable)
    {
        if (count_idtable<0)                    return EXIT_FAILURE;
        if (long_table_bsearch(sqnum)>=0)       return EXIT_FAILURE;
        if (long_table_memory(count_idtable+1)) return EXIT_FAILURE;
    }else{
        if (long_table_memory(step_idtable*4))  return EXIT_FAILURE;
    }
    count_idtable++;
    for (i=count_idtable-1;i>base_bsearch;i--)
    {
        idtable[i]=idtable[i-1];
    }
    idtable[base_bsearch]=sqnum;
    return EXIT_SUCCESS;
}

/**
 @brief  Einen Wert an position pos zurückgeben
 @return 0 bei Fehlern, bzw. der Wert >0
 @param  pos position

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS Initial
___________________________________________________________________________*/

sqlid_t long_table_value(int pos)
{
    if (!idtable)                           return 0;
    if (count_idtable<1)                    return 0;
    if (pos>=count_idtable)                 return 0;
    return idtable[pos];
}

/**
 @brief  Wrapper um count_idtable zu lesen
 @return count_idtable

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS Initial
___________________________________________________________________________*/

int long_table_count(void)
{
    return count_idtable;
}


// ----------------------------------------
//           NUR zum Testen
// ----------------------------------------

#ifdef HS_DEBUG
int long_table_tester(void)
{
//    int rc;
    int k;
    sqlid_t tester[]={10,8,3,12,44,55,78,32,11,4};
    long_table_memory(101);
    for(k=0;k<_countof(tester);k++)
        if (long_table_insert(tester[k])) die ("die welt ist schlecht");

    for(k=0;k<count_idtable;k++)
    {
        printf ("%02i - %lld\n", k, idtable[k]);
    }
//    long_table_memory(10);
//    count_idtable=8;
//    idtable[0]=10;
//    idtable[1]=22;
//    idtable[2]=30;
//    idtable[3]=53;
//    idtable[4]=61;
//    idtable[5]=77;
//    idtable[6]=95;
//    idtable[7]=100;
//    idtable[8]=102;
//    rc=long_table_find(23); printf ("23   rc=%i\n",rc);
//    rc=long_table_find(22); printf ("22   rc=%i\n",rc);
//    rc=long_table_find(53); printf ("53   rc=%i\n",rc);
//    rc=long_table_find(54); printf ("54   rc=%i\n",rc);
//    rc=long_table_find(100); printf ("100   rc=%i\n",rc);
//    rc=long_table_find(102); printf ("102   rc=%i\n",rc);
//    for (sqlid_t i=0;i<111;i++) rc=long_table_bsearch(i);
//    die(NULL);
//   return rc;
    return 0;
}
#endif
