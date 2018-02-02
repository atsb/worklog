/* worklog */
/* program for tracking time spent on different projects */
/* Truxton Fulton */
/* March 27, 1995 */
/* gcc worklog.c -lncurses -o worklog */

/* Thanks to :
     Tim Newsome <drz@cybercomm.net> (25 Dec 1995)
     Egil Kvaleberg <egilk@sn.no> (5 Sep 1996)
     Mark Sebastian Fischer <Mark.Sebastian.Fischer@horz.technopark.gmd.de> (1 Nov 1996)
     Mike Butler <mgb@mitre.org> (22 Jun 1998)
     Vaiva Bichnevicius <vaiva@vaiva.com> (28 Mar 1999)
     Gary <glory@cnsp.com> (21 Apr 1999)
   for suggestions, bug reports, bug fixes, and improvements
*/

#define		DEFAULT_LOG_FILE	"time.log"
#define		DEFAULT_CONFIG_FILE	"projects"

#define STRLEN 2048

#define IDLE_X 5
#define CR 10
#define LF 13

#include <curses.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

struct project_list
{
  char *name ;
  int key ;
  struct project_list *next ;
  long time ;
} *project_head=NULL ;

int num_projects ;
int x_update,y_update ;
int IDLE_Y ;
struct project_list *project_update, *no_category, *current_project=NULL ;
char temps[STRLEN] ;
char temps2[STRLEN] ;
char starttimes[STRLEN] ;
char *log_filename ;
int update_skip_refresh ;
long initial_time,delta_time ;


long fetch_total_time(struct project_list *project)
{
  long result ;
  double partial ;
  char specific_timefile[STRLEN] ;
  char line[STRLEN] ;
  FILE *fa ;

  result=0.0 ;
  sprintf(specific_timefile,"%s.%c",log_filename,project->key) ;
  fa=fopen(specific_timefile,"r") ;
  if(fa==NULL)
    {
      fa=fopen(specific_timefile,"w") ;
      if(fa==NULL)
	{
	  fprintf(stderr,"ERROR: cannot open specific project log file : %s\n",specific_timefile) ;
	  perror("fopen") ;
	  exit(-1) ;
	}
    }
  else
    {
      for(;1==fscanf(fa,"%[^\n]",line);fscanf(fa,"\n"))
	if(1==sscanf(line,"%lf hours :",&partial))
	  result+=(long) rint(partial*60.0*60.0) ;
    }
  fclose(fa) ;

  return(result) ;
}

void read_config_file(filename)
     char *filename ;
{
  FILE *f ;
  char firstchar,colonchar ;
  struct project_list *project ;
  char names[STRLEN] ;
  int linenum ;

  project=(struct project_list *)malloc(sizeof(struct project_list)) ;
  project->key=CR ;
  project->next=project_head ;
  project->time=0 ;
  project->name=strdup("no category yet") ;
  project_head=project ;
  no_category=project ;

  num_projects=1 ;
  linenum=0 ;
  project=project_head ;
  f=fopen(filename,"r") ;
  if(f==NULL)
    {
      fprintf(stderr,"ERROR: cannot open project configuration file : %s\n",filename) ;
      perror("fopen") ;
      exit(-1) ;
    }
  while(!feof(f))
    {
      linenum++ ;
      for(firstchar=0;!feof(f) && firstchar<=13;)
	fscanf(f,"%c",&firstchar) ;
      if(!feof(f))
	{
	  if(firstchar=='#')
	    {
	      fscanf(f,"%*[^\n]") ;
	    }
	  else
	    {
	      /* must read colon seperator */
	      fscanf(f,"%c",&colonchar) ;
	      if(colonchar!=':')
		{
		  fprintf(stderr,"ERROR: second character in project line must be a colon.\n") ;
		  fprintf(stderr,"       on line %d is '%c'\n",linenum,colonchar) ;
		  exit(-1) ;
		}
	      project=(struct project_list *)malloc(sizeof(struct project_list)) ;
	      project->key=firstchar ;
	      project->next=project_head ;
	      project->time=fetch_total_time(project) ;
	      project_head=project ;
	      fscanf(f,"%[^\n]",names) ;
	      project->name=(char *)malloc(1+strlen(names)) ;
	      sprintf(project->name,"%s",names) ;
	      num_projects++ ;
	    }
	}
    }
  fclose(f) ;
}

