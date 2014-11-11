all:
	make install 
	make runThree 
	make runFour 
	make runFive

install:
	g++ -std=c++11 ./src/main.cpp -o ./bin/balls

runThree:
	./bin/balls 3 ./sample/triangle3

runFour:
	./bin/balls 4 ./sample/triangle4

runFive:
	./bin/balls 5 ./sample/triangle5

