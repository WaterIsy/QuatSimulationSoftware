// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Networking/Public/Networking.h"
#include "ReadQuaternionFromSocket.generated.h"

UCLASS()
class QUATSIMULATION_API AReadQuaternionFromSocket : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AReadQuaternionFromSocket();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called every frame
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	//四元数右手系转左手系
	FQuat QuatRHandToLHand(float x, float y, float z, float w);


	//Socket模块
	UFUNCTION(BlueprintCallable, Category = "MySocket")
		bool SocketCreate(FString IPStr, int32 port);

	UFUNCTION(BlueprintCallable, Category = "MySocket")
		void SocketSend(FString meesage);

	UFUNCTION(BlueprintCallable, Category = "MySocket")
		void SocketReceive(bool& bReceive, FString& recvMessage);

	//接收数据暴露参
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySocket")
		FString recvData;
	//IPstr暴露参
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySocket")
		FString IPstr;
	//Port暴露参
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySocket")
		int Port;

	//组件暴露参
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat_Component")
		AActor* Actors;

	//组件root
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat_Component")
		UStaticMeshComponent* QuatRootComponent;

	//四元数暴露参

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat_Component")
		float Quat_X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat_Component")
		float Quat_Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat_Component")
		float Quat_Z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat_Component")
		float Quat_W;

	//四元数旋转体
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat_Component")
		FRotator RotatorFromQuat;

	//数组切片
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyArray")
		TArray<FString> LevelArr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat_Component")
		FString Quat_Str;

	FString StringFromBinaryArray(TArray<uint8> BinaryArray);

	FSocket* SocketClient;

	FIPv4Address ip;

};
