#pragma once
#include <condition_variable>
#include <cstdint>
#include <deque>

namespace Cotton {
	struct JobQueue;
	struct ThreadFiberContext;
	struct Counter;

	//typedef void JobEntryPoint(void* param);
	//typedef JobEntryPoint* PFN_JobEntryPoint;
	using JobEntryPoint = void(void*);
	using PFN_JobEntryPoint = JobEntryPoint*;

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
	static_assert(std::is_trivially_copyable_v<JobDecl>);

	struct JobQueue
	{
		std::deque<JobDecl> m_jobs;
		std::mutex m_mutex;
		std::condition_variable m_cvar;

		constexpr static int kMaxRetries = 100;

		JobDecl popJob();

		void pushJob(JobDecl const& job_decl);
	};

	struct Metrics
	{
		uint32_t numJobsSubmitted{0};
		uint32_t numJobsExecuted{0};
		uint32_t numJobsWaiting{0};
		uint32_t numJobsFinished{0};

		uint64_t numMicrosecondsSpentIdle{0};
		uint64_t numMicrosecondsSpentWorking{0};
		uint64_t numMicrosecondsSpentWaitingOnQueue{0};
		uint64_t numMicrosecondsSpentWaitingOnFiber{0};
	};

	// ThreadFiberContext* _getThreadFiberContext();
	void _initializeThreadForFibers(uint32_t thread_id);
	void _runWorkerThread(uint32_t thread_id);
	void _waitForAllJobs();
	bool _shouldExit();
	JobQueue& _getJobQueue();
	Metrics& _getMetrics();

	// Public API
	
	void addJob(JobDecl const& job_decl);
	void addJobs(int count, JobDecl job_decls[]);

	void waitForCounter(Counter* p_counter);
	void waitForCounterAndReset(Counter* p_counter);

	void initializeJobSystem();
	void startJobSystem();


	// Job API

	template <typename>
	struct FunctionDecl;

	template <typename R, typename... Args>
	struct FunctionDecl<R(*)(Args...)>
	{
	    static_assert(std::is_same<R, void>());
		using function_type = R(*)(Args...);
	    using param_pack = std::tuple<Args...>;

	    struct bound_function_t {
	        function_type m_function;
	        param_pack m_params;
	    } boundFn;

		FunctionDecl(function_type function) : boundFn{.m_function = function} {}

		static void wrapper(void *param)
		{
			auto& boundFn = *static_cast<bound_function_t*>(param);
			std::apply(boundFn.m_function, boundFn.m_params);
		}

		void bind(Args&&... args)
		{
			boundFn.m_params = std::make_tuple(std::forward<Args>(args)...);
		}

		auto job(Args&&... args, Counter *counter = nullptr)
		{
			bind(std::forward<Args>(args)...);

			JobDecl jobDecl {
				.m_pEntryPoint = wrapper,
				.m_pParam = &boundFn,
				.m_pCounter = counter,
			};

			return jobDecl;
		}
	};

	template <typename F>
	auto make_function_decl(F func)
	{
		return FunctionDecl<F>(func);
	}

}


#define DECLARE_FUNCTION(name, ...) \
	void name##_Function(__VA_ARGS__); \
	static auto name = Cotton::make_function_decl(name##_Function); \
	void name##_Function(__VA_ARGS__)
