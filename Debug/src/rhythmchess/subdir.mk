################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/rhythmchess/Player.cpp \
../src/rhythmchess/RhythmChess.cpp 

OBJS += \
./src/rhythmchess/Player.o \
./src/rhythmchess/RhythmChess.o 

CPP_DEPS += \
./src/rhythmchess/Player.d \
./src/rhythmchess/RhythmChess.d 


# Each subdirectory must supply rules for building sources it contributes
src/rhythmchess/%.o: ../src/rhythmchess/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/Library/Frameworks/SDL2.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_image.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_ttf.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_mixer.framework/Versions/A/Headers -I"/Users/jurriaanvandenberg/Documents/Programming/C++/WeeklyGames/Main Project/src/overture/include" -I/System/Library/Frameworks/CoreFoundation.framework/Versions/A/Headers -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


