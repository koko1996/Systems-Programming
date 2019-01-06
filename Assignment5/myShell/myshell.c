#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include<sys/wait.h> 

extern char **get_line();

void print_array(char **ptr){
	while (*ptr != NULL){
		printf("%s\n",*ptr);
		*ptr++;
	}

}
void handle_sigchld(int sig) {
  printf("Child Signal Handler has been called\n");
  int saved_errno = errno;
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
  errno = saved_errno;
}

/* function declaration */
int fork_commands(char **args, char *name, int direction, int check);
int array_length(char **arr);
char * last_string(char **arr); 
char * get_redirection_file_name(char **arr, int t);
void truncate_array(char **arr,int t);

int main() {
  char **args; 
  char *last;  
  char *name;
  int wait;
  int length;
  int dir;
  signal(SIGCHLD, handle_sigchld);
  
  while(1) {	  
    length =0;
    dir=0;
    wait=1;
    args = get_line();
	
	// Background execution
	length = array_length(args);
	if(length>0){
		last = last_string(args);
		if(length>1 &&  strcmp(last,"&") == 0){
			printf("Background execution  \n");	
			*(args+length-1)=NULL ;
			wait=0;
		}	
		dir = redirect_output(args);
		if(length>2 && (dir ==1 || dir == 2)){
			name=get_redirection_file_name(args,dir);
			printf("%s truncating\n",name);	
			truncate_array(args,dir);
			print_array(args);
		}

		if( strcmp(args[0],"exit") == 0){
		    printf("Exiting \n");
			exit(0);
		}
		else{	    
			printf("Status: %d\n",fork_commands(args,name,dir,wait));
		}
	}
		  
  }
  
}



int fork_commands(char **args, char *name, int direction, int check){
	// direction 1 means redirect output
	// direction 2 means redirect intput
	// else standard input output
    printf("---Fork commands \n");	
    pid_t pid1;
    int status;	
    FILE *fp;
    if ((pid1 = fork()) < 0) {  // failed to fork the child process
          printf("Failed to fork\n");
    }
    else if (pid1 == 0){ // child process
		printf("Executing command as child process\n");
		if (direction == 1){
		 	fp = freopen(name, "w", stdout);
		} else if (direction == 2){
		 	fp = freopen(name, "r", stdin);
		}
		execvp( *args, args);
		printf("Executing command FAILED \n");
	    	exit(errno);
     }
     else{ // parent process	
		if(check){
			printf("Waiting for the child to exit \n");
			wait(&status);
		}
     }
     
     return status;
}


char* last_string(char **arr){
	printf("---Last String  \n");		
	if(*arr == NULL){
		return NULL	;
	}
	else {
		while(*(arr+1)!= NULL){
			*arr++;
		}
		return *arr;
	}
} 

char * get_redirection_file_name(char **arr,int dir){
	printf("---get_redirection_file_name  \n");	
	char *ptr;
	char *ans;	

	if(dir == 1){
		ptr=">";
	} else if (dir == 2 ){
		ptr ="<";	
	}


	if(*arr == NULL){
		return ans;
	}
	else {
		while(*(arr+1)!= NULL){
			if (strcmp(*arr,ptr)==0){
				ans=*(arr+1);	
				break;		
			}
			*arr++;
		}
		return ans;
	}

}
int redirect_output(char **arr){
	int ans=0;
	printf("---redirect output  \n");		
	if(*arr == NULL){
		return ans;
	}
	else {
		while(*(arr+1)!= NULL){
			if (strcmp(*arr,">")==0){
				ans=1;		
				break;	
			} else if (strcmp(*arr,"<")==0){
				ans=2;
				break;			
			}else if (strcmp(*arr,"|")==0){
				ans =3;
				break;
			}

			*arr++;			
		}
		return ans;
	}
}

int array_length(char **arr){
	printf("---Array Length  \n");	
	int count=0;
	if(*arr == NULL){
		return 0;
	}
	else {
		count++;
		while(*(arr+1)!= NULL){
			*arr++;
			count++;
		}
		return count;
	}   			
} 


void truncate_array(char **arr,int dir){
	printf("---truncate_array  \n");
	char *ptr;
	if(dir == 1){
		ptr=">";
	} else if (dir == 2 ){
		ptr ="<";	
	}

	
	if(*arr == NULL){
	}
	else {
		while(*(arr+1)!= NULL){
			if (strcmp(*arr,ptr)==0){
				*arr=NULL;	
				break;		
			}
			*arr++;
		}
	}
}


