################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/overture/src/JAudio.cpp \
../src/overture/src/JBezier.cpp \
../src/overture/src/JDebug.cpp \
../src/overture/src/JInput.cpp \
../src/overture/src/JOverture.cpp \
../src/overture/src/JProfiler.cpp \
../src/overture/src/JRenderer.cpp \
../src/overture/src/JResources.cpp \
../src/overture/src/JSettings.cpp \
../src/overture/src/JUI.cpp 

OBJS += \
./src/overture/src/JAudio.o \
./src/overture/src/JBezier.o \
./src/overture/src/JDebug.o \
./src/overture/src/JInput.o \
./src/overture/src/JOverture.o \
./src/overture/src/JProfiler.o \
./src/overture/src/JRenderer.o \
./src/overture/src/JResources.o \
./src/overture/src/JSettings.o \
./src/overture/src/JUI.o 

CPP_DEPS += \
./src/overture/src/JAudio.d \
./src/overture/src/JBezier.d \
./src/overture/src/JDebug.d \
./src/overture/src/JInput.d \
./src/overture/src/JOverture.d \
./src/overture/src/JProfiler.d \
./src/overture/src/JRenderer.d \
./src/overture/src/JResources.d \
./src/overture/src/JSettings.d \
./src/overture/src/JUI.d 


# Each subdirectory must supply rules for building sources it contributes
src/overture/src/%.o: ../src/overture/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/Library/Frameworks/SDL2.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_image.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_ttf.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_mixer.framework/Versions/A/Headers -I"/Users/jurriaanvandenberg/Documents/Programming/C++/WeeklyGames/Main Project/src/overture/include" -I/System/Library/Frameworks/CoreFoundation.framework/Versions/A/Headers -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