/* there are multiple interpretations of what the "Del" key is */
int isdelkey(int keypress)
{
  return((keypress==127) || \
         (keypress==8) || \
         (keypress==263) || \
         (keypress==330)) ;
}

void draw_main_screen()
{
  struct project_list *project ;
  int i ;

  /*
    move(0,0) ;
    clrtobot() ;
  */
  sprintf(temps,"Worklog version %s",VERSIONS) ;
  attron(A_BOLD) ;
  mvaddstr(0,5,temps) ;
  attroff(A_BOLD) ;
  mvaddstr(1,1,"---------------------------") ;  /* this fixes a weirdness when using 'screen' !! */
  for(i=2,project=project_head;project!=NULL;project=project->next,i++)
    {
      attron(A_BOLD) ;
      if(project->key!=CR)
	{
	  move(i,1) ;
	  sprintf(temps,"    %c",project->key) ;
	  mvaddstr(i,1,temps) ;
	}
      else
	mvaddstr(i,1,"   CR") ;
      attroff(A_BOLD) ;
      mvaddstr(i,7,project->name) ;
      }
  i++ ;
  move(i,0) ;
  clrtoeol() ;
  attron(A_BOLD) ;
  mvaddstr(i,3,"DEL") ;
  attroff(A_BOLD) ;
  mvaddstr(i,7,"Quit") ;
  i++ ;
  move(i,0) ;
  clrtoeol() ;
  i++ ;
  move(i,0) ;
  clrtoeol() ;
  i++ ;
  move(i,0) ;
  clrtoeol() ;
  i++ ;
  move(i,0) ;
  clrtoeol() ;
}


void draw_run_options(int key)
{
  int i, j ;

  j=strlen(current_project->name)+10 ;
  attron(A_BOLD) ;
  mvaddstr(y_update,1,"->") ;
  attroff(A_BOLD) ;
      
  i=num_projects+3 ;

  if(key==CR)
    {
      attron(A_BOLD) ;
      mvaddstr(i,1,"     ") ;
      mvaddstr(i,7,"Press any of the category keys above to assign time") ;
      attroff(A_BOLD) ;
      i++ ;
    }
  attron(A_BOLD) ;
  mvaddstr(i,1,"SPACE") ;
  attroff(A_BOLD) ;
  mvaddstr(i,7,"Pause clock                                        ") ;
  i++ ;
  if(key!=CR)
    {
      attron(A_BOLD) ;
      mvaddstr(i,1,"   CR") ;
      attroff(A_BOLD) ;
      mvaddstr(i,7,"Stop clock and optionally enter description") ;
      i++ ;
    }
  attron(A_BOLD) ;
  mvaddstr(i,1,"    +") ;
  attroff(A_BOLD) ;
  mvaddstr(i,7,"Adjust by increasing time") ;
  i++ ;
  attron(A_BOLD) ;
  mvaddstr(i,1,"    -") ;
  attroff(A_BOLD) ;
  mvaddstr(i,7,"Adjust by decreasing time") ;
  i++ ;
  attron(A_BOLD) ;
  mvaddstr(i,1,"  DEL") ;
  attroff(A_BOLD) ;
  mvaddstr(i,7,"Quit") ;

  /* ----------- */

  if(key==CR)
    {
      mvaddstr(1+num_projects,4,"  ") ;
    }
  else
    {
      move(1+num_projects,0) ;
      clrtoeol() ;
    }

  
}

