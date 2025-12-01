#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "And64InlineHook/And64InlineHook.hpp"
#include "Menu/Setup.h"
#include "AutoHook/AutoHook.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"



//add the Keywords
DWORD  Coin, Gem, Score, TrUp, SetPlayerVelocity, IsInvincible, StartBolt, EndBolt, 
       StartShield, EndShield, RpUp, IsCharacterPurchased, IsPowerPurchased, CanAffordRegion,
	   IsAdRemovalPurchased, OnTriggerEnter, StartZipLine, CalcPickupDistance, GetDoubleCoinStartDistance,
	   GetTripleCoinStartDistance, GetCurrentRank, GetMaxRank, IsAdRemovalPurchasedX;
       

//------------------------------------ Hook Function Field
bool IsInfintieGem = false;
bool IsInfinteCoin = false;
bool IsInfinteScore = false;
bool IsZipLine = false;
float Velocity = 0;

void (*SetInfinteGem)(void *instanceint, int value, void* source);
void (*SetInfinteCoin)(void *instanceint, int value);
void (*SetInfinteScore)(void *instanceint, int value);
void (*SetVelocity)(void *instanceint, float value);
void (*SetZipLine)(void *instanceint);

void(*_UpdateCX)(void *instance);
void UpdateCX(void *instance){
   
	if(instance != NULL){
	  if (IsZipLine) {
            SetZipLine(instance);
			}
	  }
	 
      if(instance != NULL){
        if (IsInfintieGem) {
            SetInfinteGem(instance, 99999999, NULL);
            IsInfintieGem = false;
            }  
      }
	
	if(instance != NULL){
        if (IsInfinteCoin) {
            SetInfinteCoin(instance, 99999999);
		    IsInfinteCoin = false;
			}
        }     
	
	if (instance != NULL){
        if (IsInfinteScore) {
            SetInfinteScore(instance, 99999999);
			IsInfinteScore = false;
        }     
	}
	
	
	if (instance != NULL) {
        if (Velocity > 0) {
            SetVelocity(instance, Velocity);
        }
    }
	
	
	_UpdateCX(instance);
}

bool IsBolt = false;
bool IsShield = false;

void (*GetBolt)(void *instanceint);
void (*GoneBolt)(void *instanceint);

void (*GetShield)(void *instanceint);
void (*GoneShield)(void *instanceint);

void(*_UpdateCY)(void *instance);
void UpdateCY(void *instance){

	    if(instance != NULL){
        
		if (IsBolt) {
            GetBolt(instance);
        } else {
			GoneBolt(instance);
	    }   
		
		if (IsShield) {
            GetShield(instance);
        } else {
			GoneShield(instance);
	    }   
   
	  
	}
	
    _UpdateCY(instance);
}




// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(5);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    
    do {
        sleep(1);
    } while (!isLibraryLoaded("libStarDust.so"));
	
	
	//Here load the classes which one's offsets you want to make Auto Update
	
    //------------------------------------ Auto Update Field
	// THIS FIELD IS WELL WRITEEN By @XtremeModder aka @StardustXdd
	
	/* I Spent an hour on it think that are you have auto update skills 
	 Before Modify This Field */
	 
	auto TRRunnerPawn = new LoadClass(OBFUSCATE(""), OBFUSCATE("TRRunnerPawn")); 
    Gem = TRRunnerPawn->GetMethodOffsetByName(OBFUSCATE("AddGems"), 2);  
	Coin = TRRunnerPawn->GetMethodOffsetByName(OBFUSCATE("AddCoins"), 2);  
	Score = TRRunnerPawn->GetMethodOffsetByName(OBFUSCATE("AddScore"), 1); 
	SetPlayerVelocity = TRRunnerPawn->GetMethodOffsetByName(OBFUSCATE("SetPlayerVelocity"), 2); 
	//OnTriggerEnter = TRRunnerPawn->GetMethodOffsetByName(OBFUSCATE("OnTriggerEnter"), 1);  // No Stumble
	StartZipLine   = TRRunnerPawn->GetMethodOffsetByName(OBFUSCATE("StartZipLine"), 0); 
	CalcPickupDistance   = TRRunnerPawn->GetMethodOffsetByName(OBFUSCATE("CalcPickupDistance"), 1); 
	TrUp = TRRunnerPawn->GetMethodOffsetByName(OBFUSCATE("Update"), 0);  
	
	
	auto RunnerPowers = new LoadClass(OBFUSCATE(""), OBFUSCATE("RunnerPowers")); 
	IsInvincible = RunnerPowers->GetMethodOffsetByName(OBFUSCATE("IsInvincible"), 0);  
	StartBolt = RunnerPowers->GetMethodOffsetByName(OBFUSCATE("StartBolt"), 0);  
	EndBolt = RunnerPowers->GetMethodOffsetByName(OBFUSCATE("EndBolt"), 0);  
	StartShield = RunnerPowers->GetMethodOffsetByName(OBFUSCATE("StartShield"), 0);  
	EndShield = RunnerPowers->GetMethodOffsetByName(OBFUSCATE("EndShield"), 1);  
	RpUp = RunnerPowers->GetMethodOffsetByName(OBFUSCATE("Update"), 0);  
	
	auto PlayerStats = new LoadClass(OBFUSCATE(""), OBFUSCATE("PlayerStats")); 
	IsCharacterPurchased = PlayerStats->GetMethodOffsetByName(OBFUSCATE("IsCharacterPurchased"), 1);
	IsPowerPurchased = PlayerStats->GetMethodOffsetByName(OBFUSCATE("IsPowerPurchased"), 1);
	GetCurrentRank = PlayerStats->GetMethodOffsetByName(OBFUSCATE("GetCurrentRank"), 0);
	GetMaxRank = PlayerStats->GetMethodOffsetByName(OBFUSCATE("GetMaxRank"), 0);
	IsAdRemovalPurchasedX = PlayerStats->GetMethodOffsetByName(OBFUSCATE("IsAdRemovalPurchased"), 0);
	
	
	auto RegionManager = new LoadClass(OBFUSCATE(""), OBFUSCATE("RegionManager")); 
	CanAffordRegion = RegionManager->GetMethodOffsetByName(OBFUSCATE("CanAffordRegion"), 1);
	
	auto TRImangiStoreManager = new LoadClass(OBFUSCATE(""), OBFUSCATE("TRImangiStoreManager")); 
	IsAdRemovalPurchased = TRImangiStoreManager->GetMethodOffsetByName(OBFUSCATE("IsAdRemovalPurchased"), 0);
	IsAdRemovalPurchased = TRImangiStoreManager->GetMethodOffsetByName(OBFUSCATE("IsAdRemovalPurchased"), 0);
	
	auto GameProfile = new LoadClass(OBFUSCATE(""), OBFUSCATE("GameProfile")); 
	GetDoubleCoinStartDistance = GameProfile->GetMethodOffsetByName(OBFUSCATE("GetDoubleCoinStartDistance"), 0);
	GetTripleCoinStartDistance = GameProfile->GetMethodOffsetByName(OBFUSCATE("GetTripleCoinStartDistance"), 0);

//------------------------------------ Hook Function Field
	
	SetInfinteCoin =  (void (*)(void *, int))Coin;
	SetInfinteGem =  (void (*)(void *, int, void*))Gem;
	SetInfinteScore =  (void (*)(void *, int))Score;
	SetVelocity = (void (*)(void *, float))SetPlayerVelocity;
	SetZipLine = (void (*)(void *))StartZipLine;
	HOOK_AU(TrUp, UpdateCX, _UpdateCX);
	
	
	GetBolt = (void (*)(void *))StartBolt;
    GoneBolt = (void (*)(void *))EndBolt;
	GetShield = (void (*)(void *))StartShield;
	GoneShield = (void (*)(void *))EndShield;
	HOOK_AU(RpUp, UpdateCY, _UpdateCY);
	
	
//-------------------------------------------------------------
	
	
	
	//MSHookFunction((void *)Gem, (void *) InfiniteGem, (void **) &old_InfiniteGem);
	
    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__)

#else //To compile this code for armv7 lib only.

    LOGI(OBFUSCATE("Done"));
#endif

    //Anti-leech
    if (!iconValid || !initValid || !settingsValid) {
        //Bad function to make it crash
        sleep(5);
        int *p = 0;
        *p = 0;
    }

    return NULL;
}


jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
		    OBFUSCATE("Category_In Game Hacks"),
			OBFUSCATE("RichTextView_These Hacks Will only work while playing game"),
		    OBFUSCATE("1_Toggle_Add Infinite Gems"),
			OBFUSCATE("2_Toggle_Add Infinite Coin"),
			OBFUSCATE("3_Toggle_Add Infinite Score"),
			OBFUSCATE("7_Toggle_God Mod ( Auto Play )"),
			OBFUSCATE("4_Toggle_Get Bolt"),
			OBFUSCATE("5_Toggle_Get Shield"),
			OBFUSCATE("6_SeekBar_Player Speed_1_200"),
			//OBFUSCATE("8_Toggle_No Stumble"), //Idk
			OBFUSCATE("9_Toggle_Crazy Player*"),
			OBFUSCATE("10_Toggle_Double Coin Value"),
			OBFUSCATE("11_Toggle_Triple Coin Value"),
			OBFUSCATE("12_Toggle_Auto Pickup Far Item"),
			//OBFUSCATE("8_Toggle_Walk On Water"), //Idk
			OBFUSCATE("Category_Store Hack"),
			OBFUSCATE("21_True_Toggle_No Ads*"),
			OBFUSCATE("22_Toggle_Unlock Region"),
			OBFUSCATE("23_Toggle_Unlock Power"),
			OBFUSCATE("24_Toggle_Unlock Character"),
			OBFUSCATE("25_Toggle_Max Player Level"),
			OBFUSCATE("26_Toggle_Max Player Level"),
			OBFUSCATE("RichTextView_* = Unstable"),
			OBFUSCATE("Category_Social"),
			OBFUSCATE("ButtonLink_Telegram_https://t.me/linkuzbio"),
			OBFUSCATE("ButtonLink_Report Bug / Suggestions_https://t.me/linkuzbio"),
    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
		case 1:
			IsInfintieGem = boolean;
			break;
		
		case 2:
			IsInfinteCoin = boolean;
			break;
			
		case 3:
			IsInfinteScore = boolean;
			break;
			
		case 4:
			IsBolt = boolean;
			break;
			
		case 5:
			IsShield = boolean;
			break;
		
		case 6:
			Velocity = value;
			break;
			
		case 7:
		   PATCH_SWITCH_AU(IsInvincible, "01 00 A0 E3 1E FF 2F E1", boolean);
           break;
		/*
		case 8:
		   PATCH_SWITCH_AU(OnTriggerEnter, "01 00 A0 E3 1E FF 2F E1", boolean);
           break;
		*/
		case 9:
			IsZipLine = boolean;
		    break;
			
	    case 10:
			PATCH_SWITCH_AU(GetDoubleCoinStartDistance, "00 00 40 E3 1E FF 2F E1", boolean);
			break;
			
		case 11:
			PATCH_SWITCH_AU(GetTripleCoinStartDistance, "00 00 40 E3 1E FF 2F E1", boolean);
			break;
			
	     case 12:
		    PATCH_SWITCH_AU(CalcPickupDistance, "C8 02 44 E3 1E FF 2F E1", boolean);
			break;
			
		case 21:
			// i am not sure fix this yourself
		   PATCH_SWITCH_AU(IsAdRemovalPurchased, "01 00 A0 E3 1E FF 2F E1", boolean);
		   PATCH_SWITCH_AU(IsAdRemovalPurchasedX, "01 00 A0 E3 1E FF 2F E1", boolean);
           break;
		 
		case 22:
		   PATCH_SWITCH_AU(CanAffordRegion, "01 00 A0 E3 1E FF 2F E1", boolean);
           break;
		   
		case 23:
		   PATCH_SWITCH_AU(IsPowerPurchased, "01 00 A0 E3 1E FF 2F E1", boolean);
           break;
		  
		case 24:
		   PATCH_SWITCH_AU(IsCharacterPurchased, "01 00 A0 E3 1E FF 2F E1", boolean);
           break;
		   
		case 26:
			PATCH_SWITCH_AU(GetCurrentRank, " E7 03 00 E3 1E FF 2F E1", boolean);
			PATCH_SWITCH_AU(GetMaxRank, " E7 03 00 E3 1E FF 2F E1", boolean);
			
            break;
		   
    }
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
            {OBFUSCATE("IsGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
            {OBFUSCATE("Init"),  OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"), reinterpret_cast<void *>(Init)},
            {OBFUSCATE("SettingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
            {OBFUSCATE("GetFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
