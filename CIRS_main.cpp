#include<iostream>
#include<windows.h>
#include<string>
#include<conio.h>
#include<fstream>
#include<vector>
#include<map>
using namespace std;

#define END CLIENT
#include "CIRS_web.h"
using namespace Web;
#undef END

Socket *server;

void NetInit(string host_name,int port){
	use_net();//To find out if I can connect to the server
	server=new Socket(host_name.c_str(),port); 
	server->send("致以最崇高的敬礼!");
	if(server->recv()=="紧紧握你的手!")
		MessageBox(NULL,"OK!","DEBUG",MB_OK);
	delete server;
	unuse_net();
	exit(0);
}
void NetDestory(){
	delete server;
	unuse_net();
}

void NetSend(string msg){
	//server->send(msg);
}
string NetGet(){
	return "";//server->recv();
}

void NetInit(string hostname,int port);
void NetDestory();

void NetSend(string msg);
string NetGet();

struct Permission{
	string name;
	string id;
	int level;
	int max_level;
};

struct User{
	string name;
	string nick_name;
	string pass_word;
	vector<Permission> permissions;
};

void SendUserData(User &u){
	NetSend(u.name);
	NetSend(u.nick_name);
	NetSend(u.pass_word);
}
User ReadUserData(){
	User u;
	u.name=NetGet();
	u.nick_name=NetGet();
	u.pass_word=NetGet();
	
	return u;
}

User user;

void CommandHelp(string *){
	cout<<"\n\
=====Command Help List=====\n\
1.cmd_help:\n\
\tGet a list of command help.\n\
2.logout:\n\
\tLogout your account.\n\
==========C I R S=========="
	<<endl;
}

void Logout(string *){
	cout<<"Logouting your account..."<<endl;
	user.name="";user.nick_name="";user.pass_word="";
	Sleep(1000);
	system("cls");
}

typedef void(*ins_type)(string*);

map<string,ins_type> ins_table{
	make_pair("cmd_help",CommandHelp),
	make_pair("logout",Logout) 
};

void Welcome(){
	cout<<
"Welcome to Common Information Record System V0.0.1!"
	<<endl;
}

void Login(){
	string user_name,password="";
	char char_value[2];
	char_value[1]=0;
	cout<<"Input user name:";
	cin>>user_name;
	cout<<"Input password(use backspace to delete char and use enter to finish inputing):\n";
input_password:
	password="";
	cout<<"->";
	while(true){
		char_value[0]=getch();
		if(char_value[0]==13/*'\n'*/){
			cout<<endl;
			break;
		}else
		if(char_value[0]==8/*'\b'*/){
			if(password.size()==0)
				continue;
			COORD cur_pos;//To know where is the cur now.  |      ****<-
			CONSOLE_SCREEN_BUFFER_INFO con_info;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&con_info);
			cur_pos=con_info.dwCursorPosition;
			cur_pos.X-=1;
			//Set the cur to the last pos.                 |      ***<-*
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cur_pos);
			//Fill the next '*' with space.                |      *** <-
			cout<<" ";
			//And set the cur to the last pos again.       |      ***<-
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cur_pos);
			//Delete the last char of the password.
			password.pop_back();
		}else{
			password+=char_value;
			cout<<"*";
		}
	}
	NetSend("login_request#"+user_name);
	string real_password="pw";//NetGet();
	if(real_password=="#unknown_user"){
		cout<<"Login failed:unknown user!"<<endl;
		MessageBeep(MB_ICONHAND);
		goto input_password;
	}else
	if(real_password=="#server_maintenancing"){
		cout<<"Login failed:the server is maintenancing!"<<endl;
		MessageBeep(MB_ICONHAND);
		goto input_password;
	}else
	if(real_password!=password){
		cout<<"Login failed:wrong password!"<<endl;
		MessageBeep(MB_ICONHAND);
		goto input_password;
	}else
	{
		cout<<"Login succeeded."<<endl;
		MessageBeep(MB_OK);
		user=ReadUserData();
	}
}

void WelcomeUser(){
	cout<<"Welcome,"<<user.nick_name<<"("<<user.name<<")."<<endl;
	cout<<"Input your instruction."<<endl;
	cout<<"Or you can input \"cmd_help\" to get a command help list."<<endl;
}

int main(){
	//NetInit(_host_name,port);
start:
	Welcome();
	cout<<"Please login first."<<endl;
	Login();
	WelcomeUser();
	while(true){
		cout<<">";
		string cmd;
		getline(cin,cmd);
		if(ins_table.find(cmd)==ins_table.end()){
			cout<<"Illegitimate input."<<endl;
			continue;
		}
		ins_table[cmd](&cmd);
		if(cmd=="logout")
			goto start;
	}
	//NetDestory();
	return 0;
} 
