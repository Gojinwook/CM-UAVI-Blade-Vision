#pragma once

// Date : 2025.02.24
// Version : v2007
// Edited by : LeeGW

#include <vector>
#include <thread>
#include <mutex>

using namespace std;

class CThreadManager {
private:
	vector<thread> threads;
	mutex mtx;

	int m_iSize;

public:
	void Add(thread th)
	{
		lock_guard<mutex> lock(mtx);
		threads.emplace_back(move(th));
		m_iSize = threads.size();
	}

	void Clear()
	{
		lock_guard<mutex> lock(mtx);
		for (auto& th : threads) {
			if (th.joinable()) {
				th.join();
			}
		}
		threads.clear();
		
		m_iSize = 0;
	}

	void WaitFront()
	{
		lock_guard<mutex> lock(mtx);
		if (threads.front().joinable())
		{
			threads.front().join();
			threads.erase(threads.begin());
			m_iSize = threads.size();
		}
	}

	void WaitBack()
	{
		lock_guard<mutex> lock(mtx);
		if (threads.front().joinable())
		{
			threads.back().join();
			threads.pop_back();
			m_iSize = threads.size();
		}
	}


	int Size()
	{
		return m_iSize;
	}

	CThreadManager() {
		m_iSize = 0;
	}

	~CThreadManager() {
		Clear();
	}
};