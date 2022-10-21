#pragma once

#include "GameFramework/Actor.h"
#include "Networking.h"
#include "Serialization/Archive.h"
#include "CIReceiver.generated.h"

USTRUCT(BlueprintType)
struct FCIData {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteractionReceiver")
		int key = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteractionReceiver")
		int value = 0;
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, FCIData& Structure)
{	
	Ar << Structure.key;
	Ar << Structure.value;
	return Ar;
}

UCLASS()
class CROSSINTERACTIONRECEIVER_API ACIReceiver : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	FCIData data;
	bool update = true;
	bool gotNewData = false;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "CrossInteractionReceiver")
		void BPEvent_DataReceived();
public:
	
	FSocket* ListenSocket;
	FUdpSocketReceiver* Receiver = nullptr;
	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);
	virtual void Archive(const FArrayReaderPtr & ArrayReaderPtr);
	
	UFUNCTION(BlueprintCallable, Category = "CrossInteractionReceiver")
	 	FCIData GetData();
		
	UFUNCTION(BlueprintCallable, Category = "CrossInteractionReceiver")
	 	bool IsCIDataReady();
	
	UFUNCTION(BlueprintCallable, Category = "CrossInteractionReceiver")
		bool StartCIReceiver(const FString& SocketName,
			const int32 Port);

public:

	// /** Called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};