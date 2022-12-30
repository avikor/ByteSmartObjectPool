#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ByteSmartObjectPool.h"


struct TrivialSturct
{
	int i;
	float f;
	double d;
};


TEST_CASE("simple int pool", "[ByteSmartObjectPool]")
{
	BSPool::ByteSmartObjectPool<int, 2U> simpleIntPool{};

	BSPool::PoolItem<int> pInt1 = simpleIntPool.request(1);
	REQUIRE(*pInt1 == 1);

	{
		auto pInt2 = simpleIntPool.request();
		REQUIRE(*pInt2 == 0);

		REQUIRE_THROWS_AS(simpleIntPool.request(), BSPool::max_capacity_exception);
	}
	
	auto pInt3 = simpleIntPool.request();
	REQUIRE(*pInt3 == 0);
}

TEST_CASE("trivial struct pool", "[ByteSmartObjectPool]")
{
	BSPool::ByteSmartObjectPool<TrivialSturct, 2U> trivialSturctPool{};

	BSPool::PoolItem<TrivialSturct> trivial1 = trivialSturctPool.request(1, 2.5f, 3.3);
	REQUIRE(trivial1->i == 1);
	REQUIRE(trivial1->f == 2.5f);
	REQUIRE(trivial1->d == 3.3);

	{
		auto trivial2 = trivialSturctPool.request();
		REQUIRE(trivial2->i == 0);
		REQUIRE(trivial2->f == 0.0f);
		REQUIRE(trivial2->d == 0.0);

		REQUIRE_THROWS_AS(trivialSturctPool.request(), BSPool::max_capacity_exception);
	}

	auto trivial3 = trivialSturctPool.request();
	REQUIRE(trivial3->i == 0);
	REQUIRE(trivial3->f == 0.0f);
	REQUIRE(trivial3->d == 0.0);
}