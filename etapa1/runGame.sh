#!/bin/bash

g++ -std=c++11 MyBot.cpp -o MyBot.o
./halite -d "30 30" -s 42 "./MyBot.o"
