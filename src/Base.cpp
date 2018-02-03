#include "Base.h"

Base::Base()
{
    //ctor
}

Base::Base(string input)
{
    this->input = input;
}

Base::~Base()
{
    //dtor
}

string Base::getString()
{
	return input;
}