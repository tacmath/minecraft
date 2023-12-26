#include "thread.h"

static std::mutex mutex;
static std::condition_variable cvMesh, cvData;

ThreadControleur::ThreadControleur(void) {
//	std::cout << "cores = " << std::thread::hardware_concurrency() << std::endl;
	dataThreads = new Thread[DATA_THREAD_NUMBER];
	meshThreads = new Thread[MESH_THREAD_NUMBER];
	for (int n = 0; n < DATA_THREAD_NUMBER; n++)
		dataThreads[n].Launch(DataThreadRoutine);
	for (int n = 0; n < MESH_THREAD_NUMBER; n++)
		meshThreads[n].Launch(MeshThreadRoutine);
}

void ThreadControleur::StopThreads(void) {
	for (int n = 0; n < DATA_THREAD_NUMBER; n++)
		dataThreads[n].status = THREAD_DYING;
	for (int n = 0; n < MESH_THREAD_NUMBER; n++)
		meshThreads[n].status = THREAD_DYING;
	cvMesh.notify_all();
	cvData.notify_all();
	delete[] dataThreads;
	delete[] meshThreads;
}


void ThreadControleur::CreateMesh(std::vector<Chunk*> &chunks, std::vector<Chunk*> &chunksLoading) {
	int		thread = 0;
	size_t	chunkNb;
	Chunk	*chunk;
	
	chunkNb = chunksLoading.size();
	for (size_t c = 0; c < chunkNb; c++) {
		chunk = chunksLoading[c];
		if (chunk->threadStatus & CHUNK_PROCESSING || chunk->status != CHUNK_DATA_LOADED)
			continue;

		for (int t = 0; t < MESH_THREAD_NUMBER - 1; t++)
			if (meshThreads[t].chunkLeft > meshThreads[t + 1].chunkLeft)
				thread = t + 1;
		if (meshThreads[thread].chunkLeft == MAX_CHUNK_PER_THREAD) {
			cvMesh.notify_all();
			return;
		}
		if (!chunk->HasAllNeighbours())
			continue;
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!meshThreads[thread].chunkListLeft[n]) {
				chunk->threadStatus |= CHUNK_PROCESSING;
				chunk->LockNeighbours();
				meshThreads[thread].chunkListLeft[n] = chunk;
				meshThreads[thread].chunkLeft += 1;
				chunks.push_back(chunk);
				chunksLoading.erase(chunksLoading.begin() + c--);
				chunkNb--;
				break;
			}
		}
	}
	cvMesh.notify_all();
}

// assing the mesh creation of a chunk to a thread
void ThreadControleur::CreateMesh(Chunk* chunk) {
	int thread = 0;

	for (int t = 0; t < MESH_THREAD_NUMBER - 1; t++)
		if (meshThreads[t].chunkLeft > meshThreads[t + 1].chunkLeft)
			thread = t + 1;

	if (meshThreads[thread].chunkLeft == MAX_CHUNK_PER_THREAD
		|| !chunk->HasAllNeighbours())
		return;
	for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
		if (!meshThreads[thread].chunkListLeft[n]) {
			chunk->threadStatus |= CHUNK_PROCESSING;
			chunk->LockNeighbours();
			meshThreads[thread].chunkListLeft[n] = chunk;
			meshThreads[thread].chunkLeft += 1;
			cvMesh.notify_one();
			return;
		}
	}
}

void ThreadControleur::BindAllChunks() {
	for (int thread = 0; thread < MESH_THREAD_NUMBER; thread++) {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!meshThreads[thread].chunkDone)
				break;
			if (meshThreads[thread].chunkListDone[n]) {
				meshThreads[thread].chunkListDone[n]->Bind();
				meshThreads[thread].chunkListDone[n]->UnlockNeighbours();
				meshThreads[thread].chunkListDone[n] = 0;
				meshThreads[thread].chunkDone -= 1;
			}
		}
	}
}

