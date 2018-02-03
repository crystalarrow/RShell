#ifndef BASE_H
#define BASE_H
#include <iostream>


using namespace std;

class Base
{
    public:
        Base();
        Base(string input);
        virtual ~Base();

        string getString();
        
        virtual void run() = 0;
        
    private:
        string input;
};

#endif // BASE_H
