/*
 * MIT License
 *
 * Copyright (c) 2020-2022 Frozen Storm Interactive, Yoann Potinet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "CoreMinimal.h"
#include "SignalRValue.h"

class FSignalRInvokeResult : public FSignalRValue
{
	friend struct FSignalRInvokeResultWrapper;
public:
	FSignalRInvokeResult(const FSignalRValue& Value) : FSignalRValue(Value)
	{}

	/**
	 * Copies an existing result.
	 */
	FSignalRInvokeResult(const FSignalRInvokeResult& OtherValue) : FSignalRValue(OtherValue)
	{
		bError = OtherValue.bError;
		ErrorMessage = OtherValue.ErrorMessage;
	}

	/**
	 * Moves an existing result.
	 */
	FSignalRInvokeResult(FSignalRInvokeResult&& OtherValue) noexcept : FSignalRValue(MoveTemp(OtherValue))
	{
		bError = MoveTemp(OtherValue.bError);
		ErrorMessage = MoveTemp(OtherValue.ErrorMessage);
	}

	/**
	 * Copies an existing result.
	 */
	FSignalRInvokeResult& operator=(const FSignalRInvokeResult& OtherValue)
	{
		FSignalRValue::operator=(OtherValue);
		bError = OtherValue.bError;
		ErrorMessage = OtherValue.ErrorMessage;
		return *this;
	}

	/**
	 * Moves an existing result.
	 */
	FSignalRInvokeResult& operator=(FSignalRInvokeResult&& OtherValue) noexcept
	{
		FSignalRValue::operator=(MoveTemp(OtherValue));
		bError = MoveTemp(OtherValue.bError);
		ErrorMessage = MoveTemp(OtherValue.ErrorMessage);
		return *this;
	}

	FORCEINLINE bool HasError() const
	{
		return bError;
	}

	FORCEINLINE const FString& GetErrorMessage() const
	{
		return ErrorMessage;
	}

	FORCEINLINE static FSignalRInvokeResult Error(const FString& ErrorMessage)
	{
		FSignalRInvokeResult Result;
		Result.bError = true;
		Result.ErrorMessage = ErrorMessage;
		return Result;
	}

private:
	FSignalRInvokeResult() = default;

	bool bError = false;
	FString ErrorMessage;
};

class IHubConnection : public TSharedFromThis<IHubConnection>
{
public:
	virtual void Start() = 0;
	virtual void Stop() = 0;

	/**
	 * Delegate called when a connection has been established successfully.
	 */
	DECLARE_EVENT(IHubConnection, FOnHubConnectedEvent);
	virtual FOnHubConnectedEvent& OnConnected() = 0;

	/**
	 * Delegate called when a connection could not be established.
	 */
	DECLARE_EVENT_OneParam(IHubConnection, FOnHubConnectionErrorEvent, const FString& /* Error */);
	virtual FOnHubConnectionErrorEvent& OnConnectionError() = 0;

	/**
	 * Delegate called when a web socket connection has been closed.
	 *
	 */
	DECLARE_EVENT(IHubConnection, FHubConnectionClosedEvent);
	virtual FHubConnectionClosedEvent& OnClosed() = 0;

	DECLARE_DELEGATE_OneParam(FOnMethodInvocation, const TArray<FSignalRValue>&);
	virtual FOnMethodInvocation& On(const FString& EventName) = 0;

	DECLARE_DELEGATE_OneParam(FOnMethodCompletion, const FSignalRInvokeResult&);
	virtual FOnMethodCompletion& Invoke(const FString& EventName, const TArray<FSignalRValue>& InArguments = TArray<FSignalRValue>()) = 0;

	template <typename... ArgTypes>
	FORCEINLINE FOnMethodCompletion& Invoke(const FString& EventName, ArgTypes... Arguments)
	{
		static_assert(TAnd<TIsConstructible<FSignalRValue, ArgTypes>...>::Value, "Invalid argument type passed to IHubConnection::Invoke");
		return Invoke(EventName, TArray<FSignalRValue> { MoveTemp(Arguments)... });
	}

	virtual void Send(const FString& EventName, const TArray<FSignalRValue>& InArguments = TArray<FSignalRValue>()) = 0;

	template <typename... ArgTypes>
	FORCEINLINE void Send(const FString& EventName, ArgTypes... Arguments)
	{
		static_assert(TAnd<TIsConstructible<FSignalRValue, ArgTypes>...>::Value, "Invalid argument type passed to IHubConnection::Send");
		Send(EventName, TArray<FSignalRValue> { MoveTemp(Arguments)... });
	}

protected:

	/**
	 * Destructor
	 */
	virtual ~IHubConnection();
};

typedef TSharedPtr<IHubConnection> IHubConnectionPtr;
