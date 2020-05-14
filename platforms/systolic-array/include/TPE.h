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
#ifndef __TPE_H
#define __TPE_H

//std API
#include <iostream>

//simulator API
#include <TimedModel.h>

class TPE: public TimedModel{

private:	
	// ID
	uint8_t _x; 
	uint8_t _y;

	// Control signals
	int *_start_in;
	
	// Inputs
	int *_a_in;
	int *_b_in;
	
	// Outputs
	int *_a_out;
	int *_b_out;
		
	// Internal Registers
	int _acc;
		
public:	
    // Setters
    void SetTPEAInput(int* a_in);
    void SetTPEBInput(int* b_in);
    
	// Getters for ports
	int* GetTPEAOutput();
	int* GetTPEBOutput();
    
    // Getters for values
    int GetAInputValue();
	int GetBInputValue();
	int GetAOutputValue();
	int GetBOutputValue();
	int GetMACResult();

    // Other 
    void Reset();
    void DoMAC();	
    void ShiftTPEAInput();
    void ShiftTPEBInput();
    void ShiftTPEResult();
	SimulationTime Run();  

    TPE(string name, uint8_t x, uint8_t y, 
		int &start_in);

    ~TPE();
};

#endif /* __TPE_H */
