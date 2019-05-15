/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Thread Implementation
* @details		Contains implementation of @ref MsvThread.
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


#include "MsvThread.h"

#include "merror/MsvErrorCodes.h"


/********************************************************************************************************************************
*															Constructors and destructors
********************************************************************************************************************************/


MsvThread::MsvThread():
	MsvThread(nullptr, nullptr, nullptr)
{
}

MsvThread::MsvThread(std::shared_ptr<std::condition_variable> spConditionVariable, std::shared_ptr<std::mutex> spConditionVariableMutex, std::shared_ptr<uint64_t> spConditionVariablePredicate):
	m_isRunning(false),
	m_spConditionVariable(spConditionVariable ? spConditionVariable : std::shared_ptr<std::condition_variable>(new (std::nothrow) std::condition_variable)),
	m_spConditionVariableMutex(spConditionVariableMutex ? spConditionVariableMutex : std::shared_ptr<std::mutex>(new (std::nothrow) std::mutex)),
	m_spConditionVariablePredicate(spConditionVariablePredicate),
	m_stopRequested(false),
	m_timeout(0),
	m_dataReady(false),
	m_stopReady(false)
{
}

MsvThread::~MsvThread()
{
	//stop thread if still running
	StopThread();
	if (m_thread.joinable())
	{
		//still joinable -> detach
		m_thread.detach();
	}
}


/********************************************************************************************************************************
*															IMsvThread public methods
********************************************************************************************************************************/


bool MsvThread::IsRunning() const
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);

	return m_isRunning;
}

void MsvThread::Notify() const
{
	if (m_spConditionVariable)
	{
		std::unique_lock<std::mutex> conditionLock(*m_spConditionVariableMutex);
		m_dataReady = true;
		/*if (m_spConditionVariablePredicate)
		{
			(*m_spConditionVariablePredicate)++;
		}*/
		conditionLock.unlock();

		m_spConditionVariable->notify_all();
	}
}

MsvErrorCode MsvThread::StartThread(int32_t timeout)
{
	if (!m_spConditionVariable || !m_spConditionVariableMutex)
	{
		return MSV_ALLOCATION_ERROR;
	}

	std::lock_guard<std::recursive_mutex> lock(m_lock);

	if (IsRunning() || m_thread.joinable())
	{
		return MSV_ALREADY_RUNNING_INFO;
	}

	m_timeout = timeout;
	if (m_timeout < 0)
	{
		//negative timeout -> set stop request (only one iteration)
		m_stopRequested = true;
	}

	//create thread
	m_thread = std::thread(&MsvThread::ThreadMainInner, this);
	m_isRunning = true;

	return MSV_SUCCESS;
}

MsvErrorCode MsvThread::StopThread()
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);

	if (!IsRunning())
	{
		return MSV_NOT_RUNNING_INFO;
	}
	else if (m_stopRequested)
	{
		return MSV_ALREADY_REQUESTED_INFO;
	}

	//set stop request and call notify (to immediate thread stop)
	std::unique_lock<std::mutex> conditionLock(*m_spConditionVariableMutex);
	m_stopRequested = true;
	conditionLock.unlock();
	Notify();

	return MSV_SUCCESS;
}

MsvErrorCode MsvThread::StopAndWaitForThreadStop(int32_t timeout)
{
	//it is OK to do like this (WaitForThreadStop checks if thread is running and if is joinable)
	MSV_RETURN_FAILED(StopThread());
	return WaitForThreadStop(timeout);
}

MsvErrorCode MsvThread::WaitForThreadStop(int32_t timeout)
{
	std::unique_lock<std::recursive_mutex> lock(m_lock);

	if (!IsRunning() && !m_thread.joinable())
	{
		return MSV_NOT_RUNNING_INFO;
	}

	if (timeout <= 0)
	{
		//timeout 0 -> waiting for end of the thread
		if (m_thread.joinable())
		{
			lock.unlock();
			m_thread.join();
		}
	}
	else
	{
		//release lock (don't want to block other threads during waiting)
		lock.unlock();

		std::unique_lock<std::mutex> conditionLock(m_stopConditionVariableMutex);
		m_stopConditionVariable.wait_for(conditionLock, std::chrono::microseconds(timeout), [this] { return m_stopReady; });
		conditionLock.unlock();

		//aquire lock once again
		lock.lock();
		//check it once again (thread can be already stopped or not joinable after timeout)
		if (IsRunning())
		{
			//return without join -> there is detach called in destructor
			return MSV_STILL_RUNNING_WARN;
		}
		else if (m_thread.joinable())
		{
			lock.unlock();
			m_thread.join();
		}
	}

	return MSV_SUCCESS;
}


/********************************************************************************************************************************
*															MsvThread protected methods
********************************************************************************************************************************/


