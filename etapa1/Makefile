.PHONY: build run clean

build: MyBot

MyBot: MyBot.cpp
	g++ -std=c++11 -o MyBot MyBot.cpp

run: build
	./MyBot

clean:
	rm -f MyBot
