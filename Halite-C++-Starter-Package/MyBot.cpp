#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>
#include <queue>

#include "hlt.hpp"
#include "networking.hpp"

using namespace hlt;
using namespace std;


int valueSite(Site &site) {
    unsigned char strength;
    int value = 257;
    if (site.owner == 0) {
        strength = site.strength;
    } else {
        strength = 0;
    }
    //if (site.production == 0 && strength != 0) return 1;
    if (site.production > 1) {
        value += site.production - strength/site.production;
    } else {
        value += site.production - strength;
    }
    return value;
    //return 257 + site.production - strength / (std::max)(site.production, (unsigned char)(1));
}

int valueSite(Location &loc, GameMap &presentMap, char myID) { 
    int totalVal = 0;
    Site site = presentMap.getSite(loc);
    if (site.owner != myID) {
        totalVal += valueSite(site);
    }

    int totalProd = site.production;
    for (char d = 0; d < 5; d++) {
    	Site neighbour = presentMap.getSite(loc, d);
    	if (neighbour.owner != myID) {
    		int neighbourVal = valueSite(neighbour);
    		totalVal += (neighbourVal > 250 ? neighbourVal - 250 : 0);
                if (neighbour.owner != 0) 
                {
                    totalVal += 10;
                }
    	}
        totalProd += neighbour.production;
    }
    return totalVal;
}

int findNearestEnemyDirection(GameMap presentMap, Location loc, unsigned char myID) {
    int direction = NORTH;

	float maxDistance = 2000;
    
    for (char d = 0; d < 5; d++) {
        float distance = 0;
        Location current = loc;
        Site site = presentMap.getSite(current, d);
        while (site.owner == myID && distance < maxDistance) {
            distance++;
            current = presentMap.getLocation(current, d);
            site = presentMap.getSite(current);
        }
        
        if (distance < maxDistance) {
            direction = d;
            maxDistance = distance;
        }
    }
    
    return direction;
}

void updateNextMap(Location loc, GameMap nextMap, GameMap presentMap,
	unsigned char myID, char d) {
    Site current = presentMap.getSite(loc);
    Site next = nextMap.getSite(nextMap.getLocation(loc, d));
    
    if (next.owner == myID) {
        nextMap.getSite(nextMap.getLocation(loc, d)).strength += current.strength;
        presentMap.getSite(loc).strength = 0;
    } else {
        if (current.strength >= next.strength) {
            nextMap.getSite(nextMap.getLocation(loc, d)).owner = myID;
            nextMap.getSite(nextMap.getLocation(loc, d)).strength = presentMap.getSite(loc).strength -
				nextMap.getSite(nextMap.getLocation(loc, d)).strength;
            presentMap.getSite(loc).strength = 0;
        }
    }
}

unsigned short valueBordMove(GameMap &presentMap, const Location &loc, char direction) {
    Location newLoc = presentMap.getLocation(loc, direction);
    Site s = presentMap.getSite(newLoc);
    Site c = presentMap.getSite(loc);
    if (s.owner != c.owner) {
    	// production formula
    	unsigned short damage = valueSite(newLoc, presentMap, c.owner);
    	//if (damage == 1) return 1;
        //if (s.production == 0) damage -= 10;
    	// wait penalty
        if (s.strength >= c.strength) {
    		unsigned short penalty = (s.strength - c.strength) / (std::max)(s.production, (unsigned char)(1));
    		if (damage > penalty) damage -= penalty;
    		else return 0;
    	}
    	// over kill damage
        unsigned short overkill = 0;
    	bool hasEnemy = false;
        for (int d = 0; d<5; d++) {
    		hlt::Site sd = presentMap.getSite(newLoc, d);
    		if (sd.owner != 0 && sd.owner != c.owner) {
    			overkill += std::min(c.strength, sd.strength);
                hasEnemy = true;
    		}
    	}
        if (hasEnemy && overkill <= c.strength && s.strength > 0) {
            damage  = 0;
        } else {
            damage += overkill;
        }
    	return damage;
    } else
        return 0;
}

unsigned short maxBordMove(hlt::GameMap & presentMap, const hlt::Location & loc) {
    unsigned short maxVal = 0;
    for (int d = 0; d < 5; d++) {
    	unsigned short val = valueBordMove(presentMap, loc, d);
    	if (val > maxVal)
    	{
    		maxVal = val;
    	}
    }
    return maxVal;
}

