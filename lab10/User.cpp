/*
 * User.cpp
 *
 *  Created on: Aug 12, 2017
 *      Author: user
 */

#include "User.h"


User::User(string uname, string pass, int score){
	this->uName = uname;
	this->password = pass;
	this->score = score;
}

string User::getUsername(){
	return this->uName;
}
string User::getPassword(){
	return this->password;
}
int User::getScore(){
	return this->score;
}

void User::setScore(int score){
	this->score = score;
}




