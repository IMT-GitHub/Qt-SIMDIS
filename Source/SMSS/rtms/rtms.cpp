//#define _BSD_SIGNALS


#include	<signal.h>
//#include	<curses.h>
//#include	<unistd.h>



#include	<stdlib.h>
#include	"rtms.h"
#include	"rtms_global.h"
#include	<stdio.h>
#include	<windows.h>
#ifdef WIN32
	
	
#else

#endif

void		command_interpret();
void		process_command(char *);
void		clear_adds();
void		add_to_list(int, int);
void		debug_refresh();
void		to_lower();
void		die();
void		debug_page();
void		refresh_screen_event();
int		startup();
void		rtmscmd_print(char *);
void		rtmscmd_title(char *);
void		rtmscmd_history(char *);
void		rtmscmd_interaction(char *);
void		rtmscmd_size(char *);
void		rtmscmd_refresh(char *);
void		rtmscmd_read(char *);
void		rtmscmd_display(char *);
void		rtmscmd_set(char *);
void		rtmscmd_log(char *, int *);
//extern 	void    rtms_driver();
//extern  int	pages();


int		test_cnt;
int		current_page;

int startup()
{
	//FILE	*device_out, *device_in;
	char	debug_title[]="RTMS";
	char	version_number[]="1.0";
	char	page_title[]="Real-time Monitoring System for UH-60P FFS";
	char	*reserved_title="";
	char	*message_title="";
	char	*command_title="";

	int		i, len;

	//initscr();
	//crmode();
	//echo();
	//nonl();

	//box(stdscr,'|','-');
	// COORD Pos = {1, 2};
	// SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos)

	// addstr(debug_title);
	// move(2,2);
	// addstr("Version: ");
	// printw("%s",version_number);
	// len = strlen(project_title);
	// move(1, (COLS/2 - len/2));
	// addstr(project_title);
	// move(2,(COLS/2 - 13));
	// addstr(page_title);

	// dummy_area = subwin(stdscr,4,COLS,0,0);
	// box(dummy_area,'|','-');

	// exec_area = subwin(stdscr,2,11,1,COLS-13);
	// message_area = subwin(stdscr,message_win_size,COLS,title_win_size-1,0);
	// box(message_area,'|','-');

	// for(i=1;i<message_win_size-1;i++) {
	// 	wmove(message_area,i,1);
	// 	waddstr(message_area,"*");
	// }

	// act_message_area = subwin(message_area,message_win_size-2,COLS-3,
    //                                 title_win_size,2);
	// wmove(act_message_area,2,(COLS/2 - 35));
	// waddstr(act_message_area,reserved_title);
	// wmove(act_message_area,3,(COLS/2 - 35));
	// waddstr(act_message_area,message_title);
	// wmove(act_message_area,message_win_size-3,0);
	// scrollok(act_message_area,TRUE);

	// user_area = subwin(stdscr,LINES-(command_win_size+message_win_size+3),
    //                       COLS-2,(title_win_size+message_win_size-1),1);
	// box(user_area,'*','*');

	// command_area = subwin(stdscr,command_win_size,COLS,
    //                       LINES-command_win_size,0);
	// box(command_area,'|','-');

	// for(i=0;i<command_win_size-1;i++) {
	// 	wmove(command_area,i,1);
	// 	waddstr(command_area,">");
	// }

	// act_command_area = subwin(command_area,command_win_size-2,COLS-3,
    //                                 LINES-(command_win_size-1),2);
	// wmove(act_command_area,2,(COLS/2-35));
	// waddstr(act_command_area,reserved_title);
	// wmove(act_command_area,3,(COLS/2-35));
	// waddstr(act_command_area,command_title);
	// wmove(act_command_area,command_win_size-3,0);

	// scrollok(act_command_area,TRUE);
	// nodelay(act_command_area,TRUE);
	// keypad(act_command_area,TRUE);

	// refresh();
	// clear_adds();
	return(1);
}

void rtms_()
{
	char	message[80], int_yes_no[5], *default_term;
	int	page_number=0;

	//signal(SIGINT,die);
	//signal(SIGSEGV,die);
	// signal(SIGBUS,die);
	// signal(SIGPIPE,die);

	if(initialized) {
		//wrefresh(exec_area);
		debug_refresh();
		if(flash_count) {
			if(flash_count%50==0) {
				//wstandout(user_area);
				flash_on=1;
			}
			else if(flash_count%25==0) {
				//wstandend(user_area);
				flash_on=0;
			}
			flash_count--;
		}
		//wrefresh(user_area);
		//rtms_driver();
		command_interpret();
		//signal(SIGWINCH,refresh_screen_event);
		if(reset) {
			process_command("refresh");
			reset=0;
		}
	}
	else {
		//if(startup()==ERROR) {
			// wprintw(act_command_area,"Error occurred while"
            //                       "starting up the rtms!!!\n");
			// exit(-1);
		// }
		// else {
		// 	if(!crefresh)
		// 		process_command("read");
		// 	else
		// 		debug_page(opn);
		// 	sprintf(message,"row=%d,col=%d",LINES,COLS);
		// 	rtms_message_("rtms",message);
		// 	initialized=1;
		// }
	}
}

