#include "thread.h"

Thread::Thread(void) {
	dataThreads = new DataThread[DATA_THREAD_NUMBER];
	meshThreads = new MeshThread[MESH_THREAD_NUMBER];
	for (int n = 0; n < DATA_THREAD_NUMBER; n++)
		dataThreads[n].Launch();
	for (int n = 0; n < MESH_THREAD_NUMBER; n++)
		meshThreads[n].Launch();
}

void Thread::StopThreads(void) {
	for (int n = 0; n < DATA_THREAD_NUMBER; n++)
		dataThreads[n].status = THREAD_DYING;
	for (int n = 0; n < MESH_THREAD_NUMBER; n++)
		meshThreads[n].status = THREAD_DYING;

	for (int n = 0; n < DATA_THREAD_NUMBER; n++)
		while (dataThreads[n].status != THREAD_DEAD)
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
	for (int n = 0; n < MESH_THREAD_NUMBER; n++)
		while (meshThreads[n].status != THREAD_DEAD)
			std::this_thread::sleep_for(std::chrono::microseconds(1000));

	delete[] dataThreads;
	delete[] meshThreads;
}



// assing the mesh creation of a chunk to a thread
void Thread::CreateMesh(Chunk* chunk) {
	int thread = 0;

	for (int t = 0; t < MESH_THREAD_NUMBER - 1; t++)
		if (meshThreads[t].chunkLeft > meshThreads[t + 1].chunkLeft)
			thread = t + 1;

	if (meshThreads[thread].chunkLeft == MAX_CHUNK_PER_THREAD)
		return;
	for (int m = 0; m < 4; m++)
		if (!chunk->neighbour[m] || chunk->neighbour[m]->status < CHUNK_DATA_LOADED)
			return;
	for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
		if (!meshThreads[thread].chunkListLeft[n]) {
			chunk->threadStatus |= CHUNK_PROCESSING;
			for (int m = 0; m < 4; m++) // lock all the neighboring chunks needed to generate mesh
					chunk->neighbour[m]->threadStatus += 1;
			meshThreads[thread].chunkListLeft[n] = chunk;
			meshThreads[thread].chunkLeft += 1;
			return;
		}
	}
}

void Thread::BindAllChunks(void) {
	for (int thread = 0; thread < MESH_THREAD_NUMBER; thread++) {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (!meshThreads[thread].chunkLeft)
				break;
			if (meshThreads[thread].chunkListDone[n]) {
				meshThreads[thread].chunkListDone[n]->Bind();
				for (int m = 0; m < 4; m++) // unlock all the neighboring chunks needed to generate mesh
					meshThreads[thread].chunkListDone[n]->neighbour[m]->threadStatus -= 1;
				meshThreads[thread].chunkListDone[n] = 0;
				meshThreads[thread].chunkLeft -= 1;
			}
		}
	}
}


void Thread::LoadChunk(Chunk* chunk) {
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
			return;
		}
	}
}

void Thread::UnlockLoadedChunks(void) {
	for (int thread = 0; thread < DATA_THREAD_NUMBER; thread++) {
		for (int n = 0; n < MAX_CHUNK_PER_THREAD; n++) {
			if (dataThreads[thread].chunkListDone[n]) {
				dataThreads[thread].chunkListDone[n]->threadStatus &= 15;
				dataThreads[thread].chunkListDone[n] = 0;
				dataThreads[thread].chunkLeft -= 1;
			}
			if (!dataThreads[thread].chunkLeft)
				break;
		}
	}
}

void DataThreadRoutine(DataThread& dataThread) {		// maybe do a single thread instead of 2 specialized ones
	int n, m;

	while (dataThread.status) {
		if (!dataThread.chunkLeft) {
			std::this_thread::sleep_for(std::chrono::microseconds(3000));
			continue;
		}
		for (n = 0; n < MAX_CHUNK_PER_THREAD; n++) {			// maybe use one loop and 2 condition 
			if (dataThread.chunkListLeft[n]) {
				for (m = 0; m < MAX_CHUNK_PER_THREAD; m++) {
					if (!dataThread.chunkListDone[m]) {
						dataThread.chunkListLeft[n]->Generate();
						dataThread.chunkListDone[m] = dataThread.chunkListLeft[n];
						dataThread.chunkListLeft[n] = 0;
						break;
					}
				}
			}
		}
	}
	dataThread.status = THREAD_DEAD;
//	std::cout << "data thread is dead"<< std::endl;
}

void MeshThreadRoutine(MeshThread& meshThread) {
	int n, m;

	while (meshThread.status) {
		if (!meshThread.chunkLeft) {
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
	}
	meshThread.status = THREAD_DEAD;
//	std::cout << "mesh thread is dead" << std::endl;
}