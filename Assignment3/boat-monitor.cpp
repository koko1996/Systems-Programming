#include "boat-monitor.h"


// -----------------------------------------------------------
// FUNCTION Monitor
//    Constructure for Monitor class
// PARAMETER USAGE :
//    size - integer representing the number of passengers allowed on the boat at a time
// -----------------------------------------------------------
Monitor::Monitor(int size){
    pthread_mutex_init( &monitor, NULL);
    pthread_cond_init( &boat_wait_cond, NULL);
    pthread_cond_init( &on_boat_cond, NULL);
    pthread_cond_init( &can_wait_cond, NULL);
    pthread_cond_init( &mis_wait_cond, NULL);
    this->passengers.reserve(size);
    boat_is_active = true;
}

// -----------------------------------------------------------
// FUNCTION cannibalCanBoard
//    returns true if a cannibal can be added to the boat for the next river crossing as long as the boat
// is still active
// -----------------------------------------------------------
bool Monitor::cannibalCanBoard(){
    int can_count = 0;
    int mis_count = 0;
    if (this->passengers.size() == 3 ){
        return false;
    }

    for(auto const& value: this->passengers) {
        if (value.second == 'm'){   // is a missionary
            ++mis_count;
        } else {                    // is a cannibal
            ++can_count;
        }
    }

    return (can_count != 1 || mis_count != 1)  && boat_is_active;
}


// -----------------------------------------------------------
// FUNCTION missionaryCanBoard
//    returns true if a missonary can be added to the boat for the next river crossing as long as the boat
// is still active
// -----------------------------------------------------------
bool Monitor::missionaryCanBoard(){
    int can_count = 0;
    int mis_count = 0;
    if (this->passengers.size() == 3 ){
        return false;
    }
    
    for(auto const& value: this->passengers) {
        if (value.second == 'm'){   // is a missionary
            ++mis_count;
        } else {                    // is a cannibal
            ++can_count;
        }
    }

    return (can_count != 2 ) && boat_is_active;
}


// -----------------------------------------------------------
// FUNCTION boardToBoat
//    adds a cannibal or missonary to the boat for the next river crossing
// PARAMETER USAGE :
//    id - id of the passenger (either cannibal or missonary)
//    type -  'c' for cannibal and 'm' for missonary
// -----------------------------------------------------------
void Monitor::boardToBoat(int id,char type){
    passengers.push_back(std::make_pair(id,type));
}


// -----------------------------------------------------------
// FUNCTION CannibalArrives
//    mutually excluded function that adds the arriving cannibal to the boat for 
// next river crossing. It blocks until a spot is available and then blocks while the cannibal is
// on the boat
// PARAMETER USAGE :
//    id - id of the connibal 
// -----------------------------------------------------------
void Monitor::CannibalArrives(long id){
    pthread_mutex_lock(&monitor);                           // mutex lock
    
    // critical section
    printf("%*cCannibal %d arrives.\n",id,' ',id);
    while ( ! cannibalCanBoard()){               // check if cannibal can be added to the boat
        if (!boat_is_active){                   // exit if boat is not active anymore
                pthread_mutex_unlock(&monitor); //unlock the mutex  bofore exiting
                return;
        }
        // sleep until the next (or current) river crossing is over and then try again to get a spot on the boat
        pthread_cond_wait(&can_wait_cond, &monitor);
    }
    // since this cannibal can now be Boarded to the boat, add it to the list of passenger
    boardToBoat(id,'c');

    if (passengers_are_ready()){                // if this is the last passenger added to the list of passenger
        pthread_cond_signal(&boat_wait_cond);   // signal the boat (just in case it is sleeping)
    }


    pthread_cond_wait(&on_boat_cond, &monitor); // wait until corssing is done

    pthread_mutex_unlock(&monitor);                          // mutex unlock
}


// -----------------------------------------------------------
// FUNCTION MissionaryArrives
//    mutually excluded function that adds the arriving missonary to the boat for 
// next river crossing. It blocks until a spot is available and then blocks while the 
// missonary is on the boat
// PARAMETER USAGE :
//    id - id of the missonary 
// -----------------------------------------------------------
void Monitor::MissionaryArrives(long id){   
    pthread_mutex_lock(&monitor);                                  // mutex lock

    // critical section
    printf("%*cMissionary %d arrives.\n",id,' ',id);
    while ( ! missionaryCanBoard()){            // check if missonary can be added to the boat
        if (!boat_is_active){                   // exit if boat is not active anymore
            pthread_mutex_unlock(&monitor);     //unlock the mutex bofore exiting
            return;
        }
        // sleep until the next (or current) river crossing is over and then try again to get a spot on the boat
        pthread_cond_wait(&mis_wait_cond, &monitor); 
    }
    // since this missonary can now be Boarded to the boat, add it to the list of passenger
    boardToBoat(id,'m');

    if (passengers_are_ready()){            // if this is the last passenger added to the list of passenger
        pthread_cond_signal(&boat_wait_cond); // signal the boat (just in case it is sleeping)
    }
    pthread_cond_wait(&on_boat_cond, &monitor);     // wait until corssing is done
    
    pthread_mutex_unlock(&monitor);                                // mutex unlock

}


