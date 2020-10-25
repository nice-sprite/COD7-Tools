#pragma once
#include <memory>
template<class T>
class CircularBuffer
{

public:
	explicit CircularBuffer(size_t size) :
		m_buf{ std::unique_ptr<T[]>(new T[size]) },
		m_maxSize{ size }
	{}

	void push(T item)
	{
		m_buf[m_head] = item;
		if (m_full)
			m_tail = (m_tail + 1) % m_maxSize;
		m_head = (m_head + 1) % m_maxSize;
		m_full = m_head - m_tail;
	}

	T get()
	{
		if (empty()) return T();
		T val = m_buf[m_tail];
		m_full = false;
		m_tail = (m_tail + 1) % m_maxSize;
		return val;
	}

	T peek()
	{
		if (empty()) return T();
		return m_buf[m_tail];
	}

	const T* data() const
	{
		return m_buf.get();
	}

	size_t size()
	{
		return length();
	}

	size_t length()
	{
		size_t size = m_maxSize;
		if (!m_full)
		{
			if (m_head >= m_tail) size = m_head - m_tail;
			else size = m_maxSize + m_head - m_tail;
		}
		return size;
	}

	bool empty()
	{
		return (!m_full) && (m_head == m_tail);
	}

	bool full()
	{
		return m_full;
	}

	void reset()
	{
		m_head = m_tail;
		m_full = false;
	}

	int tail() { return m_tail; }

private:
	std::unique_ptr<T[]> m_buf;
	size_t m_head = 0, m_tail = 0;
	const size_t m_maxSize;
	bool m_full = false;
};

