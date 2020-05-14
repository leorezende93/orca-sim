/** 
 * This file is part of project URSA. More information on the project
 * can be found at URSA's repository at GitHub
 * 
 * http://https://github.com/andersondomingues/ursa 
 *
 * Copyright (C) 2018 Anderson Domingues, <ti.andersondomingues@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. **/

//std API
#include <iostream>
#include <sstream>
#include <iomanip>

//simulator API
#include <TPE.h>

TPE::TPE(std::string name,  
	//signals
	uint8_t x, uint8_t y,
	int &start) : TimedModel(name) {
   
	// Pointer initialize
	int* init_pointer = 0;
	int init_value;
	init_value = 0;
	init_pointer = &init_value;

    _x = x;
    _y = y;
	_start_in = &start;
	_a_in  = init_pointer;
	_b_in  = init_pointer;
	_a_out = init_pointer;
	_b_out = init_pointer;
		
	this->Reset();
}

TPE::~TPE(){
}

void TPE::Reset(){
	_acc = 0;
}

void TPE::DoMAC(){
	if (*_start_in == 1)
		_acc = _acc + ((*_a_in) * (*_b_in));
}

void TPE::SetTPEAInput(int* a_in){
	_a_in = a_in;
}

void TPE::SetTPEBInput(int* b_in){
	_b_in = b_in;
}

int* TPE::GetTPEAOutput(){
	return _a_out;
}

int* TPE::GetTPEBOutput(){
	return _b_out;
}

int TPE::GetAOutputValue(){
	return *_a_out;
}

int TPE::GetBOutputValue(){
	return *_b_out;
}

int TPE::GetAInputValue(){
	return *_a_in;
}

int TPE::GetBInputValue(){
	return *_b_in;
}

int TPE::GetMACResult(){
	return _acc;
}

void TPE::ShiftTPEAInput(){	
	_a_out = _a_in;
}

void TPE::ShiftTPEBInput(){	
	_b_out = _b_in;
}

void TPE::ShiftTPEResult(){	
	_b_out = &_acc;
}

SimulationTime TPE::Run(){
	this->DoMAC();
	this->ShiftTPEAInput();
	this->ShiftTPEBInput();
	this->ShiftTPEResult();
    return 1;
}
