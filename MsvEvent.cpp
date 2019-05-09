/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Event Implementation
* @details		Contains implementation of @ref MsvEvent.
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


#include "MsvEvent.h"

#include "merror/MsvErrorCodes.h"


/********************************************************************************************************************************
*															Constructors and destructors
********************************************************************************************************************************/


MsvEvent::MsvEvent():
	m_ready(false)
{

}

MsvEvent::~MsvEvent()
{

}


/********************************************************************************************************************************
*															IMsvEvent public methods
********************************************************************************************************************************/


void MsvEvent::ResetEvent()
{
	std::unique_lock<std::mutex> conditionLock(m_condVarMutex);
	m_ready = false;
}

void MsvEvent::SetEvent(bool notifyAllThreads)
{
	std::unique_lock<std::mutex> conditionLock(m_condVarMutex);
	m_ready = true;
	conditionLock.unlock();

	//mutex must be unlocked before notify

	if (notifyAllThreads)
	{
		m_condVar.notify_all();
	}
	else
	{
		m_condVar.notify_one();
	}
}

MsvErrorCode MsvEvent::WaitForEvent()
{
	std::unique_lock<std::mutex> conditionLock(m_condVarMutex);

	m_condVar.wait(conditionLock, [this] { return m_ready; });

	return MSV_SUCCESS;
}

MsvErrorCode MsvEvent::WaitForEvent(uint32_t timeout)
{
	std::unique_lock<std::mutex> conditionLock(m_condVarMutex);

	//returns false if timoouted and predicate returns false
	bool result = m_condVar.wait_for(conditionLock, std::chrono::microseconds(timeout), [this] { return m_ready; });

	return result ? MSV_SUCCESS : MSV_EXPIRED_INFO;
}

MsvErrorCode MsvEvent::WaitForEventAndReset()
{
	std::unique_lock<std::mutex> conditionLock(m_condVarMutex);

	m_condVar.wait(conditionLock, [this] { return m_ready; });

	//condition is locked we can reset without locking
	m_ready = false;

	return MSV_SUCCESS;
}

MsvErrorCode MsvEvent::WaitForEventAndReset(uint32_t timeout)
{
	std::unique_lock<std::mutex> conditionLock(m_condVarMutex);

	//returns false if timoouted and predicate returns false
	bool result = m_condVar.wait_for(conditionLock, std::chrono::microseconds(timeout), [this] { return m_ready; });

	//condition is locked we can reset without locking
	//don't have to check if expired or set (if expired m_ready is false)
	m_ready = false;

	return result ? MSV_SUCCESS : MSV_EXPIRED_INFO;
}


/** @} */	//End of group MTHREADING.
