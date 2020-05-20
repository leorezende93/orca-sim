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

void Testbench::TBInit(){
	int m,n;
	
	
	//float acc;
	//float Layer2[LAYER2_DIMENSION][LAYER2_DIMENSION];
		
	int i = 0;
	//_j = 0;
	m = 0;
	n = 0;

	//_array->SetInit(0);
	
	if (_j != 13 && _array->GetEOP() == 0) {
		printf("tb: starting testbench!\n");
		//_flag = 0;
		//for (i = 0; i < NUMBER_OF_FILTERS; i++){
			//for (_j = 0; _j < LAYER2_DIMENSION; _j++){
				//for (k = 0; k < LAYER2_DIMENSION; k++){
					//acc = Layer1_Weights_CPU[k*(NUMBER_OF_FILTERS*NUMBER_OF_FILTERS+1)];
					for (m = 0; m < FILTER_DIMENSION; m++){
						//printf("tb: starting convolution!\n");
						for (n = 0; n < FILTER_DIMENSION; n++){
							_array->SetABuffer(m,n,Layer1_Weights_CPU[(NUMBER_OF_FILTERS*NUMBER_OF_FILTERS+1)*i + FILTER_DIMENSION*m + n + 1]);
							//Layer1_Weights_CPU[26*i+5*m+n+1]
							_array->SetBBuffer(m,n,data0[IMAGE_DIMENSION*(m+STRIDE*_j) + n + STRIDE*_k]);
							//Layer1_Neurons_CPU[29*(m+2*_j)+n+2*k]
						}
					}
					
					printf("tb: starting PEs!\n");
					_array->SetInit(1);
					//_array->WaitBuffersInitialization();
					//_array->SetInit(0);
					//_array->DoComputation();
				
					//printf("tb: sum result = %d\n",_array->GetZBufferSum());
					//acc = acc + _array->GetZBufferSum();
					//Layer2[m][n] = SIGMOID(acc/100.0);
					//printf("tb: conv result = %.2f\n",Layer2[m][n]);
				//}
			//}
		//}
	}
	
	//printf("tb: conv result!\n");
	//for (m = 0; m < LAYER2_DIMENSION; m++){
	//	for (n = 0; n < LAYER2_DIMENSION; n++){
	//		printf("%.2f ",Layer2[m][n]);
	//	}
	//	printf("\n");
	//}
	//getchar();
}

void Testbench::TBStore(){
	int x,y,i = 0;
	
	if(_array->GetEOP() == 1){
		
		printf("tb: storing conv value\n");
		
		_array->SetInit(0);
		
		Layer2_f0[_j][_k] = SIGMOID((_array->GetAdderValue() + Layer1_Weights_CPU[i*(NUMBER_OF_FILTERS*NUMBER_OF_FILTERS+1)])/100.0);
		
		printf("tb: partial conv result!\n");
		for (x = 0; x <= _j; x++) {
			for (y = 0; y <= _k; y++) {
				printf("%.2f ",Layer2_f0[x][y]);
			}
			printf("\n");
		}
		
		
		_k++;
		if (_k == LAYER2_DIMENSION) {
			_j++;
			_k = 0;
		}	
		
		if (_j == LAYER2_DIMENSION)
			while(1);
		
		//getchar();
	}
}

SimulationTime Testbench::Run() {
	this->TBInit();
	_array->Run();
	this->TBStore();
    return 1;
}
