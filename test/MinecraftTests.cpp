#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MinecraftTests
{
	TEST_CLASS(EntityTest)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			Entity entity;

			std::cout << entity.aabb().to_string() << std::endl;
			//Assert::AreEqual(entity.look, entity.look);
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
