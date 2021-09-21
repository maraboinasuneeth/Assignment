// CiscoAssignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <assert.h>
#include <functional>
#include <unordered_map>

/**************************************************************/
/**************************Problem - 1*************************/
/**************************************************************/
// The RTP packet format is described at
// https://tools.ietf.org/html/rfc3550#page-13

typedef std::function<void(uint32_t)> rtp_callback_fn;
#define COMBINE_BYTES_32(BYTE1, BYTE2, BYTE3, BYTE4) (  \
			BYTE1 << 24 | BYTE2 << 16 | BYTE3 << 8 | BYTE4 \
			) \

class RtpHandler {
private:
	std::unordered_map<uint32_t, rtp_callback_fn> map_callbacks;
public:
	// register callback. when packet with matching csrc is
	// received, callback is called
	void registerCallback(uint32_t csrc, rtp_callback_fn callback) {
		// TODO 1: add implementation
		map_callbacks[csrc] = callback;
	}
	// receive RTP packet, and call any matching callback
	void receivePacket(const uint8_t*rtpPacket, const size_t len) const {
		// TODO 2: add implementation
		assert(rtpPacket != nullptr);

		for (size_t indx = 0; indx < len - 4; indx++)
		{
			uint32_t key = COMBINE_BYTES_32((uint32_t)rtpPacket[indx],
				(uint32_t)rtpPacket[indx + 1],
				(uint32_t)rtpPacket[indx + 2],
				(uint32_t)rtpPacket[indx + 3]);
			if (map_callbacks.find(key) != map_callbacks.end())
			{
				map_callbacks.find(key)->second(key);
			}
		}
	}
};

const uint8_t packet1[] = { 0x91, 0x6c, 0xd3, 0x31, 0x48, 0x0d, 0xb8, 0x9a, 0x10, 0x58, 0xf3, 0xf2,
0x84, 0x2a, 0xbb, 0x01, 0xbe, 0xde, 0x00, 0x01, 0x10, 0x28, 0x20, 0x00,
0x61, 0x01, 0x43, 0x20, 0xbb, 0x42, 0xed, 0x0f, 0x90, 0x1d, 0xe0, 0xa6 };

const uint8_t packet2[] = { 0x91, 0x6c, 0x57, 0xb4, 0xfc, 0xaa, 0x7e, 0x3e, 0x29, 0x4d, 0x07, 0x6f,
0xe8,
0x32, 0x8f, 0x01, 0xbe, 0xde, 0x00, 0x01, 0x10, 0x1e, 0x20, 0x80, 0x01, 0x02,
0x8c, 0x70, 0x02, 0x20, 0x08, 0x67, 0x20, 0x67, 0x89, 0xeb, 0xd0, 0x24, 0x40 };

const uint8_t packet3[] = { 0x91, 0x6c, 0xa4, 0x66, 0xe3, 0x8e, 0xbf, 0x36, 0xdf, 0x47, 0x72, 0x57,
0xe9,
0xcf, 0x61, 0x01, 0xbe, 0xde, 0x00, 0x01, 0x10, 0x32, 0x20, 0x00, 0x61, 0xd0,
0xba, 0x62, 0xe9, 0x8f, 0x90, 0x1d, 0xe0, 0x45, 0xe6, 0xbd, 0xe3, 0x20 };

const uint8_t packet4[] = { 0x90, 0x6c, 0xa4, 0x66, 0xe3, 0x8e, 0xbf, 0x36, 0xdf, 0x47, 0x72, 0x01,
0xbe, 0xde, 0x00, 0x01, 0x10, 0x32, 0x20, 0x00, 0x61, 0xd0, 0xba, 0x62,
0xe9, 0x8f, 0x90, 0x1d, 0xe0, 0x45, 0xe6, 0xbd, 0xe3, 0x20 };


void Solution1() noexcept
{
	// a map of CSRC to a count of the number of times the CSRC was seen
	std::unordered_map<uint32_t, std::size_t> csrcToCountMap;
	RtpHandler handler;

	rtp_callback_fn  callBackFun = [&](uint32_t csrc) {
		if (csrcToCountMap.find(csrc) == csrcToCountMap.end())
		{
			csrcToCountMap[csrc] = 0;
		}
		else
		{
			csrcToCountMap[csrc] += 1;
		}
	};
	/*

	* TODO 3: add callbacks for the following CSRC
	*/
	handler.registerCallback(0x1058f3f2, callBackFun);
	handler.registerCallback(0x842abb01, callBackFun);
	handler.registerCallback(0xe8328f01, callBackFun);

	handler.receivePacket(packet1, sizeof(packet1));
	handler.receivePacket(packet2, sizeof(packet2));
	handler.receivePacket(packet3, sizeof(packet3));
	handler.receivePacket(packet4, sizeof(packet4));

	// the following expressions should all be true, so no assert is triggered
	assert(csrcToCountMap.size() == 3);
	assert(csrcToCountMap.count(0x1058f3f2) == 1);
	assert(csrcToCountMap.count(0x842abb01) == 1);
	assert(csrcToCountMap.count(0xe8328f01) == 1);
}

