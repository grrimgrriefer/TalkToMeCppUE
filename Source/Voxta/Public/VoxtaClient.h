// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Voxta/Private/VoxtaLogUtility.h"
#include "SignalR/Private/HubConnection.h"
#include "Voxta/Private/VoxtaApiRequestHandler.h"
#include "Voxta/Private/VoxtaApiResponseHandler.h"
#include "VoxtaData/Public/AiCharData.h"
#include "VoxtaData/Public/ChatMessage.h"
#include "VoxtaData/Public/ServerResponses/ServerResponseBase.h"
#include "VoxtaData/Public/ServerResponses/ServerResponseWelcome.h"
#include "VoxtaData/Public/ServerResponses/ServerResponseCharacterList.h"
#include "VoxtaData/Public/ChatSession.h"
#include "VoxtaClient.generated.h"

UENUM(BlueprintType)
enum class VoxtaClientState : uint8
{
	Disconnected		UMETA(DisplayName = "Disconnected"),
	Connecting			UMETA(DisplayName = "Connecting"),
	Authenticated		UMETA(DisplayName = "Authenticated"),
	CharacterLobby		UMETA(DisplayName = "CharacterLobby"),
	StartingChat		UMETA(DisplayName = "StartingChat"),
	Chatting			UMETA(DisplayName = "Chatting"),
	Terminated			UMETA(DisplayName = "Terminated")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoxtaClientStateChangedSignature, VoxtaClientState, newState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoxtaClientCharacterLoadedSignature, const FAiCharData&, charData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVoxtaClientCharacterMessageReceivedSignature, const FCharDataBase&, sender, const FChatMessage&, message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoxtaClientCharacterMessageRemovedSignature, const FChatMessage&, message);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VOXTA_API UVoxtaClient : public UActorComponent
{
	GENERATED_BODY()

public:
	UVoxtaClient();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FVoxtaClientStateChangedSignature OnVoxtaClientStateChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FVoxtaClientCharacterLoadedSignature OnVoxtaClientCharacterLoadedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FVoxtaClientCharacterMessageReceivedSignature OnVoxtaClientChatMessageAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FVoxtaClientCharacterMessageRemovedSignature OnVoxtaClientChatMessageRemoved;

	void StartConnection();
	void Disconnect();

	UFUNCTION()
	void LoadCharacter(FString charId);

	UFUNCTION()
	void SendUserInput(FString inputText);

private:
	VoxtaLogUtility m_logUtility;
	TSharedPtr<IHubConnection> m_hub;
	VoxtaApiRequestHandler m_voxtaRequestApi;
	VoxtaApiResponseHandler m_voxtaResponseApi;

	VoxtaClientState m_currentState = VoxtaClientState::Disconnected;
	TUniquePtr<FUserCharData> m_userData;
	TArray<TUniquePtr<const FAiCharData>> m_characterList;
	TUniquePtr<ChatSession> m_chatSession;

	const FString m_sendMessageEventName = TEXT("SendMessage");
	const FString m_receiveMessageEventName = TEXT("ReceiveMessage");

	void StartListeningToServer();

	void OnReceivedMessage(const TArray<FSignalRValue>& Arguments);
	void OnConnected();
	void OnConnectionError(const FString& error);
	void OnClosed();

	void SendMessageToServer(const FSignalRValue& message);
	void OnMessageSent(const FSignalRInvokeResult& result);

	bool HandleResponse(const TMap<FString, FSignalRValue>& responseData);
	void HandleWelcomeResponse(const ServerResponseWelcome& response);
	void HandleCharacterListResponse(const ServerResponseCharacterList& response);
	bool HandleCharacterLoadedResponse(const ServerResponseCharacterLoaded& response);
	bool HandleChatStartedResponse(const ServerResponseChatStarted& response);
	void HandleChatMessageResponse(const ServerResponseChatMessage& response);
	void HandleChatUpdateResponse(const ServerResponseChatUpdate& response);

	void SetState(VoxtaClientState newState);
};
