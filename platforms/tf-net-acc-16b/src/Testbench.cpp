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
#include <Tensorflow.h>
#include <Testbench.h>

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
	_c = 0;
	_vet_idx = 0;
	
	// Tb control
	_wait_eop = 0;
	_end_of_layer1 = 0;
	_end_of_layer2 = 0;
	_end_of_layer3 = 0;
	_end_of_layer4 = 0;
	_end_of_simulation = 0;
	
	// Testcase control
	_cont_predict = 0;
	_cont_testcases = 0;		
	
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

void Testbench::ClearIdx(){
	_i = 0;
	_j = 0;
	_k = 0;
	_c = 0;
	_vet_idx = 0;
}

void Testbench::idxInc(int layer_dimension_h, int layer_dimension_w, int layer_dimension_c) {
	_c++;
	if (_c == layer_dimension_c) {
		_c = 0;
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
}

void Testbench::printLayer(int n_filter, int h, int w, int32_t map[]) {
	int c,idx1,idx2;
	
	for (c = 0; c < n_filter; c++){
		for (idx1 = 0; idx1 < h; idx1++){
			for (idx2 = 0; idx2 < w; idx2++){
				printf("%d ",map[(h*w*c)+ (h*idx1) + idx2]);
			}
			printf("\n");
		}
	printf("\n");
	}	
}

void Testbench::Conv2d(int16_t weight[], int32_t ifmap[], int n_channel, int filter_h, int filter_w, int stride_h, int stride_w, int input_h, int input_w, int input_c) {
	int m,n;
	
	_wait_eop = 1;
	if (_i != n_channel) {
		
		// Calculating output dimension
		if ((input_h-filter_h)%2 != 0){
			_output_h = ((input_h-filter_h+1)/stride_h);
			_output_w = ((input_w-filter_w+1)/stride_w);
		} else {
			_output_h = ((input_h-filter_h)/stride_h)+1;
			_output_w = ((input_w-filter_w)/stride_w)+1;
		}

		for (m = 0; m < filter_h; m++){
			for (n = 0; n < filter_w; n++){
					_array->SetABuffer(m,n,weight[((_i*input_c*filter_h*filter_w)+((filter_w*m)+n)+(_c*filter_h*filter_w))]);		
					_array->SetBBuffer(m,n,ifmap[input_w*(m+stride_w*_j)+n+(stride_h*_k)+(input_w*input_h*_c)]);
			}
		}	
		_array->SetInit(1);
	}
}

void Testbench::StoreOfmap(int32_t layer_ifmap[], int16_t bias[], int *layer_flag, int input_c, int output_c){
	_wait_eop = 0;
	_array->SetInit(0);
		
	if (_c == input_c-1) {
		_feature = _feature + _array->GetAdderValue();
		_feature = (_feature/SHIFT) + bias[_i];
		
		if (!(_vet_idx >= 0 && _vet_idx < _output_h * _output_w * output_c))
			printf("tb: error! index greather than allowed\n");
		
		layer_ifmap[_vet_idx] = (_feature < 0) ? 0 : _feature;
		_vet_idx++;
		_feature = 0;
	} else {
		_feature = _feature + _array->GetAdderValue();
	}

	this->idxInc(_output_h,_output_w,input_c);
	
	if (_i == output_c) {
		
		if(DEBUG_TB)
			printLayer(output_c,_output_h,_output_w,layer_ifmap);
		
		*layer_flag = 1;
		this->ClearIdx();
		_vet_idx = 0;
	}	
}

int Testbench::Dense(int16_t weight[], int32_t ifmap[], int16_t bias[], int32_t ofmap[],int n_channel, int input_h, int input_w, int input_c){
	int i,j;
	int filter_len;
	int aux,predict = 0;
	
	filter_len = input_h*input_w*input_c;
	
	for(i = 0; i < n_channel; i++) {
		ofmap[i] = bias[i];
		for (j = 0; j < filter_len; j++) {
			ofmap[i] = ofmap[i] + (weight[(i*filter_len)+j]*(ifmap[j]));
		}
		ofmap[i] = (ofmap[i]/SHIFT);
	}
	
	//Softmax activation function
	aux = -999;
	for (i = 0; i < n_channel; i++) {
		if (aux < ofmap[i]) {
			aux = ofmap[i];
			predict = i;
		}
	}
	return predict;
}

void Testbench::TBInit(){
	if (_array->GetEOP() == 0 && _wait_eop == 0 && _end_of_layer1 == 0) {
		Conv2d(Layer1_Weights,Input[_cont_testcases],16,3,3,2,2,28,28,1);
	}
	
	else if (_array->GetEOP() == 0 && _wait_eop == 0 && _end_of_layer1 == 1 && _end_of_layer2 == 0) {
		Conv2d(Layer2_Weights,Layer2_Input,8,3,3,1,1,13,13,16);
	}
	
	else if (_array->GetEOP() == 0 && _wait_eop == 0 && _end_of_layer1 == 1 && _end_of_layer2 == 1 && _end_of_layer3 == 0) {
		Conv2d(Layer3_Weights,Layer3_Input,3,3,3,2,2,11,11,8);
	}
	
	else if (_array->GetEOP() == 0 && _wait_eop == 0 && _end_of_layer1 == 1 && _end_of_layer2 == 1 && _end_of_layer3 == 1 && _end_of_layer4 == 0) {
		Conv2d(Layer4_Weights,Layer4_Input,1,3,3,1,1,5,5,3);
	}
}

void Testbench::TBStore(){
	int predict = 0;
	
	if(_array->GetEOP() == 1 && _end_of_layer1 == 0){
		StoreOfmap(Layer2_Input,Layer1_Bias,&_end_of_layer1,1,16);
	}
	
	else if(_array->GetEOP() == 1 && _end_of_layer1 == 1 && _end_of_layer2 == 0){	
		StoreOfmap(Layer3_Input,Layer2_Bias,&_end_of_layer2,16,8);		
	}
	
	else if(_array->GetEOP() == 1 && _end_of_layer1 == 1 && _end_of_layer2 == 1 && _end_of_layer3 == 0){	
		StoreOfmap(Layer4_Input,Layer3_Bias,&_end_of_layer3,8,3);	
	}	
	
	else if(_array->GetEOP() == 1 && _end_of_layer1 == 1 && _end_of_layer2 == 1 && _end_of_layer3 == 1 && _end_of_layer4 == 0){	
		StoreOfmap(Dense_Input,Layer4_Bias,&_end_of_layer4,3,1);
		if (_end_of_layer4 == 1) {
			predict = Dense(Dense_Weights,Dense_Input,Dense_Bias,Output,10,3,3,1);
			
			if(predict == Label[_cont_testcases])
				_cont_predict++;
			_cont_testcases++;
			
			if(_cont_testcases == N_TESTCASES) {
				printf("%d\n",_cont_predict);
				_end_of_simulation = 1;
			} else {
				_end_of_layer1 = 0;
				_end_of_layer2 = 0;
				_end_of_layer3 = 0;
				_end_of_layer4 = 0;
			}
		}		
	}	
}

SimulationTime Testbench::Run() {
	this->TBInit();
	_array->Run();
	this->TBStore();
    return 1;
}
