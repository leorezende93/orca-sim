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

// Model API
#include <Tensorflow.h>
#include <Testbench.h>

// PPA informations
//#include <PPA28nm.h>
#include <PPA65nm.h>


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
	
	// Conv info
	_number_of_convs = 0;
	
	// Tb control
	_wait_eop = 0;
	_end_of_layer1 = 0;
	_end_of_layer2 = 0;
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
		_number_of_convs++;
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
		Conv2d(Layer1_Weights,Input[_cont_testcases],3,3,3,2,2,28,28,1);
	}
	
	else if (_array->GetEOP() == 0 && _wait_eop == 0 && _end_of_layer1 == 1 && _end_of_layer2 == 0) {
		Conv2d(Layer2_Weights,Layer2_Input,1,3,3,1,1,13,13,8);
	}
}

void Testbench::TBStore(){
	int predict = 0;
	
	if(_array->GetEOP() == 1 && _end_of_layer1 == 0){
		StoreOfmap(Layer2_Input,Layer1_Bias,&_end_of_layer1,1,3);
	}
	
	else if(_array->GetEOP() == 1 && _end_of_layer1 == 1 && _end_of_layer2 == 0){	
		StoreOfmap(Dense_Input,Layer2_Bias,&_end_of_layer2,3,1);
		if (_end_of_layer2 == 1) {
			predict = Dense(Dense_Weights,Dense_Input,Dense_Bias,Output,10,11,11,1);
			
			if(predict == Label[_cont_testcases])
				_cont_predict++;
			_cont_testcases++;
			
			if(_cont_testcases == N_TESTCASES) {
				printf("\ntb: accuracy == %.2f\n",_cont_predict/100.0);
				
				// Simulation summary
				printf("\n--------------------------------------------------\n");
				printf("SUMMARY\n");
				printf("--------------------------------------------------\n");
				
				printf("GENERAL INFORMATIONS\n");
				printf("\tTECHNOLOGY NODE: %s\n", TECH_NODE);
				printf("\tOPERATION FREQUENCY: %.2f GHz\n", float(1/PERIOD_VALUE));
				printf("\tACCELERATOR RUN TIME: %d %s\n", RUN_TIME, PERIOD_UNIT);
				printf("\tACCELERATOR THROUGHPUT: %.2f Gb/s\n", THROUGHPUT);
				printf("\tACCELERATOR AREA: %d um^2\n", AREA);
				printf("\tAPPLICATION NUMBER OF CONVULUTIONS: %d\n",_number_of_convs);
				printf("\tAPPLICATION NUMBER OF CYCLES: %d\n", CONV_CYCLES*_number_of_convs);
				printf("\tTOTAL APPLICATION RUN TIME: %d %s\n", _number_of_convs*RUN_TIME, PERIOD_UNIT);
				
				printf("--------------------------------------------------\n");
				
				printf("MINIMUN POWER CONSUPTION CASE\n");
				printf("\tACCELERATOR DYNAMIC POWER: %d %s\n", int(POWER_STD), POWER_UNIT);
				printf("\tACCELERATOR ENERGY: %d uJ\n", int((POWER_STD*1000.0)*(RUN_TIME/1000.0)));
				printf("\tTOTAL APPLICATION ENERGY: %d mJ\n", int((POWER_STD)*((_number_of_convs*RUN_TIME)/1000000.0)));
				
				printf("--------------------------------------------------\n");
				
				printf("AVERAGE POWER  CONSUPTION  CASE\n");
				printf("\tACCELERATOR DYNAMIC POWER: %d %s\n", int(AVG_POWER), POWER_UNIT);
				printf("\tACCELERATOR ENERGY: %d uJ\n", int((AVG_POWER*1000.0)*(RUN_TIME/1000.0)));
				printf("\tTOTAL APPLICATION ENERGY: %d mJ\n", int((AVG_POWER)*((_number_of_convs*RUN_TIME)/1000000.0)));
						
				printf("--------------------------------------------------\n");
				
				printf("MAXIMUN POWER CONSUPTION CASE\n");
				printf("\tACCELERATOR DYNAMIC POWER: %d %s\n", int(POWER_AAA), POWER_UNIT);
				printf("\tACCELERATOR ENERGY: %d uJ\n", int((POWER_AAA*1000.0)*(RUN_TIME/1000.0)));			
				printf("\tTOTAL APPLICATION ENERGY: %d mJ\n", int((POWER_AAA)*((_number_of_convs*RUN_TIME)/1000000.0)));
							
				printf("--------------------------------------------------\n\n");
				
				_end_of_simulation = 1;
			} else {
				_end_of_layer1 = 0;
				_end_of_layer2 = 0;
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
