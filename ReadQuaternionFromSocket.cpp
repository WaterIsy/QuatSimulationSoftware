// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadQuaternionFromSocket.h"
#include "Engine\Engine.h"
#include "Runtime\Networking\Public\Networking.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Math/Vector.h"



// Sets default values

AReadQuaternionFromSocket::AReadQuaternionFromSocket()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SocketClient = NULL;


}

// Called when the game starts or when spawned
void AReadQuaternionFromSocket::BeginPlay()
{
	Super::BeginPlay();

	//初始化四元数为0
	Quat_X = 0;
	Quat_Y = 0;
	Quat_Z = 0;
	Quat_W = 0;
}

void AReadQuaternionFromSocket::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (SocketClient)
	{
		//关闭，销毁
		SocketClient->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SocketClient);
	}
}

// Called every frame
void AReadQuaternionFromSocket::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	//读取传感器的四元数，构造引擎的四元数体
	FQuat getQuat = QuatRHandToLHand(Quat_X, Quat_Y, Quat_Z, Quat_W);

	//四元数体->旋转体
	RotatorFromQuat = FRotator(getQuat);

	TArray<AActor*> Actors;
	TArray<UStaticMeshComponent*>Component;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("root"), Actors);
	for (AActor* Actor : Actors)
	{
		Actor->GetComponents(Component);
		for (int32 b = 0; b < Component.Num(); b++)
		{
			UStaticMeshComponent* a = Component[b];
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *(a->GetName()));

			//将四元数体赋值给组件根部旋转
			QuatRootComponent->SetRelativeRotation(RotatorFromQuat);

		}
	}
}

bool AReadQuaternionFromSocket::SocketCreate(FString IPStr, int32 port)
{
	FIPv4Address::Parse(IPStr, ip);

	IPStr = IPstr;
	port = Port;

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);//ip地址
	addr->SetPort(port);//端口

	SocketClient = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	if (SocketClient->Connect(*addr))
	{
		//在控制台打印连接成功
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("Server Connect Succeed!"));
		UE_LOG(LogTemp, Warning, TEXT("Server Connect Succeed!"));
		return true;
	}
	else
	{
		//在控制台打印连接失败
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("Server Connect Failed!"));
		UE_LOG(LogTemp, Warning, TEXT("Server Connect Failed!"));
		return false;
	}

}

//Socket发送逻辑
void AReadQuaternionFromSocket::SocketSend(FString meesage)
{


	TCHAR* seriallizedChar = meesage.GetCharArray().GetData();
	int32 size = FCString::Strlen(seriallizedChar) + 1;
	int32 sent = 0;

	if (SocketClient->Send((uint8*)TCHAR_TO_UTF8(seriallizedChar), size, sent))
	{
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("_Send Succeed!"));
		UE_LOG(LogTemp, Warning, TEXT("_Send Succeed!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("_Send Failed!"));
		UE_LOG(LogTemp, Warning, TEXT("_Send Failed!"));
	}
}

//Socket接收逻辑
void AReadQuaternionFromSocket::SocketReceive(bool& bReceive, FString& recvMessage)
{
	//留空
	recvMessage = "";
	bReceive = false;

	if (!SocketClient)
	{
		return;
	}

	TArray<uint8> ReceiveData;
	uint32 size;
	uint8 element = 0;
	//当连接，则开始接收数据
	while (SocketClient->HasPendingData(size))
	{
		//65507u
		ReceiveData.Init(element, FMath::Min(size, 65507u));
		int32 read = 0;
		SocketClient->Recv(ReceiveData.GetData(), ReceiveData.Num(), read);
	}


	if (ReceiveData.Num() <= 0)
	{
		return;
	}

	FString log = "Total Data read! num: " + FString::FromInt(ReceiveData.Num() <= 0);

	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, log);
	UE_LOG(LogTemp, Warning, TEXT("Recv log:   %s"), *log);

	//转换为Fstring格式 
	const FString ReceivedUE4String = StringFromBinaryArray(ReceiveData);

	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, log);

	UE_LOG(LogTemp, Warning, TEXT("*** %s"), *log);

	recvMessage = ReceivedUE4String;

	FString level = recvMessage;

	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, recvMessage);

	bReceive = true;

	recvData = recvMessage;

	//将字符串按"end"拆分为数组
	level.ParseIntoArray(LevelArr, TEXT("end"), false);
	

	//数组分类给到四元数暴露参
	Quat_X = FCString::Atof(*LevelArr[0]);
	Quat_Y = FCString::Atof(*LevelArr[1]);
	Quat_Z = FCString::Atof(*LevelArr[2]);
	Quat_W = FCString::Atof(*LevelArr[3]);


}



FString AReadQuaternionFromSocket::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}




//四元数右手系转左手系的方法
FQuat AReadQuaternionFromSocket::QuatRHandToLHand(float x, float y, float z, float w) {
	FQuat Quat = FQuat();

	Quat.X = -x;
	Quat.Y = -y;
	Quat.Z = z;
	Quat.W = w;

	return Quat;

}







