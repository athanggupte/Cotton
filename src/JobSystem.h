#pragma once
#include <cstdint>

namespace Cotton {
	struct ThreadFiberContext;
	struct Counter;

	typedef void JobEntryPoint(void* param);
	typedef JobEntryPoint* PFN_JobEntryPoint;

	enum class Priority : uint8_t
	{
		LOW,
		NORMAL,
		HIGH,
		CRITICAL
	};

	enum class Status : uint8_t
	{
		IDLE,     // has not started running yet [in job queue]
		RUNNING,  // executing [in fiber]
		WAITING,  // waiting on some counter/mutex; has started running [in wait list]
		FINISHED  // finished execution [terminated]
	};

	struct JobDecl
	{
		PFN_JobEntryPoint m_pEntryPoint; // not initialized to promote explicit initialization
		void*             m_pParam    { nullptr };
		Counter*          m_pCounter  { nullptr };
		Priority          m_ePriority { Priority::NORMAL };
		Status            m_eStatus   { Status::IDLE };
	};

	// ThreadFiberContext* _getThreadFiberContext();
	void _initializeThreadForFibers();
	void _runWorkerThread();
	void _waitForAllJobs();


	// Public API
	
	void runJob(JobDecl& job_decl);
	void runJobs(int count, JobDecl job_decls[]);

	void waitForCounter(Counter* p_counter);
	void waitForCounterAndReset(Counter* p_counter);

	void initializeJobSystem();
	void startJobSystem();

}
