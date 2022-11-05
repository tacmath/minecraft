#include "thread.h"

void DataThreadRoutine(DataThread& dataThread, MeshThread& meshThread) {		// maybe do a single thread instead of 2 specialized ones
	int n, m;

	while (dataThread.status) {

		if (!dataThread.chunkLeft) {
			std::this_thread::sleep_for(std::chrono::microseconds(3000));
			continue;
		}
		for (n = 0; n < MAX_CHUNK_PER_THREAD; n++) {			// maybe use one loop and 2 condition 
			if (dataThread.chunkList[n]) {
				for (m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!meshThread.chunkListLeft[m]) {
						dataThread.chunkList[n]->Generate();
						meshThread.chunkListLeft[m] = dataThread.chunkList[n];
						dataThread.chunkList[n] = 0;
						break;
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}
	dataThread.status = THREAD_DEAD;
//	std::cout << "data thread is dead"<< std::endl;
}

void MeshThreadRoutine(DataThread& dataThread, MeshThread& meshThread) {
	int n, m;

	while (meshThread.status) {
		if (!dataThread.chunkLeft) {
			std::this_thread::sleep_for(std::chrono::microseconds(3000));
			continue;
		}
		for (n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (meshThread.chunkListLeft[n]) {
				for (m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!meshThread.chunkListDone[m]) {
						meshThread.chunkListLeft[n]->createMeshData();
						meshThread.chunkListDone[m] = meshThread.chunkListLeft[n];
						meshThread.chunkListLeft[n] = 0;
						break;
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}
	meshThread.status = THREAD_DEAD;
//	std::cout << "mesh thread is dead" << std::endl;
}