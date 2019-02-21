/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Worker Interface
* @details		Contains definition of @ref IMsvWorker interface.
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


#ifndef MARSTECH_IWORKER_H
#define MARSTECH_IWORKER_H


#include "IMsvThread.h"
#include "IMsvTask.h"

MSV_DISABLE_ALL_WARNINGS

#include <memory>
#include <functional>

MSV_ENABLE_WARNINGS


/**************************************************************************************************//**
* @brief		MarsTech Worker Interface.
* @details	Interface for worker which executes jobs/tasks (functions or @ref IMsvTask).
* @see		IMsvThread
* @see		IMsvTask
******************************************************************************************************/
class IMsvWorker:
	virtual public IMsvThread
{
public:
	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~IMsvWorker() {};

	/**************************************************************************************************//**
	* @brief			Add job/task to worker.
	* @details		Adds spTask to queue.
	* @param[in]	spTask	Shared pointer to @ref IMsvTask. It will be assigned to queue and executed.
	* @see			IMsvTask
	******************************************************************************************************/
	virtual void AddTask(std::shared_ptr<IMsvTask> spTask) = 0;

	/**************************************************************************************************//**
	* @brief			Add job/task to worker.
	* @details		Adds task to queue (it creates @ref IMsvTask from task).
	* @param[in]	task		Function. It will be assigned to queue and executed.
	* @returns		MsvErrorCode
	* @retval		MsvAllocationError	When create @ref IMsvTask failed.
	* @retval		MsvSuccess				On success.
	******************************************************************************************************/
	virtual MsvErrorCode AddTask(std::function<void()>& task) = 0;

	/**************************************************************************************************//**
	* @brief			Add job/task to worker.
	* @details		Adds task to queue (it creates @ref IMsvTask from task and pContext).
	* @param[in]	task		Function. It will be assigned to queue and executed.
	* @param[in]	pContext	Context. It will be set as task parameter.
	* @returns		MsvErrorCode
	* @retval		MsvAllocationError	When create @ref IMsvTask failed.
	* @retval		MsvSuccess				On success.
	******************************************************************************************************/
	virtual MsvErrorCode AddTask(std::function<void(void*)>& task, void* pContext) = 0;
};


#endif // MARSTECH_IWORKER_H

/** @} */	//End of group MTHREADING.
