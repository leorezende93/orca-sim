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

void Testbench::Conv2d(short weights[], short ifmap[], int filter_h, int filter_w, int ifmap_dimension, int stride_w, int stride_h, int channel, int ofmap_h, int ofmap_w) {
	int m,n;
	for (m = 0; m < filter_h; m++){
		for (n = 0; n < filter_w; n++){
				_array->SetABuffer(m,n,weights[(filter_h*filter_w+1)*channel + filter_h*m + n + 1]);
				_array->SetBBuffer(m,n,ifmap[ifmap_dimension*(m+stride_w*ofmap_h) + n + stride_h*ofmap_w]);
		}
	}	
	_array->SetInit(1);
}	

void Testbench::TBInit(){
	if (_i != NUMBER_OF_FILTERS && _array->GetEOP() == 0) {
			Conv2d(Layer1_Weights_CPU,data0,FILTER_DIMENSION,FILTER_DIMENSION,IMAGE_DIMENSION,STRIDE,STRIDE,_i,_j,_k);
		}
	}

void Testbench::TBStore(){
	int x,y,idx;
	
	if(_array->GetEOP() == 1){
		_array->SetInit(0);
		Layer2[_i].ofmap[_j][_k] = _array->GetAdderValue() + Layer1_Weights_CPU[_i*(FILTER_DIMENSION*FILTER_DIMENSION+1)];
	
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
						printf("%d ",Layer2[idx].ofmap[x][y]);
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
		}		
	}
}

SimulationTime Testbench::Run() {
	this->TBInit();
	_array->Run();
	this->TBStore();
    return 1;
}
