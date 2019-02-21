/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Task Interface
* @details		Contains definition of @ref IMsvTask interface.
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


#ifndef MARSTECH_ITASK_H
#define MARSTECH_ITASK_H


/**************************************************************************************************//**
* @brief		MarsTech Task Interface.
* @details	Interface for job/task. All child classes can be executed by workers (worker threads or 
*				thread pools).
******************************************************************************************************/
class IMsvTask
{
public:
	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~IMsvTask() {};

	/**************************************************************************************************//**
	* @brief			Execute task.
	* @details		Executes job/task defined by child implementation.
	******************************************************************************************************/
	virtual void Execute() = 0;
};


#endif // MARSTECH_ITASK_H

/** @} */	//End of group MTHREADING.
