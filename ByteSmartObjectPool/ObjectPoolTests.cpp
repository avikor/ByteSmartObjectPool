#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ByteSmartObjectPool.hpp"


struct TrivialSturct
{
	int i;
	float f;
	double d;
};


TEST_CASE("simple int pool", "[ByteSmartObjectPool]")
{
	BSPool::ByteSmartObjectPool<int, 2U> trivialSturctPool{};
	
	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 0U);
	REQUIRE(!trivialSturctPool.isFull());

	BSPool::PoolItem<int> pInt1 = trivialSturctPool.request(1);
	
	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 1U);
	REQUIRE(!trivialSturctPool.isFull());

	REQUIRE(*pInt1 == 1);

	{
		auto pInt2 = trivialSturctPool.request();

		REQUIRE(trivialSturctPool.capacity() == 2U);
		REQUIRE(trivialSturctPool.size() == 2U);
		REQUIRE(trivialSturctPool.isFull());

		REQUIRE(*pInt2 == 0);

		REQUIRE_THROWS_AS(trivialSturctPool.request(), BSPool::max_capacity_exception);
	}

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 1U);
	REQUIRE(!trivialSturctPool.isFull());
	
	auto pInt3 = trivialSturctPool.request();
	REQUIRE(*pInt3 == 0);

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 2U);
	REQUIRE(trivialSturctPool.isFull());
}

TEST_CASE("trivial struct pool", "[ByteSmartObjectPool]")
{
	BSPool::ByteSmartObjectPool<TrivialSturct, 2U> trivialSturctPool{};

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 0U);
	REQUIRE(!trivialSturctPool.isFull());

	BSPool::PoolItem<TrivialSturct> trivial1 = trivialSturctPool.request(1, 2.5f, 3.3);

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 1U);
	REQUIRE(!trivialSturctPool.isFull());

	REQUIRE(trivial1->i == 1);
	REQUIRE(trivial1->f == 2.5f);
	REQUIRE(trivial1->d == 3.3);

	{
		auto trivial2 = trivialSturctPool.request();

		REQUIRE(trivialSturctPool.capacity() == 2U);
		REQUIRE(trivialSturctPool.size() == 2U);
		REQUIRE(trivialSturctPool.isFull());

		REQUIRE(trivial2->i == 0);
		REQUIRE(trivial2->f == 0.0f);
		REQUIRE(trivial2->d == 0.0);

		REQUIRE_THROWS_AS(trivialSturctPool.request(), BSPool::max_capacity_exception);
	}

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 1U);
	REQUIRE(!trivialSturctPool.isFull());

	auto trivial3 = trivialSturctPool.request();

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 2U);
	REQUIRE(trivialSturctPool.isFull());

	REQUIRE(trivial3->i == 0);
	REQUIRE(trivial3->f == 0.0f);
	REQUIRE(trivial3->d == 0.0);
}