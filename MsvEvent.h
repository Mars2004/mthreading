/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Event Implementation
* @details		Contains implementation @ref MsvEvent of @ref IMsvEvent interface.
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


#ifndef MARSTECH_EVENT_H
#define MARSTECH_EVENT_H


#include "IMsvEvent.h"

MSV_DISABLE_ALL_WARNINGS

#include <mutex>

MSV_ENABLE_WARNINGS


/**************************************************************************************************//**
* @brief		MarsTech Event Implementation.
* @details	Implementation of @ref IMsvEvent interface.
* @see		IMsvEvent
******************************************************************************************************/
class MsvEvent:
	public IMsvEvent
{
public:
	/**************************************************************************************************//**
	* @brief		Constructor.
	* @details	Creates event.
	******************************************************************************************************/
	MsvEvent();

	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~MsvEvent();

	/**************************************************************************************************//**
	* @copydoc IMsvEvent::ResetEvent()
	******************************************************************************************************/
	virtual void ResetEvent() override;

	/**************************************************************************************************//**
	* @copydoc IMsvEvent::SetEvent(bool notifyAllThreads)
	******************************************************************************************************/
	virtual void SetEvent(bool notifyAllThreads = false) override;

	/**************************************************************************************************//**
	* @copydoc IMsvEvent::WaitForEvent()
	******************************************************************************************************/
	virtual MsvErrorCode WaitForEvent() override;

	/**************************************************************************************************//**
	* @copydoc IMsvEvent::WaitForEvent(uint32_t timeout)
	******************************************************************************************************/
	virtual MsvErrorCode WaitForEvent(uint32_t timeout) override;

	/**************************************************************************************************//**
	* @copydoc IMsvEvent::WaitForEvent()
	******************************************************************************************************/
	virtual MsvErrorCode WaitForEventAndReset() override;

	/**************************************************************************************************//**
	* @copydoc IMsvEvent::WaitForEvent(uint32_t timeout)
	******************************************************************************************************/
	virtual MsvErrorCode WaitForEventAndReset(uint32_t timeout) override;

protected:
	/**************************************************************************************************//**
	* @brief		Condition variable.
	* @details	It is used for waiting for event (waits for timeout or notification).
	******************************************************************************************************/
	std::condition_variable m_condVar;

	/**************************************************************************************************//**
	* @brief		Condition variable mutex.
	* @details	Locks condition variable @ref m_condVar.
	* @see		m_condVar
	******************************************************************************************************/
	std::mutex m_condVarMutex;

	/**************************************************************************************************//**
	* @brief		Condition variable predicate.
	* @details	Flag if condition is ready.
	* @see		m_condVar
	******************************************************************************************************/
	bool m_ready;

};


#endif // MARSTECH_EVENT_H

/** @} */	//End of group MTHREADING.
