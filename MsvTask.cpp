/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Task Implementation
* @details		Contains implementation of @ref MsvTask.
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


#include "MsvTask.h"


/********************************************************************************************************************************
*															Constructors and destructors
********************************************************************************************************************************/


MsvTask::MsvTask(std::function<void()>& task):
	m_pContext(nullptr),
	m_task(task)
{

}


MsvTask::MsvTask(std::function<void(void*)>& task, void* pContext):
	m_pContext(pContext),
	m_taskWithContext(task)
{

}


/********************************************************************************************************************************
*															IMsvTask public methods
********************************************************************************************************************************/


void MsvTask::Execute()
{
	if (m_pContext)
	{
		//execute with context
		m_taskWithContext(m_pContext);
	}
	else
	{
		//execute without context
		m_task();
	}
}


/** @} */	//End of group MTHREADING.
