#include <iostream>
#include <algorithm>
#include <random>
#include <DirectXMath.h>
#include <functional>
#include <chrono>
#include "Quaternion.h"
#include "XQuaternion.h"

int TEST_COUNT = (1000000);

std::ostream& operator<<(std::ostream& os, const DirectX::XMVECTOR& obj)
{
	DirectX::XMFLOAT4 sobj{};
	DirectX::XMStoreFloat4(&sobj, obj);
	os << "x = " << sobj.x
		<< ",\ty = " << sobj.y
		<< ",\tz = " << sobj.z
		<< ",\tw = " << sobj.w;
	return os;
}

bool CompareFloat(float x, float y, float epsilon = FLT_EPSILON * 10)
{
	return abs(x - y) <= epsilon * std::max(1.0f, std::max(abs(x), abs(y)));
}

template <class T>
bool operator==(const T & lhs, DirectX::XMVECTOR rhs)
{
	DirectX::XMFLOAT4 srhs{};
	DirectX::XMStoreFloat4(&srhs, rhs);

	return CompareFloat(lhs.x(), srhs.x) &&
		CompareFloat(lhs.y(), srhs.y) &&
		CompareFloat(lhs.z(), srhs.z) &&
		CompareFloat(lhs.w(), srhs.w);
}

template <class T>
bool operator!=(const T & lhs, DirectX::XMVECTOR rhs)
{
	return !(lhs == rhs);
}


