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
#ifndef __TESTBENCH_H
#define __TESTBENCH_H

// Parameters
#define DEBUG_TB 0
#define N_TESTCASES 100

//std API
#include <iostream>
#include <math.h>

//model API
#include <UMemory.h>
#include <USignal.h>
#include <TSystolicArray.h>

// Macros
#define SIGMOID(x) (1.7159*tanh(0.66666667*x))

/**
 * @class Testbench
 * @author Leonardo Rezende
 * @date 01/04/20
 * @file Testbench.h
 * @brief This class models an entire processing element that contains
 * RAM memory, DMA for the SIMD vector unit,  HFRiscV core
 */
class Testbench: public TimedModel{

private:
	///@{ Main components of the system.
	/// the systolic array
	TSystolicArray* _array; 
	
	// Internal buffers
	int _a_buffer[N][N];
    int _b_buffer[N][N];
    int _output_h, _output_w;
	
	// Convolution buffer
	int32_t Layer2_Input[16 * 13 * 13];
	int32_t Layer3_Input[11 * 11 * 8];
	int32_t Layer4_Input[5 * 5 * 3];
	int32_t Dense_Input[3 * 3 * 1];
	int32_t Output[10];
	
	// Convolution index
	volatile int _i;
	volatile int _j;
	volatile int _k;
	volatile int _c;
	int _vet_idx;
	
	// Testbench control
	int _wait_eop;
	int _end_of_layer1;
	int _end_of_layer2;
	int _end_of_layer3;
	int _end_of_layer4;
	int _end_of_simulation;
	
	// Testcases control
	int _cont_predict;
	int _cont_testcases;
	
	// Testbench variables
	int32_t _feature = 0;
		
	// Testbench functions
	void TBInit();
	void TBStore();
	void Conv2d(int16_t weight[], int32_t ifmap[], int n_channel, int filter_h, int filter_w, int stride_h, int stride_w, int input_h, int input_w, int input_c);
	void StoreOfmap(int32_t layer_ifmap[], int16_t bias[], int *layer_flag, int input_c, int output_c);
	void printLayer(int n_filter, int h, int w, int32_t map[]);
	int Dense(int16_t weight[], int32_t ifmap[], int16_t bias[], int32_t ofmap[],int n_channel, int input_h, int input_w, int input_c);
	
public: 
	Testbench(string name);
	~Testbench();

	std::string GetName();
	int GetEndOfSimulation();
	
	void idxInc(int layer_dimension_h, int layer_dimension_w, int layer_dimension_c);
	void ClearIdx();
	
	void Reset();
	SimulationTime Run();
};

#endif /* __TESTBENCH_H */
