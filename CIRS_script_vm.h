#pragma once

class VMException:public exception{
	string msg;
public:
	VMException(string in):msg(in){
	}
	virtual const char *what()noexcept{
		return msg.c_str();
	}
};

struct VirtualMachineEnvironment{
	int pc;
	User user;
	string filename;
};

class Instruction{
protected:
	int ParseInt(string in){
		return stoi(in);
	}
	int ParsePc(string in){
		int i=ParseInt(in);
		if(i<0) throw VMException("Illegal programme counter!");
		return i;
	}
public:
	virtual void Invoke(VirtualMachineEnvironment &env) = 0;
};

class IPlaySound:public Instruction{
	string music_name;
public:
	IPlaySound(string iname):music_name(iname){
	}
	virtual void Invoke(VirtualMachineEnvironment &env){
		PlaySound(music_name.c_str(),NULL,SND_NODEFAULT|SND_ASYNC);
	}
};

class IPassword:public Instruction{
	int success=-1,failure=-1;
	string question,answer;
public:
	IPassword(string i1,string i2):question(i1),answer(i2){
	}
	void SetSuccess(string in){
		success=ParsePc(in);
	}
	void SetFailure(string in){
		failure=ParsePc(in);
	}
	bool HasSuccess(){
		return success!=-1;
	}
	bool HasFailure(){
		return failure!=-1;
	}
	virtual void Invoke(VirtualMachineEnvironment &env){
		cout<<"Answer the password:"<<question<<endl<<"->";
		string u_answer;
		cin>>u_answer;
		env.pc=(u_answer==answer?success:failure);
	}
};

class IReportError:public Instruction{
	string str;
public:
	IReportError(string in):str(in){
	}
	string GetErrorMessage(){
		return str;
	}
	virtual void Invoke(VirtualMachineEnvironment &env){
		env.pc=-2;
	}
};

class IIfPermission:public Instruction{
	enum class Relationship{
		GT,LT,EQ,NEQ,NLT,NGT
	}relationship;
	Permission p;
	int _true,_false;
	Relationship ParseRelationship(string in){
		if(in.length()!=1&&in.length()!=2) throw VMException("Parse Error!");
		if(in==">") return Relationship::GT;
		else if(in=="<") return Relationship::LT;
		else if(in=="=") return Relationship::EQ;
		else if(in=="!=") return Relationship::NEQ;
		else if(in=="!<") return Relationship::NLT;
		else if(in=="!>") return Relationship::NGT;
		else throw VMException("Parse Error!");
	}
	Permission ParsePermission(string in){
		string p_name=in.substr(0,in.find('-'));
		string p_level=in.substr(in.find('-')+1);
		cout<<p_name<<" "<<p_level<<endl;
		return Permission(p_name,ParseInt(p_level));
	}
public:
	IIfPermission(string rs,string in_p,string __true,string __false):
		relationship(ParseRelationship(rs)),p(ParsePermission(in_p)),
		_true(ParsePc(__true)),_false(ParsePc(__false)){
	}
	virtual void Invoke(VirtualMachineEnvironment &env){
		Permission up=env.user.GetPermission(p.name);
		env.pc=(up.LevelIsEnough(p)?_true:_false); 
	}
};

class IOpen:public Instruction{
	string filename;
public:
	IOpen(string name):filename(name){
	}
	virtual void Invoke(VirtualMachineEnvironment &env){
		env.pc=-1;
		env.filename=filename;
	}
};

class IExit:public Instruction{
public:
	virtual void Invoke(VirtualMachineEnvironment &env){
		env.pc=-1;
	}
};

class IPrint:public Instruction{
	string msg;
public:
	IPrint(string str):msg(str){
	}
	virtual void Invoke(VirtualMachineEnvironment &env){
		cout<<msg<<endl;
	}
};

struct Script{
	vector<Instruction *> inss;
	bool executed=false;
	VirtualMachineEnvironment env;
	string Execute(User user){
		env.filename="execute_failed";
		env.pc=0;
		env.user=user;
		while(true){
			int old_pc=env.pc;
			inss[env.pc]->Invoke(env);
			if(env.pc==-1)
				break;
			if(env.pc==-2){
				IReportError *e=(IReportError *)inss[old_pc];
				throw VMException("Runtime Error:"+e->GetErrorMessage());
			}
			if(env.pc==old_pc)
				env.pc++;
		}
		for(auto i:inss)
			delete i;
		return env.filename;
	}
};

void Trim(string &s) 
{
    if (s.empty()) 
    	return;
    s.erase(0,s.find_first_not_of(" "));
    if (s.empty()) 
    	return;
    s.erase(s.find_last_not_of(" ") + 1);
}

void Tr(string &s){
	for(auto &c:s)
		if((c=='\t')||(c=='\n'))
			c=' ';
}

vector<string> SplitCode(string code){
	static const char *_format="abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    bool eof=false;
    vector<string> signs;
	int c=code[0];
    bool readSigns=(strchr(_format,c)==NULL);
    while(!code.empty()){
		char buf[1024];
		string format=string("%[")+(readSigns?"^":"")+_format+"]";
		sscanf(code.c_str(),format.c_str(),buf);
		readSigns= !readSigns;
		string tmp=buf;
		code=code.substr(tmp.size());
		Tr(tmp); 
		Trim(tmp);
		if(tmp!="")
			signs.push_back(tmp);
	}
	return signs;
}

Script ComplieScript(string code){
	vector<string> words=SplitCode(code);
	Script s;
	for(int i=0;i<words.size();i++){
#define SYM (words[i])
#define NEXT (words[++i])
#define LAST (words[--i])
		if(SYM=="playsound")//playsound xxx.mp3
			s.inss.push_back(new IPlaySound(NEXT));
		else if(SYM=="password"){//password xxx success xxx failure xxx
			IPassword *ip=new IPassword(NEXT,NEXT);
			if(NEXT=="success") ip->SetSuccess(NEXT);
			else if(SYM=="failure") ip->SetFailure(NEXT);
			else throw VMException("Complie failed!");
			if(NEXT=="success"&&(!ip->HasSuccess())) ip->SetSuccess(NEXT);
			else if(SYM=="failure"&&(!ip->HasFailure())) ip->SetFailure(NEXT);
			else throw VMException("Complie failed!");
			s.inss.push_back(ip);
		}else if(SYM=="error")//error xxx
			s.inss.push_back(new IReportError(NEXT));
		else if(SYM=="if"){
			if(NEXT=="permission")//if permission > xxx-xxx xxx else xxx
				s.inss.push_back(new IIfPermission(NEXT,NEXT,NEXT,(NEXT=="else"?NEXT:(LAST,"noelse"))));
		}else if(SYM=="rem"){//rem some rem endrem
			while(NEXT!="endrem");
		}else if(SYM=="open"){//open xxx
			s.inss.push_back(new IOpen(NEXT));
		}else if(SYM=="exit"){//exit
			s.inss.push_back(new IExit());
		}else if(SYM=="print"){//print xxx
			s.inss.push_back(new IPrint(NEXT));
		}else throw VMException("Complie failed:unknown ins-"+SYM+"-");
#undef SYM
#undef NEXT
#undef LAST
	}
	return s;
}
