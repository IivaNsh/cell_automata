#include "read_file.h"
#include <sstream>
#include <fstream>
#include <iostream>

std::string readfile(const char* name){
    std::stringstream s;
    
    std::cout<<"opening file -> "<<name;

    std::ifstream file(name);

    if(!file.is_open()){
        std::cout<<" -> could not open file..\n";
        return "";
    }
    std::cout<<" -> opened -> reading file.. \n";
    while(file){
        char c = file.get();
        s<<c;
        //std::cout<<c;
    }
    return s.str();
}