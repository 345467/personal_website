#include<iostream>
#include<fstream>
#include<windows.h>
#include<string>
using namespace std;

#define END SERVER
#include "CIRS_web.h"
using namespace Web;
#undef END

bool CIRS_exit=false;
int number_of_threads=0;

DWORD __stdcall SocketListener(LPVOID arg){
	Socket so=*(Socket *)arg;//To find out if I can connect to the client
	string msg=so.recv();
	if(msg=="致以最崇高的敬礼!"){
		so.send("紧紧握你的手!");
		MessageBox(NULL,"OK!","Caption",MB_OK);
	}else if(msg=="#close"){
		cout<<"exit"<<endl;
		CIRS_exit=true;
	}
	--number_of_threads;
	so.close();
	return 0;
}

void PrintInfo(){
	system("cls");
	cout<<"+=Common Information Record System Server V0.0.1=+"<<endl;
	cout<<"| Number of Threads:"<<number_of_threads<<"                            |"<<endl;
	cout<<"+================================================+"<<endl;
}

int main(){
	use_net();
	ServerSocket ss(port);
	while(!CIRS_exit){
		PrintInfo();
		Socket so=ss.accept();
		CreateThread(NULL,0,SocketListener,&so,0,NULL);
		++number_of_threads;
		Sleep(100);
	}
	ss.close();
	unuse_net();
	return 0;
}
