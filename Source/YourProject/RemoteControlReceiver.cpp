/* ===========================
   UE5 Remote Control Receiver
   C++ Implementation
   =========================== */

#include "RemoteControlReceiver.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "JsonObjectConverter.h"
#include "Misc/DefaultValueHelper.h"

ARemoteControlReceiver::ARemoteControlReceiver()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.016f; // 60 FPS

    // Default settings
    bEnableLogging = true;
}

// ===========================
// Actor Lifecycle
// ===========================

void ARemoteControlReceiver::BeginPlay()
{
    Super::BeginPlay();

    Log(FString::Printf(TEXT("Remote Control Receiver initializing on port %d"), HTTPServerPort));
    StartHTTPServer(HTTPServerPort);

    // Broadcast initial state
    OnSunRotationChanged.Broadcast(CurrentSunRotation, true);
    OnSunIntensityChanged.Broadcast(CurrentSunIntensity, true);
    OnDayTimeChanged.Broadcast(CurrentDayTime, true);
    OnSunPositionChanged.Broadcast(CurrentSunPosition, true);
}

void ARemoteControlReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopHTTPServer();
    Super::EndPlay(EndPlayReason);
}

void ARemoteControlReceiver::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Process any pending updates
    // This can be extended for smooth transitions
}

// ===========================
// HTTP Server
// ===========================

void ARemoteControlReceiver::StartHTTPServer(int32 Port)
{
    if (bServerRunning)
    {
        Log("HTTP Server already running!");
        return;
    }

    FHttpModule& HttpModule = FHttpModule::Get();
    
    // Create HTTP request handler
    auto OnHTTPRequest = [this](const FHttpServerPtr InServer, const FHttpRequestPtr InRequest)
    {
        if (!InRequest.IsValid())
            return true;

        FString Path = InRequest->GetPathAndQuery();
        Log(FString::Printf(TEXT("HTTP Request: %s"), *Path));

        // Parse JSON body
        FString Body = InRequest->GetBody();
        TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            FString Command = JsonObject->GetStringField("command");
            ProcessCommand(Command, JsonObject);

            // Send success response
            auto Response = FHttpServerResponse::Create();
            Response->SetCode(EHttpServerResponseCodes::Ok);
            Response->SetContent(TEXT("{\"status\": \"success\"}"));
            InServer->SendResponse(InRequest, Response);
        }
        else
        {
            // Send error response
            auto Response = FHttpServerResponse::Create();
            Response->SetCode(EHttpServerResponseCodes::BadRequest);
            Response->SetContent(TEXT("{\"status\": \"error\", \"message\": \"Invalid JSON\"}"));
            InServer->SendResponse(InRequest, Response);
        }

        return true;
    };

    // Start server on specified port
    HTTPServer = FHttpServerModule::Get().StartServer();
    
    if (HTTPServer.IsValid())
    {
        FString ListenURL = FString::Printf(TEXT("0.0.0.0:%d"), Port);
        HTTPServer->Bind(ListenURL, TEXT("/control"), FHttpRequestHandler::CreateLambda(OnHTTPRequest));
        bServerRunning = true;
        Log(FString::Printf(TEXT("HTTP Server started on port %d"), Port));
    }
    else
    {
        Log("Failed to start HTTP Server!");
    }
}

void ARemoteControlReceiver::StopHTTPServer()
{
    if (HTTPServer.IsValid())
    {
        FHttpServerModule::Get().StopServer(HTTPServer);
        bServerRunning = false;
        Log("HTTP Server stopped");
    }
}

// ===========================
// Command Processing
// ===========================

void ARemoteControlReceiver::ProcessCommand(const FString& Command, const TSharedPtr<FJsonObject>& JsonObject)
{
    if (!JsonObject.IsValid())
        return;

    Log(FString::Printf(TEXT("Processing command: %s"), *Command));

    if (Command == "sun-rotation")
    {
        float Rotation = JsonObject->GetNumberField("value");
        SetSunRotation(Rotation);
    }
    else if (Command == "sun-intensity")
    {
        float Intensity = JsonObject->GetNumberField("value");
        SetSunIntensity(Intensity);
    }
    else if (Command == "day-time")
    {
        float Hours = JsonObject->GetNumberField("value");
        SetDayTime(Hours);
    }
    else if (Command == "sun-position-x" || Command == "sun-position-y" || Command == "sun-position-z")
    {
        float Value = JsonObject->GetNumberField("value");
        FVector NewPosition = CurrentSunPosition;

        if (Command == "sun-position-x") NewPosition.X = Value;
        else if (Command == "sun-position-y") NewPosition.Y = Value;
        else if (Command == "sun-position-z") NewPosition.Z = Value;

        SetSunPosition(NewPosition);
    }
    else if (Command == "scene-change")
    {
        FString SceneName = JsonObject->GetStringField("value");
        ChangeScene(SceneName);
    }
    else if (Command == "mesh-color")
    {
        FString MaterialName = JsonObject->GetStringField("meshName");
        FString HexColor = JsonObject->GetStringField("color");
        FLinearColor Color = HexToColor(HexColor);
        SetMaterialColor(MaterialName, Color);
    }
    else if (Command == "quick-action")
    {
        FString ActionName = JsonObject->GetStringField("value");
        ExecuteQuickAction(ActionName);
    }
}

