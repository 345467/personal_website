#pragma once

struct Permission{
	string name;
	int level=-1;
	Permission(string iname,int ilevel):name(iname),level(ilevel){
	}
	Permission():name("NoPermission"),level(-1){
	}
	int LevelIsEnough(Permission an){
		if(an.name!=name) return -1;
		return level>=an.level;
	}
}no_permission;

class User{
	vector<Permission> permissions;
public:
	Permission GetPermission(string name){
		for(auto &i:permissions)
			if(i.name==name)
				return i;
		return no_permission;
	} 
};

#include "CIRS_file_pkg.h"
#include "CIRS_script_vm.h"

struct Document{
	string title;
	Script s;
	vector<string> text;
};

void RenderDocument(const Document &d){
	system("cls");
	cout<<"        "<<d.title<<endl;
	for(const string &s:d.text)
		cout<<s<<endl;
	return;
}

//TODO:Pack and Unpack Documents