int main()
{
	std::cout << "Performing " << TEST_COUNT << " calculation tests between DirectX::XMVECTOR, SlowMath::Quaternion and FastMath::Quaternion." << std::endl;

	std::cout << "Configuration: " <<
#ifdef _DEBUG
		"Debug"
#else
		"Release"
#endif
		<< std::endl;

	std::cout << "Platform: " <<
#ifdef _WIN64 
		"x64"
#else
		"x86"
#endif
		<< std::endl;

	std::cout << std::endl << "=============Press enter to continue=============" << std::endl;
	getchar();

	// Prepare data
	std::cout << "Preparing data for calculation..." << std::endl;

	auto* xmq = new DirectX::XMVECTOR[TEST_COUNT * 2];
	auto* slq = new SlowMath::Quaternion[TEST_COUNT * 2];
	auto* fsq = new FastMath::Quaternion[TEST_COUNT * 2];

	auto* xmResult = new DirectX::XMVECTOR[TEST_COUNT];
	auto* slResult = new SlowMath::Quaternion[TEST_COUNT];
	auto* fsResult = new FastMath::Quaternion[TEST_COUNT];

	auto* t = new float[TEST_COUNT];

	std::cout << "\tDirectX::XMVECTOR*\tis " << ((uintptr_t(xmq) & 15) == 0 ? "" : "not") << " memory aligned." << std::endl;
	std::cout << "\tSlowMath::Quaternion*\tis " << ((uintptr_t(slq) & 15) == 0 ? "" : "not") << " memory aligned." << std::endl;
	std::cout << "\tFastMath::Quaternion*\tis " << ((uintptr_t(fsq) & 15) == 0 ? "" : "not") << " memory aligned." << std::endl;

	// Random number generator
	const unsigned seed = unsigned(std::chrono::system_clock::now().time_since_epoch().count());
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> dist_pi(0.0f, 3.1415926535f);
	const std::uniform_real_distribution<float> dist_zero_one(0.0f, 1.0f);
	auto random_radian = std::bind(dist_pi, generator);

	for (int i = 0; i < TEST_COUNT * 2; ++i)
	{
		// Create quaternions with DirectXMath library
		xmq[i] = DirectX::XMQuaternionRotationRollPitchYaw(random_radian(), random_radian(), random_radian());
		DirectX::XMFLOAT4 xmf{};
		XMStoreFloat4(&xmf, xmq[i]);
		// Copy to my quaternions
		slq[i] = SlowMath::Quaternion(xmf.x, xmf.y, xmf.z, xmf.w);
		fsq[i] = FastMath::Quaternion(xmf.x, xmf.y, xmf.z, xmf.w);
	}

	// Randomize t
	for (int i = 0; i < TEST_COUNT; ++i)
		t[i] = dist_zero_one(generator);

	std::cout << std::endl << "=============Press enter to continue=============" << std::endl;
	getchar();

	// Calculation

	// DirectXMath
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < TEST_COUNT; ++i)
	{
		xmResult[i] = DirectX::XMQuaternionSlerp(xmq[2 * i], xmq[2 * i + 1], t[i]);
	}

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	const double xmTime = time_span.count();

	// SlowMath
	t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < TEST_COUNT; ++i)
	{
		slResult[i] = SlowMath::Quaternion::Slerp(slq[2 * i], slq[2 * i + 1], t[i]);
	}

	t2 = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	const double slTime = time_span.count();

	// FastMath
	t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < TEST_COUNT; ++i)
	{
		fsResult[i] = FastMath::Quaternion::Slerp(fsq[2 * i], fsq[2 * i + 1], t[i]);
	}

	t2 = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	const double fsTime = time_span.count();

	// SlowMath : Fast
	t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < TEST_COUNT; ++i)
	{
		slResult[i] = SlowMath::Quaternion::Slerp(slq[2 * i], slq[2 * i + 1], t[i]);
	}

	t2 = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	const double slfTime = time_span.count();

	// FastMath : Fast
	t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < TEST_COUNT; ++i)
	{
		fsResult[i] = FastMath::Quaternion::Slerp(fsq[2 * i], fsq[2 * i + 1], t[i]);
	}

	t2 = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	const double fsfTime = time_span.count();

	// Output Results
	std::cout << "Time cost by " << TEST_COUNT << " slerp calculations:" << std::endl;
	if (xmTime * 1000 > 5)
		std::cout << "\tDirectX::XMQuaternionSlerp:\t\t" << xmTime * 1000.0 << "ms" << std::endl;
	else
		std::cout << "\tDirectX::XMQuaternionSlerp:\t\t" << xmTime * 1000000.0 << "us" << std::endl;
	if (slTime * 1000 > 5)
		std::cout << "\tSlowMath::Quaternion::Slerp:\t\t" << slTime * 1000.0 << "ms" << std::endl;
	else
		std::cout << "\tSlowMath::Quaternion::Slerp:\t\t" << slTime * 1000000.0 << "us" << std::endl;
	if (slfTime * 1000 > 5)
		std::cout << "\tSlowMath::Quaternion::FastSlerp:\t" << slfTime * 1000.0 << "ms" << std::endl;
	else
		std::cout << "\tSlowMath::Quaternion::FastSlerp:\t" << slfTime * 1000000.0 << "us" << std::endl;
	if (fsTime * 1000 > 5)
		std::cout << "\tFastMath::Quaternion::Slerp:\t\t" << fsTime * 1000.0 << "ms" << std::endl;
	else
		std::cout << "\tFastMath::Quaternion::Slerp:\t\t" << fsTime * 1000000.0 << "us" << std::endl;
	if (fsfTime * 1000 > 5)
		std::cout << "\tFastMath::Quaternion::FastSlerp:\t" << fsfTime * 1000.0 << "ms" << std::endl;
	else
		std::cout << "\tFastMath::Quaternion::FastSlerp:\t" << fsfTime * 1000000.0 << "us" << std::endl;

	std::cout << std::endl << "=============Press enter to continue=============" << std::endl;
	getchar();

	// Results correctness
	std::cout << "Results correctness tests (only failed tests shown):" << std::endl << std::endl;
	for (int i = 0; i < TEST_COUNT; ++i)
	{
		if (slResult[i] != xmResult[i] || fsResult[i] != xmResult[i])
		{
			std::cout << "\tTest " << i << " Failed!" << std::endl;
			std::cout << "\t\tOperands:" << std::endl;
			std::cout << "\t\t\tQ1: " << xmq[2 * i] << std::endl;
			std::cout << "\t\t\tQ2: " << xmq[2 * i + 1] << std::endl;
			std::cout << "\t\t\t t: " << t[i] << std::endl;
			std::cout << "\t\tResults:" << std::endl;
			std::cout << "\t\t\tDirectXMath Result: " << xmResult[i] << std::endl;
			std::cout << "\t\t\t   SlowMath Result: " << slResult[i] << std::endl;
			std::cout << "\t\t\t   FastMath Result: " << fsResult[i] << std::endl;
			std::cout << std::endl;
		}
	}

	std::cout << std::endl << "=============Press enter to continue=============" << std::endl;
	getchar();

	delete[] xmq;
	delete[] slq;
	delete[] fsq;
	delete[] xmResult;
	delete[] slResult;
	delete[] fsResult;
	delete[] t;

	return 0;
}
