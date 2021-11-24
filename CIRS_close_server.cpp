#include<windows.h>
#include<string>
#include<iostream>
#include<fstream> 
using namespace std;

#define END CLIENT
#include "CIRS_web.h"
using namespace Web;
#undef END

int main(){
	use_net();
	Socket so(_host_name.c_str(),port);
	so.send("#close");
	so.close();
	unuse_net();
	return 0;
}
