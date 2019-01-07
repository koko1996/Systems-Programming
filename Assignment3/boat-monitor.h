#ifndef MONITOR_H
#define MONITOR_H

#include <cstdio>
#include <stdlib.h>
#include <pthread.h>
#include <vector> 
#include <utility>      // std::pair, std::make_pair
#include <semaphore.h>

class Monitor {

private:
    pthread_mutex_t monitor;            // mutex to provide mutual exclusion for the functions in this class
    pthread_cond_t  boat_wait_cond;     // condition variable for boat to wait on when waiting for passengers 
    pthread_cond_t  on_boat_cond;       // condition variable for passengers to wait on until crossing the river
    pthread_cond_t  can_wait_cond;      // condition variable for cannibals waiting for spot on the boat
    pthread_cond_t  mis_wait_cond;      // condition variable for missionaries waiting for spot on the boat
    std::vector<std::pair<int,char>> passengers; // container to hold the information of the passenger on the boat
    bool boat_is_active;                // boolean representing boat's status true if boat is active false otherwise

    int get_index_of_cannibal_passenger();        // get the index of first cannibal passenger in the list of passengers
    int get_number_of_missionary_passengers(); // get the number of missionary passengers
    bool passengers_are_ready();               // are all the passengers ready (constraints met) for crossing
    void awake_waiting_passengers();           // wake up all the passengers that are waiting for a spot on the boat
    bool cannibalCanBoard();                   // can a cannibal be added to boat (checks for constaints)
    bool missionaryCanBoard();                 // can a missornary be added to boat (checks for constaints)
    void boardToBoat(int,char);                // boat a member (cannibal or missonary) to the boat
    void print_passengers(long );              // print all the passengers on the boat before crossing the river

public:
    Monitor(int);                       // constructure

    void CannibalArrives(long id);      // mutual excluded (synchronized) register Cannibal to cross the river 

    void MissionaryArrives(long id);    // mutual excluded (synchronized) register Missionary to cross the river 

    void BoatReady(long iteration);     // mutual excluded (synchronized) boat is ready for the next round

    void BoatDone(long iteration);      // mutual excluded (synchronized) boat has finished crossing and all people are on the other side

    void set_boat_inactive();           // set boat's status to inactive 
    
    bool boat_active();                 // returns true if the boat is still active

};


#endif