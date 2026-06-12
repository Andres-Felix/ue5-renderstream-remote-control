/* ===========================
   UE5 Remote Control Receiver
   C++ Backend for Blueprint Integration
   =========================== */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "RemoteControlReceiver.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSunRotationChanged, float, Rotation, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSunIntensityChanged, float, Intensity, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDayTimeChanged, float, Hours, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSunPositionChanged, FVector, Position, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSceneChanged, FString, SceneName, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialColorChanged, FString, MaterialName, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_One_Param(FOnQuickActionExecuted, FString, ActionName);

/**
 * Remote Control Receiver for UE5
 * Receives HTTP commands from the web control panel
 * Broadcasts events to Blueprint for scene control
 */
UCLASS()
class YOURPROJECT_API ARemoteControlReceiver : public AActor
{
    GENERATED_BODY()

public:
    ARemoteControlReceiver();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;

    // ===========================
    // HTTP Server Setup
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Remote Control")
    void StartHTTPServer(int32 Port = 8080);

    UFUNCTION(BlueprintCallable, Category = "Remote Control")
    void StopHTTPServer();

    // ===========================
    // Event Dispatchers
    // ===========================

    UPROPERTY(BlueprintAssignable, Category = "Remote Control|Events")
    FOnSunRotationChanged OnSunRotationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Remote Control|Events")
    FOnSunIntensityChanged OnSunIntensityChanged;

    UPROPERTY(BlueprintAssignable, Category = "Remote Control|Events")
    FOnDayTimeChanged OnDayTimeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Remote Control|Events")
    FOnSunPositionChanged OnSunPositionChanged;

    UPROPERTY(BlueprintAssignable, Category = "Remote Control|Events")
    FOnSceneChanged OnSceneChanged;

    UPROPERTY(BlueprintAssignable, Category = "Remote Control|Events")
    FOnMaterialColorChanged OnMaterialColorChanged;

    UPROPERTY(BlueprintAssignable, Category = "Remote Control|Events")
    FOnQuickActionExecuted OnQuickActionExecuted;

    // ===========================
    // Configuration
    // ===========================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Control|Config")
    int32 HTTPServerPort = 8080;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Control|Config")
    bool bEnableLogging = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Control|Config")
    FVector DefaultSunDirection = FVector(1.0f, 0.0f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Control|Config")
    float MaxSunIntensity = 2.0f;

    // ===========================
    // Current State
    // ===========================

    UPROPERTY(BlueprintReadOnly, Category = "Remote Control|State")
    float CurrentSunRotation = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Remote Control|State")
    float CurrentSunIntensity = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Remote Control|State")
    float CurrentDayTime = 12.0f; // 0-24 hours

    UPROPERTY(BlueprintReadOnly, Category = "Remote Control|State")
    FVector CurrentSunPosition = FVector(0.0f, 0.0f, 0.0f);

    UPROPERTY(BlueprintReadOnly, Category = "Remote Control|State")
    FString CurrentScene = "default";

    // ===========================
    // Blueprint Functions
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Remote Control|Control")
    void SetSunRotation(float Rotation);

    UFUNCTION(BlueprintCallable, Category = "Remote Control|Control")
    void SetSunIntensity(float Intensity);

    UFUNCTION(BlueprintCallable, Category = "Remote Control|Control")
    void SetDayTime(float Hours);

    UFUNCTION(BlueprintCallable, Category = "Remote Control|Control")
    void SetSunPosition(FVector Position);

    UFUNCTION(BlueprintCallable, Category = "Remote Control|Control")
    void ChangeScene(FString SceneName);

    UFUNCTION(BlueprintCallable, Category = "Remote Control|Control")
    void SetMaterialColor(FString MaterialName, FLinearColor Color);

    UFUNCTION(BlueprintCallable, Category = "Remote Control|Control")
    void ExecuteQuickAction(FString ActionName);

    // ===========================
    // Helper Functions
    // ===========================

    UFUNCTION(BlueprintCallable, Category = "Remote Control|Utils")
    FString GetCurrentState() const;

private:
    // HTTP Request Handler
    void HandleHTTPRequest(const FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bWasSuccessful);
    void ProcessCommand(const FString& Command, const TSharedPtr<FJsonObject>& JsonObject);

    // JSON Helpers
    FLinearColor HexToColor(const FString& HexColor) const;

    // Server reference
    FHttpServerPtr HTTPServer;
    bool bServerRunning = false;

    // Logging helper
    void Log(const FString& Message) const;
};
