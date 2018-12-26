#include <stdio.h>
#include  <signal.h>

void myInterruptHandler (int sig) {
	 char  c,d;
	 int correct = 0;
	 printf("Shell");
	 c = getchar();
	 d = getchar();
	 d = getchar();
	 while (correct==0){
	 if (c == 'e' || c == 'E'){
		 correct = 1;
		 exit(0);
	 } else if (c == 'r' || c == 'R'){
		 		correct = 1;
	 } else if (c == 'k' || c == 'K'){
		 		correct = 1;
	 } else if (c == 's' || c == 'S'){
		 		correct = 1;
	 } else if (c == 'o' || c == 'O'){
		 		correct = 1;
	 } else if (c == 't' || c == 'T'){
		 		correct = 1;
	 } else {

	 }

}

int main() {
	 signal(SIGINT, myInterruptHandler);
	 return 0;
}
