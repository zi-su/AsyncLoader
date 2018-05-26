#pragma once

#include <vector>
#include <mutex>
#include <queue>
#include <atomic>

struct RequestInfo {
	void* buffer_;
	char* fileName_;
	size_t fileSize_;

	RequestInfo(void* buffer, char* fileName, size_t fileSize) {
		buffer_ = buffer;
		fileName_ = fileName;
		fileSize_ = fileSize;
	}

};

class AsyncLoadRequestManager {
public:

	enum class STATE {
		IDLE,
		LOADING,
		JOIN,
		FINISH,

		BALANCER,
	};

	AsyncLoadRequestManager();
	~AsyncLoadRequestManager();

	void ThreadStart();
	static void ThreadFunc(std::queue<RequestInfo>* requests);

	void PushRequest(RequestInfo& fileName);
	

	bool IsLoading()const;
	STATE GetState()const { return state_; }
	bool IsRequested()const { return requests.size() > 0; }
	void Cancel() { cancel_ = true; };
	void Finish();
private:
	std::mutex async_mutex;
	static std::atomic<STATE> state_;
	static std::atomic<bool> cancel_;
	std::queue<RequestInfo> requests;
	
	std::thread loadThread;
	static uint32_t cpu_num;

	static std::vector<std::thread> loadThreads;
	static std::vector<std::queue<RequestInfo> > requestQueue;
	static std::vector<bool> loadResults;
};

