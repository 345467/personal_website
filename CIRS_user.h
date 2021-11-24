#pragma once

class Permission{
	string name;
	int level=-1;
public:
	Permission(string iname,int ilevel):name(iname),level(ilevel){
	}
	Permission():name("NoPermission"),level(-1){
	}
	string GetName(){
		return name;
	}
	int GetLevel(){
		return level;
	}
	bool LevelIsEnough(Permission an){
		if(an.name!=name) return false;
		return level>=an.level;
	}
	bool IsEqual(Permission an){
		return (an.name==name)&&(an.level==level);
	}
}no_permission;

bool operator== (Permission a,Permission b){
	return a.IsEqual(b);
}

bool operator!= (Permission a,Permission b){
	return !(a==b);
}

bool operator> (Permission a,Permission b){
	return a.LevelIsEnough(b);
}

bool operator< (Permission a,Permission b){
	return !(a>b);
}

bool operator>= (Permission a,Permission b){
	return (a>b)||(a==b);
}

bool operator<= (Permission a,Permission b){
	return
}

class ServerUser;

class User{
	friend class ServerUser;
	string name;
	string password;
public:
	string GetPassword(){
		return password;
	} 
	string GetUserName(){
		return name;
	}
};

class ServerUser{
	User u;
public:
	void SetPassword(string pw){
		u.password=pw;
	}
	void SetUserName(string name){
		u.name=name;
	}
};
