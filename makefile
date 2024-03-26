all:ClientMain SeverMain
ClientMain: ClientMain.cpp MClient.cpp MClient.H
	g++ ClientMain.cpp MClient.cpp -o ClientMain
SeverMain: SeverMain.cpp MSever.cpp MSever.h
	g++ SeverMain.cpp MSever.cpp -o SeverMain
clean:
	rm test SeverMain ClientMain