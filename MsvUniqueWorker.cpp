/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Unique Work Worker Implementation
* @details		Contains implementation of @ref MsvWorker.
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


#include "MsvUniqueWorker.h"
#include "MsvWorker_Factory.h"

#include "MsvTask.h"

#include "merror/MsvErrorCodes.h"


/********************************************************************************************************************************
*                                              Constructors and destructors
********************************************************************************************************************************/


MsvUniqueWorker::MsvUniqueWorker(std::shared_ptr<MsvWorker_Factory> spFactory):
	MsvUniqueWorker(nullptr, nullptr, nullptr, spFactory)
{
}

MsvUniqueWorker::MsvUniqueWorker(std::shared_ptr<std::condition_variable> spConditionVariable, std::shared_ptr<std::mutex> spConditionVariableMutex, std::shared_ptr<uint64_t> spSharedConditionPredicate, std::shared_ptr<MsvWorker_Factory> spFactory):
	MsvThread(spConditionVariable, spConditionVariableMutex, spSharedConditionPredicate),
	m_spFactory(spFactory ? spFactory : MsvWorker_Factory::Get())
{

}

MsvUniqueWorker::~MsvUniqueWorker()
{
}


/********************************************************************************************************************************
*                                              IMsvUniqueWorker public methods
********************************************************************************************************************************/


MsvErrorCode MsvUniqueWorker::SetTask(std::shared_ptr<IMsvTask> spTask)
{
	std::lock_guard<std::recursive_mutex> lock(m_taskLock);

	MsvErrorCode errorCode = MSV_SUCCESS;

	if (m_spTask)
	{
		//some work is already set -> set new work, but return info message
		errorCode = MSV_ALREADY_SET_INFO;
	}

	m_spTask = spTask;
	Notify();

	return errorCode;
}

MsvErrorCode MsvUniqueWorker::SetTask(std::function<void()>& task)
{
	//create task
	std::shared_ptr<IMsvTask> spTask;
	if (m_spFactory)
	{
		spTask = m_spFactory->GetIMsvTask(task);
	}

	//set task to worker
	if (spTask)
	{
		return SetTask(spTask);
	}
	else
	{
		return MSV_ALLOCATION_ERROR;
	}

	return MSV_SUCCESS;
}

MsvErrorCode MsvUniqueWorker::SetTask(std::function<void(void*)>& task, void* pContext)
{
	//create task
	std::shared_ptr<IMsvTask> spTask;
	if (m_spFactory)
	{
		spTask = m_spFactory->GetIMsvTask(task, pContext);
	}

	//add task to queue

	if (spTask)
	{
		return SetTask(spTask);
	}
	else
	{
		return MSV_ALLOCATION_ERROR;
	}

	return MSV_SUCCESS;
}


/********************************************************************************************************************************
*															IMsvThread public methods
********************************************************************************************************************************/


bool MsvUniqueWorker::IsRunning() const
{
	return MsvThread::IsRunning();
}

void MsvUniqueWorker::Notify() const
{
	MsvThread::Notify();
}

MsvErrorCode MsvUniqueWorker::StartThread(int32_t timeout)
{
	return MsvThread::StartThread(timeout);
}

MsvErrorCode MsvUniqueWorker::StopThread()
{
	return MsvThread::StopThread();
}

MsvErrorCode MsvUniqueWorker::StopAndWaitForThreadStop(int32_t timeout)
{
	return MsvThread::StopAndWaitForThreadStop(timeout);
}

MsvErrorCode MsvUniqueWorker::WaitForThreadStop(int32_t timeout)
{
	return MsvThread::WaitForThreadStop(timeout);
}


/********************************************************************************************************************************
*                                              MsvThread protected methods
********************************************************************************************************************************/


void MsvUniqueWorker::ThreadMain()
{
	std::unique_lock<std::recursive_mutex> lock(m_taskLock);

	if (m_spTask)
	{
		//get task
		std::shared_ptr<IMsvTask> spTask = m_spTask;

		//unlock (anyone can set new task during current task execution)
		lock.unlock();
		spTask->Execute();
	}
}


/** @} */	//End of group MTHREADING.