// void rtms_float_(int row,int col,float *address)
// {
// 	if(addmat[row][col].address==NULL) {
// 		addmat[row][col].address=(int)address;
// 		addmat[row][col].type='f';
// 		add_to_list(row,col);
// 	}
// 	else
// 		wprintw(act_command_area,"You are trying to view multiple"
//                                 " symbol at the same x,y coordination!!!\n");
// }

// void rtms_double_(int row,int col,double *address)
// {
// 	if(addmat[row][col].address==NULL) {
// 		addmat[row][col].address=(int)address;
// 		addmat[row][col].type='d';
// 		add_to_list(row,col);
// 	}
// 	else
// 		wprintw(act_command_area,"You are trying to view multiple"
//                                   " symbol at the same x,y coordination!!!\n");
// }

// void rtms_int_(int row,int col,int *address)
// {
// 	if(addmat[row][col].address==NULL) {
// 		addmat[row][col].address=(int)address;
// 		addmat[row][col].type='i';
// 		add_to_list(row,col);
// 	}
// 	else
// 		wprintw(act_command_area,"You are trying to view multiple"
//                              " symbol at the same x,y coordination!!!\n");
// }

// void rtms_short_(int row,int col,short int *address)
// {
// 	if(addmat[row][col].address==NULL) {
// 		addmat[row][col].address=(int)address;
// 		addmat[row][col].type='s';
// 		add_to_list(row,col);
// 	}
// 	else
// 		wprintw(act_command_area,"You are trying to view m@ltiple"
//                                  " symbol at the same x,y coordination!!!\n");
// }

// void rtms_boolean_(int row,int col,char *address)
// {
// 	if(addmat[row][col].address==NULL) {
// 		addmat[row][col].address=(int)address;
// 		addmat[row][col].type='b';
// 		add_to_list(row,col);
// 	}
// 	else
// 		wprintw(act_command_area,"You are trying to view multiple"
//                                  " symbol at the same x,y coordination!!!\n");
// }

// /* rtms_enumaration is made by woo for ada enumaration type handling */
// void rtms_enumeration_(int row,int col,char *address)
// {
// 	if(addmat[row][col].address==NULL) {
// 		addmat[row][col].address=(int)address;
// 		addmat[row][col].type='e';
// 		add_to_list(row,col);
// 	}
// 	else
// 		wprintw(act_command_area,"You are trying to view multiple"
//                                  " symbol at the same x,y coordination!!!\n");
// }

void rtms_text_(int row,int col,char address[])
{
	// wmove(user_area,row,col); 
	// wprintw(user_area,"%-s",address); 

}

void rtms_exec_text_(int row,int col,char *address)
{
	// wmove(exec_area,row-1,col-1);
	// wprintw(exec_area,"%-s",*address); 
}

void rtms_exec_int_(int row,int col, int *address)
{
	// wmove(exec_area,row-1,col-1);
	// wstandout(exec_area);
	// wprintw(exec_area,"%-5d",*address);
	// wstandend(exec_area);
}

// rtms_message_(char *caller, char *message)
// {
// 	// wprintw(act_message_area,"\n %-s : %-s  current_page = %d",caller,message,current_page);
// 	// wrefresh(act_message_area);
// }

void die()
{
	//endwin();
	system("echo Error occurred during processing!!!\n");
	exit(0);
}

void refresh_screen_event()
{
	reset=1;
}

void debug_page(int page_number)
{
	int     ok;

	// clear_adds();
	// werase(user_area);
	// box(user_area,'*','*');

	// if(page_number==-1) {
	// 	opn=-1;
	// 	wrefresh(user_area);
	// }
	// else if(page_number==-2) {
	// 	wprintw(act_command_area, "You have entered non-numeric"
    //                                " page identifier\n");
	// 	wrefresh(user_area);
	// }
	// else {
	// 	ok=pages(page_number); 
	// 	if(ok)
	// 		opn=page_number;	
	// 	else {
	// 		wprintw(act_command_area,"Cannot find debug_page_"
    //                                    "%d!!!\n", page_number);
	// 		wrefresh(user_area);
		
	// 	}
	// 	wrefresh(user_area);
	// }
}

