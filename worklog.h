#define		DEFAULT_LOG_FILE	"time.log"
#define		DEFAULT_CONFIG_FILE	"projects"

#define STRLEN 2048

#define IDLE_X 5
#define CR 10
#define LF 13

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

int num_projects;
int x_update,y_update;
int IDLE_Y;
struct project_list *project_update, *no_category, *current_project=NULL;
char temps[STRLEN];
char temps2[STRLEN];
char starttimes[STRLEN];
char *log_filename;
int update_skip_refresh;
long initial_time,delta_time;
long fetch_total_time(struct project_list *);
void read_config_file(char *);
int isdelkey(int);
void draw_main_screen(void);
void draw_run_options(int);
void alarm_handler(int);
void do_resize(void);
void resize_handler(int);
int clock_on(int);
void exit_handler(int);