void alarm_handler(flag)
     int flag ;
{
  double seconds,minutes,hours ;

  signal(SIGALRM,alarm_handler) ;
  project_update->time+=1 ;
  if(!update_skip_refresh)
    {
      delta_time=project_update->time-initial_time ;
      seconds=(double) delta_time ;
      if(seconds<60.0)
	sprintf(temps,"%0.2f seconds",seconds) ;
      else
	{
	  minutes=seconds / 60.0 ;
	  if(minutes<60.0)
	    sprintf(temps,"%0.2f minutes",minutes) ;
	  else
	    {
	      hours = seconds / 3600.0 ;
	      sprintf(temps,"%0.2f hours",hours) ;
	    }
	}

      if(project_update!=no_category)
	{
	  seconds=(double) project_update->time ;
	  if(seconds<60.0)
	    sprintf(temps2,"  (total %0.2f seconds)",seconds) ;
	  else
	    {
	      minutes=seconds / 60.0 ;
	      if(minutes<60.0)
		sprintf(temps2,"  (total %0.2f minutes)",minutes) ;
	      else
		{
		  hours = seconds / 3600.0 ;
		  sprintf(temps2,"  (total %0.2f hours)",hours) ;
		}
	    }
	  strcat(temps,temps2) ;
	}
      attron(A_UNDERLINE) ;
      move(y_update,x_update) ;
      clrtoeol() ;
      mvaddstr(y_update,x_update,temps) ;
      attroff(A_UNDERLINE) ;
    }
}

void do_resize()
{
  endwin() ;
  
  /* usleep(10000) ; */
  /* fprintf(stderr,"{RESIZE}") ; */
  /* resizeterm(y,x) ; */

  initscr() ;
  cbreak() ;
  noecho() ;
  keypad(stdscr,TRUE) ;

  IDLE_Y = LINES-1 ;

  draw_main_screen() ;
  if(current_project!=NULL)
    draw_run_options(current_project->key) ;
}

void resize_handler(int flag)
{
  signal(SIGWINCH,resize_handler) ;
  ungetch(12) ;
}

void exit_handler(int flag)
{
  struct project_list *project ;
  FILE *f ;
  double seconds,minutes,hours ;
  time_t t ;

  f=fopen(log_filename,"a") ;
  if(f==NULL)
    {
      fprintf(stderr,"ERROR: cannot open project log file : %s\n",log_filename) ;
      perror("fopen") ;
      exit(-1) ;
    }

  fprintf(f,"-- Worklog summary begins : %s --\n",starttimes) ;
  for(project=project_head;project!=NULL;project=project->next)
    if(project->time>0)
      {
	seconds=(double) project->time ;
	if(seconds<60.0)
	  sprintf(temps,"%0.2f seconds",seconds) ;
	else
	  {
	    minutes=seconds / 60.0 ;
	    if(minutes<60.0)
	      sprintf(temps,"%0.2f minutes",minutes) ;
	    else
	      {
		hours = seconds / 3600.0 ;
		sprintf(temps,"%0.2f hours",hours) ;
	      }
	  }
	fprintf(f,"%s : total %s\n",project->name,temps) ;
      }
  time(&t) ;
  sprintf(temps,"%s",asctime(localtime(&t))) ;
  if(temps[strlen(temps)-1]<14)
    temps[strlen(temps)-1]=0 ;
  fprintf(f,"-- Worklog summary ends : %s --\n\n",temps) ;
  fclose(f) ;
  endwin() ;
  fprintf(stderr,"\n") ;
  exit(0) ;
}

