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
#include <iostream>
#include <sstream>

//model API
#include <ProcessingTile.h>

/** 
 * Default constructor.
 * Instantiate and bind internal hardware to each
 * other. */
ProcessingTile::ProcessingTile() {
	// Systolic Array
	_array  = new TSystolicArray(this->GetName() + ".systolic_array", 
				_a_buffer, _b_buffer);

	this->Reset();
}

ProcessingTile::~ProcessingTile(){
	delete(_array);
}

void ProcessingTile::Reset(){
	_array->Reset();
}

TSystolicArray* ProcessingTile::GetSystolicArray(){
	return _array;
} 

std::string ProcessingTile::ToString(){
	stringstream ss;
	ss << this->GetName() << "={" << _array->GetName() <<"}";
	return ss.str();
}

USignal<uint32_t>* ProcessingTile::GetSignalHostTime(){
	return _signal_hosttime;
}

std::string ProcessingTile::GetName(){
	return "core!";
}
