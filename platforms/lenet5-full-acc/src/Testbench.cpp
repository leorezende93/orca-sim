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
	
	// Tb control
	_parallel_control = 0;
	_end_of_layer2 = 0;
	_end_of_simulation = 0;
	_vet_idx = 0;
	this->ClearIdx();
		
	// Reset
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

void Testbench::Conv(int weights[][FILTER_DIMENSION], int feature_map[][FILTER_DIMENSION]){
	int m,n;
	for (m = 0; m < FILTER_DIMENSION; m++){
		for (n = 0; n < FILTER_DIMENSION; n++){
			_array->SetABuffer(m,n,weights[m][n]);
			_array->SetBBuffer(m,n,feature_map[m][n]);
		}
	}		
	_array->SetInit(1);
}

void Testbench::idxInc(int layer_dimension_w, int layer_dimension_h){
	_k++;
	if (_k == layer_dimension_w) {
		_k = 0;
		_j++;
		if (_j == layer_dimension_h) {
			_j = 0;
			_i++;
		}
	}
}

void Testbench::ClearIdx(){
	_i = 0;
	_j = 0;
	_k = 0;
}


void Testbench::calculateLayer5(float* Layer4_Neurons_CPU, short* Layer4_Weights_CPU, float* Layer5_Neurons_CPU){
	float somme;
	int i, j;
	for( i=0;i<10;i++){
		somme = Layer4_Weights_CPU[101*i];
		for( j=0;j<100;j++)
			somme += Layer4_Weights_CPU[1+101*i+j] * Layer4_Neurons_CPU[j];
		Layer5_Neurons_CPU[i] = SIGMOID(somme/100.0);
		
	}
}

void Testbench::TBInit(){
	int m,n;
	int weights[FILTER_DIMENSION][FILTER_DIMENSION];
	int ifmap[FILTER_DIMENSION][FILTER_DIMENSION];
	
	if (_i != LAYER2_NUMBER_OF_FILTERS && _array->GetEOP() == 0 && _end_of_layer2 == 0) {
		for (m = 0; m < FILTER_DIMENSION; m++){
			for (n = 0; n < FILTER_DIMENSION; n++){
				weights[m][n] = Layer1_Weights_CPU[(FILTER_DIMENSION*FILTER_DIMENSION+1)*_i + FILTER_DIMENSION*m + n + 1];
				ifmap[m][n] = data6[IMAGE_DIMENSION*(m+STRIDE*_j) + n + STRIDE*_k];
			}
		}	
		this->Conv(weights, ifmap);
	}
	
	if (_i != LAYER3_NUMBER_OF_FILTERS && _array->GetEOP() == 0 && _end_of_layer2 == 1 && _end_of_layer3 == 0) {
		for (m = 0; m < FILTER_DIMENSION; m++){
			for (n = 0; n < FILTER_DIMENSION; n++){		 
				weights[m][n] = Layer2_Weights_CPU[26*6*_i+1+6*(n+5*m)+_parallel_control] ;
				ifmap[m][n] = Layer2_Output[13*13*_parallel_control+13*(2*_j+m)+(2*_k+n)];
			}
		}
		this->Conv(weights, ifmap);
	}
	
	if (_i != LAYER4_DIMENSION && _array->GetEOP() == 0 && _end_of_layer2 == 1 && _end_of_layer3 == 1 && _end_of_simulation == 0) {
		for (_k = 0; _k < FILTER_DIMENSION; _k++){
			for (m = 0; m < FILTER_DIMENSION; m++){		 
				weights[_k][m] = Layer3_Weights_CPU[_i*(1+50*25)+1 + m + _k*5 + _j*25];
				ifmap[_k][m] = Layer4_Input[m+5*_k+25*_j] * SHIFT;
			}
		}
		this->Conv(weights, ifmap);
	}
}

void Testbench::TBStore(){
	int idx;
	
	int aux = -999;
	int result;

	if(_array->GetEOP() == 1 && _end_of_layer2 == 0){
		_array->SetInit(0);
		
		Layer2_Output[_vet_idx] = SIGMOID((_array->GetAdderValue() + Layer1_Weights_CPU[_i*(FILTER_DIMENSION*FILTER_DIMENSION+1)])/100.0) * SHIFT;
		
		this->idxInc(LAYER2_DIMENSION,LAYER2_DIMENSION);
		_vet_idx++;
				
		if (_i == LAYER2_NUMBER_OF_FILTERS) {
			_end_of_layer2 = 1;
			this->ClearIdx();
			_vet_idx = 0;
			printf("tb: layer2 conv done (HW)!\n");
		}
	} 
	
	else if(_array->GetEOP() == 1 && _end_of_layer2 == 1 && _end_of_layer3 == 0){
		_array->SetInit(0);
		Layer3_Output[_vet_idx] = Layer3_Output[_vet_idx] + _array->GetAdderValue();
		_parallel_control++;
		
		if (_parallel_control == 6) {
			Layer4_Input[_vet_idx] = SIGMOID(((Layer3_Output[_vet_idx]/SHIFT) + Layer2_Weights_CPU[_i*(FILTER_DIMENSION*FILTER_DIMENSION+1)*_parallel_control])/100.0);			
			this->idxInc(LAYER3_DIMENSION,LAYER3_DIMENSION);
			_vet_idx++;
			_parallel_control = 0;
		}
		
		if (_i == LAYER3_NUMBER_OF_FILTERS) {
			_end_of_layer3 = 1;
			this->ClearIdx();
			_vet_idx = 0;
			_parallel_control = 0;
			printf("tb: layer3 conv done (HW)!\n");
		}
	}
	
	else if(_array->GetEOP() == 1 && _end_of_layer2 == 1 && _end_of_layer3 == 1 && _end_of_simulation == 0){
		_array->SetInit(0);
		Layer4_Output[_i]= Layer4_Output[_i] + _array->GetAdderValue();
		_j++;
	
		if (_j == LAYER4_NUMBER_OF_FILTERS) {
			Layer5_Input[_i] = SIGMOID(((Layer4_Output[_i]/SHIFT) + Layer3_Weights_CPU[_i*(1+50*25)])/100.0);			
			_j = 0;
			_i++;
		}
		
		if (_i == LAYER4_DIMENSION) {
			printf("tb: layer4 conv done (HW)!\n");
									
			this->ClearIdx();
			
			calculateLayer5(Layer5_Input,Layer4_Weights_CPU,Layer5_Output);
			
			printf("tb: layer5 conv done (SW)!\n");
			for(idx=0;idx<LAYER5_DIMENSION;idx++){
				if (aux < Layer5_Output[idx]) {
					aux = Layer5_Output[idx];
					result = idx;
				}
			}
			printf("tb: number %d recognized!\n",result);
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