void command_interpret()
{
	int		i;
	int		input_char;
	char		message[80];
	static	int	cmdcnt, hist_ptr;
	static	char	command[80]="THIS IS THE DEFAULT COMMAND ARRAY\0";



	//waddch(act_command_area,8);
	//input_char=wgetch(act_command_area);
	//waddch(act_command_area,' '|A_REVERSE);

//printf("input => %d", input_char);
	// if((input_char==8)&&(cmdcnt>0)) {
	// 	wclrtoeol(act_command_area);
	// 	command[cmdcnt--]='\0';
	// }
	// else if(input_char==13) { // KEY_ENTER ????

	// 	waddch(act_command_area,8);
	// 	wclrtoeol(act_command_area);
	// 	command[cmdcnt]='\0';
	// 	strcpy(org_command,command);
	// 	to_lower(command);
	// 	scroll(act_command_area);
	// 	wmove(act_command_area,command_win_size-3,0);
	// 	process_command(command);
	// 	for(i=0;i<cmdcnt;i++)
	// 		command[cmdcnt]=' ';
	// 	cmdcnt=0;
	// }
	// else if(input_char == 339) {
	// 	current_page++;
	// 	debug_page(current_page);
	// 	sprintf(message,"row=%d,col=%d",LINES,COLS);
	// 	rtms_message_("rtms",message);
	// }
	// else if(input_char == 338) {
	// 	current_page--;
	// 	debug_page(current_page);
	// 	sprintf(message,"row=%d,col=%d",LINES,COLS);
	// 	rtms_message_("rtms",message);
	// }
	// else if(input_char!=8)
	// 	if(input_char!=ERR) {
	// 		command[cmdcnt++]=input_char;
	// }
}

void process_command(char *command)
{
	static int		logflg;
	int		i;
	char		message[80];


	// if((logflg)&&(strncmp(command,"log",3)))
	// 	fprintf(logptr,"%s\n",command);

	// if((*command=='#')||(*command=='!')||(*command=='\n')||
	// 	(*command=='\0')||(*command=='*'))
	// 	return;
	// else if(!strncmp(command,"page",4)) {
	// 	if(!strncmp(command+5,"off",3)) {
	// 		debug_page(-1);
	// 		return;
	// 	}
	// 	else {
	// 		for(i=5;*(command+i)!=NULL;i++)
	// 			if((*(command+i)<'0')||(*(command+i)>'9')) {
	// 				debug_page(-2);
	// 				return;
	// 			}
	// 		debug_page(atoi(command+5)); 
	// 		current_page = atoi(command+5);
	// 		sprintf(message,"row=%d,col=%d",LINES,COLS);
	// 		rtms_message_("rtms",message);
	// 	}
	// }
	// else if(!strncmp(command,"set",3)) {
	// 	rtmscmd_set(command);
	// 	return;
	// }
	// else if(!strncmp(command,"display",7)) {
	// 	rtmscmd_display(command);
	// 	return;
	// }
	// else if(!strncmp(command,"refresh",7)) {
	// 	rtmscmd_refresh(command);
	// 	return;
	// }
	// else if(!strncmp(command,"read",4)) {
	// 	rtmscmd_read(command);
	// 	return;
	// }
	// else if(!strncmp(command,"print",5)) {
	// 	rtmscmd_print(command);
	// 	return;
	// }
	// else if(!strncmp(command,"size",4)) {
	// 	rtmscmd_size(command);
	// 	return;
	// }
	// else if(!strncmp(command,"log",3)) {
	// 	rtmscmd_log(command,&logflg);
	// 	return;
	// }
	// else if(!strncmp(command,"flash",5)) {
	// 	if(*(command+5)=='\0') {
	// 		wprintw(act_command_area,"Flash command incorrectly"
	// 								"invoked!!!\n");
	// 		return;
	// 	}
	// 	else {
	// 		flash_count=atoi(command+5)*50;
	// 	}
	// }
	// else if(!strncmp(command,"title",5)) {
	// 	rtmscmd_title(command);
	// 	return;
	// }
	// else if(!strncmp(command,"interaction",11)) {
	// 	rtmscmd_interaction(command);
	// 	return;
	// }
	// else if(!strncmp(command,"help",4)) {
	// 	wprintw(act_command_area,"No Help available");
	// 	scroll(act_command_area);
	// 	wmove(act_command_area,command_win_size-3,0);
	// }
	// else if(!strncmp(command,"wait",4)) {
	// 	sleep(atoi(command+4));
	// 	wrefresh(user_area);
	// }
	// else if(!strncmp(command,"stop",4)) {
	// 	endwin();
	// 	system("clear");
	// 	rtmscmd_flag=0;
	// 	return;
	// }
	// else if(!strncmp(command,"quit",4)) {
	// 	endwin();
	// 	system("clear");
	// 	exit(0);
	// }
	// else {
	// 	wprintw(act_command_area,"I don't understand the command");
	// 	scroll(act_command_area);
	// 	wmove(act_command_area,command_win_size-3,0);
	// }
}