// ===========================
// Control Functions
// ===========================

void ARemoteControlReceiver::SetSunRotation(float Rotation)
{
    CurrentSunRotation = FMath::Clamp(Rotation, -360.0f, 360.0f);
    OnSunRotationChanged.Broadcast(CurrentSunRotation, true);
    Log(FString::Printf(TEXT("Sun Rotation: %.2f°"), CurrentSunRotation));
}

void ARemoteControlReceiver::SetSunIntensity(float Intensity)
{
    CurrentSunIntensity = FMath::Clamp(Intensity, 0.0f, MaxSunIntensity);
    OnSunIntensityChanged.Broadcast(CurrentSunIntensity, true);
    Log(FString::Printf(TEXT("Sun Intensity: %.2f"), CurrentSunIntensity));
}

void ARemoteControlReceiver::SetDayTime(float Hours)
{
    CurrentDayTime = FMath::Clamp(Hours, 0.0f, 24.0f);
    OnDayTimeChanged.Broadcast(CurrentDayTime, true);
    
    int32 Hour = (int32)CurrentDayTime;
    int32 Minute = (int32)((CurrentDayTime - Hour) * 60.0f);
    Log(FString::Printf(TEXT("Day Time: %02d:%02d"), Hour, Minute));
}

void ARemoteControlReceiver::SetSunPosition(FVector Position)
{
    CurrentSunPosition = Position;
    OnSunPositionChanged.Broadcast(CurrentSunPosition, true);
    Log(FString::Printf(TEXT("Sun Position: X=%.2f, Y=%.2f, Z=%.2f"), 
        CurrentSunPosition.X, CurrentSunPosition.Y, CurrentSunPosition.Z));
}

void ARemoteControlReceiver::ChangeScene(FString SceneName)
{
    CurrentScene = SceneName;
    OnSceneChanged.Broadcast(CurrentScene, true);
    Log(FString::Printf(TEXT("Scene Changed: %s"), *CurrentScene));
}

void ARemoteControlReceiver::SetMaterialColor(FString MaterialName, FLinearColor Color)
{
    OnMaterialColorChanged.Broadcast(MaterialName, true);
    Log(FString::Printf(TEXT("Material %s Color: R=%.2f, G=%.2f, B=%.2f, A=%.2f"), 
        *MaterialName, Color.R, Color.G, Color.B, Color.A));
}

void ARemoteControlReceiver::ExecuteQuickAction(FString ActionName)
{
    if (ActionName == "reset")
    {
        SetSunRotation(0.0f);
        SetSunIntensity(1.0f);
        SetDayTime(12.0f);
        SetSunPosition(FVector(0.0f, 0.0f, 0.0f));
    }
    else if (ActionName == "sunrise")
    {
        SetDayTime(6.0f);
    }
    else if (ActionName == "sunset")
    {
        SetDayTime(18.0f);
    }
    else if (ActionName == "noon")
    {
        SetDayTime(12.0f);
    }

    OnQuickActionExecuted.Broadcast(ActionName);
    Log(FString::Printf(TEXT("Quick Action Executed: %s"), *ActionName));
}

// ===========================
// Utility Functions
// ===========================

FLinearColor ARemoteControlReceiver::HexToColor(const FString& HexColor) const
{
    FString Hex = HexColor;
    
    // Remove '#' if present
    if (Hex.StartsWith(TEXT("#")))
    {
        Hex = Hex.RightChop(1);
    }

    // Convert hex string to integer
    uint32 ColorValue = 0;
    if (Hex.Len() == 6)
    {
        sscanf_s(TCHAR_TO_ANSI(*Hex), "%x", &ColorValue);
    }

    // Extract RGB components
    uint8 R = (ColorValue >> 16) & 0xFF;
    uint8 G = (ColorValue >> 8) & 0xFF;
    uint8 B = ColorValue & 0xFF;

    return FLinearColor(R / 255.0f, G / 255.0f, B / 255.0f, 1.0f);
}

FString ARemoteControlReceiver::GetCurrentState() const
{
    FString StateJSON = FString::Printf(
        TEXT("{\"sunRotation\": %.2f, \"sunIntensity\": %.2f, \"dayTime\": %.2f, "
             "\"sunPosition\": {\"x\": %.2f, \"y\": %.2f, \"z\": %.2f}, \"scene\": \"%s\"}"),
        CurrentSunRotation,
        CurrentSunIntensity,
        CurrentDayTime,
        CurrentSunPosition.X,
        CurrentSunPosition.Y,
        CurrentSunPosition.Z,
        *CurrentScene
    );

    return StateJSON;
}

void ARemoteControlReceiver::Log(const FString& Message) const
{
    if (bEnableLogging)
    {
        UE_LOG(LogTemp, Warning, TEXT("[RemoteControl] %s"), *Message);
    }
}
