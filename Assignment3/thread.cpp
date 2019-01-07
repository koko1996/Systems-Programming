#include "thread.h"

// The monitor object to handle river crossing
static Monitor * boat_monitor = new Monitor(NUMBER_OF_PASSENGERS_AT_A_TIME);

// -----------------------------------------------------------
// FUNCTION cannibal
//    Main method for cannibals
// PARAMETER USAGE :
//    id_number - integer representing the id of the cannibal
// -----------------------------------------------------------
void *cannibal(void * id_number) {
	long id = (long) id_number;
	printf("%*cCannibal %d starts.\n",id,' ',id);
    while (boat_monitor->boat_active()) {
        sleep(2);                           // take a rest
        boat_monitor->CannibalArrives(id);  // register to cross the river 
        // do other stuffs and come back for another river crossing
    }
}

// -----------------------------------------------------------
// FUNCTION cannibal
//    Main method for missionaries
// PARAMETER USAGE :
//    id_number - integer representing the id of the missionary
// -----------------------------------------------------------
void *missionary(void * id_number) {
	long id = (long) id_number;
	printf("%*cMissionary %d starts.\n",id,' ',id);
    while (boat_monitor->boat_active()) {
        sleep(2);                               // take a rest
        boat_monitor->MissionaryArrives(id);    // register to cross the river 
        // do other stuffs and come back for another river crossing
    }
}


// -----------------------------------------------------------
// FUNCTION cannibal
//    Main method for the boat
// PARAMETER USAGE :
//    id_number - integer representing the number of iteration of the boat (river crossings)
// -----------------------------------------------------------
void *boat(void * number_iter) {
    long iteration = (long) number_iter;
    int i =1;
	printf("***** BOAT thread starts.\n");
    while (i <= iteration ) {
        sleep(2);                           // take a rest
        boat_monitor->BoatReady(i);         // ready for the next round
        sleep(3);                           // row the boat
        if (i == iteration){                // indicate that the boat has finished all the iterations 
            boat_monitor->set_boat_inactive();  // to cannibals and missionaries to exit
        }
        boat_monitor->BoatDone(i);          // all people are on the other side 
        ++i;                                // come back for another river crossing 
    }
    printf("***** BOAT %d crosses have been made.\n",iteration);
    printf("***** BOAT This river cross is closed indefinitely for renovation.\n");
}






