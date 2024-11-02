#pragma once
#include "Core.h"

namespace Tridium {

#pragma region Ref

	template <typename T>
	class Ref
	{
	public:
		Ref();
		Ref( GUID a_ID );
		Ref( const Ref<T>& a_Other); // Copy Constructor
		Ref( Ref<T>&& a_Other ) noexcept; // Move Constructor
		Ref<T>& operator=( const Ref<T>& a_Other ); // Copy Assignment
		Ref<T>& operator=( Ref<T>&& a_Other ) noexcept; // Move Assignment
		~Ref() = default;

		T& operator*() const { return *m_AssetPtr; }
		T* operator->() const { return m_AssetPtr.get(); }

	private:
		GUID m_ID;
		SharedPtr<T> m_AssetPtr;
	};
}