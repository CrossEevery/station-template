#include "CIReceiver.h"

ACIReceiver::ACIReceiver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ListenSocket = NULL;
}

void ACIReceiver::Recv(const FArrayReaderPtr & ArrayReaderPtr, const FIPv4Endpoint & EndPt)
{
	if (!&ArrayReaderPtr) {
		UE_LOG(LogTemp, Warning, TEXT("Cannot read array, nullptr returned."));
		return;
	}
	gotNewData = true;

	if (update)
	{
		Archive(ArrayReaderPtr);
	}
}

bool ACIReceiver::StartCIReceiver(const FString & SocketName, const int32 Port)
{
	FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);
	int32 BufferSize = 2 * 1024 * 1024;
	ListenSocket = FUdpSocketBuilder(*SocketName).AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);
	
	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	Receiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("CIReceiver"));
	Receiver->OnDataReceived().BindUObject(this, &ACIReceiver::Recv);
	Receiver->Start();
	return true;
}

FCIData ACIReceiver::GetData()
{
	update = true;	// Ready for new data
	gotNewData = false;
	return data;
}

bool ACIReceiver::IsCIDataReady() {
	return gotNewData;
}

void ACIReceiver::Archive(const FArrayReaderPtr & ArrayReaderPtr) {

	FCIData receivedData;
	*ArrayReaderPtr << receivedData;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d, %d"), receivedData.key, receivedData.value));
	data = receivedData;
}


void ACIReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	delete Receiver;
	Receiver = nullptr;

	//Clear all sockets
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}