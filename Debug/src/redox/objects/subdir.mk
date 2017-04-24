################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/redox/objects/container.cpp \
../src/redox/objects/saltbridge.cpp 

OBJS += \
./src/redox/objects/container.o \
./src/redox/objects/saltbridge.o 

CPP_DEPS += \
./src/redox/objects/container.d \
./src/redox/objects/saltbridge.d 


# Each subdirectory must supply rules for building sources it contributes
src/redox/objects/%.o: ../src/redox/objects/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/Library/Frameworks/SDL2.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_image.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_ttf.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_mixer.framework/Versions/A/Headers -I"/Users/jurriaanvandenberg/Documents/Programming/C++/WeeklyGames/Main Project/src/overture/include" -I/System/Library/Frameworks/CoreFoundation.framework/Versions/A/Headers -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


