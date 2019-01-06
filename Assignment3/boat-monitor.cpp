#include "boat-monitor.h"
#include <vector> 
#include <pair> 
#include <semaphore.h>

class Monitor {
private:
    pthread_mutex_t monitor;
    pthread_cond_t  boat_wait_cond;
    pthread_cond_t  on_boat_cond;
    pthread_cond_t  can_wait_cond;
    pthread_cond_t  mis_wait_cond;
    std::vector<pair<int,char>> passengers; //isMissonary
    int get_id_of_cannibal_passenger();
    int get_number_of_missionaries_passengers();
    void select_passengers();
    bool passengers_are_ready();
    void awake_waiting_passengers();
    bool cannibalCanBoard();
    bool missionarCanBoard();
    void boardToBoat(int,char);
    std::vector<pair<int,char>>  get_passengers();
public:
    Monitor();
    void CannibalArrives(...);  // register to cross the river 

    void MissionaryArrives(...);  // register to cross the river 

    void BoatReady(long iteration);  // ready for the next round

    void BoatDone(long iteration);   // all people are on the other side

};

Monitor::Monitor(){
} 

bool cannibalCanBoard(){
    int can_count = 0;
    int mis_count = 0;
    if (this->passengers.size() == 3 ){
        return false;
    }
    
    for(auto const& value: this->passengers) {
        if (value.second == 'm'){
            ++mis_count;
        } else {
            ++can_count;
        }
    }

    // return (! (can_count == 1 && mis_count == 1))
    return (can_count != 1 || mis_count != 1)

}

bool missionarCanBoard(){
    int can_count = 0;
    int mis_count = 0;
    if (this->passengers.size() == 3 ){
        return false;
    }
    
    for(auto const& value: this->passengers) {
        if (value.second == 'm'){
            ++mis_count;
        } else {
            ++can_count;
        }
    }
    return (can_count != 2 )
}

void boardToBoat(int id,char type){
    passengers.push_back(std::make_pair(id,type)));
}



void CannibalArrives(long id){
    pthread_mutex_lock(&monitor);
    printf("%*cCannibal %d arrives.\n",id,' ',id);
    while ( ! cannibalCanBoard()){
        pthread_cond_wait(&can_wait_cond, &monitor); 
    }
    boardToBoat(id,'c');

    if (passengers_are_ready()){
        pthread_cond_signal(&boat_wait_cond); 
    }
    pthread_cond_wait(&on_boat_cond, &monitor); 
    pthread_mutex_unlock(&monitor);
}

void MissionaryArrives(long id){
    printf("%*cMissionary %d arrives.\n",id,' ',id);
    while ( ! missionarCanBoard()){
        pthread_cond_wait(&mis_wait_cond, &monitor); 
    }
    boardToBoat(id,'m');

    if (passengers_are_ready()){
        pthread_cond_signal(&boat_wait_cond); 
    }
    pthread_cond_wait(&on_boat_cond, &monitor); 
    pthread_mutex_unlock(&monitor);
}

void Monitor::awake_waiting_passengers(){

}

bool Monitor::passengers_are_ready(){
    return (this->passengers.size() == 3 );
}

std::vector<pair<int,char>>  Monitor::get_passengers(){
        // auto size = passengers.size();
        // int missionaries = get_number_of_missionaries_passengers();
        // if (missionaries == 3){
        //     printf("MONITOR(%ld): three missionaries (%d, %d, %d) are selected",iteration,passengers[0].first,passengers[1].first,passengers[2].first);
        // } else if (missionaries==2){
        //     int can_id =get_id_aaof_cannibal_passenger();
        //     printf("MONITOR(%ld): one cannibal (c) and two missionaries (m, m) are selected",iteration,passengers[can_id].first,passengers[(can_id + 1)%3].first,passengers[(can_id + 2)%3].first);
        // } else {
        //     printf("MONITOR(%ld): three cannibals (%d, %d, %d) are selected",iteration,passengers[0].first,passengers[1].first,passengers[2].first);
        // }
}


void Monitor::BoatReady(long iteration){
        pthread_mutex_lock(&monitor);

        awake_waiting_passengers();

        if( ! passengers_are_ready() ) {
            pthread_cond_wait(&boat_wait_cond, &monitor); 
        }
        auto array = get_passengers();
        printf("***** Boat load (%d): Passenger list (%c%d, %c%d, %c%d)",iteration, array[0].second,array[0].first , array[1].second,array[1].first , array[2].second,array[2].first);

        pthread_mutex_unlock(&monitor);

}

void Monitor::BoatDone(long iteration){
        pthread_mutex_lock(&monitor);
        printf("***** Boat load (%d): Completed",iteration);
        passengers.clear();
        pthread_cond_signal(&on_boat_cond); 
        pthread_cond_signal(&on_boat_cond); 
        pthread_cond_signal(&on_boat_cond); 
        pthread_mutex_unlock(&monitor);
}


int Monitor::get_id_of_cannibal_passenger(){
    int ans = 0 ;
    bool not_found = true;
    for(std::vector<T>::iterator it = this->passengers.begin(); not_found && it != this->passengers.end() ; ++it) {
        if (value.second == false){
            ans = std::distance(aVector.begin(), it); // index
            not_found= false;
        }
    }   
    return ans;
}

int Monitor::get_number_of_missionaries_passengers(){
    int ans = 0 ;
    for(auto const& value: this->passengers) {
        if (value.second == 'm'){
            ++ans;
        }
    }
    return ans;
}