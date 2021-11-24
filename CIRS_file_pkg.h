#pragma once

namespace{
	
	vector<string> split(string str,string pattern){
    	string::size_type pos;
    	vector<string> result;
    	str+=pattern;
    	int size=str.size();
    	for(int i=0;i<size;i++){
        	pos=str.find(pattern,i);
        	if(pos<size){
            	string s=str.substr(i,pos-i);
            	result.push_back(s);
            	i=pos+pattern.size()-1;
        	}
    	}
    	return result;
	}
	void CreateFile(string name){
		cout<<"CreateFile:"<<name<<endl;
        const char *fileName=name.c_str(),*tag;
        for(tag=fileName;*tag;tag++)
            if(*tag=='\\') {
                char buf[1024],path[1024];
                strcpy(buf,fileName);
                buf[strlen(fileName)-strlen(tag)+1]=0;
                strcpy(path,buf);
                if(access(path,6)==-1) {
                    mkdir(path);
                }
            }
        ofstream tmp(name);
        if(!tmp.is_open())
			throw "CAN_NOT_OPEN_FILE";
        tmp.close();
	}
	vector<string> FindFiles(string dir){
		
	}
	
}

namespace FilePkg
{

	class Exception:public exception{
			string msg;
		public:
			Exception(string in):msg(in) {
			}
			virtual const char *what()noexcept {
				return msg.c_str();
			}
	};
	
	struct PkgInfo{
		string src_file_name;
		int number_of_child_files;
		vector<string> child_file_names;
	};

	void UnPack(string src_file_path,string dst_dir){
		ifstream fin(src_file_path,ios::binary);
		if(!fin.good()) throw Exception("Illegal file path!");
		int magic;
		fin.read((char*)&magic,sizeof(int));
		if(magic!=0x1321B5F) throw Exception("Illegal file type!");
		
		PkgInfo pi;
		int tmp;
		char *buf;
		
		fin.read((char*)&tmp,sizeof(int));
		buf=new(nothrow) char[tmp+1];
		if(buf==NULL) throw Exception("Out of memory!");
		fin.read(buf,tmp);
		buf[tmp]=0;
		pi.src_file_name=buf;
		delete[] buf;
		
		fin.read((char*)&pi.number_of_child_files,sizeof(int));
		
		fin.read((char*)&tmp,sizeof(int));
		buf=new(nothrow) char[tmp+1];
		if(buf==NULL) throw Exception("Out of memory!");
		fin.read(buf,tmp);
		buf[tmp]=0;
		string child_file_names=buf;
		delete[] buf;
		
		pi.child_file_names=split(child_file_names,"`");
		for(string &child_file_name:pi.child_file_names){
			fin.read((char*)&tmp,sizeof(int));
			bool have_to_unpack=false;
			if(tmp<0)
				have_to_unpack=true,tmp= -tmp;
			try{
				CreateFile(dst_dir+"\\"+child_file_name);
			}catch(const char *str){
				throw Exception(str);
			}
			ofstream out((dst_dir+"\\"+child_file_name).c_str(),ios::binary);
			if(!out.good()) throw Exception("Failed to create file!");
			buf=new(nothrow) char[tmp];
			if(buf==NULL) throw Exception("Can't alloc memory for file!");
			fin.read(buf,tmp);
			out.write(buf,tmp);
			out.close();
			if(have_to_unpack){
				vector<string> s=split(child_file_name,"\\");
				s=split(s[s.size()-1],".");
				string name="";
				for(int i=0;i<s.size()-1;i++)
					name+=s[i];
				UnPack(dst_dir+"\\"+child_file_name,dst_dir+"\\"+name);
				remove((dst_dir+"\\"+child_file_name).c_str());
			}
			delete[] buf;
		}
		
		fin.close();
	}
	
	void Pack(string src_dir,string dst_file_path){
		vector<string> s=split(src_dir,"\\");
		string dst_file_name=dst_file_path+"\\"+s[s.size()-1]+".CIRS_pkg";
		try{
			CreateFile(dst_file_name);
		}catch(const char *str){
			throw Exception(str);
		}
		ofstream out(dst_file_name.c_str(),ios::binary);
		if(!out.good()) throw Exception("Failed to create file!");
		
		long hFile=0;
    	struct _finddata_t fileinfo;
    	string p;
    	vector<string> files,pack_files;
    	if((hFile=_findfirst(p.assign(src_dir).append("\\*").c_str(),&fileinfo))!=-1){
        	do
            	if((fileinfo.attrib&_A_SUBDIR)){
                	if(strcmp(fileinfo.name,".")!=0 && strcmp(fileinfo.name,"..")!=0){
                		Pack(src_dir+"\\"+fileinfo.name,src_dir);
                		pack_files.push_back(src_dir+"\\"+fileinfo.name+".CIRS_pkg");
					}
            	}else
            		if(strcmp(fileinfo.name,".")!=0 && strcmp(fileinfo.name,"..")!=0
						&&find(pack_files.begin(),pack_files.end(),//Unknown if-condition(doge)
						p.assign(src_dir).append("\\").append(fileinfo.name))==pack_files.end())
                			files.push_back(p.assign(src_dir).append("\\").append(fileinfo.name));
        	while(_findnext(hFile,&fileinfo)==0);
        	_findclose(hFile);
    	}
    	
    	int tmp=0x1321B5F;
		out.write((char*)&tmp,sizeof(int));
		
		string file_name=s[s.size()-1]+".CIRS_pkg";
		tmp=file_name.size();
		out.write((char*)&tmp,sizeof(int));
		out.write(file_name.c_str(),tmp);
		
		tmp=files.size()+pack_files.size();
		out.write((char*)&tmp,sizeof(int));
		
		file_name="";
		for(auto &str:files){
			vector<string> s2=split(str,"\\");
			file_name+=s2[s2.size()-1]+"`";
		}
		for(auto &str:pack_files){
			vector<string> s2=split(str,"\\");
			file_name+=s2[s2.size()-1]+"`";
		}
		file_name.pop_back();
		tmp=file_name.size();
		out.write((char*)&tmp,sizeof(int));
		out.write(file_name.c_str(),tmp);
		
		for(auto &s:files){
			ifstream in(s.c_str(),ios::binary);
			auto pos=in.tellg();
			in.seekg(0,ios_base::end);
			tmp=in.tellg()-pos;
			in.seekg(0);
			char *buf=new(nothrow) char[tmp];
			if(buf==NULL) throw Exception("Out of memory!");
			in.read(buf,tmp);
			out.write((char*)&tmp,sizeof(int));
			out.write(buf,tmp);
			in.close();
		}
		for(auto &s:pack_files){
			ifstream in(s.c_str(),ios::binary);
			auto pos=in.tellg();
			in.seekg(0,ios_base::end);
			tmp=in.tellg()-pos;
			in.seekg(0);
			char *buf=new(nothrow) char[tmp];
			if(buf==NULL) throw Exception("Out of memory!");
			in.read(buf,tmp);
			int tmp2=-tmp;
			out.write((char*)&tmp2,sizeof(int));
			out.write(buf,tmp);
			in.close();
			remove(s.c_str());
		}
		out.close();
	}

}
