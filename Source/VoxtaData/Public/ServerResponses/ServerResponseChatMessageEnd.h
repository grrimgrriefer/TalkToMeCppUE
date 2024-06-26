// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "ServerResponseChatMessageBase.h"

/// <summary>
/// Read-only data struct containing the relevant data of the 'replyEnd' response
/// from the VoxtaServer.
/// </summary>
struct ServerResponseChatMessageEnd : public IServerResponseChatMessageBase
{
public:
	const FString m_senderId;

	explicit ServerResponseChatMessageEnd(FStringView messageId,
			FStringView senderId,
			FStringView sessionId) :
		IServerResponseChatMessageBase(messageId, sessionId),
		m_senderId(senderId)
	{
	}

	/// <summary>
	/// Identifies the response type as MessageEnd.
	/// </summary>
	/// <returns>Returns MessageType::MessageEnd.</returns>
	MessageType GetMessageType() const final
	{
		return MessageType::MessageEnd;
	}
};