all:ClientMain SeverMain
ClientMain: ClientMain.cpp MClient.cpp
	g++ ClientMain.cpp MClient.cpp -o ClientMain
SeverMain: SeverMain.cpp MSever.cpp
	g++ SeverMain.cpp MSever.cpp -o SeverMain