void add_to_list(int row, int col)
{
	// if(head==NULL) {
	// 	head=tail=(ln *)calloc(1,(unsigned)sizeof(ln));
	// 	if(head==NULL)
	// 		wprintw(act_command_area,"Memory Allocation Error");
	// 	head->nxt=NULL;
	// 	head->row=row;
	// 	head->col=col;
	// }
	// else {
	// 	tail->nxt=(ln *)calloc(1,(unsigned)sizeof(ln));
	// 	if(tail->nxt==NULL)
	// 		wprintw(act_command_area,"Memory allocation error\n");
	// 	tail=tail->nxt;
	// 	tail->nxt=NULL;
	// 	tail->row=row;
	// 	tail->col=col;
	// }
}

void clear_adds()
{
	int		i,j;
	ln		*tmp=head, *nxt;

	while(tmp!=NULL) {
		nxt=tmp->nxt;
		free(tmp);
		tmp=nxt;
	}
	head=NULL;

	for(i=0;i<200;i++) {
		for(j=0;j<200;j++) {
			addmat[i][j].address=NULL;
			addmat[i][j].type=NULL;
		}
	}
}

void debug_refresh()
{
	char	page_command[20];
	ln		*tmp=head;

	if(display_change)
		debug_page(opn);
	display_change=0;
	// while(tmp!=NULL) {
	// 	wmove(user_area,tmp->row,tmp->col);
	// 	switch(out_mode) {
	// 		case 1 :
	// 			switch(addmat[tmp->row][tmp->col].type) {
	// 				case 'f' :
	// 					wprintw(user_area,float_dis,
	// 							*(float *)addmat[tmp->row][tmp->col].address);
	// 					break;
	// 				case 'd' :
	// 					wprintw(user_area,double_dis,
	// 							*(double *)addmat[tmp->row][tmp->col].address);
	// 					break;
	// 				case 'i' :
	// 					wprintw(user_area,int_dis,
	// 							*(int *)addmat[tmp->row][tmp->col].address);
	// 					break;
	// 				case 's' :
	// 					wprintw(user_area,short_dis,
	// 							*(short *)addmat[tmp->row][tmp->col].address);
	// 					break;
	// 				case 't' :
	// 					wprintw(user_area,"%-s",
	// 							(char *)addmat[tmp->row][tmp->col].address);
	// 					break;
	// 				case 'e' : /* Added by Woo for Ada enumeration type handling */
	// 					wprintw(user_area,int_dis,
	// 							(int)*(char *)addmat[tmp->row][tmp->col].address);
	// 					break;
	// 				case 'b' :
	// 					if((int)*(char *)addmat[tmp->row][tmp->col].address) {
	// 						if(!flash_on)
	// 							wstandout(user_area);
	// 						else
	// 							wstandend(user_area);
	// 						waddstr(user_area,"TRUE");
	// 						if(!flash_on)
	// 							wstandend(user_area);
	// 						else
	// 							wstandout(user_area);
	// 					}
	// 					else {
	// 						waddstr(user_area,"FALSE");
	// 					}
	// 					break;
	// 				default :
	// 					wprintw(act_command_area,"This cannot be"
	// 											" happening");
	// 					break;
	// 			}
	// 			break;
	// 		case 2 :
	// 			if(addmat[tmp->row][tmp->col].type=='t')
	// 				wprintw(user_area,"%-s",
	// 						(char *)addmat[tmp->row][tmp->col].address);
	// 			else
	// 				wprintw(user_area, "%-o",
	// 						addmat[tmp->row][tmp->col].address);
	// 			break;
	// 		case 3 :
	// 			if(addmat[tmp->row][tmp->col].type=='t')
	// 				wprintw(user_area, "%-s",
	// 						(char *)addmat[tmp->row][tmp->col].address);
	// 			else
	// 				wprintw (user_area, "%-X",
	// 						addmat[tmp->row][tmp->col].address);
	// 			break;
	// 		default :
	// 			wprintw(user_area,"This cannot be happening");
	// 	}
	// 	tmp=tmp->nxt;
	// }
}
	

void to_lower(char *arg)
{
	int      i;

	for(i=0;*(arg+i)!='\0';i++) {
		if((*(arg+i)>='A')&&(*(arg+i)<='Z'))
			*(arg+i)=*(arg+i)+32;
	}
}

void rtms_interaction(char *command) /* Name Changed */
{
	if(int_enable) {
		// wmove(act_command_area, command_win_size-3,0);
		// wclrtoeol(act_command_area);
		// wprintw(act_command_area,"Interaction : %s\n",command);
		strcpy(org_command,command);
		process_command(command);
	}
}
