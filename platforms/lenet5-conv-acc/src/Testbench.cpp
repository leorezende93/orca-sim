/** 
 * This file is part of pro_ject URSA. More information on the pro_ject
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
#include <fstream>

//model API
#include <Testbench.h>
#include <LenetData.h>

/** 
 * Default constructor.
 * Instantiate and bind internal hardware to each
 * other. */
Testbench::Testbench(string name) : TimedModel(name) {
	// Systolic Array
	_array  = new TSystolicArray(this->GetName() + ".systolic_array");
	
	// Conv index
	_i = 0;
	_j = 0;
	_k = 0;
	_end_of_simulation = 0;
	
	// Log file
	log.open("./platforms/lenet5-conv-acc/output_log.txt");
	log.precision(2);
	log << std::fixed;
	
	this->Reset();
}

Testbench::~Testbench(){
	delete(_array);
}

void Testbench::Reset(){
	_array->Reset();
}

std::string Testbench::GetName(){
	return ".testbench";
}

int Testbench::GetEndOfSimulation(){
	return _end_of_simulation;
}

void Testbench::TBInit(){
	int m,n;
	
	if (_i != NUMBER_OF_FILTERS && _array->GetEOP() == 0) {
			for (m = 0; m < FILTER_DIMENSION; m++){
				for (n = 0; n < FILTER_DIMENSION; n++){
					_array->SetABuffer(m,n,Layer1_Weights_CPU[(FILTER_DIMENSION*FILTER_DIMENSION+1)*_i + FILTER_DIMENSION*m + n + 1]);
					_array->SetBBuffer(m,n,data0[IMAGE_DIMENSION*(m+STRIDE*_j) + n + STRIDE*_k]);
				}
			}	
			_array->SetInit(1);
		}
	}

void Testbench::TBStore(){
	int x,y,idx;
	
	if(_array->GetEOP() == 1){
		_array->SetInit(0);
		Layer2[_i].ofmap[_j][_k] = SIGMOID((_array->GetAdderValue() + Layer1_Weights_CPU[_i*(FILTER_DIMENSION*FILTER_DIMENSION+1)])/100.0);
	
		_k++;
		if (_k == LAYER2_DIMENSION) {
			_k = 0;
			_j++;
			if (_j == LAYER2_DIMENSION) {
				_j = 0;
				_i++;
			}
		}	
		
		if (_i == NUMBER_OF_FILTERS) {
			printf("tb: conv result!\n");
			for (idx = 0; idx < NUMBER_OF_FILTERS; idx++) {
				printf("tb: layer2 -> filter%d:\n",idx);
				for (x = 0; x < LAYER2_DIMENSION; x++) {
					for (y = 0; y < LAYER2_DIMENSION; y++) {
						printf("%.2f ",Layer2[idx].ofmap[x][y]);
						log << Layer2[idx].ofmap[x][y] << " ";
					}
					printf("\n");
					log << endl;
				}
				printf("\n");
				log << endl;
			}
			log.close();
			printf("tb: end of simulation!\n\n");
			_end_of_simulation = 1;
			//abort();
		}		
	}
}

SimulationTime Testbench::Run() {
	this->TBInit();
	_array->Run();
	this->TBStore();
    return 1;
}
