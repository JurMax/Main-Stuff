################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/redox/RedoxMain.cpp \
../src/redox/RedoxUI.cpp 

OBJS += \
./src/redox/RedoxMain.o \
./src/redox/RedoxUI.o 

CPP_DEPS += \
./src/redox/RedoxMain.d \
./src/redox/RedoxUI.d 


# Each subdirectory must supply rules for building sources it contributes
src/redox/%.o: ../src/redox/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/Library/Frameworks/SDL2.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_image.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_ttf.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_mixer.framework/Versions/A/Headers -I"/Users/jurriaanvandenberg/Documents/Programming/C++/WeeklyGames/Main Project/src/overture/include" -I/System/Library/Frameworks/CoreFoundation.framework/Versions/A/Headers -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


