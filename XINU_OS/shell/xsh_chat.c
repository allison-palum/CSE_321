/** 
 * @file     xsh_chat.c
 * @provides xsh_chat
 *
 * $Id: xsh_chat.c 226 2007-07-12 01:18:27Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	char id[30];
	char pwd[30];
} Accounts;



command xsh_chat(ushort stdout, ushort stdin, ushort stderr, ushort nargs, char *args[]){	
	if(nargs == 2 && strncmp(args[1], "--help", 6) == 0) {
		fprintf(stdout, "Usage: clear\n");
		fprintf(stdout, "Clears the terminal.\n");
		fprintf(stdout, "\t--help\t display this help and exit\n");
		return SYSERR;
	}
	
	int i;
	int user = 0;	
	Accounts accounts[2];
	strcpy(accounts[0].id, "allison");
	strcpy(accounts[0].pwd, "password");
	strcpy(accounts[1].id, "usr1");
	strcpy(accounts[1].pwd, "usr1_pwd");
	int status = 0;
	char input_id[30];
	char input_pwd[30];
	
	fprintf(stdout, "Enter your ID: ");
	read(stdin, input_id, 30);
	fprintf(stdout, "Enter the password: ");
	read(stdin, input_pwd, 30);
	
	//Login check
	for(i = 0; i < 2; i++) {
		if(strncmp(accounts[i].id, input_id, 3) == 0 && strncmp(accounts[i].pwd, input_pwd, 3)  == 0) { 
			fprintf(stdout, "\n--------------\n");
			fprintf(stdout, "Log in success\n");
			fprintf(stdout, "--------------\n");
			fprintf(stdout, "Welcome %s!!!!!\n\n", accounts[i].id);
			fprintf(stdout, "Type ""bye"" to quit\n\n");
			input_id[3] = '\0';
			status = 1;
			break;
		}
	}
	if(status == 0) {
		fprintf(stdout, "\n--------------\n");
		fprintf(stdout, "Log in fail\n");
		fprintf(stdout, "--------------\n\n");
	}
		
	char message[30];
	
	while(1) {
		if(user == 0) {
			fprintf(stdout, "User 0 -- Enter your message: \n");
			read(stdin, message, 30);
			write(TTY1, message, strlen(message));
			if(message == "bye") {
				break;
			}
			user = 1;
		}
		else if(user == 1) {
			fprintf(stdout, "User 1 -- Enter your message: \n");
			read(stdin, message, 30);
			write(TTY0, message, strlen(message));
			if(message == "bye") {
				break;
			}
			user = 0; 
		}
	}
	
	/*there were no errors so, return OK */
	return OK;
}
