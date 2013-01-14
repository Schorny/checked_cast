#include "checked_cast.hpp"
#include <iostream>

class Base 
{ 
public: 
	Base() { std::cout<<"base()\n"; } 
	Base(Base const&) { std::cout<<"base(base const&)\n"; } 
	virtual ~Base(){std::cout<<"~base()\n";} 
}; 

class Der : public Base {}; 

Base& q() {
	return *new Base();
}

int main() 
{ 
	try 
	{ 

		Base const* pb = new Der; 
		Der const* rd = checked_cast<Der const*>(pb); 
		Base* px=new Base; 
		Der* d = checked_cast<Der*>(px); 
		Der const& r2=checked_cast<Der const&>(*pb); 
		Der d2; 
		Base& r3=d2; 
		Der& d3=checked_cast<Der&>(r3); 
		Base& r4=d2; 
		Der const& d4=checked_cast<Der const&>(r4); 

		Der* d5 = checked_cast<Der*>(new Base());
		Der* d6 = checked_cast<Der*>(&q());


		Base* p=new Base; 
		Der& r=checked_cast<Der&>(*p); //error, p is Base

	} 
	catch(bad_checked_cast const& e) 
	{ 
		std::cout<<e.what()<<'\n'; 
		std::cout<<"Source: "<<e.source_type().name()<<'\n'; 
		std::cout<<"Target: "<<e.target_type().name()<<'\n'; 
	} 
	catch(std::exception const& e) 
	{ 
		std::cout<<e.what()<<'\n'; 
	} 
	catch(...) 
	{ 
		std::cout<<"unknown exception\n"; 
	} 
}