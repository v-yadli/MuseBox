#pragma once

#using <system.dll>

using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Threading;

namespace System
{
	namespace IO
	{
		// This class was ported to managed c++ from a C# example found online. I would credit the author here, but I honestly don't know where the files were originally from, nor was there a license attached.

		generic<class T>
		public ref class CircularBuffer : System::Collections::Generic::ICollection<T>, System::Collections::Generic::IEnumerable<T>, System::Collections::ICollection
		{
		private:
			int capacity;
			int size;
			int head;
			int tail;
			bool _allowOverflow;
			array<T>^ buffer;
	
			[NonSerialized]
			Object^ syncRoot;
	
		public:

			CircularBuffer(int capacity);
	
			CircularBuffer(int capacity, bool allowOverflow);
	
			virtual property bool AllowOverflow { bool get(); void set(bool value); };
	
			virtual property int Capacity { int get(); void set (int value); };
	
			virtual property int Size { int get(); };
	
			virtual bool Contains(T item);
	
			virtual void Clear();
	
			int Put(array<T>^ src);
	
			int Put(array<T>^ src, int offset, int count);
	
			void Put(T item);
	
			void Skip(int count);
	
			array<T>^ Get(int count);
	
			int Get(array<T>^ dst);
	
			int Get(array<T>^ dst, int offset, int count);
	
			T Get();
	
			void CopyTo(array<T>^ array);
	
			virtual void CopyTo(array<T>^ array, int arrayIndex);
	
			void CopyTo(int index, array<T>^ array, int arrayIndex, int count);
	
			virtual System::Collections::IEnumerator^ EnumerableGetEnumerator() = System::Collections::IEnumerable::GetEnumerator;

			virtual System::Collections::Generic::IEnumerator<T>^ EnumerableTGetEnumerator() = System::Collections::Generic::IEnumerable<T>::GetEnumerator;
	
			array<T>^ GetBuffer();
	
			array<T>^ ToArray();
	
			#pragma region ICollection<T> Members
	
			virtual property int Count { int get(); };

			virtual property bool IsReadOnly { bool get(); };
	
			virtual void Add(T item);
	
			virtual bool Remove(T item);
	
			#pragma endregion
	
			#pragma region ICollection Members
	
			virtual property bool IsSynchronized { bool get(); };
	
			virtual property Object^ SyncRoot { Object^ get(); };
	
			virtual void CopyTo(Array^ _array, int arrayIndex);
			#pragma endregion
		};
	} // end namespace
} // end namespace