#include<bits/stdc++.h>
using namespace std;
#include<windows.h>
/**
#include<Mmsystem.h>
#include "CIRS_document_renderer.h"

int main(){
	Document d;
	d.title="A Title for Test";
	d.text.push_back("  Some text...");
	d.text.push_back("  Some other text...");
	RenderDocument(d);
	try{
		Script s=ComplieScript("\
print print_test\n\
rem rem test endrem\n\
open test_doc1\n\
");
		User test_user;
		s.Execute(test_user);
		cout<<s.env.filename<<":"<<s.env.pc<<endl;
	}catch(VMException vme){
		cout<<vme.what()<<endl;
	}
	return 0;
}
/**/
/**
#include<direct.h>
#include "CIRS_file_pkg.h"
int main(){
	try{
		FilePkg::Pack("E:\\Common Information Record System\\rc\\file_pkg_test\\test_files",
					"E:\\Common Information Record System\\rc\\file_pkg_test");
		FilePkg::UnPack("E:\\Common Information Record System\\rc\\file_pkg_test\\test_files.CIRS_pkg",
						"E:\\Common Information Record System\\rc\\file_pkg_test\\test_files_o");
	}catch(FilePkg::Exception &e){
		cout<<"ERROR:"<<e.what();
	}
	return 0;
}
/**/
#include "CIRS_document_renderer.h"
int main(){
	ServerUser su;
	su.SetPassword("0xB850C");
	su.SetUserName("345467");
	User u=su.GetUser();
	cout<<s.GetPassword()<<endl;
	cout<<s.GetUserName()<<endl;
	return 0;
}
