#include "pch.h"
#include "CppUnitTest.h"

#include <glm/gtx/string_cast.hpp>

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<glm::vec3>(const glm::vec3& vec) {
				std::string string = glm::to_string(vec);
				return std::wstring(string.begin(), string.end());
			}
		}
	}
}


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MinecraftTests
{
	TEST_CLASS(EntityTest)
	{
	private:
		Chunk *createFlatChunkData(glm::ivec2 pos, unsigned char height, unsigned char blockID) {
			Chunk *chunk;

			chunk = new Chunk;
			chunk->SetPosistion(pos.x, pos.y);
			Chunk::chunksMap[GET_CHUNK_ID(pos.x, pos.y)] = chunk;
			if (!(chunk->cubes = (unsigned char*)calloc(1, sizeof(unsigned char) * 256 * CHUNK_SIZE * CHUNK_SIZE)))
				return chunk;
			memset(chunk->cubes, blockID, height * CHUNK_SIZE * CHUNK_SIZE);
			chunk->status = CHUNK_DATA_LOADED;
			return chunk;
		}

		void initTest() {
			for (auto chunk : Chunk::chunksMap)
				delete chunk.second;
			Chunk::chunksMap.clear();
			createFlatChunkData(glm::ivec2(0), 1, 1);
		}

	public:

		TEST_METHOD(ShouldMoveAndCollide)
		{
			// GIVEN
			Entity entity;
			glm::vec3 velocity;

			initTest();
			entity.size = glm::vec3(1);
			entity.position = glm::vec3(1, 1.5f, 1);
			velocity = glm::vec3(0);
			

			entity.Move(velocity, 0.5f);
			Assert::IsTrue(entity.Grounded());
			Assert::AreEqual(glm::vec3(1.0f, 1.05f, 1.0f), entity.position);
		}
	};

	TEST_CLASS(EntityUtilityTest)
	{
	public:

		TEST_METHOD(ShouldMoveAxisAndNotCollide)
		{
			//using
			AABB				box(glm::vec3(0), glm::vec3(1));
			float				movement = -1.0f;
			std::vector<AABB>	colliders;
			colliders.push_back(box.translate(glm::vec3(0, -2.1, 0)));

			//expected
			float expected = movement;

			//actual
			float actual = MoveAxis(box, movement, colliders, glm::vec3(0, 1, 0));
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(ShouldMoveAxisAndCollide)
		{
			//using
			AABB				box(glm::vec3(0), glm::vec3(1));
			float				movement = -1.0f;
			std::vector<AABB>	colliders;
			colliders.push_back(box.translate(glm::vec3(0, -1, 0)));

			//expected
			float expected = 0;

			//actual
			float actual = MoveAxis(box, movement, colliders, glm::vec3(0, 1, 0));
			Assert::AreEqual(expected, actual);

			box = box.translate(glm::vec3(0, 0.5f, 0));
			actual = MoveAxis(box, movement, colliders, glm::vec3(0, 1, 0));
			Assert::AreEqual(-0.45f, actual);

			box = box.translate(glm::vec3(0, 0.5f, 0));
			actual = MoveAxis(box, movement, colliders, glm::vec3(0, 1, 0));
			Assert::AreEqual(-0.95f, actual);
		}
	};
}
