/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Thread Pool Interface
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


#ifndef MARSTECH_ITHREADPOOL_H
#define MARSTECH_ITHREADPOOL_H


#include "IMsvTask.h"

#include "merror/MsvError.h"

MSV_DISABLE_ALL_WARNINGS

#include <memory>
#include <functional>

MSV_ENABLE_WARNINGS


/**************************************************************************************************//**
* @brief		MarsTech Thread Pool Interface.
* @details	Thread pool with task/task queue.
******************************************************************************************************/
class IMsvThreadPool
{
public:
	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~IMsvThreadPool() {}

	/**************************************************************************************************//**
	* @brief			Add job/task to worker.
	* @details		Adds spTask to queue.
	* @param[in]	spTask	Shared pointer to @ref IMsvTask. It will be assigned to queue and executed
	*								by one of thread pool worker thread.
	* @see			IMsvTask
	******************************************************************************************************/
	virtual void AddTask(std::shared_ptr<IMsvTask> spTask) = 0;

	/**************************************************************************************************//**
	* @brief			Add job/task to worker.
	* @details		Adds task to queue (it creates @ref IMsvTask from task).
	* @param[in]	task		Function. It will be assigned to queue and executed by one of thread pool
	*								worker thread.
	* @returns		MsvErrorCode
	* @retval		MsvAllocationError	When create @ref IMsvTask failed.
	* @retval		MsvSuccess				On success.
	******************************************************************************************************/
	virtual MsvErrorCode AddTask(std::function<void()>& task) = 0;

	/**************************************************************************************************//**
	* @brief			Add job/task to worker.
	* @details		Adds task to queue (it creates @ref IMsvTask from task and pContext).
	* @param[in]	task		Function. It will be assigned to queue and executed by one of thread pool
	*								worker thread.
	* @param[in]	pContext	Context. It will be set as task parameter.
	* @returns		MsvErrorCode
	* @retval		MsvAllocationError	When create @ref IMsvTask failed.
	* @retval		MsvSuccess				On success.
	******************************************************************************************************/
	virtual MsvErrorCode AddTask(std::function<void(void*)>& task, void* pContext) = 0;

	/**************************************************************************************************//**
	* @brief			Check if thread pool is running.
	* @details		Returns flag if thread pool is running (true) or not (false).
	* @returns		bool
	* @retval		true	When thread pool is running.
	* @retval		false	When thread pool is not running.
	******************************************************************************************************/
	virtual bool IsRunning() const = 0;

	/**************************************************************************************************//**
	* @brief			Start thread pool.
	* @details		Starts thread pool execution with specific thread count.
	* @param[in]	threadCount						Thread count, which thread pool will create.
	* @returns		MsvErrorCode
	* @retval		MSV_ALLOCATION_ERROR			On failed.
	* @retval		MSV_ALREADY_RUNNING_INFO	When thread pool is already running (interpreted as success too).
	* @retval		MsvSuccess						On success.
	******************************************************************************************************/
	virtual MsvErrorCode StartThreadPool(uint16_t threadCount = 4) = 0;

	/**************************************************************************************************//**
	* @brief			Stop thread pool.
	* @details		Stops thread pool execution (send request to stop to all worker threads).
	* @returns		MsvErrorCode
	* @retval		MSV_NOT_RUNNING_INFO			When thread pool is not running (interpreted as success too).
	* @retval		MSV_ALREADY_REQUESTED_INFO	When stop request has been already called (interpreted as success too).
	* @retval		MsvSuccess						On success.
	******************************************************************************************************/
	virtual MsvErrorCode StopThreadPool() = 0;

	/**************************************************************************************************//**
	* @brief			Stop thread pool and wait for thread pool stop.
	* @details		Stops thread pool execution (send request to stop to worker threads) and waits for its stop. Internally
	*					calls @ref StopThreadPool and @ref WaitForThreadPoolStop.
	* @param[in]	timeout							Wait timeout in microseconds (it is used as parameter of @ref IMsvThread::WaitForThreadStop).
	* @returns		MsvErrorCode
	* @retval		MSV_NOT_RUNNING_INFO			When thread is not running (interpreted as success too).
	* @retval		MSV_ALREADY_REQUESTED_INFO	When stop request has been already called (interpreted as success too).
	* @retval		MSV_STILL_RUNNING_WARN		When timeouted - thread pool is still running (interpreted as failed).
	* @retval		MsvSuccess						On success.
	* @see			StopThreadPool
	* @see			WaitForThreadPoolStop
	******************************************************************************************************/
	virtual MsvErrorCode StopAndWaitForThreadPoolStop(int32_t timeout = 30000000) = 0;

	/**************************************************************************************************//**
	* @brief			Wait for thread pool stop.
	* @details		Waits (blocks current calling thread) for thread pool stop. @ref StopThreadPool must be called before
	*					this method. When timeout is 0, waits infinite to thread pool stop.
	* @param[in]	timeout							Wait timeout in microseconds.
	* @returns		MsvErrorCode
	* @retval		MSV_NOT_RUNNING_INFO			When thread pool is not running (interpreted as success too).
	* @retval		MSV_STILL_RUNNING_WARN		When timeouted - thread pool is still running (interpreted as failed).
	* @retval		MsvSuccess						On success.
	* @see			StopThread
	******************************************************************************************************/
	virtual MsvErrorCode WaitForThreadPoolStop(int32_t timeout = 30000000) = 0;
};


#endif // !MARSTECH_ITHREADPOOL_H

/** @} */	//End of group MTHREADING.