/**************************************************************/
/**************************Problem - 2*************************/
/**************************************************************/
/*
Consider a video conferencing meeting where a log for participants joining and leaving time is
maintained.Find the time at which there are maximum participants in the meeting.Note that
entries in the log are not in any order.
Input: join[] = { 1, 2, 9, 5, 5 }
	leave[] = { 4, 5, 12, 9, 12 }
	First participant joins at 1 and leaves at 4,
	second participant joins at 2 and leaves at 5, and so on.
	Output : 5
	There are maximum 3 guests at time 5.
*/
void Solution2() noexcept
{
	const uint8_t join[] = { 1, 2, 9, 5, 5 };
	const uint8_t leave[] = { 4, 5, 12, 9, 12 };

	const uint8_t MAX_HOURS = 24;
	uint32_t map_meet_hours[MAX_HOURS] = { 0 };
	uint32_t max_persons_meet = 0;
	uint8_t max_persons_meet_time = 0;

	for (size_t indx = 0; indx < sizeof(join); indx++)
	{
		for (size_t time = join[indx]; time <= leave[indx]; time++)
			map_meet_hours[time] += 1;
	}

	// Find the maximum.
	for (size_t indx = 0; indx < MAX_HOURS; indx++)
	{
		if (max_persons_meet < map_meet_hours[indx])
		{
			max_persons_meet = map_meet_hours[indx];
			max_persons_meet_time = static_cast<uint8_t>(indx);
		}
	}

	// Found maxtime and num of persons.
	printf("\nMax number of people in a meeting = %d Time at which Max number of people are present = %d \n", max_persons_meet, max_persons_meet_time);
}

/**************************************************************/
/**************************Problem - 3*************************/
/**************************************************************/
/*
You are given an integer array nums sorted in ascending order, and an integer target.
Suppose that nums is rotated at some pivot unknown to you beforehand
(i.e., [0,1,2,4,5,6,7] might become [4,5,6,7,0,1,2]).
If target is found in the array return its index, otherwise, return -1.
Assume All values of nums are unique.

Example 1:
Input: nums = [4,5,6,7,0,1,2], target = 0
Output: 4
Example 2:
Input: nums = [4,5,6,7,0,1,2], target = 3
Output: -1
Example 3:
Input: nums = [1], target = 0
Output: -1
*/

int16_t FindMatchedIndex(const int16_t nums[], const uint16_t start_index, const uint16_t end_index, const int16_t target)
{
	auto low = start_index;
	auto high = end_index;
	auto mid = (low + high) / 2;

	while (low < high)
	{
		if (target > nums[mid] )
		{
			low = mid + 1;
		}
		else if (target < nums[mid])
		{
			high = mid - 1;
		}
		else
		{
			return mid;
		}

		mid = (low + high) / 2;
	}

	if (target == nums[mid])
		return mid;

	return -1;
}

uint16_t FindPivotIndex(const int16_t nums[], const uint16_t uleft, const  uint16_t uright)
{
	auto left = uleft;
	auto right = uright;
	if (nums[left] < nums[right])
		return 0;

	while (left <= right) {
		int pivot = (left + right) / 2;
		if (nums[pivot] > nums[pivot + 1])
			return pivot + 1;
		else {
			if (nums[pivot] < nums[left])
				right = pivot - 1;
			else
				left = pivot + 1;
		}
	}

	return 0;
}

void Solution3() noexcept
{
	int16_t nums[] = { 4, 5, 6, 7 ,8, 0, 1, 2 };
	
	uint16_t target = 0;
	uint16_t start_index = 0;
	uint16_t end_index = sizeof(nums) /sizeof(decltype(nums[0])) -1;
	size_t resultIndex = -1;

	auto pivotIndex = FindPivotIndex(nums, start_index, end_index);
	if (target == nums[pivotIndex])
	{
		 resultIndex = pivotIndex;
	}
	else if (target <= nums[end_index] && target > nums[pivotIndex])
	{
		resultIndex = FindMatchedIndex(nums, pivotIndex , end_index, target);
	}
	else
	{
		resultIndex = FindMatchedIndex(nums, 0, pivotIndex -1, target);
	}

	if (resultIndex == -1)
	{
		printf("\nTarget number (%d) not found!\n", target);
	}
	else
	{
		printf("\nTarget number (%d) found at (%d)!\n", target , resultIndex);
	}
}

int main(int argc, char** argv) {
	// Solution to the Problem 1. 
	Solution1();

	// Solution to the Problem 2.
	Solution2();

	// Solution to the Problem 3.
	Solution3();
}