int clock_on(key)
     int key ;
{
  struct project_list *project ;
  int i,j,found,quit ;
  int keypress ;
  struct itimerval timer_value ;
  int modification,success ;
  FILE *f,*fa ;
  time_t t ;
  double seconds,minutes,hours ;
  char comments[STRLEN] ;
  char specific_timefile[STRLEN] ;

  comments[0]=0 ;
  keypress=key ;
  for(found=0,i=2,project=project_head;!found && project!=NULL;project=project->next,i++)
    if(project->key==key)		     /* try to find exact match */
      {
	found=1;
	break;
      }
  
  if(!found) 
    for(found=0,i=2,project=project_head;!found && project!=NULL;project=project->next,i++)
      if(tolower(project->key)==tolower(key)) /* try to find case-insensitive match */
	{
	  found=1;
	  key=project->key;		     /* remember exact key */
	  break;
	}
  
  if(found)
    {
      current_project=project ;
      initial_time=project->time ;
      if(key!=CR)  /* transfer time from no category */
	{
	  project->time+=no_category->time ;
	  no_category->time=0 ;
	}

      
      project_update=project ;
      found=1 ;
      j=strlen(project->name)+10 ;

      y_update=i ;
      x_update=j ;
      draw_run_options(key) ;

      timer_value.it_value.tv_sec=1 ;
      timer_value.it_value.tv_usec=0 ;
      timer_value.it_interval.tv_sec=1 ;
      timer_value.it_interval.tv_usec=0 ;
      setitimer(ITIMER_REAL,&timer_value,0) ;

      for(quit=0;!quit;)
	{
	  update_skip_refresh=0 ;
	  nodelay(stdscr,TRUE) ;
	  for(keypress=ERR;keypress==ERR;usleep(1000))
	    keypress=mvgetch(IDLE_Y,IDLE_X) ;
	  update_skip_refresh=1 ;
	  nodelay(stdscr,FALSE) ;
	  switch(keypress)
	    {
	    case ' ' :
	      timer_value.it_value.tv_sec=0 ;
	      timer_value.it_value.tv_usec=0 ;
	      timer_value.it_interval.tv_sec=0 ;
	      timer_value.it_interval.tv_usec=0 ;
	      setitimer(ITIMER_REAL,&timer_value,0) ;
	      attron(A_BLINK) ;
	      mvaddstr(y_update,1,"->") ;
	      attroff(A_BLINK) ;
	      
	      i=num_projects+9 ;
	      mvaddstr(i,5,"-- Clock paused.  Press any key to resume --") ;
	      move(i,49) ;
	      keypress=getch() ;
	      move(i,0) ;
	      clrtoeol() ;
	      
	      attron(A_BOLD) ;
	      mvaddstr(y_update,1,"->") ;
	      attroff(A_BOLD) ;
	      timer_value.it_value.tv_sec=1 ;
	      timer_value.it_value.tv_usec=0 ;
	      timer_value.it_interval.tv_sec=1 ;
	      timer_value.it_interval.tv_usec=0 ;
	      setitimer(ITIMER_REAL,&timer_value,0) ;
	      break ;
	    case '+' :
	      i=num_projects+9 ;
	      attrset(A_BOLD) ;
	      mvaddstr(i,5,"Enter number of minutes to add : ") ;
	      attrset(A_BOLD) ;
	      echo();
	      success=(mvscanw(i,38,"%d",&modification)==1) ;
	      noecho();
	      if(success)
		if(modification<0)
		  success=0 ;
	      attrset(A_NORMAL) ;
	      move(i,0) ;
	      clrtoeol() ;
	      if(success)
		{
		  project->time+=60*modification ;
		}
	      else
		{
		  beep() ;
		  mvaddstr(i,5,"Error : input value must be a positive integer") ;
		  mvaddstr(i+1,5,"-- Press any key --") ;
		  move(i+1,24) ;
		  keypress=getch() ;
		  move(i,0) ;
		  clrtoeol() ;
		  move(i+1,0) ;
		  clrtoeol() ;
		}
	      break ;
	    case '-' :
	      i=num_projects+9 ;
	      attrset(A_BOLD) ;
	      mvaddstr(i,5,"Enter number of minutes to subtract : ") ;
	      attrset(A_BOLD) ;
	      echo();
	      success=(mvscanw(i,43,"%d",&modification)==1) ;
	      noecho();
	      if(success)
		if(modification<0)
		  success=0 ;
	      attrset(A_NORMAL) ;
	      move(i,0) ;
	      clrtoeol() ;
	      if(success)
		{
		  project->time-=60*modification ;
		}
	      else
		{
		  beep() ;
		  mvaddstr(i,5,"Error : input value must be a positive integer") ;
		  mvaddstr(i+1,5,"-- Press any key --") ;
		  move(i+1,24) ;
		  keypress=getch() ;
		  move(i,0) ;
		  clrtoeol() ;
		  move(i+1,0) ;
		  clrtoeol() ;
		}
	      break ;
	    case CR :
	      if(key!=CR)
		{
		  i=num_projects+9 ;
		  attrset(A_BOLD) ;
		  mvaddstr(i,5,"Enter comment : ") ;
		  attrset(A_BOLD) ;
		  echo();
		  mvgetstr(i,21,comments) ;
		  noecho();
		  attrset(A_NORMAL) ;
		  move(i,0) ;
		  clrtobot() ;
		  quit=1 ;
		}
	      break ;
	    case 12 :  /* control-L */
#ifdef KEY_RESIZE
	    case KEY_RESIZE :  /* sent by ncurses sometimes */
#endif
	      do_resize() ;
	      break ;;;
	    default :
              if(isdelkey(keypress) || (key==CR))
                quit=1 ;  /* fall back to main loop */
	      break ;
	    }
	}
      if(key!=CR)
	{
	  sprintf(specific_timefile,"%s.%c",log_filename,key) ;
	  f=fopen(log_filename,"a") ;
	  if(f==NULL)
	    {
	      fprintf(stderr,"ERROR: cannot open project log file : %s\n",log_filename) ;
	      perror("fopen") ;
	      exit(-1) ;
	    }
	  fa=fopen(specific_timefile,"a") ;
	  if(fa==NULL)
	    {
	      fprintf(stderr,"ERROR: cannot open specific project log file : %s\n",specific_timefile) ;
	      perror("fopen") ;
	      exit(-1) ;
	    }
	  fprintf(f,"%s ",project->name) ;
	  delta_time=project->time-initial_time ;
	  seconds=(double) delta_time ;
	  hours = seconds / 3600.0 ;
	  sprintf(temps,"%8.2f hours : ",hours) ;
	  fprintf(fa,"%s",temps) ;
	  if(strlen(comments)>0)
	    {
	      fprintf(f,"(%s) ",comments) ;
	      fprintf(fa,"%s",comments) ;
	    }
	  else
	    fprintf(fa,"<no description>") ;
	  if(delta_time>0)
	    {
	      seconds=(double) delta_time ;
	      if(seconds<60.0)
		sprintf(temps,"%0.2f seconds",seconds) ;
	      else
		{
		  minutes=seconds / 60.0 ;
		  if(minutes<60.0)
		    sprintf(temps,"%0.2f minutes",minutes) ;
		  else
		    {
		      hours = seconds / 3600.0 ;
		      sprintf(temps,"%0.2f hours",hours) ;
		    }
		}
	    }
	  fprintf(f,": %s : finished ",temps) ;
	  fprintf(fa," : [%s] : finished ",temps) ;
	  time(&t) ;
	  sprintf(temps,"%s",asctime(localtime(&t))) ;
	  if(temps[strlen(temps)-1]<14)
	    temps[strlen(temps)-1]=0 ;
	  fprintf(f,"%s\n",temps) ;
	  fprintf(fa,"%s\n",temps) ;
	  fclose(f) ;
	  fclose(fa) ;
	}
      timer_value.it_value.tv_sec=0 ;
      timer_value.it_value.tv_usec=0 ;
      timer_value.it_interval.tv_sec=0 ;
      timer_value.it_interval.tv_usec=0 ;
      setitimer(ITIMER_REAL,&timer_value,0) ;
      update_skip_refresh=0 ;
      alarm_handler(0) ;
      mvaddstr(y_update,1,"  ") ;
    }
  else
    if(!isdelkey(key) && key!=12)
      {
        /* fprintf(stderr,"\n%c%cunknown key=%d (press control-L to refresh screen)%c%c\n",10,13,key,10,13) ; */
	beep() ;
	keypress=ERR ;
      }

  /* no current project now... */
  current_project=NULL ;

  return(keypress) ;
}

