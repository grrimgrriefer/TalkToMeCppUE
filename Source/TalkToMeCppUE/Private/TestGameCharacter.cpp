// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "TestGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

ATestGameCharacter::ATestGameCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	m_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_voxtaClient = CreateDefaultSubobject<UVoxtaClient>(TEXT("VoxtaClient"));
	m_audioPlaybackHandler = CreateDefaultSubobject<UVoxtaAudioPlayback>(TEXT("AudioPlayback"));
	m_hud = nullptr;
}

void ATestGameCharacter::StartVoxtaClient()
{
	m_voxtaClient->StartConnection();
}

void ATestGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	m_voxtaClient->VoxtaClientStateChangedEvent.AddDynamic(this, &ATestGameCharacter::VoxtaClientStateChanged);
	m_hud = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<ATalkToMeCppUeHUD>();
	if (!TryConnectToHud())
	{
		UE_LOGFMT(LogCore, Error, "Failed to connect the ATestGameCharacter to the ATalkToMeCppUeHUD.");
	}
}

void ATestGameCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	m_voxtaClient->VoxtaClientStateChangedEvent.RemoveDynamic(this, &ATestGameCharacter::VoxtaClientStateChanged);
	if (!TryDisconnectToHud() && EndPlayReason != EEndPlayReason::Quit)
	{
		UE_LOGFMT(LogCore, Error, "Failed to clean up the bindings between the ATestGameCharacter and the ATalkToMeCppUeHUD.");
	}
	m_hud = nullptr;
}

void ATestGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("StartVoxta"), IE_Pressed, this, &ATestGameCharacter::StartVoxtaClient);
}

bool ATestGameCharacter::TryConnectToHud()
{
	if (m_voxtaClient && m_hud)
	{
		m_voxtaClient->VoxtaClientStateChangedEvent.AddDynamic(m_hud, &ATalkToMeCppUeHUD::VoxtaClientStateChanged);
		m_voxtaClient->VoxtaClientCharacterRegisteredEvent.AddUniqueDynamic(m_hud, &ATalkToMeCppUeHUD::VoxtaClientCharacterRegistered);
		m_voxtaClient->VoxtaClientCharMessageAddedEvent.AddUniqueDynamic(m_hud, &ATalkToMeCppUeHUD::AddTextMessage);
		m_voxtaClient->VoxtaClientCharMessageRemovedEvent.AddUniqueDynamic(m_hud, &ATalkToMeCppUeHUD::RemoveTextMessage);

		m_hud->CharButtonClickedEvent.AddUniqueDynamic(m_voxtaClient, &UVoxtaClient::StartChatWithCharacter);
		m_hud->UserInputCommittedEvent.AddUniqueDynamic(m_voxtaClient, &UVoxtaClient::SendUserInput);

		return true;
	}
	UE_LOGFMT(LogCore, Error, "Failed to connect the HUD and the VoxtaClient with each other.");
	return false;
}

bool ATestGameCharacter::TryDisconnectToHud()
{
	if (m_voxtaClient && m_hud)
	{
		m_voxtaClient->VoxtaClientStateChangedEvent.RemoveDynamic(m_hud, &ATalkToMeCppUeHUD::VoxtaClientStateChanged);
		m_voxtaClient->VoxtaClientCharacterRegisteredEvent.RemoveDynamic(m_hud, &ATalkToMeCppUeHUD::VoxtaClientCharacterRegistered);
		m_voxtaClient->VoxtaClientCharMessageAddedEvent.RemoveDynamic(m_hud, &ATalkToMeCppUeHUD::AddTextMessage);
		m_voxtaClient->VoxtaClientCharMessageRemovedEvent.RemoveDynamic(m_hud, &ATalkToMeCppUeHUD::RemoveTextMessage);

		m_hud->CharButtonClickedEvent.RemoveDynamic(m_voxtaClient, &UVoxtaClient::StartChatWithCharacter);
		m_hud->UserInputCommittedEvent.RemoveDynamic(m_voxtaClient, &UVoxtaClient::SendUserInput);
		return true;
	}
	UE_LOGFMT(LogCore, Warning, "Failed to disconnect the HUD and the VoxtaClient with each other.");
	return false;
}

void ATestGameCharacter::VoxtaClientStateChanged(VoxtaClientState newState)
{
	if (newState == VoxtaClientState::Chatting)
	{
		for (const FAiCharData* aiCharacter : m_voxtaClient->GetChatSession()->m_characters)
		{
			//FString componentName = FString::Format(*FString(TEXT("AudioPlayback {0}")), { aiCharacter->GetName() });
			//m_audioPlaybackHandler = NewObject<UVoxtaAudioPlayback>(this, *componentName);
			m_audioPlaybackHandler->InitializeAudioPlayback(m_voxtaClient, aiCharacter->GetId());
		}
	}
}