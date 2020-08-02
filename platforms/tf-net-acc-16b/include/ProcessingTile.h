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
#ifndef __ProcessingTile_H
#define __ProcessingTile_H

#define NFILTER 5

//std API
#include <iostream>

//model API
#include <UMemory.h>
#include <USignal.h>
#include <Testbench.h>

/**
 * @class ProcessingTile
 * @author Leonardo Rezende
 * @date 01/04/20
 * @file ProcessingTile.h
 * @brief This class models an entire processing element that contains
 * RAM memory, DMA for the SIMD vector unit,  HFRiscV core
 */
class ProcessingTile{

private:

	///@{ Main components of the system.
	/// the systolic array
	Testbench* _tb; 
	
	// Internal buffers
	int _a_buffer[N][N];
    int _b_buffer[N][N];
	
	//hosttime magic wire
	uint32_t _shosttime;
	USignal<uint32_t>* _signal_hosttime;
		
public: 
	ProcessingTile();
	~ProcessingTile();
	
	/**
	 * @brief Get current signal for systime signal
	 * @return A pointer to the instance of signal
	 */
	USignal<uint32_t>* GetSignalHostTime();
		
	// Getters
	Testbench* GetTestbench();
	int GetTbEndOfSimulation();
	std::string GetName();
	
	// Others
	void Reset();
	std::string ToString();
};


#endif /* __ProcessingTile_H */
