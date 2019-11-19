/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#pragma once
#include "platform.hpp"
#include <cstring>
#include <cassert>

using proc_t = void(*)();

class vmt_base_hook
{
protected:
	constexpr vmt_base_hook() = default;

public:
  ~vmt_base_hook()
	{
		if(m_new_vmt)
			delete[] (m_new_vmt - 1);
	}

  vmt_base_hook(const vmt_base_hook&) = delete;
  vmt_base_hook(vmt_base_hook&&) = delete;

  vmt_base_hook& operator=(const vmt_base_hook&) = delete;
  vmt_base_hook& operator=(vmt_base_hook&&) = delete;

protected:
	auto initialize(proc_t* original_table) -> void
	{
		m_old_vmt = original_table;

		size_t size = 0;
		while(m_old_vmt[size] && platform::is_code_ptr(m_old_vmt[size]))
			++size;

		m_new_vmt = (new proc_t[size + 1]) + 1;
		//std::copy(m_old_vmt - 1, m_old_vmt + size, m_new_vmt - 1);
		memcpy(m_new_vmt - 1, m_old_vmt - 1, sizeof(void*) * (size + 1));
	}

	constexpr auto leak_table() -> void
	{
		m_new_vmt = nullptr;
	}

	auto hook_instance(void* inst) const -> void
	{
		auto& vtbl = *reinterpret_cast<proc_t**>(inst);
		assert(vtbl == m_old_vmt || vtbl == m_new_vmt);
		vtbl = m_new_vmt;
	}

	auto unhook_instance(void* inst) const -> void
	{
		auto& vtbl = *reinterpret_cast<proc_t**>(inst);
		assert(vtbl == m_old_vmt || vtbl == m_new_vmt);
		vtbl = m_old_vmt;
	}

	auto initialize_and_hook_instance(void* inst) -> bool
	{
		auto& vtbl = *reinterpret_cast<proc_t**>(inst);
		auto initialized = false;
		if(!m_old_vmt)
		{
			initialized = true;
			initialize(vtbl);
		}
		hook_instance(inst);
		return initialized;
	}

	template <typename Fn>
	auto hook_function(Fn hooked_fn, const std::size_t index) -> Fn
	{
		m_new_vmt[index] = (proc_t)(hooked_fn);
		return (Fn)(m_old_vmt[index]);
	}

	template<typename T>
	auto apply_hook(std::size_t idx) -> void
	{
		T::m_original = hook_function(&T::hooked, idx);
	}

	template <typename Fn = uintptr_t>
	auto get_original_function(const int index) -> Fn
	{
		return (Fn)(m_old_vmt[index]);
	}

private:
	proc_t* m_new_vmt = nullptr;
	proc_t* m_old_vmt = nullptr;
};

class vmt_smart_hook : vmt_base_hook
{
public:
	vmt_smart_hook(void* class_base)
		: m_class{class_base}
	{
		initialize_and_hook_instance(class_base);
	}

	~vmt_smart_hook()
	{
		unhook_instance(m_class);
	}

  vmt_smart_hook(const vmt_smart_hook&) = delete;
  vmt_smart_hook(vmt_smart_hook&&) = delete;

  vmt_smart_hook& operator=(const vmt_smart_hook&) = delete;
  vmt_smart_hook& operator=(vmt_smart_hook&&) = delete;

	auto rehook() const -> void
	{
		hook_instance(m_class);
	}

	auto unhook() const -> void
	{
		unhook_instance(m_class);
	}

	using vmt_base_hook::apply_hook;
	using vmt_base_hook::get_original_function;
	using vmt_base_hook::hook_function;

private:
	void* m_class = nullptr;
};

class vmt_multi_hook : vmt_base_hook
{
public:
	constexpr vmt_multi_hook() = default;

  ~vmt_multi_hook()
	{
		leak_table();
	}

  vmt_multi_hook(const vmt_multi_hook&) = delete;
  vmt_multi_hook(vmt_multi_hook&&) = delete;

  vmt_multi_hook& operator=(const vmt_multi_hook&) = delete;
  vmt_multi_hook& operator=(vmt_multi_hook&&) = delete;

	using vmt_base_hook::apply_hook;
	using vmt_base_hook::get_original_function;
	using vmt_base_hook::hook_function;
	using vmt_base_hook::hook_instance;
	using vmt_base_hook::unhook_instance;
	using vmt_base_hook::initialize;
	using vmt_base_hook::initialize_and_hook_instance;
};