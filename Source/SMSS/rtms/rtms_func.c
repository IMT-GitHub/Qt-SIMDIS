#include <curses.h>
#include <stdlib.h>
#include "rtms_externs.h"

void rtmscmd_print(char *command)
{
	char option[15];

	move(LINES-2,2);
	refresh();
	if (*(command+5)!='\0') {
		strcpy(option,command+6);
		if (!strncmp(option,"-rv",3))
			system("/usr/bin/x11/xdpr -nobdrs -rv -id $WINDOWID");
		else
			wprintw(act_command_area, "Not a valid option for PRINT!!\n");
		}
	else
		system("/usr/bin/x11/xdpr --nobdrs -id $WINDOWID");
	redrawwin(act_command_area);
	scroll(act_command_area);
	wmove(act_command_area, command_win_size-3, 0);
}
 
/****   Title Command **************/
void rtmscmd_title(char *command)
{
	size_t	len;
	char	blanks[200];

	if (*(command+5)=='\0')
		wprintw(act_command_area, "Title incorrectly ...   !!\n");
	else {
		strcpy(project_title, org_command+6);
		move(1, (COLS/2 - (COLS/2-12)));
		sprintf(blanks, "%*s", COLS-25, " ");
		addstr(blanks);
		len=strlen(project_title);
		move(1, (COLS/2 - len/2));
		addstr(project_title);
		wrefresh(dummy_area);
	}
}

/****Interaction Command*************/
void rtmscmd_interaction(char *command)
{
	if (*(command+11)=='\0')
		wprintw(act_command_area, " Error ... \n");
	else if  (*(command+13)== 'n') {
		int_enable=1;
		wprintw(act_command_area, "Iteraction enabled \n");
	}
	else if  (*(command+13)== 'f') {
		int_enable=0;
		wprintw(act_command_area, "Iteraction disabled \n");
	}
	else
		wprintw(act_command_area,
                          "Iteraction command incorrectly invoiced!! \n");
}

/*****Refreshs the screen****************/
void rtmscmd_refresh()
{
	endwin() ;
	initialized=0;
	crefresh=1;
	rtms_();
}

/*****Size command**********************/
void rtmscmd_size(char *command)
{
	if (*(command+4)=='\0') {
		message_win_size=7;
		command_win_size=7;
		wprintw(act_command_area,
					"Resetting command and Message area to default line",
					" number setting!!\n");
	}
	else if (!strncmp(command+5, "com",3))
		command_win_size=atoi(command+9)+2;
	if (command_win_size==0) command_win_size=7;
	if (message_win_size==0) message_win_size=7;
	rtmscmd_refresh();
}

/*****Read command*************************/
void rtmscmd_read (char *command)
{
	FILE *fptr=NULL;
	char file[80];
	char dfault[22]="rtms_read.default\0";
	char file_com[80];
	int i;

	if (*(command+4)=='\0') {
		wprintw(act_command_area, "Reading default !!\n");
		wmove(act_command_area, command_win_size-3,0);
		strcpy(file, dfault);
	}
	else if((*(command+5)=='/') && (*(command+6)=='\0')) {
		wprintw(act_command_area, "unable to process read request\n");
		wmove(act_command_area, command_win_size-3,0);
		wrefresh(act_command_area);
		return;
	}
	else {
		for (i=5;*(org_command+i)==' ';i++) {}
		strcpy(file, org_command+i);
	}

	fptr=fopen(file, "r");
	if (fptr==NULL) {
		wprintw(act_command_area, "Unable ... \n");
		scroll(act_command_area);
		wmove(act_command_area, command_win_size-3,0);
		wrefresh(act_command_area);
		return;
	}
	while (fgets(file_com, 82, fptr) != NULL) {
		for (i=0;*(file_com+i) != '\0'; i++) {
			if (*(file_com+i) == '\n')
				*(file_com+i)='\0';
		}
		strcpy(org_command, file_com);
		to_lower(file_com);
		if(*org_command != '\0')
			wprintw(act_command_area, "%s\n", org_command);
		process_command(file_com);
	}
	wprintw(act_command_area, "file read completed!!");
	scroll(act_command_area);
	wmove(act_command_area, command_win_size-3,0);
}

void rtmscmd_log(char *command, int *logflg)
{
	if(*logflg) {
		wprintw(act_command_area,"Ending log",command+4);
		*logflg=0;
		fclose(logptr);
	}
	else {
		if(*(command+3)=='\0') {
			wprintw(act_command_area,"You must specify"
						" a filename\n");
			return;
		}
		wprintw(act_command_area,"Beginning log to file\n",command+4);
		*logflg=1;
		logptr=fopen(command+4,"w");
		if(logptr==NULL) {
			wprintw(act_command_area,"Error opening log file\n");
			*logflg=0;
		}
	}
}

