#pragma once
#include <vector>
#include "Engine/Async/Spinlock.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations


//-----------------------------------------------------------------------------------------------
template<typename T>
class ThreadSafeVector
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	ThreadSafeVector(){}
	~ThreadSafeVector(){}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	size_t Size() const { return m_data.size(); } 

	//-----------------------------------------------------------------------------------------------
	// Methods

	// Iterators
			auto	begin() { return m_data.begin(); }
			auto	end() { return m_data.end(); }
			auto	rbegin() { return m_data.rbegin(); }
			auto	rend() { return m_data.rend(); }
	const	auto	begin() const { return m_data.begin(); }
	const	auto	end() const { return m_data.end(); }
	const	auto	rbegin() const { return m_data.rbegin(); }
	const	auto	rend() const { return m_data.rend(); }
	
	
	// Lock methods
	void LockEnter() const { m_lock.Enter(); }
	void LockLeave() const { m_lock.Leave(); }

	// Management methods
	void PushBack( const T& value )
	{
		m_lock.Enter(); // Only I have access
		m_data.push_back(value);
		m_lock.Leave(); // Releases the lock
	}

	bool Pop( T* outValue ) // Returns true if it succeeds
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

	void Erase( size_t index )
	{
		m_lock.Enter();
		m_data.erase(m_data.begin() + index);
		m_lock.Leave();
	}

	void Erase( const T& value )
	{
		m_lock.Enter();
		std::vector<T>::iterator found = std::find(m_data.begin(), m_data.end(), value);
		if(found != m_data.end())
		{
			m_data.erase(found);
		}
		m_lock.Leave();
	}

	size_t Find( const T& value )
	{
		m_lock.Enter();
		size_t found = UINT32_MAX;
		for(size_t index = 0; index < m_data.size(); index++)
		{
			if(m_data[index] == value)
			{
				found = index;
			}
		}
		m_lock.Leave();
		return found;
	}

	void Clear()
	{
		m_lock.Enter();
		m_data.clear();
		m_lock.Leave();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Operators
	T&	operator[] (int index)
	{
		return m_data[index];
	}

	T&	operator[] (size_t index)
	{
		return m_data[index];
	}

	//-----------------------------------------------------------------------------------------------
	// Members
			std::vector<T>	m_data;
	mutable Spinlock		m_lock;
};