// -----------------------------------------------------------
// FUNCTION get_index_of_cannibal_passenger
//    returns the index of first cannibal passenger in the list of passengers 
// -----------------------------------------------------------
int Monitor::get_index_of_cannibal_passenger(){    
    int ans = 0 ;
    bool not_found = true;

    for (int i =0; not_found &&  i < this->passengers.size();++i){
        if (this->passengers[i].second == 'c'){
            not_found= false;
            ans = i;
        }
    }   
    return ans;
}

// -----------------------------------------------------------
// FUNCTION get_number_of_missionary_passengers
//    returns the number of missionary passengers on the list
// -----------------------------------------------------------
int Monitor::get_number_of_missionary_passengers(){
    int ans = 0 ;
    for(auto const& value: this->passengers) {
        if (value.second == 'm'){
            ++ans;
        }
    }
    return ans;
}

// -----------------------------------------------------------
// FUNCTION awake_waiting_passengers
//    awakes all the waiting passengers so that they can fight for spot on the boat for next river crossing 
// -----------------------------------------------------------
void Monitor::awake_waiting_passengers(){
   pthread_cond_broadcast(&mis_wait_cond);   
   pthread_cond_broadcast(&can_wait_cond);  
}

// -----------------------------------------------------------
// FUNCTION passengers_are_ready
//    returns true if all the passengers are ready and boat should start crossing the river
// -----------------------------------------------------------
bool Monitor::passengers_are_ready(){
    return (this->passengers.size() == 3 );
}

// -----------------------------------------------------------
// FUNCTION print_passengers
//    Montor function that prints the selected passengers of the next river crossing
// PARAMETER USAGE :
//    iteration - the current iteration number
// -----------------------------------------------------------
void  Monitor::print_passengers(long iteration){
        int missionaries = get_number_of_missionary_passengers();
        if (missionaries == 3){
            printf("MONITOR(%ld): three missionaries (%d, %d, %d) are selected\n",iteration,passengers[0].first,passengers[1].first,passengers[2].first);
        } else if (missionaries==2){
            int can_index =this->get_index_of_cannibal_passenger();
            printf("MONITOR(%ld): one cannibal (%d) and two missionaries (%d, %d) are selected\n",iteration,passengers[can_index].first,passengers[(can_index + 1)%3].first,passengers[(can_index + 2)%3].first);
        } else {
            printf("MONITOR(%ld): three cannibals (%d, %d, %d) are selected\n",iteration,passengers[0].first,passengers[1].first,passengers[2].first);
        }
}


// -----------------------------------------------------------
// FUNCTION BoatReady
//    mutually excluded function that makes sure that the passengers are ready for river crossing
// starts the boat (starts the crossing)
// PARAMETER USAGE :
//    iteration - the current iteration number
// -----------------------------------------------------------
void Monitor::BoatReady(long iteration){
        pthread_mutex_lock(&monitor);           // mutex lock       

        // since there must be one boat thread calling this function no need to use while instead of if
        // as there can be at most one thread waiting on boat_wait_cond conditional variable 
        if( ! passengers_are_ready() ) {        // check if passenger list is ready        
            // awake the passengers and let the fight for the spots left on the boat for next iteration
            awake_waiting_passengers();      
            // sleep until passengers are ready
            pthread_cond_wait(&boat_wait_cond, &monitor); 
        }

        print_passengers(iteration);    // print the list of passengers chosen for crossing

        printf("***** Boat load (%d): Passenger list (%c%d, %c%d, %c%d)\n",iteration, this->passengers[0].second,this->passengers[0].first , this->passengers[1].second,this->passengers[1].first , this->passengers[2].second,this->passengers[2].first);
        
        pthread_mutex_unlock(&monitor);        // mutex unlock
}

// -----------------------------------------------------------
// FUNCTION BoatDone
//    mutually excluded function that indicates the boat has crossed the river for the iteration times
// and it does the neccessary clean up. It cleans up the list of passengers for next iteration and releases 
// the threads that were on the boat 
// PARAMETER USAGE :
//    iteration - the current iteration number
// -----------------------------------------------------------
void Monitor::BoatDone(long iteration){
        pthread_mutex_lock(&monitor);           // mutex lock       

        printf("***** Boat load (%d): Completed\n",iteration);
        
        // release the threads that were on the boat for the last crossing
        for(int i =0; i < this->passengers.size();++i){
            pthread_cond_signal(&on_boat_cond);     
        }
        this->passengers.clear();    // clean up the list of passengers 
        
        // awake all the cannibals and missonaries so that they can fight for spots for next river crossing
        awake_waiting_passengers();  

        pthread_mutex_unlock(&monitor);           // mutex unlock       
}

// -----------------------------------------------------------
// FUNCTION set_boat_inactive
//   sets boat's status to inactive
// -----------------------------------------------------------
void Monitor::set_boat_inactive(){
    this->boat_is_active=false;
}

// -----------------------------------------------------------
// FUNCTION boat_active
//   returns true if boat's status is active
// -----------------------------------------------------------
bool Monitor::boat_active(){
    return this->boat_is_active;
}

