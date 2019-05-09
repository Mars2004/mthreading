/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Event Interface
* @details		Contains definition of @ref IMsvEvent interface.
* @author		Martin Svoboda
* @date			04.04.2019
* @copyright	GNU General Public License (GPLv3).
******************************************************************************************************/


/*
This file is part of MarsTech Threading.

MarsTech Dependency Injection is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MarsTech Promise Like Syntax is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar. If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef MARSTECH_IEVENT_H
#define MARSTECH_IEVENT_H


#include "merror/MsvError.h"


/**************************************************************************************************//**
* @brief		MarsTech Event Interface.
* @details	Interface for event.
******************************************************************************************************/
class IMsvEvent
{
public:
	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~IMsvEvent() {};

	/**************************************************************************************************//**
	* @brief			Reset event.
	* @details		Resets event for next usage.
	******************************************************************************************************/
	virtual void ResetEvent() = 0;

	/**************************************************************************************************//**
	* @brief			Set event.
	* @details		Set events - notifies one thread or all threads (based on notifyAllThreads parameter).
	* @param[in]	notifyAllThreads		Flag to notify all threads (true) or one of threads (false).
	******************************************************************************************************/
	virtual void SetEvent(bool notifyAllThreads = false) = 0;

	/**************************************************************************************************//**
	* @brief			Wait for event.
	* @details		Waits for event until @ref SetEvent isn't set.
	******************************************************************************************************/
	virtual MsvErrorCode WaitForEvent() = 0;

	/**************************************************************************************************//**
	* @brief			Wait for event.
	* @details		Waits for event until @ref SetEvent isn't set or timeouted.
	* @param[in]	timeout					Timeout in microseconds.
	******************************************************************************************************/
	virtual MsvErrorCode WaitForEvent(uint32_t timeout) = 0;

	/**************************************************************************************************//**
	* @brief			Wait for event and reset.
	* @details		Waits for event until @ref SetEvent isn't set. Resets event when is set.
	******************************************************************************************************/
	virtual MsvErrorCode WaitForEventAndReset() = 0;

	/**************************************************************************************************//**
	* @brief			Wait for event and reset.
	* @details		Waits for event until @ref SetEvent isn't set or timeouted. Resets event when is set.
	* @param[in]	timeout					Timeout in microseconds.
	******************************************************************************************************/
	virtual MsvErrorCode WaitForEventAndReset(uint32_t timeout) = 0;
};


#endif // MARSTECH_IEVENT_H

/** @} */	//End of group MTHREADING.
