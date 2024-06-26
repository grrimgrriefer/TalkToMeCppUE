// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "ServerResponseType.h"

/// <summary>
/// Abstract read-only data struct that all responses from the VoxtaServer derive from.
/// Main use for this is to ensure a streamlined public API.
/// </summary>
struct IServerResponseBase
{
public:
	virtual ~IServerResponseBase() = default;

	/// <summary>
	/// Const fuction that fetches the type of Response that the derived type represents.
	/// </summary>
	/// <returns>The type that identifies the derived response.</returns>
	virtual ServerResponseType GetType() const = 0;
};
