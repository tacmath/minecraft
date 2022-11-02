#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#include <thread>
#include<vector>
#include "chunk.h"
#include <chrono>

#define MAX_CHUNK_PER_THREAD 1000

class MeshTread;
class DataTread;
void DataThreadRoutine(DataTread& dataTread, MeshTread& meshTread);
void MeshThreadRoutine(DataTread& dataTread, MeshTread& meshTread);

class DataTread {
public:
	Chunk** chunkList;
	int chunkLeft;

	DataTread() {
		chunkLeft = 0;
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
	Chunk** chunkList;			// maybe do chunkListLeft and chunkListDone
	int chunkLeft;
	int chunkDone;

	MeshTread() {
		chunkLeft = 0;
		chunkDone = 0;
		chunkList = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~MeshTread() {
		free(chunkList);
	}
};

void DataThreadRoutine(DataTread& dataTread, MeshTread& meshTread) {
	while (1) {

		if (dataTread.chunkLeft <= 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(10000));
			continue;
		}

		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (dataTread.chunkList[n] && dataTread.chunkList[n]->status == CHUNK_UNLOADED) {
				for (int m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!meshTread.chunkList[m]) {
						dataTread.chunkList[n]->Generate();
						meshTread.chunkList[m] = dataTread.chunkList[n];
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
}

void MeshThreadRoutine(DataTread& dataTread, MeshTread& meshTread) {
	while (1) {
		if (meshTread.chunkLeft > 0) {
			for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
				if (meshTread.chunkList[n] && meshTread.chunkList[n]->status == CHUNK_DATA_LOADED) {
					meshTread.chunkList[n]->createMeshData();
					meshTread.chunkLeft -= 1;
					meshTread.chunkDone += 1;
				}
			}
		//	std::cout << "mesh thread  chunk left = " << meshTread.chunkLeft << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(10000));
	}
}

class Thread {
private:
	DataTread dataTreads;
	MeshTread meshTreads;
	
public:
	Thread() {
		dataTreads.Launch(meshTreads);
	}

	void AddChunk(Chunk* chunk) {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!dataTreads.chunkList[n]) {
				chunk->threadStatus = CHUNK_PROCESSING;
				dataTreads.chunkList[n] = chunk;
				dataTreads.chunkLeft += 1;
				return;
			}
		}
		std::cout << "a chunk could not be assigned " << std::endl;
	}

	void BindAllChunks(std::vector<Chunk*> &chunks) {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!meshTreads.chunkDone)
				return ;
			if (meshTreads.chunkList[n] && meshTreads.chunkList[n]->status == CHUNK_LOADED) {
				meshTreads.chunkList[n]->Bind();			//bind can be called after deleted (surely the cause of the seg fault)
			//	chunks.push_back(meshTreads.chunkList[n]);
				meshTreads.chunkList[n] = 0;
				meshTreads.chunkDone -= 1;
				
			}
		}
	}
};

#endif