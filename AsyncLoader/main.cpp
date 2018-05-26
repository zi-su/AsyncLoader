#include "AsyncLoadRequestManager.h"

#include <chrono>
#include <thread>

int main() {
	AsyncLoadRequestManager loader;

	
	{//���C�����[�v
		
		//�F�X�X�V���������[�h���N�G�X�g��ς�
		for(int i = 0 ; i < 100 ; i++){

			//�ǂݍ��ރt�@�C��
			struct _stat buf;
			int fd=0;
			_stat("test.jpg", &buf);
			void* buff = nullptr;	//�A���P�[�^����A���P�[�g���Ă����o�b�t�@

			loader.PushRequest(RequestInfo(buff, "test.jpg", buf.st_size));
		}
		
		//�񓯊����[�h�X�^�[�g
		auto loadstart = std::chrono::system_clock::now();
		if (loader.IsRequested()) {
			loader.ThreadStart();
		}

		//�F�X�X�V�������񓯊����[�h�҂�.NowLoading�`��ς݂Ƃ��B�ҋ@�Ƃ��B
		while (loader.IsLoading()) {
			//printf("Waiting...\n");

			//���[�U�[���͂ɂ��L�����Z������
			{
			}
		}
		auto loadend = std::chrono::system_clock::now();
		auto elaps = std::chrono::duration_cast<std::chrono::microseconds>(loadend - loadstart);
		printf("load elaps %lld ms", elaps.count());
		//���C������

		//�`�揈��
	}

	//���C�����[�v�I��
	loader.Finish();
	return 0;
}