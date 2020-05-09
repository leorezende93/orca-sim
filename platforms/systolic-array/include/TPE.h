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
	int *_shift_acc_in;
	int *_shift_z_in;
	
	// Inputs
	uint32_t _a_in;
	uint32_t _b_in;
	uint32_t _z_in;
	
	// Outputs
	uint32_t _z_out;
	
	// Internal Registers
	uint32_t _acc;
	
public:	
    // Setters
    void SetTPEAInput(uint32_t z_out);
    void SetTPEBInput(uint32_t z_in);
    void SetTPEZInput(uint32_t z_in);
    
    // Getters
    uint8_t GetTPEZInput();
	uint8_t GetTPEX();
	uint8_t GetTPEY();
	uint32_t GetMACResult();
	uint32_t GetTPEZOutput();
	

    // Other 
    SimulationTime Run();
    void DoMAC();		
    void ShiftTPEResult();
    void Reset();

    TPE(string name, uint8_t x, uint8_t y, 
		int &start_in, int &shift_acc_in, int &shift_z_in,
		uint32_t a_in, uint32_t b_in, uint32_t z_in);

    ~TPE();
};

#endif /* __TPE_H */
