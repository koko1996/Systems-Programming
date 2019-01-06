#include "thread.h"


void *cannibal(void * id_number) {
	long id = (long) id_number;
	printf("%*cCannibal %d starts.\n",id,' ',id);
    // while (1) {
    //      Delay();               // take a rest 
    //      CannibalArrives(...);  // register to cross the river 
    //      // other stuffs 
    //      // come back for another river crossing
    // }
}


void *missionary(void * id_number) {
	long id = (long) id_number;
	printf("%*cMissionary %d starts.\n",id,' ',id);
    // while (1) {
    //      Delay();               // take a rest 
    //      MissionaryArrives(...);  // register to cross the river =
    //      // other stuffs 
    //      // come back for another river crossing
    // }
}



void *boat(void * number_iter) {
    long iteration = (long) number_iter;
    int i =1;
	printf("***** BOAT thread starts.\n");
    while (i <= iteration ) {
        //  sleep(2);                     // take a rest
        //  BoatReady(iteration);         // ready for the next round
        //  sleep(3);                     // row the boat
        //  BoatDone();                   // all people are on the other side
         // come back for another river crossing 
         ++i;
    }
}






