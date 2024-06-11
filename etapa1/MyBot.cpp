#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"

using namespace std;


void get_elem_start(set<hlt::Move>& moves, hlt::GameMap& presentMap,
            set<hlt::Location>& mySites, unsigned char& myID, vector<hlt::Location>& armata) {

    for (unsigned short i = 0; i <presentMap.height; i++) { // inaltimea - y
        for (unsigned short j = 0; j < presentMap.width; j++) { // latimea - x
            if (presentMap.getSite({ j, i }).owner == myID) {
                hlt::Location newSite = { j, i };

                if (mySites.find(newSite) == mySites.end()) {
                    // This is a new location, add it to the set and the vector
                    mySites.insert(newSite);
                    armata.push_back(newSite);
                }
            }
        }
    }

    unsigned short last_elem_X = armata[armata.size() - 1].x;
    unsigned short last_elem_Y = armata[armata.size() - 1].y;

    // verificam patratele din jurul ultimului element din "armata"
    auto up = presentMap.getSite({last_elem_X, last_elem_Y}, NORTH);
    auto down = presentMap.getSite({last_elem_X, last_elem_Y}, SOUTH);
    auto left = presentMap.getSite({last_elem_X, last_elem_Y}, WEST);
    auto right = presentMap.getSite({last_elem_X, last_elem_Y}, EAST);

    unsigned char up_strength = up.strength;
    unsigned char down_strength = down.strength;
    unsigned char left_strength = left.strength;
    unsigned char right_strength = right.strength;

    if(up.owner == myID){
        up_strength = (unsigned char) 250;
    }
    if(down.owner == myID){
        down_strength = (unsigned char) 250;
    }
    if(left.owner == myID){
        left_strength = (unsigned char) 250;
    }
    if(right.owner == myID){
        right_strength = (unsigned char) 250;
    }

    unsigned char min_strength = min({up_strength, down_strength, left_strength, right_strength});

    // pentru fiecare element mai putin ultimul din "armata" calculam directia
    for (int i = 1; i < armata.size(); i++) {
        short direction_x = armata[i].x - armata[i-1].x;
        short direction_y = armata[i].y - armata[i-1].y;

        if (presentMap.getSite({ last_elem_X, last_elem_Y }, STILL).strength < min_strength / 2) {
            moves.insert({ { armata[i-1].x, armata[i-1].y }, STILL });
            continue;
        }

        if (direction_x == 1) {
            moves.insert({ { armata[i-1].x, armata[i-1].y }, EAST });
        } else if (direction_x == -1) {
            moves.insert({ { armata[i-1].x, armata[i-1].y }, WEST });
        } else if (direction_y == 1) {
            moves.insert({ { armata[i-1].x, armata[i-1].y }, SOUTH });
        } else if (direction_y == -1) {
            moves.insert({ { armata[i-1].x, armata[i-1].y }, NORTH });
        }
    }


    if (presentMap.getSite({ last_elem_X, last_elem_Y }, STILL).strength < min_strength) {
        moves.insert({ { last_elem_X, last_elem_Y }, (unsigned char) STILL });
        return;
    }

    if (min_strength == up.strength) {
        moves.insert({ {last_elem_X, last_elem_Y}, (unsigned char) NORTH });
    } else if (min_strength == down.strength) {
        moves.insert({ { last_elem_X, last_elem_Y }, (unsigned char) SOUTH });
    } else if (min_strength == left.strength) {
        moves.insert({ { last_elem_X, last_elem_Y }, (unsigned char) WEST });
    } else if (min_strength == right.strength) {
        moves.insert({ { last_elem_X, last_elem_Y }, (unsigned char) EAST });
    }
}


int main() {
    srand(time(NULL));

    std::cout.sync_with_stdio(0);
    /////////////////////

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);

    sendInit("MyBot");

    std::set<hlt::Move> moves;
    int contor = 15; // pentru primele 15 frame-uri, puterile comune se vor uni
    set<hlt::Location> mySites;
    std::vector<hlt::Location> armata;
    while(true) {
        moves.clear();

        getFrame(presentMap);

        if (contor > 0) {
            get_elem_start(moves, presentMap, mySites, myID, armata);
            contor--;
            sendFrame(moves);
            continue;
        }

        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    auto up = presentMap.getSite(presentMap.getLocation({b,a},1));
                    auto down = presentMap.getSite(presentMap.getLocation({b,a},3));
                    auto left = presentMap.getSite(presentMap.getLocation({b,a},4));
                    auto right = presentMap.getSite(presentMap.getLocation({b,a},2));

                    float up_quality = (float)up.production/up.strength;
                    float down_quality = (float)down.production/down.strength;
                    float left_quality = (float)left.production/left.strength;
                    float right_quality = (float)right.production/right.strength;

                    if(up.owner == myID){
                        up_quality = -1;
                    }
                    if(down.owner == myID){
                        down_quality = -1;
                    }
                    if(left.owner == myID){
                        left_quality = -1;
                    }
                    if(right.owner == myID){
                        right_quality = -1;
                    }

                    float max_quality = max({up_quality, down_quality, left_quality, right_quality});

                    int val1;


                    if (presentMap.getSite({b,a}).strength < 10) {
                        val1 = 0;
                    } 
                    else if(max_quality > -1) {
                        if (max_quality == up_quality && up.owner != myID){
                            val1 = 1;
                        }
                        else if (max_quality == down_quality && down.owner != myID){
                            val1 = 3;
                        }
                        else if (max_quality == left_quality && left.owner != myID){
                            val1 = 4;
                        }
                        else if (max_quality == right_quality && right.owner != myID){
                            val1 = 2;
                        }
                    } else {
                        val1 = rand() % 3;
                        if (val1 == 0) val1 = 2;
                    }
                    moves.insert({ { b, a }, (unsigned char) val1 });
                }
            }
        }

        sendFrame(moves);
    }

    return 0;
}