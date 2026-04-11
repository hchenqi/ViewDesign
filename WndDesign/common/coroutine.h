#pragma once

#include "uncopyable.h"
#include "function_traits.h"

#include <optional>
#include <coroutine>
#include <functional>


namespace ViewDesign {


template<class T>
struct _Value_Container {
	void Set() {}
	void Get() {}
};

template<class T> requires (!std::is_void_v<T>)
struct _Value_Container<T> {
private:
	std::optional<T> value;
public:
	void Set(T value) { this->value = std::move(value); }
	T Get() { return std::move(std::exchange(value, std::nullopt).value()); }
};


template<class T = void>
struct Task : private Uncopyable, private _Value_Container<T> {
private:
	struct promise_base;
	template<class T> struct promise;
public:
	using promise_type = promise<T>;

private:
	Task(promise_base& handle) : handle(&handle) { this->handle->SetTask(this); }
public:
	Task(Task&& task) : _Value_Container<T>(std::move(task)), handle(task.handle) { task.handle = nullptr; if (handle) { handle->SetTask(this); } }
	void operator=(Task&& task) { _Value_Container<T>::operator=(std::move(task)); handle = task.handle; task.handle = nullptr; if (handle) { handle->SetTask(this); } }
	~Task() { if (handle) { handle->SetTask(nullptr); } }
private:
	ref_ptr<promise_base> handle;
public:
	bool await_ready() { return handle == nullptr; }
	void await_suspend(std::coroutine_handle<> continuation) { handle->SetOuterContinuation(continuation); }
	T await_resume() { return _Value_Container<T>::Get(); }

private:
	struct promise_base {
	protected:
		ref_ptr<Task> task = nullptr;
		std::coroutine_handle<> outer_continuation = nullptr;
	public:
		~promise_base() { if (task) { task->handle = nullptr; } if (outer_continuation) { outer_continuation.destroy(); } }
	public:
		void SetTask(ref_ptr<Task> task) { this->task = task; }
		void SetOuterContinuation(std::coroutine_handle<> outer_continuation) { this->outer_continuation = outer_continuation; }
	public:
		Task get_return_object() { return Task(*this); }
		auto initial_suspend() noexcept { return std::suspend_never{}; }
		auto final_suspend() noexcept { return std::suspend_never{}; }
		void unhandled_exception() {}
	};

	template<class T> requires (std::is_void_v<T>)
	struct promise<T> : promise_base {
	private:
		promise_base::task;
		promise_base::outer_continuation;
	public:
		void return_void() { if (outer_continuation) { outer_continuation(); outer_continuation = nullptr; } }
	};

	template<class T> requires (!std::is_void_v<T>)
	struct promise<T> : promise_base {
	private:
		promise_base::task;
		promise_base::outer_continuation;
	public:
		void return_value(T value) { if (task) { task->Set(std::move(value)); } if (outer_continuation) { outer_continuation(); outer_continuation = nullptr; } }
	};
};


template<class T = void>
struct TaskAwaitable;


template<class T = void>
struct Continuation {
public:
	using value_type = T;
private:
	friend struct TaskAwaitable<T>;
private:
	Continuation(std::function<void(T)> function, std::coroutine_handle<> continuation) : function(function), continuation(continuation) {}
public:
	Continuation() : Continuation(nullptr) {}
	Continuation(nullptr_t) : function(nullptr), continuation(nullptr) {}
private:
	std::function<void(T)> function;
	std::coroutine_handle<> continuation;
public:
	template<class U = T> requires(std::is_void_v<U>) void operator()() const { function(); }
	template<class U = T> requires(!std::is_void_v<U>) void operator()(U value) const { function(std::move(value)); }
	void Destroy() { continuation.destroy(); }
};


template<class T>
struct TaskAwaitable : private _Value_Container<T> {
private:
	std::function<void(Continuation<T>)> task;
public:
	TaskAwaitable(std::function<void(Continuation<T>)> task) : task(task) {}
public:
	bool await_ready() { return false; }
	void await_suspend(std::coroutine_handle<> continuation) {
		if constexpr (std::is_void_v<T>) {
			task(Continuation<T>(continuation, continuation));
		} else {
			task(Continuation<T>([this, continuation](T value) { _Value_Container<T>::Set(value); continuation(); }, continuation));
		}
	}
	T await_resume() { return _Value_Container<T>::Get(); }
};


template<class Func>
TaskAwaitable(Func) -> TaskAwaitable<typename function_traits<Func>::template argument_type<0>::value_type>;


template<class Func>
auto SetTask(Func task) {
	return TaskAwaitable(task);
}


} // namespace ViewDesign
