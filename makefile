all:ClientMain SeverMain
ClientMain: ClientMain.cpp MClient.cpp MClient.h Cmd.h
	g++ ClientMain.cpp MClient.cpp -o ClientMain -l jsoncpp
SeverMain: SeverMain.cpp MSever.cpp MSever.h Cmd.h
	g++ SeverMain.cpp MSever.cpp -o SeverMain -l jsoncpp
clean:
	rm test SeverMain ClientMain