void rtmscmd_set(char *command)
{
	int		myrow, mycol,i;

	display_change=1;
	myrow=atoi(command+3);
	for(i=4;((*(command+i)!=' ')&&(*(command+i)!=','));i++) {}
	mycol=atoi(command+i+1);
	for(i++;((*(command+i)!=' ')&&(*(command+i)!=','));i++) {}
	if((mycol>200)||(myrow>200)) {
		wprintw(act_command_area,"Too large row/column\n");
		return;
	}

	switch(addmat[myrow][mycol].type) {
		case 'f' :
			*(float *)addmat[myrow][mycol].address=(float)atof(command+i+1);
			break;
		case 'd' :
			*(double *)addmat[myrow][mycol].address=(double)atof(command+i+1);
			break;
		case 'i' :
			*(int *)addmat[myrow][mycol].address=(int)atoi(command+i+1);
			break;
		case 's' :
			*(short *)addmat[myrow][mycol].address=(short)atoi(command+i+1);
			break;
		case 'e' :
			*(char *)addmat[myrow][mycol].address=(char)atoi(command+i+1);
			break;
		case 'b' :
			if(!strncmp(command+i+1,"true",4))
				*(char *)addmat[myrow][mycol].address=(char)1;
			else if(!strncmp(command+i+1,"false",5))
				*(char *)addmat[myrow][mycol].address=(char)0;
			else if(!strncmp(command+i+1,"t",1))
				*(char *)addmat[myrow][mycol].address=(char)1;
			else if(!strncmp(command+i+1,"f",1))
				*(char *)addmat[myrow][mycol].address=(char)0;
			else if(atoi(command+i+1))
				*(char *)addmat[myrow][mycol].address=(char)1;
			else
				*(char *)addmat[myrow][mycol].address=(char)0;
			break;
		default :
			wprintw(act_command_area,"Cannot process SET command\n");
			break;
	}
	wrefresh(user_area);
}

void rtmscmd_display(char *command)
{
	int      i,prec;

	display_change=1;
	out_mode=1;
	if(*(command+7)=='\0')
		return;
	if(!strncmp(command+8,"float",5)) {
		if(*(command+13)=='\0') {
			strcpy(float_dis,FLOAT_DEF);
			wprintw(act_command_area,"Reseting float to default"
                                           " display format\n");
			scroll(act_command_area);
			wmove(act_command_area,command_win_size-3,0);
			return;
		}
		for(i=14;*(command+i)!='.';i++) {}
		prec=atoi(command+i+1);
		for(i=14;(*(command+i)!=' ')&&(*(command+i)!=',');i++) {}
		if((*(command+i)==' ')||(*(command+i)==',')) {
			if(*(command+i+1)=='0') {
				sprintf(float_dis,"%%-%do",atoi(command+14));
				return;
			}
			if(*(command+i+1)=='h') {
				sprintf(float_dis,"%%-%dX",atoi(command+14));
				return;
			}
		}
		sprintf(float_dis,"%%-%d.%df",atoi(command+14),prec);
	}
	else if(!strncmp(command+8,"double",6)) {
		if(*(command+14)=='\0') {
			strcpy(double_dis,DOUBLE_DEF);
			wprintw(act_command_area,"Reseting double");
			scroll(act_command_area);
			wmove(act_command_area,command_win_size-3,0);
			return;
		}
		for(i=15;*(command+i)!='.';i++) {}
		prec=atoi(command+i+1);
		for(i=15;(*(command+i)!=' ')&&(*(command+i)!=',');i++) {} 
		if((*(command+i)==' ')||(*(command+i)==',')) {
			if(*(command+i+1)=='0') {
				sprintf(double_dis,"%%-%do",atoi(command+15));
				return;
			}
			if(*(command+i+1)=='h') {
				sprintf(double_dis,"%%-%dX",atoi(command+15));
				return;
			}
		}

		sprintf(double_dis,"@%-%d.%d1f",atoi(command+15),prec);
	}
	else if(!strncmp(command+8,"integer",7)) {
		if(*(command+15)=='\0') {
			strcpy(int_dis,INT_DEF);
			wprintw(act_command_area,"Reseting integer");
			scroll(act_command_area);
			wmove(act_command_area,command_win_size-3,0);
			return;
		}
		for(i=16;*(command+i)!='.';i++) {}
		prec=atoi(command+i+1);
		for(i=16;(*(command+i)!=' ')&&(*(command+i)!=',');i++) {}
		if((*(command+i)==' ')||(*(command+i)==',')) {
			if(*(command+i+1)=='0') {
				sprintf(int_dis,"%%-%do",atoi(command+16));
				return;
			}
			if(*(command+i+1)=='h') {
				sprintf(int_dis,"%%-%dX",atoi(command+16));
				return;
			}
		}
		sprintf(int_dis,"%%-%dd",atoi(command+16),prec);
	}
	else if(!strncmp(command+8,"short",5)) {
		if(*(command+13)=='\0') {
			strcpy(short_dis,SHORT_DEF);
			wprintw(act_command_area,"Reseting short");
			scroll(act_command_area);
			wmove(act_command_area,command_win_size-3,0);
			return;
		}
		for(i=14;*(command+i)!='.';i++) {}
		prec=atoi(command+i+1);
		for(i=14;(*(command+i)!=' ')&&(*(command+i)!=',');i++) {}
		if((*(command+i)==' ')||(*(command+i)==',')) {
			if(*(command+i+1)=='\0') {
				sprintf(short_dis,"%%-%do",atoi(command+14));
				return;
			}
			if(*(command+i+1)=='h') {
				sprintf(short_dis,"%%-%d.4hX",atoi(command+14));
				return;
			}
		}
		sprintf(int_dis,"%%-%dd",atoi(command+14),prec);
	}
	else if(!strncmp(command+8,"address",7)) {
		if(*(command+15)=='\0') 
			wprintw(act_command_area,"You must specify 'o',"
                                            " 'h' or off\n");
		else if(*(command+16)=='\0') {
			if(*(command+17)!='f')
				out_mode=2;
			else
				out_mode=1;
		}
		else if(*(command+16)=='h')
			out_mode=3;
		else
			wprintw(act_command_area,"*****");
	}
	else {
		wprintw(act_command_area,"You have invoked...");
	}
}
