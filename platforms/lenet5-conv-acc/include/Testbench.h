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
#define NUMBER_OF_FILTERS 6
#define STRIDE 2

// Matrix dimensions
#define IMAGE_DIMENSION 29
#define FILTER_DIMENSION 5
#define LAYER2_DIMENSION 13

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
	
	// Software buffer
    typedef struct {
		int  ofmap[LAYER2_DIMENSION][LAYER2_DIMENSION];
	} Layer2OutputFeatureMap;
	Layer2OutputFeatureMap Layer2[NUMBER_OF_FILTERS];   
	
	// Output log generation
	ofstream log;		
	
	// Convolution index
	int _i,_j,_k;
	
	// Testbench control
	int _end_of_simulation;
	
	// Testbench functions
	void TBInit();
	void TBStore();
	void Conv2d(short weights[], short ifmap[], int filter_h, int filter_w, int ifmap_dimension, int stride_w, int stride_h, int channel, int ofmap_h, int ofmap_w);
	
public: 
	Testbench(string name);
	~Testbench();

	std::string GetName();
	int GetEndOfSimulation();
	
	void Reset();
	SimulationTime Run();
};

#endif /* __TESTBENCH_H */
