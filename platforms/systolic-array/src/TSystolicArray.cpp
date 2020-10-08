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
#include <sstream>
#include <iomanip>
#include <iostream>

//simulator API
#include <TSystolicArray.h>

TSystolicArray::TSystolicArray(std::string name,  
	//signals
	int _a_buffer[][N],
	int _b_buffer[][N]) : TimedModel(name) {
    
    int x,y;
     
    for (x = 0; x < N; x++){
		for (y = 0; y < N; y++){
			_PE[x][y]  = new TPE(this->GetName() + ".PE" + to_string(x) + to_string(y),
								x, y,
								_start,_shift_acc,_shift_out);
						
			_z_buffer[x][y] = 0;
			_a_buffer[x][y] = 0;
			_b_buffer[x][y] = 0;
		}
	}
	
	for (x = 0; x < N; x++){
		for (y = 1; y < N; y++){
			_PE[x][y]->SetTPEAInput(_PE[x][y-1]->GetTPEAOutput());
		}
	}
	
	for (x = 1; x < N; x++){
		for (y = 0; y < N; y++){
			_PE[x][y]->SetTPEBInput(_PE[x-1][y]->GetTPEBOutput());
			_PE[x][y]->SetTPEZInput(_PE[x-1][y]->GetTPEZOutput());
		}
	}
}

TSystolicArray::~TSystolicArray(){
}

void TSystolicArray::Reset(){
	int x,y;
	
	_systolic_array_state = SystolicArrayState::INIT_ARRAY;
	_cont_row = 0;
	_cont_column = 0;
		
	for (x = 0; x < N; x++){
		for (y = 0; y < N; y++){
			_PE[x][y]->Reset();
			_z_buffer[x][y] = 0;
		}
	}
		
	// Initialize matrixs
	for (x = 0; x < N; x++){
		for (y = 0; y < N; y++){		
			_a_buffer[x][y] = (x*2) + 5 + y;
			_b_buffer[x][y] = (y+1) + x;
		}
	}	
}

void TSystolicArray::InitArray(){
	int x,y;
	
	switch(_systolic_array_state){	
		case SystolicArrayState::INIT_ARRAY:{					
			_start = 0;
			
			for (x = 0; x < N; x++) {
				_PE[x][0]->SetAInputValue(_a_buffer[x][_cont_column]);
				_PE[x][0]->ShiftTPEAInput();
				_PE[0][x]->SetBInputValue(_b_buffer[_cont_row][x]);
				_PE[0][x]->ShiftTPEBInput();
			}
			_cont_column = _cont_column + 1;
			_cont_row = _cont_row + 1;		
														
			for (x = 0; x < N; x++){
				for (y = 1; y < N; y++){
					_PE[x][y]->ShiftTPEAInput();
				}
			}
			
			for (x = 1; x < N; x++){
				for (y = 0; y < N; y++){
					_PE[x][y]->ShiftTPEBInput();
				}
			}
									
			if (_cont_column <= N) 
				_systolic_array_state = SystolicArrayState::START_MULT;
			else {
				_systolic_array_state = SystolicArrayState::SHIFT_OUT;
				_start = 0;
				_cont_column = 0;
				_cont_row = 0;
				_shift_acc = 1;
				_shift_out = 0;
			}
		}break;	
		default: break;
	}
}

void TSystolicArray::StartMult(){	
	switch(_systolic_array_state){	
		case SystolicArrayState::START_MULT:{
			_start = 1;
			_systolic_array_state = SystolicArrayState::INIT_ARRAY;
		} default: break;
	}
}

void TSystolicArray::ShiftOut(){
	int y;
		
	switch(_systolic_array_state){			
		case SystolicArrayState::SHIFT_OUT:{
			_start = 0;	
			_shift_out = 1;
			_shift_acc = 0;
			
			if (_cont_row < N){
				_shift_out = 1;
				
				for (y = 0; y < N; y++) {
					_z_buffer[N-(_cont_row+1)][y] = _PE[N-1][y]->GetZOutputValue();
				}			
			}
							
			_cont_row++;
		
			if (_cont_row == N)
				_systolic_array_state = SystolicArrayState::END_OP;
		}break;
		default: break;
	}
}

void TSystolicArray::EndOp(){
	int x,y;
	
	switch(_systolic_array_state){			
			case SystolicArrayState::END_OP:{
			printf("systolic_array: accumulator result!\n");
			for (x = 0; x < N; x++){
				for (y = 0; y < N; y++){
					printf("%d ", _PE[x][y]->GetMACResult());
				}
				printf("\n");
			}
			
			printf("systolic_array: shifted out result!\n");
			for (x = 0; x < N; x++){
				for (y = 0; y < N; y++){
					printf("%d ", _z_buffer[x][y]);
				}
				printf("\n");
			}
			
			printf("Done!\n");
			while(1);
		}break;
		default: break;
	}
}

std::string TSystolicArray::GetName() {
	return ".systolic_array";
}

SimulationTime TSystolicArray::Run() {
	int x,y;
	
	this->EndOp();
	this->ShiftOut();
	this->InitArray();
	this->StartMult();
	    
    for (x = N-1; x >= 0; x--){
		for (y = N-1; y >= 0; y--){
			_PE[x][y]->Run();
		}
	}
    
    return 1;
}