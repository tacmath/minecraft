#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#include <thread>
#include <vector>
#include "chunk.h"
#include <chrono>

#define MAX_CHUNK_PER_THREAD 100

#define DATA_THREAD_NUMBER 2
#define MESH_THREAD_NUMBER 1

#define THREAD_ALIVE 1
#define THREAD_DYING 0
#define THREAD_DEAD 2

class Thread;
void DataThreadRoutine(Thread& dataThread);
void MeshThreadRoutine(Thread& meshThread);

class Thread {			//no need to have the two classes they are too similar
public:
	Chunk** chunkListLeft;
	Chunk** chunkListDone;
	int chunkLeft;
	char status;

	Thread() {
		chunkLeft = 0;
		status = THREAD_ALIVE;
		chunkListLeft = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
		chunkListDone = (Chunk**)calloc(MAX_CHUNK_PER_THREAD, sizeof(Chunk*));
	}

	~Thread() {
		free(chunkListLeft);
		free(chunkListDone);
	}

	void Launch(void (*routine)(Thread&)) {
		std::thread(routine, std::ref(*this)).detach();
	}
};
/*
	for now the thread routine :

	main assing chunk ---> dataThread find a place for the result and generate data ---> meshThread find a place for the result and create mesh ---> main bind result


	and later maybe use a list of char to lock places in the array

*/

class ThreadControleur {
private:
	Thread *dataThreads;
	Thread *meshThreads;
	
public:
	// default contuctor that launch the threads
	ThreadControleur(void);

	// send a signal to the threads and wait for them to properly close
	void StopThreads(void);

	// assing the generation of a chunk to a thread
	void LoadChunk(Chunk* chunk);

	// assing the generation of multiple chunk to the threads
	void LoadChunk(std::vector<Chunk*> &chunks);

	// assing the generation of a chunk to a thread
	void CreateMesh(std::vector<Chunk*> &chunks, std::vector<Chunk*> &chunksLoading);

	// assing the generation of a chunk to a thread
	void CreateMesh(Chunk* chunk);

	// bind all the completed chunks from the threads
	void BindAllChunks();

	// unlock all the loaded chunks from the threads
	void UnlockLoadedChunks(void);
};

#endif