void ThreadControleur::LoadChunk(std::vector<Chunk*> &chunks) {
	int thread = 0;
	Chunk *chunk;

	for (size_t c = 0; c < chunks.size(); c++) {
		chunk = chunks[c];
		if (chunk->threadStatus & CHUNK_PROCESSING || chunk->status != CHUNK_UNLOADED)
			continue;

		for (int t = 0; t < DATA_THREAD_NUMBER - 1; t++)
			if (dataThreads[t].chunkLeft > dataThreads[t + 1].chunkLeft)
				thread = t + 1;
		if (dataThreads[thread].chunkLeft == MAX_CHUNK_PER_THREAD) {
			cvData.notify_all();
			return;
		}
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!dataThreads[thread].chunkListLeft[n]) {
				chunk->threadStatus |= CHUNK_PROCESSING;
				dataThreads[thread].chunkListLeft[n] = chunk;
				dataThreads[thread].chunkLeft += 1;
				break;
			}
		}
	}
	cvData.notify_all();
}

void ThreadControleur::LoadChunk(Chunk* chunk) {
	int thread = 0;

	for (int t = 0; t < DATA_THREAD_NUMBER - 1; t++)
		if (dataThreads[t].chunkLeft > dataThreads[t + 1].chunkLeft)
			thread = t + 1;
	if (dataThreads[thread].chunkLeft == MAX_CHUNK_PER_THREAD)
		return;
	for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
		if (!dataThreads[thread].chunkListLeft[n]) {
			chunk->threadStatus |= CHUNK_PROCESSING;
			dataThreads[thread].chunkListLeft[n] = chunk;
			dataThreads[thread].chunkLeft += 1;
			cvData.notify_one();
			return;
		}
	}
}

void ThreadControleur::UnlockLoadedChunks(void) {
	for (int thread = 0; thread < DATA_THREAD_NUMBER; thread++) {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!dataThreads[thread].chunkDone)
				break;
			if (dataThreads[thread].chunkListDone[n]) {
				dataThreads[thread].chunkListDone[n]->threadStatus &= 15;
				dataThreads[thread].chunkListDone[n] = 0;
				dataThreads[thread].chunkDone -= 1;
			}
		}
	}
}

void DataThreadRoutine(Thread& dataThread) {
	int n, m;

	while (dataThread.status) {
		if (!dataThread.chunkLeft) {
			std::unique_lock lock(mutex);
			cvData.wait(lock, [&] {return dataThread.chunkLeft > 0 || dataThread.status == THREAD_DYING; });
		}
		for (n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (dataThread.chunkListLeft[n]) { //a lot of time is spent to syncronise the cache 
				for (m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!dataThread.chunkListDone[m]) {
						dataThread.chunkListLeft[n]->Generate();
						dataThread.chunkListDone[m] = dataThread.chunkListLeft[n];
						dataThread.chunkListLeft[n] = 0;
						dataThread.chunkDone++;
						dataThread.chunkLeft--;
						break;
					}
				}
			}
		}
	}
	dataThread.status = THREAD_DEAD;
//	std::cout << "data thread is dead"<< std::endl;
}

void MeshThreadRoutine(Thread& meshThread) {
	int n, m;

	while (meshThread.status) {
		if (!meshThread.chunkLeft) {
			std::unique_lock lock(mutex);
			cvMesh.wait(lock, [&] {return meshThread.chunkLeft > 0 || meshThread.status == THREAD_DYING; });
		}
		for (n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (meshThread.chunkListLeft[n]) { //a lot of time is spent to syncronise the cache 
				for (m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!meshThread.chunkListDone[m]) {
						meshThread.chunkListLeft[n]->createMeshData();
						meshThread.chunkListDone[m] = meshThread.chunkListLeft[n];
						meshThread.chunkListLeft[n] = 0;
						meshThread.chunkDone++;
						meshThread.chunkLeft--;
						break;
					}
				}
			}
		}
	}
	meshThread.status = THREAD_DEAD;
//	std::cout << "mesh thread is dead" << std::endl;
}