void MsvThread::HandleCaughtException(const std::exception_ptr pException)
{
	//just rethrows (implement exception handling in child class if needed)
	if (pException)
	{
		std::rethrow_exception(pException);
	}

	//example how to get std:exception from pException:	
	/*
	try
	{
		if (pException)
		{
			std::rethrow_exception(pException);
		}
	}
	catch (const std::exception& exception)
	{
		//handle std::exception (exception.what() is accessbile now)
	}
	*/
}

void MsvThread::OnThreadStart()
{
	//implement in child class if needed
}

void MsvThread::OnThreadStop()
{
	//implement in child class if needed
}

void MsvThread::ThreadMain()
{
	//implement in child class if needed
}


/********************************************************************************************************************************
*															MsvThread private methods
********************************************************************************************************************************/


void MsvThread::ThreadMainInner()
{
	try
	{
		//inicialize thread
		OnThreadStart();
		
		if (m_timeout < 0)
		{
			//thread main method/function
			ThreadMain();
		}
		else if (m_spConditionVariablePredicate)
		{
			//thread main method/function -> synchronization by shared condition
			ThreadMainInnerSharedPredicate();
		}
		else
		{
			//thread main method/function -> synchronization by this thread unique condition
			ThreadMainInnerDataReadyPredicate();
		}

		//uninitialize thread
		OnThreadStop();

	}
	catch (...)
	{
		HandleCaughtException(std::current_exception());
	}

	//execution is stopped -> notify stop condition
	std::unique_lock<std::mutex> stopConditionLock(m_stopConditionVariableMutex);
	m_stopReady = true;
	stopConditionLock.unlock();
	m_stopConditionVariable.notify_one();

	//set stopped flag
	std::unique_lock<std::recursive_mutex> lock(m_lock);
	m_isRunning = false;
	lock.unlock();
}

void MsvThread::ThreadMainInnerDataReadyPredicate()
{
	if (m_timeout == 0)
	{
		do
		{
			//thread main method/function
			ThreadMain();

			std::unique_lock<std::mutex> conditionLock(*m_spConditionVariableMutex);
			//zero timeout -> just wait for notify (blocks thread until notify is called)
			m_spConditionVariable->wait(conditionLock, [this] { return m_dataReady || m_stopRequested; });
			m_dataReady = false;

			//unlock condition mutex
			conditionLock.unlock();

		} while (!m_stopRequested);
	}
	else
	{
		do
		{
			//thread main method/function
			ThreadMain();

			std::unique_lock<std::mutex> conditionLock(*m_spConditionVariableMutex);
			//zero timeout -> just wait for notify (blocks thread until notify is called)
			//positive timeout -> waits for timeout period or notify (blocks thread until timeout is reached or notify is called)
			m_spConditionVariable->wait_for(conditionLock, std::chrono::microseconds(m_timeout), [this] { return m_dataReady || m_stopRequested; });
			m_dataReady = false;

			//unlock condition mutex
			conditionLock.unlock();

		} while (!m_stopRequested);
	}
	//negative timeout means execute once -> do not wait for nothing -> loop ends (see StartThread method)
}

void MsvThread::ThreadMainInnerSharedPredicate()
{
	if (m_timeout == 0)
	{
		do
		{
			//thread main method/function
			ThreadMain();

			std::unique_lock<std::mutex> conditionLock(*m_spConditionVariableMutex);
			//zero timeout -> just wait for notify (blocks thread until notify is called)
			m_spConditionVariable->wait(conditionLock, [this] { return (*m_spConditionVariablePredicate) > 0 || m_stopRequested; });
			//non zero check is done in predicate
			if (!m_stopRequested)
			{
				(*m_spConditionVariablePredicate)--;
			}

			//unlock condition mutex
			conditionLock.unlock();

		} while (!m_stopRequested);
	}
	else
	{
		do
		{
			//thread main method/function
			ThreadMain();

			std::unique_lock<std::mutex> conditionLock(*m_spConditionVariableMutex);
			//zero timeout -> just wait for notify (blocks thread until notify is called)
			//positive timeout -> waits for timeout period or notify (blocks thread until timeout is reached or notify is called)
			m_spConditionVariable->wait_for(conditionLock, std::chrono::microseconds(m_timeout), [this] { return (*m_spConditionVariablePredicate) > 0 || m_stopRequested; });
			if ((*m_spConditionVariablePredicate) > 0 && !m_stopRequested)
			{
				(*m_spConditionVariablePredicate)--;
			}

			//unlock condition mutex
			conditionLock.unlock();

		} while (!m_stopRequested);
	}
	//negative timeout means execute once -> do not wait for nothing -> loop ends (see StartThread method)
}


/** @} */	//End of group MTHREADING.
