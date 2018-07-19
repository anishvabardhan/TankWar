#pragma once
#include "Engine/Async/Spinlock.hpp"
#include <queue>

//-----------------------------------------------------------------------------------------------
// Forward Declarations


//-----------------------------------------------------------------------------------------------
template<typename T>
class ThreadSafeQueue
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	ThreadSafeQueue(){}
	~ThreadSafeQueue(){}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	
	//-----------------------------------------------------------------------------------------------
	// Methods
	void Enqueue( const T& value )
	{
		m_lock.Enter(); // Only I have access
		m_data.push(value);
		m_lock.Leave(); // Releases the lock
	}

	bool Dequeue( T* outValue ) // Returns true if it succeeds
	{
		m_lock.Enter();
		bool hasItem = !m_data.empty();

		if(hasItem)
		{
			*outValue = m_data.front();
			m_data.pop();
		}

		m_lock.Leave();

		return hasItem;
	}

	//-----------------------------------------------------------------------------------------------
	// Members
	std::queue<T> m_data;
	Spinlock	  m_lock;
};

