﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class ENakamaStoragePermissionRead : uint8
{
	// The object is only readable by server runtime
	NO_READ UMETA(DisplayName = "No Read"),
	// Only the user who owns it may access
	OWNER_READ UMETA(DisplayName = "Owner Read"),
	// Any user can read the object
	PUBLIC_READ UMETA(DisplayName = "Public Read"),

};

UENUM(BlueprintType)
enum class ENakamaStoragePermissionWrite : uint8
{
	// The object is only writable by server runtime
	NO_WRITE UMETA(DisplayName = "No Write"),
	// Only the user who owns it may write
	OWNER_WRITE UMETA(DisplayName = "Owner Write"),

};