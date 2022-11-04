#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#include <thread>
#include<vector>
#include "chunk.h"
#include <chrono>

#define MAX_CHUNK_PER_THREAD 1000

#define THREAD_NUMBER 2

#define THREAD_ALIVE 1
#define THREAD_DYING 0
#define THREAD_DEAD 2

class MeshThread;
class DataThread;
void DataThreadRoutine(DataThread& dataThread, MeshThread& meshThread);
void MeshThreadRoutine(DataThread& dataThread, MeshThread& meshThread);

class DataThread {
public:
	Chunk** chunkList;
	int chunkLeft;
	char status;

	DataThread() {
		chunkLeft = 0;
		status = THREAD_ALIVE;
		chunkList = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~DataThread() {
		free(chunkList);
	}

	void Launch(MeshThread& meshThread) {
		std::thread(DataThreadRoutine, std::ref(*this), std::ref(meshThread)).detach();
		std::thread(MeshThreadRoutine, std::ref(*this), std::ref(meshThread)).detach();
	}
};

class MeshThread {
public:
	Chunk** chunkListLeft;
	Chunk** chunkListDone;
	int chunkLeft;
	int chunkDone;
	char status;

	MeshThread() {
		chunkLeft = 0;
		chunkDone = 0;
		status = THREAD_ALIVE;
		chunkListLeft = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*)); // maybe do a buffer double the size instead of 2 buffers
		chunkListDone = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~MeshThread() {
		free(chunkListLeft);
		free(chunkListDone);
	}
};

void DataThreadRoutine(DataThread& dataThread, MeshThread& meshThread) {
	while (dataThread.status) {

		if (dataThread.chunkLeft <= 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(10000));
			continue;
		}

		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (dataThread.chunkList[n]) {
				for (int m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!meshThread.chunkListLeft[m]) {
						dataThread.chunkList[n]->Generate();
						meshThread.chunkListLeft[m] = dataThread.chunkList[n];
						dataThread.chunkList[n] = 0;
						dataThread.chunkLeft -= 1;
						meshThread.chunkLeft += 1;
						break;
					}
				}

				
			}
		}
	//	std::cout << "data thread  chunk left = " << dataThread.chunkLeft << std::endl;
	}
	dataThread.status = THREAD_DEAD;
//	std::cout << "data thread is dead"<< std::endl;
}

void MeshThreadRoutine(DataThread& dataThread, MeshThread& meshThread) {
	while (meshThread.status) {
		if (meshThread.chunkLeft <= 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(10000));
			continue;
		}
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (meshThread.chunkListLeft[n]) {
				for (int m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!meshThread.chunkListDone[m]) {
						meshThread.chunkListLeft[n]->createMeshData();
						meshThread.chunkListDone[m] = meshThread.chunkListLeft[n];
						meshThread.chunkListLeft[n] = 0;
						meshThread.chunkLeft -= 1;
						meshThread.chunkDone += 1;
						break;
					}
				}
			}
		}
		//	std::cout << "mesh thread  chunk left = " << meshThread.chunkLeft << std::endl;
	}
	meshThread.status = THREAD_DEAD;
//	std::cout << "mesh thread is dead" << std::endl;
}

class Thread {
private:
	DataThread *dataThreads;
	MeshThread *meshThreads;
	
public:
	Thread() {
		dataThreads = new DataThread[THREAD_NUMBER];
		meshThreads = new MeshThread[THREAD_NUMBER];
		for (int n = 0; n < THREAD_NUMBER; n++)
			dataThreads[n].Launch(meshThreads[n]);
	}

	void StopThreads() {
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

	void AddChunk(Chunk* chunk) {
		int thread = 0;

		for (int t = 0; t < THREAD_NUMBER - 1; t++)
			if (dataThreads[t].chunkLeft > dataThreads[t + 1].chunkLeft)
				thread = t + 1;
		
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!dataThreads[thread].chunkList[n]) {
				chunk->threadStatus |= CHUNK_PROCESSING;
				for (int m = 0; m < 4; m++)
					if (chunk->neighbour[m] && chunk->neighbour[m]->status >= CHUNK_DATA_LOADED) {
						chunk->neighbour[m]->threadStatus +=1;
						chunk->neighbourLoaded |= 1 << m;
					}
				dataThreads[thread].chunkList[n] = chunk;
				dataThreads[thread].chunkLeft += 1;
				return;
			}
		}
	}

	void BindAllChunks() {
		for (int thread = 0; thread < THREAD_NUMBER; thread++) {
			for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
				if (!meshThreads[thread].chunkDone)
					break ;
				if (meshThreads[thread].chunkListDone[n]) {
					meshThreads[thread].chunkListDone[n]->Bind();			//bind can be called after deleted (surely the cause of the seg fault)
					for (int m = 0; m < 4; m++)
						if ((meshThreads[thread].chunkListDone[n]->neighbourLoaded >> m) & 1)
							meshThreads[thread].chunkListDone[n]->neighbour[m]->threadStatus -=1;
				//	chunks.push_back(meshThreads[t].chunkList[n]);
					meshThreads[thread].chunkListDone[n] = 0;
					meshThreads[thread].chunkDone -= 1;
				}
			}
		}
	}
};

#endif