#include "CircularBuffer.h"

using namespace System::IO;

generic<class T> CircularBuffer<T>::CircularBuffer(int capacity)
{
	if (capacity < 0)
		throw gcnew System::ArgumentException("capacity must be greater than or equal to zero.", "capacity");

	this->capacity = capacity;
	size = 0;
	head = 0;
	tail = 0;
	buffer = gcnew array<T>(capacity);
	AllowOverflow = false;
} // end CircularBuffer

generic<class T> CircularBuffer<T>::CircularBuffer(int capacity, bool allowOverflow)
{
	if (capacity < 0)
		throw gcnew System::ArgumentException("capacity must be greater than or equal to zero.", "capacity");

	this->capacity = capacity;
	size = 0;
	head = 0;
	tail = 0;
	buffer = gcnew array<T>(capacity);
	AllowOverflow = allowOverflow;
} // end CircularBuffer

generic<class T> bool CircularBuffer<T>::AllowOverflow::get()
{
	return _allowOverflow;
} // end get

generic<class T> void CircularBuffer<T>::AllowOverflow::set(bool value)
{
	_allowOverflow = value;
} // end set

generic<class T> int CircularBuffer<T>::Capacity::get()
{
	return capacity; 
} // end get

 generic<class T> void CircularBuffer<T>::Capacity::set (int value)
{
	if (value == capacity)
	{
		return;
	} // end if

	if (value < size)
	{
		throw gcnew System::ArgumentOutOfRangeException("value", "value must be greater than or equal to the buffer size.");
	} // end if

	array<T>^ dst = gcnew array<T>(value);

	if (size > 0)
	{
		CopyTo(dst);
	} // end if

	buffer = dst;
	capacity = value;
} // end set

generic<class T> int CircularBuffer<T>::Size::get() 
{
	return size;
} // end get 

generic<class T> bool CircularBuffer<T>::Contains(T item)
{
	int bufferIndex = head;
	EqualityComparer<T>^ comparer = EqualityComparer<T>::Default;
	for (int i = 0; i < size; i++, bufferIndex++)
	{
		if (bufferIndex == capacity)
			bufferIndex = 0;

		if (item == nullptr && buffer[bufferIndex] == nullptr)
			return true;
		else if ((buffer[bufferIndex] != nullptr) &&
			comparer->Equals(buffer[bufferIndex], item))
			return true;
	}

	return false;
} // end Contains

generic<class T> void CircularBuffer<T>::Clear()
{
	size = 0;
	head = 0;
	tail = 0;
}

generic<class T> int CircularBuffer<T>::Put(array<T>^ src)
{
	return Put(src, 0, src->Length);
}

generic<class T> int CircularBuffer<T>::Put(array<T>^ src, int offset, int count)
{
	int realCount = AllowOverflow ? count : System::Math::Min(count, capacity - size);
	int srcIndex = offset;
	for (int i = 0; i < realCount; i++, tail++, srcIndex++)
	{
		if (tail == capacity)
			tail = 0;
		buffer[tail] = src[srcIndex];
	}
	size = System::Math::Min(size + realCount, capacity);
	return realCount;
}

generic<class T> void CircularBuffer<T>::Put(T item)
{
	if (!AllowOverflow && size == capacity)
		throw gcnew InternalBufferOverflowException("Buffer is full.");

	buffer[tail] = item;
	if (tail++ == capacity)
		tail = 0;
	size++;
}

generic<class T> void CircularBuffer<T>::Skip(int count)
{
	head += count;
	if (head >= capacity)
		head -= capacity;
}

generic<class T> array<T>^ CircularBuffer<T>::Get(int count)
{
	array<T>^ dst = gcnew array<T>(count);
	Get(dst);
	return dst;
}

generic<class T> int CircularBuffer<T>::Get(array<T>^ dst)
{
	return Get(dst, 0, dst->Length);
}

generic<class T> int CircularBuffer<T>::Get(array<T>^ dst, int offset, int count)
{
	int realCount = System::Math::Min(count, size);
	int dstIndex = offset;
	for (int i = 0; i < realCount; i++, head++, dstIndex++)
	{
		if (head == capacity)
			head = 0;
		dst[dstIndex] = buffer[head];
	}
	size -= realCount;
	return realCount;
}

generic<class T> T CircularBuffer<T>::Get()
{
	if (size == 0)
		throw gcnew System::InvalidOperationException("Buffer is empty.");

	T item = buffer[head];
	if (head++ == capacity)
		head = 0;
	size--;
	return item;
}

generic<class T> void CircularBuffer<T>::CopyTo(array<T>^ array)
{
	CopyTo(array, 0);
}

generic<class T> void CircularBuffer<T>::CopyTo(array<T>^ array, int arrayIndex)
{
	CopyTo(0, array, arrayIndex, size);
}

generic<class T> void CircularBuffer<T>::CopyTo(int index, array<T>^ array, int arrayIndex, int count)
{
	if (count > size)
		throw gcnew System::ArgumentOutOfRangeException("count", "count cannot be greater than the buffer size.");

	int bufferIndex = head;
	for (int i = 0; i < count; i++, bufferIndex++, arrayIndex++)
	{
		if (bufferIndex == capacity)
			bufferIndex = 0;
		array[arrayIndex] = buffer[bufferIndex];
	}
}
generic<class T> System::Collections::IEnumerator^ CircularBuffer<T>::EnumerableGetEnumerator()
{
	int bufferIndex = head;
	for (int i = 0; i < size; i++, bufferIndex++)
	{
		if (bufferIndex == capacity)
			bufferIndex = 0;

		//yield return buffer[bufferIndex];
	}

	//FIXME: Need to figure out how to implement this.
	throw gcnew System::NotImplementedException();
}

generic<class T> System::Collections::Generic::IEnumerator<T>^ CircularBuffer<T>::EnumerableTGetEnumerator()
{
	//FIXME: Not really sure what to do here
	throw gcnew System::NotImplementedException();
}

generic<class T> array<T>^ CircularBuffer<T>::GetBuffer()
{
	return buffer;
}

generic<class T> array<T>^ CircularBuffer<T>::ToArray()
{
	array<T>^ dst = gcnew array<T>(size);
	CopyTo(dst);
	return dst;
}

#pragma region ICollection<T> Members

generic<class T> int CircularBuffer<T>::Count::get()
{ 
	return Size;
}

generic<class T> bool CircularBuffer<T>::IsReadOnly::get()
{
	return false;
}

 generic<class T> void CircularBuffer<T>::Add(T item)
{
	Put(item);
}

generic<class T> bool CircularBuffer<T>::Remove(T item)
{
	if (size == 0)
		return false;

	Get();
	return true;
}

#pragma endregion

#pragma region ICollection Members

generic<class T> bool CircularBuffer<T>::IsSynchronized::get()
{ 
	return false;
}

generic<class T> System::Object^ CircularBuffer<T>::SyncRoot::get()
{
	if (syncRoot == nullptr)
		Interlocked::CompareExchange(syncRoot, gcnew Object(), nullptr);
	return syncRoot;
}

generic<class T> void CircularBuffer<T>::CopyTo(System::Array^ _array, int arrayIndex)
{
	CopyTo((array<T>^)_array, arrayIndex);
}
#pragma endregion