int main(argc,argv)
     int argc ;
     char **argv ;
{
  char *config_filename ;
  int keypress ;
  int quit,i ;
  FILE *lf ;
  time_t t ;
  time(&t) ;

  if(argc>1)
    {
      if(argc>3 ||
	 (strstr(argv[1],"-h")==argv[1]) ||
	 (strstr(argv[1],"-?")==argv[1]))
	{
	  fprintf(stderr,"USAGE: %s [<project config file> [<log file>]]\n",argv[0]) ;
	  exit(0) ;
	}
    }
#ifdef DEFAULT_DIR_ENVVAR
  else
    {					     /* no args given */
      const char* default_dir;
      if((default_dir=getenv(DEFAULT_DIR_ENVVAR)))
	if(chdir(default_dir))
	  {
	    fprintf(stderr,"WARNING: Could not change to default directory ($%s = %s)\n",DEFAULT_DIR_ENVVAR,default_dir) ;
	    perror("chdir");
	  }
	else
	  {
	    fprintf(stderr,"INFO: Using default directory %s (environment variable %s)\n",default_dir,DEFAULT_DIR_ENVVAR) ;
	  }
    }
#endif /* def DEFAULT_DIR_ENVVAR */
  
  if (argc>2)
    log_filename=argv[2] ;
  else
    log_filename=DEFAULT_LOG_FILE;
  if (argc>1)
    config_filename=argv[1] ;
  else
    config_filename=DEFAULT_CONFIG_FILE;
  read_config_file(config_filename) ;
  lf=fopen(log_filename,"a") ;
  if(lf==NULL)
    {
      fprintf(stderr,"ERROR: cannot open project log file : %s\n",log_filename) ;
      perror("fopen") ;
      exit(-1) ;
    }
  fclose(lf) ;

  sprintf(starttimes,"%s",asctime(localtime(&t))) ;
  if(starttimes[strlen(starttimes)-1]<14)
    starttimes[strlen(starttimes)-1]=0 ;

  initscr() ;
  cbreak() ;
  noecho() ;
  keypad(stdscr,TRUE) ;

  IDLE_Y = LINES-1 ;

  signal(SIGALRM,alarm_handler) ;

  signal(SIGHUP,exit_handler) ;
  signal(SIGINT,exit_handler) ;
  signal(SIGQUIT,exit_handler) ;
  signal(SIGTERM,exit_handler) ;
  signal(SIGWINCH,resize_handler) ;

  keypress=CR ;
  draw_main_screen() ;
  for(quit=0;!quit;)
    {
      if(keypress==CR)
	keypress=mvgetch(IDLE_Y,IDLE_X) ;
      keypress=clock_on(keypress) ;
      draw_main_screen() ;
      if(keypress==ERR)
	keypress=mvgetch(IDLE_Y,IDLE_X) ;
      if(keypress==12)
	{
	  do_resize() ;
	  keypress=CR ;	  
	}
      if(isdelkey(keypress))
	{
	  i=num_projects+5 ;
	  attron(A_BLINK) ;
	  mvaddstr(i,5,"Press DEL once more to quit") ;
	  beep() ;
	  attroff(A_BLINK) ;
	  keypress=mvgetch(IDLE_Y,IDLE_X) ;
	  mvaddstr(i,5,"                           ") ;
          if(isdelkey(keypress))
	    quit=1 ;
	  keypress=CR ;
	}
    }
  exit_handler(0) ;
  return(0) ;
}
