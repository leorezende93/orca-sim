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
#define SHIFT 100000
#define STRIDE 2
#define NUMBER_OF_LAYERS 5
#define LAYER2_NUMBER_OF_FILTERS 6
#define LAYER3_NUMBER_OF_FILTERS 50
#define LAYER4_NUMBER_OF_FILTERS 50

// Matrix dimensions
#define IMAGE_DIMENSION 29
#define FILTER_DIMENSION 5
#define LAYER2_DIMENSION 13
#define LAYER3_DIMENSION 5
#define LAYER4_DIMENSION 100
#define LAYER5_DIMENSION 10

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
	
	int Layer2_Output[LAYER2_NUMBER_OF_FILTERS * LAYER2_DIMENSION * LAYER2_DIMENSION];
	int Layer3_Output[LAYER3_NUMBER_OF_FILTERS * LAYER3_DIMENSION * LAYER3_DIMENSION];
	int Layer4_Output[LAYER4_DIMENSION];
	
	float Layer4_Input[LAYER3_NUMBER_OF_FILTERS * LAYER3_DIMENSION * LAYER3_DIMENSION];
	float Layer5_Input[LAYER4_DIMENSION];
	
	float Layer5_Output[LAYER5_DIMENSION];
		
	// Convolution index
	int _i,_j,_k;
	int _vet_idx;
	
	// Testbench control
	int _parallel_control;
	int _end_of_layer2;
	int _end_of_layer3;
	int _end_of_simulation;
	
	// Testbench functions
	void TBInit();
	void TBStore();
	void ClearIdx();
	void idxInc(int layer_dimension_w, int layer_dimension_h);
	void calculateLayer5(float* Layer4_Neurons_CPU, short* Layer4_Weights_CPU, float* Layer5_Neurons_CPU);
	
	// Neural network functions
	void Conv(int weights[][FILTER_DIMENSION], int feature_map[][FILTER_DIMENSION]);
	
public: 
	Testbench(string name);
	~Testbench();

	std::string GetName();
	int GetEndOfSimulation();
	
	void Reset();
	SimulationTime Run();
};

#endif /* __TESTBENCH_H */
