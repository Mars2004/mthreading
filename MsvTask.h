/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Task Implementation
* @details		Contains implementation @ref MsvTask of @ref IMsvTask interface.
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


#ifndef MARSTECH_TASK_H
#define MARSTECH_TASK_H


#include "IMsvTask.h"

#include "merror/MsvCompiler.h"
MSV_DISABLE_ALL_WARNINGS

#include <functional>

MSV_ENABLE_WARNINGS


/**************************************************************************************************//**
* @brief		MarsTech Task Wrapper.
* @details	Implementation for different job/tasks to @ref IMsvTask interface.
* @see		IMsvTask
******************************************************************************************************/
class MsvTask:
	public IMsvTask
{
public:
	/**************************************************************************************************//**
	* @brief		Constructor.
	* @details	Creates wrapper which executes void() function.
	* @param		task			Reference to task (void() function).
	******************************************************************************************************/
	MsvTask(std::function<void()>& task);

	/**************************************************************************************************//**
	* @brief		Constructor.
	* @details	Creates wrapper which executes void(void*) function and sets pContext as the first parameter.
	* @param		task			Reference to task (void(void*) function).
	* @param		pContext		Context for task (the first parameter of task).
	******************************************************************************************************/
	MsvTask(std::function<void(void*)>& task, void* pContext);

protected:
	/**************************************************************************************************//**
	* @copydoc IMsvTask::Execute()
	******************************************************************************************************/
	virtual void Execute() override;

protected:
	/**************************************************************************************************//**
	* @brief		Task context.
	* @details	Pointer to context which is used as the first task parameter.
	* @see		m_taskWithContext
	******************************************************************************************************/
	void* m_pContext;

	/**************************************************************************************************//**
	* @brief		Task without context.
	* @details	Job/task function.
	******************************************************************************************************/
	std::function<void()> m_task;

	/**************************************************************************************************//**
	* @brief		Task with context.
	* @details	Job/task function with context (the first parameter is context).
	* @see		m_pContext
	******************************************************************************************************/
	std::function<void(void*)> m_taskWithContext;
};


#endif // MARSTECH_TASK_H

/** @} */	//End of group MTHREADING.
