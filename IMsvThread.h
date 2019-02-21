/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Thread Interface
* @details		Contains definition of @ref IMsvThreadPool interface.
* @author		Martin Svoboda
* @date			26.11.2018
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


#ifndef MARSTECH_ITHREAD_H
#define MARSTECH_ITHREAD_H


#include "merror/MsvError.h"

#include "merror/MsvCompiler.h"
MSV_DISABLE_ALL_WARNINGS

#include <cstdint>

MSV_ENABLE_WARNINGS


/**************************************************************************************************//**
* @brief		MarsTech Thread Interface.
* @details	Interface for threads.
* @see		MsvThread
******************************************************************************************************/
class IMsvThread
{
public:
	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~IMsvThread() {};

	/**************************************************************************************************//**
	* @brief			Check if thread is running.
	* @details		Returns flag if thread is running (true) or not (false).
	* @returns		bool
	* @retval		true	When thread is running.
	* @retval		false	When thread is not running.
	******************************************************************************************************/
	virtual bool IsRunning() const = 0;

	/**************************************************************************************************//**
	* @brief			Notify thread.
	* @details		Notifies thread - wakes up sleeping thread.
	******************************************************************************************************/
	virtual void Notify() const = 0;

	/**************************************************************************************************//**
	* @brief			Start thread.
	* @details		Starts thread execution. There can be three modes based on timeout value. Negative value
	*					means execute ones, zero value means execute and wait for @ref Notify, positive value
	*					means execute in loop timeout (thread can be woke up by @ref Notify too).
	* @param[in]	timeout							Timeout in microseconds.
	* @returns		MsvErrorCode
	* @retval		MSV_ALLOCATION_ERROR			On failed.
	* @retval		MSV_ALREADY_RUNNING_INFO	When thread is already running (interpreted as success too).
	* @retval		MsvSuccess						On success.
	* @warning		Call @ref StartThread only once (at least before @ref StopThread and @ref WaitForThreadStop
	*					or crash can occure). 
	******************************************************************************************************/
	virtual MsvErrorCode StartThread(int32_t timeout = 0) = 0;

	/**************************************************************************************************//**
	* @brief			Stop thread.
	* @details		Stops thread execution (send request to stop thread).
	* @returns		MsvErrorCode
	* @retval		MSV_NOT_RUNNING_INFO			When thread is not running (interpreted as success too).
	* @retval		MSV_ALREADY_REQUESTED_INFO	When stop request has been already called (interpreted as success too).
	* @retval		MsvSuccess						On success.
	******************************************************************************************************/
	virtual MsvErrorCode StopThread() = 0;

	/**************************************************************************************************//**
	* @brief			Stop thread and wait for thread stop.
	* @details		Stops thread execution (send request to stop thread) and waits for its stop. Internally
	*					calls @ref StopThread and @ref WaitForThreadStop.
	* @param[in]	timeout							Wait timeout in microseconds (it is used as parameter of @ref WaitForThreadStop).
	* @returns		MsvErrorCode
	* @retval		MSV_NOT_RUNNING_INFO			When thread is not running (interpreted as success too).
	* @retval		MSV_ALREADY_REQUESTED_INFO	When stop request has been already called (interpreted as success too).
	* @retval		MSV_STILL_RUNNING_WARN		When timeouted - thread is still running (interpreted as failed).
	* @retval		MsvSuccess						On success.
	* @see			StopThread
	* @see			WaitForThreadStop
	******************************************************************************************************/
	virtual MsvErrorCode StopAndWaitForThreadStop(int32_t timeout = 30000000) = 0;

	/**************************************************************************************************//**
	* @brief			Wait for thread stop.
	* @details		Waits (blocks current calling thread) for thread stop. @ref StopThread must be called before
	*					this method. When timeout is 0, waits infinite to thread stop.
	* @param[in]	timeout							Wait timeout in microseconds.
	* @returns		MsvErrorCode
	* @retval		MSV_NOT_RUNNING_INFO			When thread is not running (interpreted as success too).
	* @retval		MSV_STILL_RUNNING_WARN		When timeouted - thread is still running (interpreted as failed).
	* @retval		MsvSuccess						On success.
	* @see			StopThread
	******************************************************************************************************/
	virtual MsvErrorCode WaitForThreadStop(int32_t timeout = 30000000) = 0;
};


#endif // !MARSTECH_ITHREAD_H

/** @} */	//End of group MTHREADING.
