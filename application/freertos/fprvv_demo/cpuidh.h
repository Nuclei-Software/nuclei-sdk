
#ifdef __cplusplus
extern "C" {
#endif

extern char    timeday[30];
extern double  theseSecs;
extern double  startSecs;
extern double  secs;

void start_time();
void end_time();
void local_time();
#ifdef __MACH__
int get_nprocs_conf();
int get_nprocs();
long get_phys_pages();
int getpagesize();
#endif

#ifdef __cplusplus
};
#endif