unsigned short valueInnerMove (hlt::GameMap &presentMap, const hlt::Location &loc, unsigned char direction) {
    if (presentMap.getSite(loc).strength <= presentMap.getSite(loc).production * 4) return 0;
    bool hasEnemy = false;
    for (char d =0; d <5; d++) {
        hlt::Site s = presentMap.getSite(loc, d);
        if (s.owner == 0 && s.strength == 0) hasEnemy = true;
    }
    if (hasEnemy) return 0;
    
    hlt::Location newLoc = presentMap.getLocation(loc, direction);
    unsigned short val = maxBordMove(presentMap, newLoc);
    unsigned short sum = presentMap.getSite(loc).strength + presentMap.getSite(newLoc).strength;
    unsigned char myID = presentMap.getSite(loc).owner;
    unsigned char distance = 0;
    unsigned short maxDistance = (std::min)(presentMap.width, presentMap.height) / 2;
    while(presentMap.getSite(newLoc).owner == myID && distance < maxDistance) {
    	newLoc = presentMap.getLocation(newLoc, direction);
        ++distance;
    }
    unsigned short valueBoard = valueSite(newLoc, presentMap, myID);
    unsigned short distPenalty = 5 * distance;
    if (valueBoard > distPenalty) {
        valueBoard -= distPenalty;
    } else {
        valueBoard = 0;
    }
    if (val > 5) {
        val -= 5;
    } else {
        val = 0;
    }
    if (hasEnemy) return valueBoard; 
    else return std::max(val, valueBoard);
}


unsigned short valueMove(hlt::GameMap & presentMap, const hlt::Location & loc, unsigned char direction) {
    hlt::Location newLoc = presentMap.getLocation(loc, direction);
    if (presentMap.getSite(loc).owner != presentMap.getSite(newLoc).owner) {
    	return valueBordMove(presentMap, loc, direction);
    } else {
    	return valueInnerMove(presentMap, loc, direction);
    }
}

unsigned char boarderOptimize(hlt::GameMap & presentMap, const hlt::Location & loc) {
    hlt::Site s = presentMap.getSite(loc);
    unsigned short valueMax = ((s.strength + s.production > 200 )? 0 : (1 + s.production * 10));
    unsigned char direction = STILL;
    for (int d = 0; d < 5; d++) {
    	unsigned short value = valueMove(presentMap, loc, d);
    	if (value > valueMax) {
    		direction = d;
    		valueMax = value;
    	}
    }
    return direction;
}

Move assignMove(Location t, GameMap nextMap, GameMap presentMap,
	unsigned char myID) {
    Site site = presentMap.getSite(t);
    if (site.strength == 0) {
         return Move{t, STILL};
    }
	
    unsigned char direction = STILL;
	bool isBorder = false;
	for (int i = 1; i < 5; i++) {
		if (nextMap.getSite(presentMap.getLocation(t, i)).owner != myID) {
			isBorder = true;
		}
	}
    
    if (isBorder) {
        unsigned char goTo = boarderOptimize(presentMap, t);
        Site next = presentMap.getSite(t, goTo); 
        if (next.strength >= site.strength && next.owner != myID) {
            return Move{t, STILL};
        } else {
            return Move{t, goTo};
        }
    }

    if(site.strength <= site.production * 5) {
        direction = STILL;
        return Move{t, STILL};
    }

    direction = findNearestEnemyDirection(presentMap, t, myID);
    return Move{t, direction};
}


            
int main() {
	unsigned char myID;

	GameMap presentMap;
	GameMap nextMap;

    srand(time(NULL));
    
    cout.sync_with_stdio(0);
    getInit(myID, presentMap);
    sendInit("Best Bot");
    
    set<Move> moves;


    unsigned int halfMap = presentMap.width * presentMap.height / 2;
    while(true) {
        moves.clear();
        
        getFrame(presentMap);
        
        nextMap = presentMap;
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                hlt::Location t = { b, a};
                if (presentMap.getSite(t).owner == myID) {
					bool movedPiece = false;
                    moves.insert(assignMove(t, nextMap, presentMap, myID));
                }
            }
        }
        sendFrame(moves);
    }
    return 0;
}