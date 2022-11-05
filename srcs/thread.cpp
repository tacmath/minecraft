#include "thread.h"

Thread::Thread(void) {
	dataThreads = new DataThread[THREAD_NUMBER];
	meshThreads = new MeshThread[THREAD_NUMBER];
	for (int n = 0; n < THREAD_NUMBER; n++)
		dataThreads[n].Launch(meshThreads[n]);
}

void Thread::StopThreads(void) {
	for (int n = 0; n < THREAD_NUMBER; n++) {
		meshThreads[n].status = THREAD_DYING;
		dataThreads[n].status = THREAD_DYING;
	}
	for (int n = 0; n < THREAD_NUMBER; n++)
		while (meshThreads[n].status != THREAD_DEAD || dataThreads[n].status != THREAD_DEAD)
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
	delete[] dataThreads;
	delete[] meshThreads;
}

void Thread::AddChunk(Chunk* chunk) {
	int thread = 0;

	for (int t = 0; t < THREAD_NUMBER - 1; t++)
		if (dataThreads[t].chunkLeft > dataThreads[t + 1].chunkLeft)
			thread = t + 1;

	for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
		if (!dataThreads[thread].chunkList[n]) {
			chunk->threadStatus |= CHUNK_PROCESSING;
			for (int m = 0; m < 4; m++)
				if (chunk->neighbour[m] && chunk->neighbour[m]->status >= CHUNK_DATA_LOADED) {
					chunk->neighbour[m]->threadStatus += 1;
					chunk->neighbourLoaded |= 1 << m;
				}
			dataThreads[thread].chunkList[n] = chunk;
			dataThreads[thread].chunkLeft += 1;
			return;
		}
	}
}

void Thread::BindAllChunks(void) {
	for (int thread = 0; thread < THREAD_NUMBER; thread++) {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!dataThreads[thread].chunkLeft)
				break;
			if (meshThreads[thread].chunkListDone[n]) {
				meshThreads[thread].chunkListDone[n]->Bind();
				for (int m = 0; m < 4; m++)
					if ((meshThreads[thread].chunkListDone[n]->neighbourLoaded >> m) & 1)
						meshThreads[thread].chunkListDone[n]->neighbour[m]->threadStatus -= 1;
				meshThreads[thread].chunkListDone[n] = 0;
				dataThreads[thread].chunkLeft -= 1;
			}
		}
		//	std::cout << "chunk left = " << dataThreads[thread].chunkLeft << std::endl;
	}
}


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