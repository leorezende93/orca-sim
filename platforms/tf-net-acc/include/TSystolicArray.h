/** 
 * This file is part of project URSA. More information on the project
 * can be found at 
 *
 * URSA's repository at GitHub: http://https://github.com/andersondomingues/ursa
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
#ifndef __TSYSTOLICARRAY_H
#define __TSYSTOLICARRAY_H

#define N 3
#define DEBUG 0

//simulator API
#include <TimedModel.h>
#include <TPE.h>

#include <string>

enum class SystolicArrayState{
	INIT_ARRAY,
	START_MULT,
	SHIFT_OUT,
	END_OP,
};

class TSystolicArray: public TimedModel{

private:	
	TPE* _PE[N][N];
	
	uint64_t _a_buffer[N][N]; 
	uint64_t _b_buffer[N][N];
		
	int64_t _z_buffer[N][N];
	
	int64_t _acc;
    
    SystolicArrayState _systolic_array_state;
    
    // Control signals
    int _init;
    int _start;
    int _eop;
    int _shift_acc;
    int _shift_out;
    
    // Internal registers
    int _cont_row;
	int _cont_column;
	
	// States
	void InitArray();
	void StartMult();
	void ShiftOut();
	void EndOp();

public:
	// Setters
	void SetABuffer(int i, int j, uint64_t data);
	void SetBBuffer(int i, int j, uint64_t data);
	void SetInit(int init);
    
    // Getters
    int64_t GetAdderValue();
    int GetEOP();
    
    // Other 
    SimulationTime Run();
    std::string GetName();
    void Reset();  

    TSystolicArray(string name);

    ~TSystolicArray();
};

#endif /* __TSYSTOLICARRAY_H */
