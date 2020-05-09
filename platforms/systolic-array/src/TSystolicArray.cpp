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
								_start,_shift_acc,_shift_z,0,0,0);	
			_z_buffer[x][y] = 0;
			_a_buffer[x][y] = 0;
			_b_buffer[x][y] = 0;
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
}

void TSystolicArray::DoComputation(){
	int x,y;
	
	_start = 0;
	
	switch(_systolic_array_state){	
		case SystolicArrayState::INIT_ARRAY:{
			
			// Initialize matrixs
			for (x = 0; x < N; x++){
				for (y = 0; y < N; y++){		
					_z_buffer[x][y] = 0;
					_a_buffer[x][y] = (x*2) + 5;
					_b_buffer[x][y] = (y+1);
				}
			}
			
			for (x = 0; x < N; x++){
				for (y = 0; y < N; y++){
					_PE[x][y]->SetTPEAInput(_a_buffer[x][_cont_column]);  
				}
			}
						
			for (x = 0; x < N; x++){
				for (y = 0; y < N; y++){
					_PE[x][y]->SetTPEBInput(_b_buffer[x][y]);  
				}
			}
			
			if (_cont_column < N) {
				_systolic_array_state = SystolicArrayState::START_MULT;
				_cont_column++;
			} else
				_systolic_array_state = SystolicArrayState::SHIFT_OUT;
		}break;
		
		case SystolicArrayState::START_MULT:{
			_start = 1;
			for (x = 0; x < N; x++){
				for (y = 0; y < N; y++){
					_PE[x][y]->DoMAC();
				}
			}
			_systolic_array_state = SystolicArrayState::INIT_ARRAY;
		}break;
		
		case SystolicArrayState::SHIFT_OUT:{
			if (_cont_row == 0) {
				_shift_acc = 1;	
				_shift_z   = 0;
			} else if (_cont_row < N){
				_shift_acc = 0;
				_shift_z   = 1;
			} else {
				_shift_acc = 0;
				_shift_z   = 0;
			}		
		
			if (_cont_row < N){
				for (x = N-1; x >= 0; x--){
					for (y = N-1; y >= 0; y--){
						_PE[x][y]->ShiftTPEResult();
					}
				}
				
				for (y = 0; y < N; y++) {
					_z_buffer[N-(_cont_row+1)][y] = _PE[N-1][y]->GetTPEZOutput();
				}

				for (x = 0; x < N-1; x++){
					for (y = 0; y < N; y++){
						_PE[x+1][y]->SetTPEZInput(_PE[x][y]->GetTPEZOutput());
					}
				}				
			}
					
			_cont_row++;
			if (_cont_row == N)
				_systolic_array_state = SystolicArrayState::END_OP;
		}break;
		
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
	}
}

std::string TSystolicArray::GetName() {
	return ".systolic_array";
}

SimulationTime TSystolicArray::Run() {
	this->DoComputation();
    return 4;
}
