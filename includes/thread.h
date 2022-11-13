#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#include <thread>
#include <vector>
#include "chunk.h"
#include <chrono>

#define MAX_CHUNK_PER_THREAD 1000

#define DATA_THREAD_NUMBER 3
#define MESH_THREAD_NUMBER 2

#define THREAD_ALIVE 1
#define THREAD_DYING 0
#define THREAD_DEAD 2

class MeshThread;
class DataThread;
void DataThreadRoutine(DataThread& dataThread);
void MeshThreadRoutine(MeshThread& meshThread);

class DataThread {			//no need to have the two classes they are too similar
public:
	Chunk** chunkListLeft;
	Chunk** chunkListDone;
	int chunkLeft;
	char status;

	DataThread() {
		chunkLeft = 0;
		status = THREAD_ALIVE;
		chunkListLeft = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
		chunkListDone = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~DataThread() {
		free(chunkListLeft);
		free(chunkListDone);
	}

	void Launch() {
		std::thread(DataThreadRoutine, std::ref(*this)).detach();
	}
};

class MeshThread {
public:
	Chunk** chunkListLeft;
	Chunk** chunkListDone;
	int chunkLeft;
	char status;

	MeshThread() {
		chunkLeft = 0;
		status = THREAD_ALIVE;
		chunkListLeft = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*)); // maybe do a buffer double the size instead of 2 buffers
		chunkListDone = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~MeshThread() {
		free(chunkListLeft);
		free(chunkListDone);
	}

	void Launch() {
		std::thread(MeshThreadRoutine, std::ref(*this)).detach();
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
	void LoadChunk(Chunk* chunk);

	// assing the generation of a chunk to a thread
	void CreateMesh(Chunk* chunk);

	// bind all the completed chunks from the threads
	void BindAllChunks(void);

	// unlock all the loaded chunks from the threads
	void UnlockLoadedChunks(void);
};

#endif