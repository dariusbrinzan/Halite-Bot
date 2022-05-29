#!/bin/bash

g++ -std=c++11 MyBot.cpp -o MyBot.o
./halite -d "40 40" "./MyBot.o" "./DBotv4_mac_64"
