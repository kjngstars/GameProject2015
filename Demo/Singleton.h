#pragma once

#include <cassert>
#include <memory>

template<typename T>
class Singleton
{
private:
	static	T* _instance;
	
public:
	Singleton()
	{
		assert(_instance == nullptr);
		if (_instance == nullptr)
		{
			_instance = static_cast<T*>(this);
		}
	}

	virtual ~Singleton()
	{
		_instance = nullptr;
	}

	static T& GetInstance()
	{
		return *_instance;
	}

	static T* GetInstancePtr()
	{
		return _instance;
	}
};

template<typename T>
T* Singleton<T>::_instance = nullptr;
