#include <curses.h>
#include "rtms.h"

extern	char	project_title[100];
extern	int		message_win_size;
extern	int		title_win_size;
extern	int		command_win_size;
extern	char	history[HISTORY+1][80];
extern	int		hist_index, hist_cnt, max_flag;
extern	int		out_mode;
extern	int		reset,crefresh;
extern	char	float_dis[10], int_dis[10], double_dis[10], short_dis[10];
extern  sn		addmat[200][200];
extern  ln		*head, *pres, *tail;
extern  WINDOW	*dummy_area, *exec_area, *user_area;
extern  WINDOW  *act_message_area, *message_area;
extern  WINDOW  *act_command_area, *command_area;
extern  char    term_dev[20], term_type[20];
extern  char    org_command[80];
extern  int     initialized, reset, display_change, opn, int_enable;
extern  int     flash_count, flash_on, interACT_flag;
extern  FILE    *logptr;

