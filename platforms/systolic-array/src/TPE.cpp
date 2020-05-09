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
	int &start, int &shift_acc, int &shift_z_in,
	uint32_t a_in, uint32_t b_in, uint32_t z_in) : TimedModel(name) {
    	    
    _x = x;
    _y = y;
	_start_in = &start;
	_shift_acc_in = &shift_acc;
	_shift_z_in = &shift_z_in;
	_a_in = a_in;
	_b_in = b_in;
	_z_in = z_in;
	
	this->Reset();
}

TPE::~TPE(){
}

void TPE::Reset(){
	_z_out = 0;
	_acc = 0;
}

void TPE::DoMAC(){
	if (*_start_in == 1)
		_acc = _acc + (_a_in * _b_in);
}

uint8_t TPE::GetTPEZInput(){
	return _z_in;
}

uint8_t TPE::GetTPEX(){
	return _x;
}

uint8_t TPE::GetTPEY(){
	return _y;
}

uint32_t TPE::GetMACResult(){
	return _acc;
}

uint32_t TPE::GetTPEZOutput(){
	return _z_out;
}

void TPE::SetTPEAInput(uint32_t a_in){
	_a_in = a_in;
}

void TPE::SetTPEBInput(uint32_t b_in){
	_b_in = b_in;
}

void TPE::SetTPEZInput(uint32_t z_in){
	_z_in = z_in;
}

void TPE::ShiftTPEResult(){	
	if (*_shift_acc_in == 1)
		_z_out = _acc;
	
	if (*_shift_z_in == 1)
		_z_out = _z_in;		
}

SimulationTime TPE::Run(){
	this->DoMAC();
	this->ShiftTPEResult();
    return 1;
}
