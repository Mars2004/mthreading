/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Thread Implementation
* @details		Contains implementation @ref MsvThread of @ref IMsvThread interface.
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


#ifndef MARSTECH_THREAD_H
#define MARSTECH_THREAD_H


#include "IMsvThread.h"

MSV_DISABLE_ALL_WARNINGS

#include <thread>
#include <mutex>
#include <chrono>

MSV_ENABLE_WARNINGS


/**************************************************************************************************//**
* @brief		MarsTech Thread Implementation.
* @details	Base implementation of thread. If you need class representing thread, inherit from this
*				base implementation and implement/override at least one of these protected methods @ref OnThreadStart,
*				@ref OnThreadStop, @ref ThreadMain.
* @see		IMsvThread
******************************************************************************************************/
class MsvThread:
	virtual public IMsvThread
{
public:
	/**************************************************************************************************//**
	* @brief			Constructor.
	******************************************************************************************************/
	MsvThread();

	/**************************************************************************************************//**
	* @brief			Constructor.
	* @details		Constructs @ref MsvThread with shared condition variable.
	* @param[in]	spConditionVariable				Shared condition variable.
	* @param[in]	spConditionVariableMutex		Shared condition variable mutex.
	* @param[in]	spConditionVariablePredicate	Shared condition variable predicate.
	* @warning		Use this constructor only for threads with shared condition variable. In most cases
	*					you will need default constructor @ref MsvThread().
	* @warning		@ref Notify will notify all threads with shared condition variable.
	* @warning		If spConditionVariablePredicate is not null, notification must be done outside
	*					of this class. Notify might not work properly.
	* @see			m_spConditionVariable
	******************************************************************************************************/
	MsvThread(std::shared_ptr<std::condition_variable> spConditionVariable, std::shared_ptr<std::mutex> spConditionVariableMutex, std::shared_ptr<uint64_t> spConditionVariablePredicate);

	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~MsvThread();

	/**************************************************************************************************//**
	* @copydoc IMsvThread::IsRunning()
	******************************************************************************************************/
	virtual bool IsRunning() const override;

	/**************************************************************************************************//**
	* @copydoc	IMsvThread::Notify()
	* @warning	If @ref m_spConditionVariablePredicate is not null, notification must be done outside
	*				of this class. Notify might not work properly.
	******************************************************************************************************/
	virtual void Notify() const override;

	/**************************************************************************************************//**
	* @copydoc IMsvThread::StartThread()
	******************************************************************************************************/
	virtual MsvErrorCode StartThread(int32_t timeout) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThread::StopThread()
	******************************************************************************************************/
	virtual MsvErrorCode StopThread() override;

	/**************************************************************************************************//**
	* @copydoc IMsvThread::StopAndWaitForThreadStop()
	******************************************************************************************************/
	virtual MsvErrorCode StopAndWaitForThreadStop(int32_t timeout) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThread::WaitForThreadStop()
	******************************************************************************************************/
	virtual MsvErrorCode WaitForThreadStop(int32_t timeout) override;

protected:
	/**************************************************************************************************//**
	* @brief			Handle caught exception.
	* @details		It is called when some exception is caught (from @ref OnThreadStart, @ref ThreadMain
	*					or @ref OnThreadStop).
	* @note			Implement/override if needed in your child object (base implementation just rethrows exception).
	******************************************************************************************************/
	virtual void HandleCaughtException(const std::exception_ptr pException);

	/**************************************************************************************************//**
	* @brief			On thread start.
	* @details		It is called once before @ref ThreadMain method. It can initialize and set everything
	*					ready for @ref ThreadMain.
	* @note			Implement/override if needed in your child object (base implementation does nothing).
	* @warning		It is called only once before loop calling @ref ThreadMain.
	******************************************************************************************************/
	virtual void OnThreadStart();

	/**************************************************************************************************//**
	* @brief			On thread stop.
	* @details		It is called once after @ref ThreadMain method. It can uninitialize and set everything
	*					ready for thread stop.
	* @note			Implement/override if needed in your child object (base implementation does nothing).
	* @warning		It is called only once after end of loop calling @ref ThreadMain.
	******************************************************************************************************/
	virtual void OnThreadStop();

	/**************************************************************************************************//**
	* @brief			Thread main method.
	* @details		Thread main - thread entry point. Do your job/work in its.
	* @note			Implement/override if needed in your child object (base implementation does nothing).
	* @warning		It is called in loop (or ones if @ref StartThread has been called with negative timeout).
	* @see			StartThread
	******************************************************************************************************/
	virtual void ThreadMain();

private:
	/**************************************************************************************************//**
	* @brief			Inner thread main.
	* @details		It is real thread entry point which implements execution loop. It calls @ref OnThreadStart,
	*					@ref ThreadMain (in loop) and @ref OnThreadStop.
	* @note			Execution loop is exectuted only once (@ref ThreadMain is called only once) when
	*					@ref StartThread has been called with negative timeout).
	* @note			Execution loop is exectuted once and waits for @ref Notify to next execution
	*					(@ref ThreadMain is called once when @ref Notify is called) when	@ref StartThread
	*					has been called with zero timeout).
	* @note			Execution loop is exectuted once and waits for timeout or @ref Notify to next execution
	*					(@ref ThreadMain is called once when timeout expires or @ref Notify is called) when
	*					@ref StartThread has been called with positive timeout).
	* @warning		Do not change its implementation.
	* @see			StartThread
	* @see			Notify
	* @see			OnThreadStart
	* @see			ThreadMain
	* @see			OnThreadStop
	* @see			m_timeout
	******************************************************************************************************/
	virtual void ThreadMainInner();

	/**************************************************************************************************//**
	* @brief			Inner thread main for unique condition.
	* @details		It is called from @ref ThreadMainInner when no shared condition predicate exists. Condition
	*					variable predicate is @ref m_spConditionVariablePredicate.
	* @note			This method and @ref ThreadMainInnerSharedPredicate are created for optimalization ->
	*					there are no checks in main loop if there is shared condition or not.
	* @warning		Do not change its implementation.
	* @see			ThreadMainInner
	* @see			ThreadMainInnerSharedPredicate
	* @see			m_dataReady
	******************************************************************************************************/
	void ThreadMainInnerDataReadyPredicate();

	/**************************************************************************************************//**
	* @brief			Inner thread main for shared condition.
	* @details		It is called from @ref ThreadMainInner when shared condition predicate exists. Condition
	*					variable predicate is @ref m_dataReady.
	* @note			This method and @ref ThreadMainInnerDataReadyPredicate are created for optimalization ->
	*					there are no checks in main loop if there is shared condition or not.
	* @warning		Do not change its implementation.
	* @see			ThreadMainInner
	* @see			ThreadMainInnerDataReadyPredicate
	* @see			m_dataReady
	******************************************************************************************************/
	void ThreadMainInnerSharedPredicate();

protected:
	/**************************************************************************************************//**
	* @brief		Condition variable mutex.
	* @details	Locks condition variable @ref m_spConditionVariable.
	* @see		m_spConditionVariable
	******************************************************************************************************/
	std::mutex m_conditionVariableMutex;

	/**************************************************************************************************//**
	* @brief		Flag if data is ready.
	* @details	It is one of checks in @ref m_conditionVariableMutex predicate. It is set in
	*				@ref Notify and checked in @ref ThreadMainInner.
	* @see		Notify
	* @see		ThreadMainInner
	******************************************************************************************************/
	mutable bool m_dataReady;

	/**************************************************************************************************//**
	* @brief		Flag if thread is running (true) or not (false).
	* @see		IsRunning
	******************************************************************************************************/
	bool m_isRunning;

	/**************************************************************************************************//**
	* @brief		Thread mutex.
	* @details	Locks this object for thread safety access.
	******************************************************************************************************/
	mutable std::recursive_mutex m_lock;

	/**************************************************************************************************//**
	* @brief		Condition variable.
	* @details	It is used for timeout waiting (waits for timeout) or for notifications (@ref Notify calls).
	* @see		MsvThread(std::shared_ptr<std::condition_variable> spConditionVariable)
	******************************************************************************************************/
	std::shared_ptr<std::condition_variable> m_spConditionVariable;

	/**************************************************************************************************//**
	* @brief		Condition variable mutex.
	* @details	It is used for @ref m_spConditionVariable locking.
	* @see		MsvThread(std::shared_ptr<std::condition_variable> spConditionVariable, std::shared_ptr<std::mutex> spConditionVariableMutex, std::shared_ptr<uint64_t> spConditionVariablePredicate)
	* @see		m_spConditionVariable
	******************************************************************************************************/
	std::shared_ptr<std::mutex> m_spConditionVariableMutex;

	/**************************************************************************************************//**
	* @brief		Condition variable predicate.
	* @details	It is one of checks in @ref m_spConditionVariable predicate. It is set outside of the class
	*				and checked in @ref ThreadMainInner.
	* @see		MsvThread(std::shared_ptr<std::condition_variable> spConditionVariable, std::shared_ptr<std::mutex> spConditionVariableMutex, std::shared_ptr<uint64_t> spConditionVariablePredicate)
	* @see		m_spConditionVariable
	* @see		ThreadMainInner
	******************************************************************************************************/
	std::shared_ptr<uint64_t> m_spConditionVariablePredicate;

	/**************************************************************************************************//**
	* @brief		Stop condition variable.
	* @details	It is used to wait for thread stop timeout waiting (@ref WaitForThreadStop).
	* @see		WaitForThreadStop
	******************************************************************************************************/
	std::condition_variable m_stopConditionVariable;

	/**************************************************************************************************//**
	* @brief		Stop condition variable mutex.
	* @details	Locks condition variable @ref m_stopConditionVariable.
	* @see		m_stopConditionVariable
	* @see		WaitForThreadStop
	******************************************************************************************************/
	std::mutex m_stopConditionVariableMutex;

	/**************************************************************************************************//**
	* @brief		Flag if thread stop is ready.
	* @details	It is one of checks in @ref m_stopConditionVariable predicate. It is set at the end of
	*				@ref ThreadMainInner and checked in @ref WaitForThreadStop.
	* @see		StopThread
	* @see		WaitForThreadStop
	* @see		ThreadMainInner
	******************************************************************************************************/
	bool m_stopReady;

	/**************************************************************************************************//**
	* @brief		Flag if thread stop is requested (true) or not (false).
	* @details	When this flag is set (true), the execution loop will stop and thread will be stopped.
	* @see		StopThread
	******************************************************************************************************/
	bool m_stopRequested;

	/**************************************************************************************************//**
	* @brief		Thread object.
	******************************************************************************************************/
	std::thread m_thread;

	/**************************************************************************************************//**
	* @brief			Execution loop timeout.
	* @details		Stored timeout of @ref StartThread timeout parameter.
	* @note			Execution loop is exectuted only once (@ref ThreadMain is called only once) when
	*					@ref StartThread has been called with negative timeout).
	* @note			Execution loop is exectuted once and waits for @ref Notify to next execution
	*					(@ref ThreadMain is called once when @ref Notify is called) when	@ref StartThread
	*					has been called with zero timeout).
	* @note			Execution loop is exectuted once and waits for timeout or @ref Notify to next execution
	*					(@ref ThreadMain is called once when timeout expires or @ref Notify is called) when
	*					@ref StartThread has been called with positive timeout).
	* @see			StartThread
	* @see			ThreadMainInner
	******************************************************************************************************/
	int32_t m_timeout;	
};


#endif // !MARSTECH_THREAD_H

/** @} */	//End of group MTHREADING.
