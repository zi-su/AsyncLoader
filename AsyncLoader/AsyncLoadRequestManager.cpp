#include "AsyncLoadRequestManager.h"
#include <thread>
#include <assert.h>
std::atomic<AsyncLoadRequestManager::STATE> AsyncLoadRequestManager::state_;
uint32_t AsyncLoadRequestManager::cpu_num;
std::atomic<bool> AsyncLoadRequestManager::cancel_;
std::vector<std::thread> AsyncLoadRequestManager::loadThreads;
std::vector<std::queue<RequestInfo> > AsyncLoadRequestManager::requestQueue;
std::vector<bool> AsyncLoadRequestManager::loadResults;

static void LoadFunc(std::queue<RequestInfo>* request, int id) {
	while (!request->empty()) {
		RequestInfo info = request->front();

		//�t�@�C���ǂݍ��ݏ���
		FILE* fp;
		errno_t err = fopen_s(&fp, info.fileName_, "rb");
		assert(err == 0);
		{
			//fread����
			void* p = new char[info.fileSize_];
			info.buffer_ = p;
			size_t ret = fread_s(info.buffer_, info.fileSize_, info.fileSize_, 1, fp);
		}
		fclose(fp);
		request->pop();
	}

}

AsyncLoadRequestManager::AsyncLoadRequestManager() {
	state_ = STATE::IDLE;
	cpu_num = std::thread::hardware_concurrency();
	loadThread = std::thread(ThreadFunc, &requests);

	//cpu_num = 1;
	loadThreads.resize(cpu_num);
	requestQueue.resize(cpu_num);
	loadResults.resize(cpu_num);
}

AsyncLoadRequestManager::~AsyncLoadRequestManager() {
	loadThread.join();
}

void AsyncLoadRequestManager::ThreadStart() {
	state_ = STATE::LOADING;
}

void AsyncLoadRequestManager::PushRequest(RequestInfo& fileName) {
	requests.push(std::move(fileName));
};

bool AsyncLoadRequestManager::IsLoading()const {
	return state_ == STATE::LOADING || state_ == STATE::JOIN; 
}

void AsyncLoadRequestManager::Finish() { state_ = STATE::FINISH; }

void AsyncLoadRequestManager::ThreadFunc(std::queue<RequestInfo>* requests) {
	while (true) {
		switch (state_) {
		case STATE::IDLE:
			break;
		case STATE::LOADING:
		{
			//���N�G�X�g���eCPU�R�A�ɕ������ă��[�h�X���b�h�쐬
			//�X���b�h���Ƃ̃��[�h���N�G�X�g��
			int loadRequestPerThread = requests->size() / cpu_num;
			int mod = requests->size() % cpu_num;
			for (uint32_t i = 0; i < cpu_num; i++) {
				for (int j = 0; j < loadRequestPerThread; j++) {
					auto r = requests->front();
					requestQueue[i].push(r);
					requests->pop();
				}
			}
			{
				//�]�蕪���L���[�ɒǉ�
				int targetQueue = 0;
				for (int i = 0; i < mod; i++) {
					auto r = requests->front();
					requestQueue[targetQueue].push(r);
					targetQueue++;
					if (targetQueue >= 8) {
						targetQueue = 0;
					}
					requests->pop();
				}
			}
			
			//�쐬�������N�G�X�g�Ń��[�h�X���b�h�쐬
			for (uint32_t i = 0; i < cpu_num; i++) {
				loadThreads[i] = std::thread(LoadFunc, &requestQueue[i], i);
			}
			//�X�e�[�g���W���C���ֈڍs
			state_ = STATE::JOIN;
		}
			break;
		
		case STATE::JOIN:
			//�e���[�h�X���b�h�̏I���҂�
			for (auto it = loadThreads.begin(); it != loadThreads.end(); it++) {
				it->join();
			}
			//�A�C�h���ֈڍs
			state_ = STATE::IDLE;
		case STATE::FINISH:
			break;
		}

		if (state_ == STATE::FINISH) {
			break;
		}
	}
	//state = STATE::IDLE;
}