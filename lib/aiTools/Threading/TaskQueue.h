// aiTools is a free C++ library of AI tools.
// Copyright (C) 2013 - 2016  Benjamin Schnieders

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program, see file LICENCE.txt.
// Alternativley, see <http://www.gnu.org/licenses/>.

#ifndef __AITOOLS_TASK_QUEUE_H_INCLUDED
#define __AITOOLS_TASK_QUEUE_H_INCLUDED

#include <thread>
#include <mutex>
#include <deque>
#include <future>

namespace aiTools
{
	namespace Threading
	{
		template<typename TaskType, typename ReturnType>
		class TaskQueue
		{
		public:
			TaskQueue();
			~TaskQueue()
			{
				scheduleShutdown();
				mWorkerThread.join();
			}

			ReturnType addTask(TaskType task)
			{
				std::lock_guard<std::mutex> lock(mAccessMutex);
				mTasks.push_back(std::move(task));
				return mTasks.back().get_future();
			}

			void operator()();

			void scheduleShutdown()
			{
				mShallStop = true;
			}

		private:
			std::mutex mAccessMutex;
			std::deque<TaskType> mTasks;
			std::thread mWorkerThread;
			volatile bool mShallStop;
		};

		template<typename TaskType, typename ReturnType>
		TaskQueue<TaskType, ReturnType>::TaskQueue() : mShallStop(false)
		{
			std::thread tempThread(std::ref(*this));
			mWorkerThread.swap(tempThread);
		}
		
		template<typename TaskType, typename ReturnType>
		void TaskQueue<TaskType, ReturnType>::operator()()
		{
			while(!mShallStop)
			{
				//work off a bunch of tasks:
				std::deque<TaskType> chosenTasks;

				{
					std::lock_guard<std::mutex> lock(mAccessMutex);
					chosenTasks.swap(mTasks);
				}

				while(!chosenTasks.empty())
				{
					TaskType task = std::move(chosenTasks.front());
					chosenTasks.pop_front();

					try
					{
						task();
					}
					catch(const std::exception& e)
					{
						std::cout << "exception while working on task, " << e.what() << std::endl;
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		}
	}
}


#endif // __AITOOLS_TASK_QUEUE_H_INCLUDED
