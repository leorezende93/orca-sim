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
	int &start, int &shift_acc, int &shift_out) : TimedModel(name) {

	_a_in  = new USignal<int16_t>(0x00000000,this->GetName() + ".a_in");
	_b_in  = new USignal<int16_t>(0x00000000,this->GetName() + ".b_in");
	_z_in  = new USignal<int32_t>(0x00000000,this->GetName() + ".z_in");
	_a_out = new USignal<int16_t>(0x00000000,this->GetName() + ".a_out");
	_b_out = new USignal<int16_t>(0x00000000,this->GetName() + ".b_out");
	_z_out = new USignal<int32_t>(0x00000000,this->GetName() + ".z_out");

    _x = x;
    _y = y;
	_start_in = &start;
	_shift_acc_in = &shift_acc;
	_shift_out_in = &shift_out;
		
	this->Reset();
}

TPE::~TPE(){
}

void TPE::Reset(){
	_acc = 0;
}

void TPE::DoMAC(){
	if (*_start_in == 1)
		_acc = _acc + ((_a_in->Read()) * (_b_in->Read()));
}

void TPE::SetTPEAInput(USignal<int16_t>* a_in){
	_a_in = a_in;
}

void TPE::SetTPEBInput(USignal<int16_t>* b_in){
	_b_in = b_in;
}

void TPE::SetTPEZInput(USignal<int32_t>* z_in){
	_z_in = z_in;
}
  
void TPE::SetAInputValue(int16_t value){
	_a_in->Write(value);
}

void TPE::SetBInputValue(int16_t value){
	_b_in->Write(value);
}
   
USignal<int16_t>* TPE::GetTPEAOutput(){
	return _a_out;
}

USignal<int16_t>* TPE::GetTPEBOutput(){
	return _b_out;
}

USignal<int32_t>* TPE::GetTPEZOutput(){
	return _z_out;
}

int16_t TPE::GetAOutputValue(){
	return _a_out->Read();
}

int16_t TPE::GetBOutputValue(){
	return _b_out->Read();
}

int32_t TPE::GetZOutputValue(){
	return _z_out->Read();
}

int16_t TPE::GetAInputValue(){
	return _a_in->Read();
}

int16_t TPE::GetBInputValue(){
	return _b_in->Read();
}

int32_t TPE::GetMACResult(){
	return _acc;
}

void TPE::ShiftTPEAInput(){	
	_a_out->Write(_a_in->Read());
}

void TPE::ShiftTPEBInput(){	
	_b_out->Write(_b_in->Read());
}

void TPE::ShiftTPEZInput(){	
	if (*_shift_out_in == 1)
		_z_out->Write(_z_in->Read());
}

void TPE::ShiftTPEResult() {	
	if (*_shift_acc_in == 1) 
		_z_out->Write(_acc);
}

std::string TPE::GetName() {
	return ".PE";
}

SimulationTime TPE::Run(){
	this->DoMAC();
	this->ShiftTPEResult();
	this->ShiftTPEZInput();
    return 1;
}
