
// #ifdef WIN32 
//  //#include <ncurses.h>
// #else
// #include <curses.h>

// #endif

/* Global Variables Definition */
char	project_title[100]=PROJECT_TITLE;
int		title_win_size=4;
int		message_win_size=7;
int		command_win_size=7;

/* Global Declaration of History Buffer */
char	history[HISTORY+1][80];
int		hist_index=0;
int		hist_cnt=0;
int		max_flag=0;

/* Blobal Declaration of Output Mode */
int		out_mode=1;
int		reset=0;
int		crefresh=0;

/* Global Variables for Various Display Format */
char	float_dis[10]=FLOAT_DEF;
char	double_dis[10]=DOUBLE_DEF;
char	int_dis[10]=INT_DEF;
char	short_dis[10]=SHORT_DEF;

/* Global Definitions of the Symbol Matrix and Valid Entry List*/
sn		addmat[200][200];
ln		*head=NULL, *pres=NULL, *tail=NULL;

/*Subwindow Areas for Debugger Task */
// WINDOW	*dummy_area, *exec_area, *user_area;
// WINDOW	*act_message_area, *message_area;
// WINDOW	*act_command_area, *command_area;

char	term_dev[20];
char	term_type[20]="iris-ansi";
char	org_command[80]="THIS IS THE DEFAULT COMMAND ARRAY";

int		initialized;
int		nrefresh;
int		display_change;
int		opn=-1;
int		int_enable;
int		flash_count;
int		flash_on;
int		rtmscmd_flag;
//FILE	*logptr;

