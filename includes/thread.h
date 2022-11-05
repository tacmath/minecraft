#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#include <thread>
#include <vector>
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
	char status;

	MeshThread() {
		status = THREAD_ALIVE;
		chunkListLeft = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*)); // maybe do a buffer double the size instead of 2 buffers
		chunkListDone = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~MeshThread() {
		free(chunkListLeft);
		free(chunkListDone);
	}
};

/*
	for now the thread routine :

	main assing chunk ---> dataThread find a place for the result and generate data ---> meshThread find a place for the result and create mesh ---> main bind result


	and later maybe use a list of char to lock places in the array

*/

class Thread {
private:
	DataThread *dataThreads;
	MeshThread *meshThreads;
	
public:
	// default contuctor that launch the threads
	Thread(void);

	// send a signal to the threads and wait for them to properly close
	void StopThreads(void);

	// assing the generation of a chunk to a thread
	void AddChunk(Chunk* chunk);

	// bind all the completed chunks from the threads
	void BindAllChunks(void);
};

#endif