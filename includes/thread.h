#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#include <thread>
#include<vector>
#include "chunk.h"
#include <chrono>

#define MAX_CHUNK_PER_THREAD 1000

#define THREAD_ALIVE 1
#define THREAD_DYING 0
#define THREAD_DEAD 2

class MeshTread;
class DataTread;
void DataThreadRoutine(DataTread& dataTread, MeshTread& meshTread);
void MeshThreadRoutine(DataTread& dataTread, MeshTread& meshTread);

class DataTread {
public:
	Chunk** chunkList;
	int chunkLeft;
	char status;

	DataTread() {
		chunkLeft = 0;
		status = THREAD_ALIVE;
		chunkList = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~DataTread() {
		free(chunkList);
	}

	void Launch(MeshTread& meshTread) {
		std::thread(DataThreadRoutine, std::ref(*this), std::ref(meshTread)).detach();
		std::thread(MeshThreadRoutine, std::ref(*this), std::ref(meshTread)).detach();
	}
};

class MeshTread {
public:
	Chunk** chunkListLeft;
	Chunk** chunkListDone;
	int chunkLeft;
	int chunkDone;
	char status;

	MeshTread() {
		chunkLeft = 0;
		chunkDone = 0;
		status = THREAD_ALIVE;
		chunkListLeft = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*)); // maybe do a buffer double the size instead of 2 buffers
		chunkListDone = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~MeshTread() {
		free(chunkListLeft);
		free(chunkListDone);
	}
};

void DataThreadRoutine(DataTread& dataTread, MeshTread& meshTread) {
	while (dataTread.status) {

		if (dataTread.chunkLeft <= 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(10000));
			continue;
		}

		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (dataTread.chunkList[n]) {
				for (int m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!meshTread.chunkListLeft[m]) {
						dataTread.chunkList[n]->Generate();
						meshTread.chunkListLeft[m] = dataTread.chunkList[n];
						dataTread.chunkList[n] = 0;
						dataTread.chunkLeft -= 1;
						meshTread.chunkLeft += 1;
						break;
					}
				}

				
			}
		}
	//	std::cout << "data thread  chunk left = " << dataTread.chunkLeft << std::endl;
	}
	dataTread.status = THREAD_DEAD;
//	std::cout << "data thread is dead"<< std::endl;
}

void MeshThreadRoutine(DataTread& dataTread, MeshTread& meshTread) {
	while (meshTread.status) {
		if (meshTread.chunkLeft <= 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(10000));
			continue;
		}
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (meshTread.chunkListLeft[n]) {
				for (int m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!meshTread.chunkListDone[m]) {
						meshTread.chunkListLeft[n]->createMeshData();
						meshTread.chunkListDone[m] = meshTread.chunkListLeft[n];
						meshTread.chunkListLeft[n] = 0;
						meshTread.chunkLeft -= 1;
						meshTread.chunkDone += 1;
						break;
					}
				}
			}
		}
		//	std::cout << "mesh thread  chunk left = " << meshTread.chunkLeft << std::endl;
	}
	meshTread.status = THREAD_DEAD;
//	std::cout << "mesh thread is dead" << std::endl;
}

class Thread {
private:
	DataTread dataTreads;
	MeshTread meshTreads;
	
public:
	Thread() {
		dataTreads.Launch(meshTreads);
	}

	void StopThread() {
		meshTreads.status = THREAD_DYING;
		dataTreads.status = THREAD_DYING;
		while (meshTreads.status != THREAD_DEAD || dataTreads.status != THREAD_DEAD)
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}

	void AddChunk(Chunk* chunk) {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!dataTreads.chunkList[n]) {
				chunk->threadStatus |= CHUNK_PROCESSING;
				for (int m = 0; m < 4; m++)
					if (chunk->neighbour[m] && chunk->neighbour[m]->status >= CHUNK_DATA_LOADED) {
						chunk->neighbour[m]->threadStatus +=1;
						chunk->neighbourLoaded |= 1 << m;
					}
				dataTreads.chunkList[n] = chunk;
				dataTreads.chunkLeft += 1;
				return;
			}
		}
		//std::cout << "a chunk could not be assigned " << std::endl;
	}

	void BindAllChunks() {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!meshTreads.chunkDone)
				return ;
			if (meshTreads.chunkListDone[n]) {
				meshTreads.chunkListDone[n]->Bind();			//bind can be called after deleted (surely the cause of the seg fault)
				for (int m = 0; m < 4; m++)
					if ((meshTreads.chunkListDone[n]->neighbourLoaded >> m) & 1)
						meshTreads.chunkListDone[n]->neighbour[m]->threadStatus -=1;
			//	chunks.push_back(meshTreads.chunkList[n]);
				meshTreads.chunkListDone[n] = 0;
				meshTreads.chunkDone -= 1;
			}
		}
	}
};

#endif